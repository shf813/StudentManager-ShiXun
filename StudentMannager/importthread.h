#ifndef IMPORTTHREAD_H
#define IMPORTTHREAD_H

#include <QThread>
#include <QStringList>

class ImportThread : public QThread
{
    Q_OBJECT
public:
    explicit ImportThread(const QString &filePath, QObject *parent = nullptr);

protected:
    void run() override;

signals:
    void importFinished(bool success, const QString &msg);
    void progressUpdate(int value);

private:
    QString m_filePath;
    // 解析 CSV 一行，处理引号转义
    QStringList parseCsvLine(const QString &line) const;
};

#endif // IMPORTTHREAD_H
