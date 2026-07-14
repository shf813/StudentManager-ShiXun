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
    // 写入UTF-8 BOM标记
    file.write(QByteArray::fromHex("EFBBBF"));
    int totalRow = m_model->rowCount();

    // 写入固定表头
    QStringList header = {"学号", "姓名", "性别", "年龄", "专业", "身份证号", "手机号"};
    if(m_isCsv)
        out << header.join(",") << "\n";
    else
        out << header.join("\t") << "\n";

    // 逐行写入数据
    for(int r = 0; r < totalRow; r++)
    {
        // 响应取消操作
        if (isInterruptionRequested())
        {
            file.close();
            emit exportFinished(false, "导出已被手动取消");
            return;
        }

        QStringList rowData;
        rowData << m_model->index(r,1).data().toString()
                << m_model->index(r,2).data().toString()
                << m_model->index(r,3).data().toString()
                << m_model->index(r,4).data().toString()
                << m_model->index(r,5).data().toString()
                << m_model->index(r,6).data().toString()
                << m_model->index(r,7).data().toString();

        // CSV转义处理
        if(m_isCsv)
        {
            QStringList csvRow;
            for (auto& field : rowData)
            {
                field.replace("\"", "\"\"");
                csvRow << "\"" + field + "\"";
            }
            out << csvRow.join(",") << "\n";
        }
        else
        {
            out << rowData.join("\t") << "\n";
        }

        emit progressUpdate(static_cast<int>((r+1)*100.0/totalRow));
    }

    file.close();
    emit exportFinished(true, QString("导出完成，共 %1 条学生数据").arg(totalRow));
}
