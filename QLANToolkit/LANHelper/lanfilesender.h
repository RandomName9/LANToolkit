#ifndef LANFILESENDER_H
#define LANFILESENDER_H

#include <QObject>

class LANFileSender : public QObject
{
    Q_OBJECT
public:
    explicit LANFileSender(QObject *parent = nullptr);

    constexpr static unsigned short FILE_SEND_PORT=10086;

    void SetupSendInfo(QString Filename, QString TargetHost);
private:
    class QTcpSocket *Socket=nullptr;

    class QFile *File;

    bool bHasSendFileInfo=false;

    QByteArray SendBuf;
    qint64 PayLoadSize=4048;   //write 4kb every time
    qint64 SentFileSize;
    qint64 TotalFileSize;
    QString FileDesc;

signals:

    void FinishSendFile();

public slots:

    void SendFile(qint64 Size);

    void SendFileInfo();
};

#endif // LANFILESENDER_H
