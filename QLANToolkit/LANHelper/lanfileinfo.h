#ifndef LANFILEINFO_H
#define LANFILEINFO_H

#include <QObject>
#include <QFileInfo>


//a class hold the socket and file
class LANFileContext : public QObject
{
    Q_OBJECT
public:
    constexpr static qint64 PAYLOAD_SIZE=1024*1024; //1mb
    constexpr static quint16 CONNECT_PORT=10086;

    using QObject::QObject;

    ~LANFileContext();

    void ResetFile();



protected:
    class QFile *File=nullptr;
    bool bHasSendFileInfo=false;
    QByteArray Buf;
    qint64 ProcessedFileSize=0;
    qint64 TotalFileSize=0;
    QString FileDesc;
    class  QTcpSocket *Socket=nullptr;

};

//the recver(server), listen the client connect and broadcast udp recv
class LANFileServer : public LANFileContext
{
    Q_OBJECT
public:
    explicit LANFileServer(QObject *parent = nullptr);

    void SetRecvTotalSize(qint64 Size)
    {
        RecvFilesProceedSize=0;
        RecvFilesTotalSize=Size;
    }


     void SetReceiveFileDirPath(QString DirPath){ this->DirPath=DirPath;}
private:
    class  QTcpServer *Server=nullptr;

    qint64 RecvFilesProceedSize=0;
    qint64 RecvFilesTotalSize=0;
    QString DirPath=QString("D:/LANToolKitReceive/");
signals:

    void OnUpdateRecvProgress(float CurProgress,float TotalProgress);
public slots:
    void OnNewConnection();
    void OnReadyRead();
};


//the sender(client), when recv the udp recv from server,make the connect to server using tcp send the file
class LANFileClient : public LANFileContext
{
    Q_OBJECT
public:
    explicit LANFileClient(QObject *parent = nullptr);


    //for each file iterate usage
    void SetupSendInfo(QString Filename, QString TargetHost);


    QString GetFilesDescription();

    QString GetSendNetworkInfo() const;

    void SetFilesToSend(QString FileName);
    QString ConnectToReceiver(QString TargetHost);


signals:

    void FinishSendFile();

    void OnUpdateSend(float CurrentProgress,float TotalProgress);

public slots:

    void SendFile(qint64 Size);


private:


    void IterateFiles(QString FileName);



    QList<QFileInfo> Files;

    mutable qint64 FilesProcessSize=0;
    mutable qint64 FilesTotalSize=0;

    int FilesSize=0;
    qint32 CurrentSendIndex=0;
    QString CurrentTargetHost;

    QString RootDir;


    QString GetFileRelativePath(class QFileInfo &FileInfo);

};


#endif // LANFILEINFO_H
