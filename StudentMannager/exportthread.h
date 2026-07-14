#ifndef EXPORTTHREAD_H
#define EXPORTTHREAD_H

#include <QThread>
#include <QSqlTableModel>
#include <QString>

class ExportThread : public QThread
{
    Q_OBJECT
public:
    explicit ExportThread(QSqlTableModel* model, const QString& filePath, bool isCsv, QObject *parent = nullptr);

protected:
    void run() override;

signals:
    void exportFinished(bool success, const QString& msg);
    void progressUpdate(int value);

private:
    QSqlTableModel* m_model;
    QString m_filePath;
    bool m_isCsv;
};

#endif // EXPORTTHREAD_H
