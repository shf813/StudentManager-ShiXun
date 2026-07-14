#include "excelimportthread.h"
#include "xlsxdocument.h"
#include "xlsxworkbook.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

using namespace QXlsx;

ExcelImportThread::ExcelImportThread(const QString &filePath, QObject *parent)
    : QThread(parent), m_filePath(filePath)
{
}

void ExcelImportThread::run()
{
    // ===== 创建线程独立数据库连接 =====
    const QString connectionName = QStringLiteral("import_excel_conn");
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        db.setDatabaseName("school_db.db");      // 与主线程相同数据库文件
        if (!db.open()) {
            emit importFinished(false, "数据库连接失败：" + db.lastError().text());
            return;
        }
    }

    // 后续统一使用此连接
    QSqlDatabase db = QSqlDatabase::database(connectionName);

    // ===== 打开 Excel 文件 =====
    Document xlsx(m_filePath);
    if (!xlsx.load()) {
        emit importFinished(false, "无法打开Excel文件，请确认文件未被其他程序占用");
        db.close();
        QSqlDatabase::removeDatabase(connectionName);
        return;
    }

    if (xlsx.sheetNames().isEmpty()) {
        emit importFinished(false, "文件中没有找到工作表");
        db.close();
        QSqlDatabase::removeDatabase(connectionName);
        return;
    }

    QString sheetName = xlsx.sheetNames().first();
    xlsx.selectSheet(sheetName);

    int totalRows = xlsx.dimension().rowCount();
    if (totalRows < 1) {
        emit importFinished(false, "工作表为空");
        db.close();
        QSqlDatabase::removeDatabase(connectionName);
        return;
    }

    // 判断第一行是否为标题
    QString firstCellValue = xlsx.read(1, 1).toString();
    bool hasHeader = firstCellValue.contains("学号", Qt::CaseInsensitive);
    int startRow = hasHeader ? 2 : 1;

    // 开启事务提升性能
    db.transaction();

    int successCount = 0;
    int failCount = 0;
    QStringList errors;
    int processed = 0;

    for (int row = startRow; row <= totalRows; ++row) {
        if (isInterruptionRequested()) {
            db.rollback();
            emit importFinished(false, "导入已被手动取消");
            db.close();
            QSqlDatabase::removeDatabase(connectionName);
            return;
        }

        // 读取7列数据
        QString stuNo   = xlsx.read(row, 1).toString().trimmed();
        QString name    = xlsx.read(row, 2).toString().trimmed();
        QString gender  = xlsx.read(row, 3).toString().trimmed();
        QString ageStr  = xlsx.read(row, 4).toString().trimmed();
        QString major   = xlsx.read(row, 5).toString().trimmed();
        QString idCard  = xlsx.read(row, 6).toString().trimmed();
        QString phone   = xlsx.read(row, 7).toString().trimmed();

        // 跳过完全空行
        if (stuNo.isEmpty() && name.isEmpty() && gender.isEmpty() &&
            ageStr.isEmpty() && major.isEmpty() && idCard.isEmpty() && phone.isEmpty())
            continue;

        if (stuNo.isEmpty() || name.isEmpty()) {
            errors.append(QString("第 %1 行学号或姓名为空").arg(row));
            failCount++;
            continue;
        }

        int age = 0;
        if (!ageStr.isEmpty()) {
            bool ok;
            age = ageStr.toInt(&ok);
            if (!ok) {
                errors.append(QString("第 %1 行年龄格式错误").arg(row));
                failCount++;
                continue;
            }
        }

        // 检查学号是否已存在
        QSqlQuery checkQuery(db);
        checkQuery.prepare("SELECT id FROM students WHERE stu_no = ?");
        checkQuery.addBindValue(stuNo);
        checkQuery.exec();

        if (checkQuery.next()) {
            // 更新已有记录
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
                errors.append(QString("第 %1 行更新失败：%2").arg(row).arg(updateQuery.lastError().text()));
                failCount++;
            } else {
                successCount++;
            }
        } else {
            // 插入新记录
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
                errors.append(QString("第 %1 行插入失败：%2").arg(row).arg(insertQuery.lastError().text()));
                failCount++;
            } else {
                successCount++;
            }
        }

        processed++;
        int percent = (processed * 100) / (totalRows - startRow + 1);
        emit progressUpdate(percent);
    }

    // 提交事务
    if (!db.commit()) {
        db.rollback();
        emit importFinished(false, QString("提交事务失败：%1").arg(db.lastError().text()));
        db.close();
        QSqlDatabase::removeDatabase(connectionName);
        return;
    }

    // 整理结果
    QString resultMsg = QString("导入完成：成功 %1 条，失败 %2 条。").arg(successCount).arg(failCount);
    if (!errors.isEmpty()) {
        resultMsg += "\n错误详情（前10条）：\n" + errors.mid(0, 10).join("\n");
    }
    emit importFinished(true, resultMsg);

    // 清理连接
    db.close();
    QSqlDatabase::removeDatabase(connectionName);
}
