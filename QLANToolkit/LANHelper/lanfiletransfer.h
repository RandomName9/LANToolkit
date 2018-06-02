#ifndef LANFILETRANSFER_H
#define LANFILETRANSFER_H

#include <QObject>
#include <LANHelper/lanbroadcast.h>

class LANFileTransfer: public LANBroadcast
{
     Q_OBJECT
public:


    explicit LANFileTransfer(QObject *parent = nullptr);

    void SetupSendFileInfo(QString Filename, QString TargetHost);




    //receiver set where the sender is
    void SetSenderHost(QString SenderHost);

    //receiver set where sent file save on disk
    void SetReceiveFileDirPath(QString DirPath);

    //Sender set what files to send, and return the description of those file
    //this is useful to seperate the files and targets info by dividing into two function
    QString SetFilesToSend(QString FileName);

    //Sender set where the receiver is, and return the connection result;
    QString ConnectToReceiver(QString TargetHost);


    void BroadcastSendFileRequire(QString ReceiverIpAddr);


signals:

    void OnSendProgressUpdate(float CurFileProgress,float TotalProgress);
    void OnRecvProgressUpdate(float CurFileProgress,float TotalProgress);


private:
    class LANFileClient* Client;
    class LANFileServer* Server;



};

#endif // LANFILETRANSFER_H
