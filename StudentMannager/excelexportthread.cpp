#include "excelexportthread.h"
#include "xlsxdocument.h"
#include "xlsxformat.h"
#include <QDebug>

ExcelExportThread::ExcelExportThread(QSqlTableModel *model, const QString &filePath, QObject *parent)
    : QThread(parent), m_model(model), m_filePath(filePath)
{
}

void ExcelExportThread::run()
{
    QXlsx::Document xlsx;
    QXlsx::Format headerFormat;
    headerFormat.setFontBold(true);
    headerFormat.setPatternBackgroundColor(QColor("#4472C4"));
    headerFormat.setFontColor(QColor(Qt::white));
    headerFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter);

    // 写表头
    QStringList headers = {"学号", "姓名", "性别", "年龄", "专业", "身份证号", "手机号"};
    for (int col = 0; col < headers.size(); ++col) {
        xlsx.write(1, col + 1, headers.at(col), headerFormat);
        xlsx.setColumnWidth(col + 1, 15);  // 设置列宽
    }

    int totalRow = m_model->rowCount();
    for (int r = 0; r < totalRow; ++r) {
        if (isInterruptionRequested()) {
            emit exportFinished(false, "导出已被手动取消");
            return;
        }

        // 数据从第2行开始
        xlsx.write(r + 2, 1, m_model->index(r, 1).data().toString());  // 学号
        xlsx.write(r + 2, 2, m_model->index(r, 2).data().toString());  // 姓名
        xlsx.write(r + 2, 3, m_model->index(r, 3).data().toString());  // 性别
        xlsx.write(r + 2, 4, m_model->index(r, 4).data().toInt());     // 年龄（数字格式）
        xlsx.write(r + 2, 5, m_model->index(r, 5).data().toString());  // 专业
        xlsx.write(r + 2, 6, m_model->index(r, 6).data().toString());  // 身份证号
        xlsx.write(r + 2, 7, m_model->index(r, 7).data().toString());  // 手机号

        int percent = static_cast<int>((r + 1) * 100.0 / totalRow);
        emit progressUpdate(percent);
    }

    if (!xlsx.saveAs(m_filePath)) {
        emit exportFinished(false, "文件保存失败，请检查路径是否可写或文件是否被占用");
        return;
    }

    emit exportFinished(true, QString("导出完成，共 %1 条学生数据").arg(totalRow));
}
