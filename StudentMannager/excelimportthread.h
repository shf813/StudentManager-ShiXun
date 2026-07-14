#ifndef EXCELIMPORTTHREAD_H
#define EXCELIMPORTTHREAD_H

#include <QThread>
#include <QString>
#include <QStringList>

class ExcelImportThread : public QThread
{
    Q_OBJECT
public:
    explicit ExcelImportThread(const QString &filePath, QObject *parent = nullptr);

signals:
    void importFinished(bool success, const QString &msg);
    void progressUpdate(int value);

protected:
    void run() override;

private:
    QString m_filePath;
};

#endif // EXCELIMPORTTHREAD_H
