#ifndef EXCELEXPORTTHREAD_H
#define EXCELEXPORTTHREAD_H

#include <QThread>
#include <QSqlTableModel>
#include <QString>

class ExcelExportThread : public QThread
{
    Q_OBJECT
public:
    explicit ExcelExportThread(QSqlTableModel *model, const QString &filePath, QObject *parent = nullptr);

protected:
    void run() override;

signals:
    void exportFinished(bool success, const QString &msg);
    void progressUpdate(int value);

private:
    QSqlTableModel *m_model;
    QString m_filePath;
};

#endif // EXCELEXPORTTHREAD_H
