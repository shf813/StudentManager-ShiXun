#include "exportthread.h"
#include <QFile>
#include <QTextStream>

ExportThread::ExportThread(QSqlTableModel *model, const QString &filePath, bool isCsv, QObject *parent)
    : QThread(parent), m_model(model), m_filePath(filePath), m_isCsv(isCsv)
{

}

void ExportThread::run()
{
    QFile file(m_filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit exportFinished(false, "文件创建失败：" + file.errorString());
        return;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    file.write(QByteArray::fromHex("EFBBBF"));  // UTF-8 BOM
    int totalRow = m_model->rowCount();

    // 写入表头
    QStringList header = {"学号", "姓名", "性别", "年龄", "专业", "身份证号", "手机号"};
    if (m_isCsv)
        out << header.join(",") << "\n";
    else
        out << header.join("\t") << "\n";

    // 逐行写入数据
    for (int r = 0; r < totalRow; ++r)
    {
        if (isInterruptionRequested())
        {
            file.close();
            emit exportFinished(false, "导出已被手动取消");
            return;
        }

        // 提取需要特殊处理的字段
        QString stuNo   = m_model->index(r, 1).data().toString();
        QString idCard  = m_model->index(r, 6).data().toString();
        QString phone   = m_model->index(r, 7).data().toString();

        if (m_isCsv)
        {
            // 对长数字字段前加制表符，避免 Excel 科学计数法
            stuNo   = "\t" + stuNo;
            idCard  = "\t" + idCard;
            phone   = "\t" + phone;
        }

        QStringList rowData;
        rowData << stuNo
                << m_model->index(r, 2).data().toString()   // 姓名
                << m_model->index(r, 3).data().toString()   // 性别
                << m_model->index(r, 4).data().toString()   // 年龄
                << m_model->index(r, 5).data().toString()   // 专业
                << idCard
                << phone;

        if (m_isCsv)
        {
            // CSV 转义：字段内双引号变为两个双引号，并用双引号包裹整个字段
            QStringList csvRow;
            for (auto &field : rowData)
            {
                field.replace("\"", "\"\"");
                csvRow << "\"" + field + "\"";
            }
            out << csvRow.join(",") << "\n";
        }
        else
        {
            // TSV (制表符分隔) 直接输出
            out << rowData.join("\t") << "\n";
        }

        emit progressUpdate(static_cast<int>((r + 1) * 100.0 / totalRow));
    }

    file.close();
    emit exportFinished(true, QString("导出完成，共 %1 条学生数据").arg(totalRow));
}
