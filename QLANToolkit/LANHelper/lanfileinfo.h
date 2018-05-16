#ifndef LANFILEINFO_H
#define LANFILEINFO_H

#include <QObject>
#include <QFileInfo>

class LANFileContext : public QObject
{
    Q_OBJECT
public:
    constexpr static qint64 PAYLOAD_SIZE=1024*1024; //512kb
    constexpr static quint16 CONNECT_PORT=10086; //send 4kb every time

    using QObject::QObject;

    ~LANFileContext();

    void ResetFile();



protected:
    class QFile *File;
    bool bHasSendFileInfo=false;
    QByteArray Buf;
    qint64 ProcessedFileSize;
    qint64 TotalFileSize;
    QString FileDesc;
    class  QTcpSocket *Socket=nullptr;

};


class LANFileServer : public LANFileContext
{
    Q_OBJECT
public:
    explicit LANFileServer(QObject *parent = nullptr);

    void SetSenderHost(QString SenderHost);

     void SetReceiveFileDirPath(QString DirPath){ this->DirPath=DirPath;}
private:
    class  QTcpServer *Server=nullptr;

    QString DirPath=QString("D:/LANToolKitReceive/");
signals:

public slots:
    void OnNewConnection();
    void OnReadyRead();
};


class LANFileClient : public LANFileContext
{
    Q_OBJECT
public:
    explicit LANFileClient(QObject *parent = nullptr);

    void SetupSendInfo(QString Filename, QString TargetHost);

    QString GetFilesDescription();

    void SetFilesToSend(QString FileName);
    QString ConnectToReceiver(QString TargetHost);


signals:

    void FinishSendFile();

public slots:

    void SendFile(qint64 Size);


private:


    void IterateFiles(QString FileName);



    QList<QFileInfo> Files;
    qint32 CurrentSendIndex=0;
    QString CurrentTargetHost;

    QString RootDir;


    QString GetFileRelativePath(class QFileInfo &FileInfo);

};


#endif // LANFILEINFO_H
