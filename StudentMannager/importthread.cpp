#include "importthread.h"
#include <QFile>
#include <QTextStream>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

ImportThread::ImportThread(const QString &filePath, QObject *parent)
    : QThread(parent), m_filePath(filePath)
{
}

QStringList ImportThread::parseCsvLine(const QString &line) const
{
    QStringList fields;
    QString field;
    bool inQuotes = false;

    for (int i = 0; i < line.size(); ++i) {
        QChar c = line.at(i);
        if (inQuotes) {
            if (c == '"') {
                // 可能遇到双引号转义（""）或结束引号
                if (i + 1 < line.size() && line.at(i + 1) == '"') {
                    field += '"';
                    ++i; // 跳过下一个引号
                } else {
                    inQuotes = false;
                }
            } else {
                field += c;
            }
        } else {
            if (c == '"') {
                inQuotes = true;
            } else if (c == ',') {
                fields.append(field.trimmed());
                field.clear();
            } else {
                field += c;
            }
        }
    }
    fields.append(field.trimmed()); // 最后一个字段
    return fields;
}

void ImportThread::run()
{
    // ===== 创建线程独立数据库连接 =====
    const QString connectionName = QStringLiteral("import_csv_conn");
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        db.setDatabaseName("school_db.db");
        if (!db.open()) {
            emit importFinished(false, "数据库连接失败：" + db.lastError().text());
            return;
        }
    }
    QSqlDatabase db = QSqlDatabase::database(connectionName);

    // ===== 打开 CSV 文件 =====
    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit importFinished(false, QString("无法打开文件：%1").arg(file.errorString()));
        db.close();
        QSqlDatabase::removeDatabase(connectionName);
        return;
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);

    // 读取第一行，判断是否有表头
    QString firstLine = in.readLine();
    if (firstLine.startsWith(QChar::ByteOrderMark))
        firstLine.remove(0, 1);
    bool hasHeader = firstLine.contains("学号", Qt::CaseInsensitive);

    // 如果无表头，需要把第一行当作数据处理，这里为简化一律重新定位
    // 统计总行数用于进度条（可优化，为清晰暂保留原逻辑）
    file.seek(0);
    QTextStream countStream(&file);
    countStream.setEncoding(QStringConverter::Utf8);
    int totalLines = 0;
    while (!countStream.atEnd()) {
        countStream.readLine();
        totalLines++;
    }
    // 如果包含表头，数据行少一行
    int dataLines = hasHeader ? totalLines - 1 : totalLines;

    // 重新定位到数据开始位置
    file.seek(0);
    QTextStream dataStream(&file);
    dataStream.setEncoding(QStringConverter::Utf8);
    if (hasHeader) {
        dataStream.readLine(); // 跳过表头
    }

    // 开启事务
    db.transaction();

    int successCount = 0;
    int failCount = 0;
    QStringList errors;
    int processed = 0;
    int lineNum = 0;

    while (!dataStream.atEnd()) {
        if (isInterruptionRequested()) {
            db.rollback();
            emit importFinished(false, "导入已被手动取消");
            file.close();
            db.close();
            QSqlDatabase::removeDatabase(connectionName);
            return;
        }

        QString line = dataStream.readLine().trimmed();
        lineNum++;
        if (line.isEmpty()) continue;

        // 解析 CSV 行
        QStringList fields = parseCsvLine(line);
        if (fields.size() < 7) {
            errors.append(QString("第 %1 行字段数不足 (需7个字段)").arg(lineNum));
            failCount++;
            continue;
        }

        QString stuNo   = fields.at(0).trimmed();
        QString name    = fields.at(1).trimmed();
        QString gender  = fields.at(2).trimmed();
        QString ageStr  = fields.at(3).trimmed();
        QString major   = fields.at(4).trimmed();
        QString idCard  = fields.at(5).trimmed();
        QString phone   = fields.at(6).trimmed();

        if (stuNo.isEmpty() || name.isEmpty()) {
            errors.append(QString("第 %1 行学号或姓名为空").arg(lineNum));
            failCount++;
            continue;
        }

        int age = 0;
        if (!ageStr.isEmpty()) {
            bool ok;
            age = ageStr.toInt(&ok);
            if (!ok) {
                errors.append(QString("第 %1 行年龄格式错误").arg(lineNum));
                failCount++;
                continue;
            }
        }

        // 检查是否存在
        QSqlQuery checkQuery(db);
        checkQuery.prepare("SELECT id FROM students WHERE stu_no = ?");
        checkQuery.addBindValue(stuNo);
        checkQuery.exec();

        if (checkQuery.next()) {
            // 更新
            QSqlQuery updateQuery(db);
            updateQuery.prepare("UPDATE students SET name=?, gender=?, age=?, major=?, id_card=?, phone=? WHERE stu_no=?");
            updateQuery.addBindValue(name);
            updateQuery.addBindValue(gender);
            updateQuery.addBindValue(age);
            updateQuery.addBindValue(major);
            updateQuery.addBindValue(idCard);
            updateQuery.addBindValue(phone);
            updateQuery.addBindValue(stuNo);

            if (!updateQuery.exec()) {
                errors.append(QString("第 %1 行更新失败：%2").arg(lineNum).arg(updateQuery.lastError().text()));
                failCount++;
            } else {
                successCount++;
            }
        } else {
            // 插入
            QSqlQuery insertQuery(db);
            insertQuery.prepare("INSERT INTO students (stu_no, name, gender, age, major, id_card, phone) "
                                "VALUES (?, ?, ?, ?, ?, ?, ?)");
            insertQuery.addBindValue(stuNo);
            insertQuery.addBindValue(name);
            insertQuery.addBindValue(gender);
            insertQuery.addBindValue(age);
            insertQuery.addBindValue(major);
            insertQuery.addBindValue(idCard);
            insertQuery.addBindValue(phone);

            if (!insertQuery.exec()) {
                errors.append(QString("第 %1 行插入失败：%2").arg(lineNum).arg(insertQuery.lastError().text()));
                failCount++;
            } else {
                successCount++;
            }
        }

        processed++;
        int percent = (processed * 100) / dataLines;
        emit progressUpdate(percent);
    }

    file.close();

    if (!db.commit()) {
        db.rollback();
        emit importFinished(false, QString("提交事务失败：%1").arg(db.lastError().text()));
        db.close();
        QSqlDatabase::removeDatabase(connectionName);
        return;
    }

    QString resultMsg = QString("导入完成：成功 %1 条，失败 %2 条。").arg(successCount).arg(failCount);
    if (!errors.isEmpty()) {
        resultMsg += "\n错误详情（前10条）：\n" + errors.mid(0, 10).join("\n");
    }
    emit importFinished(true, resultMsg);

    db.close();
    QSqlDatabase::removeDatabase(connectionName);
}
