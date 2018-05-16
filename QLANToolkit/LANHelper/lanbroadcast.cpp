#include "lanbroadcast.h"
#include <QUdpSocket>
#include <QDataStream>

LANBroadcast::LANBroadcast(QObject *parent) : QObject(parent)
{
    Socket=new QUdpSocket(this);
    Socket->bind(BROADCAST_PORT,QUdpSocket::ShareAddress);
    connect(Socket,SIGNAL(readyRead()),this,SLOT(onReadyRead()));

}

void LANBroadcast::WriteInfo(QString Data)
{
    QByteArray ByteData=Data.toUtf8();
    Socket->writeDatagram(ByteData,QHostAddress::Broadcast,BROADCAST_PORT);
}

void LANBroadcast::BroadcastFileTransRequire(QString SenderHost, QString ReceiverHost,QString FileDesc, qint64 TotalFileSize)
{
    QString Info=QString("FILE TRANSF REQUEST:%1 -> %2 with %3, size in bytes %4").arg(SenderHost).arg(ReceiverHost).arg(FileDesc).arg(TotalFileSize);



    //TODO:use factory pattern instead------------------------------------------------
    QByteArray ByteData=Info.toUtf8();
    Socket->writeDatagram(ByteData,QHostAddress::Broadcast,BROADCAST_PORT);
}

void LANBroadcast::OnReceiveFileTransRequire(QString SenderHost, QString FileDesc, qint64 TotalFileSize)
{
    //TODO:use factory pattern instead------------------------------------------------
}



void LANBroadcast::onReadyRead()
{
    while(Socket->hasPendingDatagrams() )
    {
        QByteArray Datagram;
        Datagram.resize(Socket->pendingDatagramSize());
        Socket->readDatagram(Datagram.data(),Datagram.size());

       // QDataStream InStream(Datagram);

        //Currently use sscanf to extract broadcast message
        //further will use factory instead
        QString Message(Datagram);
        qDebug()<<Message;

    }

}





NetworkMessge::NetworkMessge(LANBroadcast &Broadcast, QDataStream &InStream)
{
    InStream>>MessageFlag;
}

NetworkFileTransMessage::NetworkFileTransMessage(LANBroadcast &Broadcast, QDataStream &InStream):NetworkMessge(Broadcast,InStream)
{
    InStream>>SenderHost>>FileDesc>>TotalFileSize;
}
