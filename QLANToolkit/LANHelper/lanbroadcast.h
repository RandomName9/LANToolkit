#ifndef LANBROADCAST_H
#define LANBROADCAST_H

#include <QObject>

#include <QSet>

#include <utility.h>

struct FSendFileRequest
{
    QString Name;
    qint64 FilesCount;
    qint64 FilesTotalSize;


    QString toQString() const
    {
        return QString("FilesName:%1 FilesCount:%2 FilesSize:%3").arg(Name).arg(FilesCount).arg(Utility::GetSizeDescription(FilesTotalSize));
    }
};

//class provide for udp broadcast info and readinfo
class LANBroadcast : public QObject
{
    Q_OBJECT

public:
    explicit LANBroadcast(QObject *parent = nullptr);


    void WriteInfo(QString Data);

    void SetLocalHost(QString InLocalHost);


    bool IsInSameLan(QString IpAddr);

    void BroadcastFileTransRequire(QString SenderHost, QString ReceiverHost,QString FileDesc);

    void BroadcastFileTransReply(QString SenderHost);


    QString GetFileSendToLocalRequestDetail(QString IpAddr, bool &OutHasRequest);

    qint64 GetTotalRecvFilesSizeFromQuest(QString IpAddr);

    void BroadcastFileTransRequire(QString ReceiverHost,QString FileDesc);


    void SetBroadcastEnable(bool val);


    bool IsBroadcastEnable()
    {
        return bIsEnable;
    }



public:

    constexpr static quint16 BROADCAST_PORT=666;




signals:



    void OnReceiveFileSendToLocalRequest(QString Sender,FSendFileRequest Request);

    void OnReceiveReceiverReplyLocal(QString Receiver);

    void OnOnlineHostFound(QString HostIpAddr);





public slots:
    void onReadyRead();






private:
    class QUdpSocket *Socket;


    //decode the message info by its prefix
    inline QString GetMessageInfo(QString Message,QString Prefix){ return Message.startsWith(Prefix)?Message.split(Prefix+":")[1]:"";}


    QSet<QString> OnlineHosts;


    //k-Sender v-Detail, this is design for ui display info
    QMap<QString,FSendFileRequest> ReceivedSendToLocalRequest;

    QString LocalHost;

    bool bIsEnable=false;


};

#endif // LANBROADCAST_H
