#ifndef LANBROADCAST_H
#define LANBROADCAST_H

#include <QObject>

class NetworkMessge
{
public:
    explicit  NetworkMessge(class LANBroadcast &Broadcast,QDataStream &InStream);



    QString MessageFlag;
};

class NetworkFileTransMessage:public NetworkMessge
{

    explicit  NetworkFileTransMessage(class LANBroadcast &Broadcast,QDataStream &InStream);

public:
    QString SenderHost;
    QString FileDesc;
    qint64 TotalFileSize;

};




//class provide for udp broadcast info and readinfo
class LANBroadcast : public QObject
{
    Q_OBJECT

public:
    explicit LANBroadcast(QObject *parent = nullptr);


    void WriteInfo(QString Data);

    void BroadcastFileTransRequire(QString SenderHost, QString ReceiverHost,QString FileDesc, qint64 TotalFileSize);



public:

    constexpr static quint16 BROADCAST_PORT=10010;

protected:

    virtual void OnReceiveFileTransRequire(QString SenderHost,QString FileDesc,qint64 TotalFileSize);
signals:

public slots:
    void onReadyRead();






private:
    class QUdpSocket *Socket;
};

#endif // LANBROADCAST_H
