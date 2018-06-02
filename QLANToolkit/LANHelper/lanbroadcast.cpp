#include "lanbroadcast.h"
#include <QUdpSocket>
#include <QDataStream>
#include <QTimer>

LANBroadcast::LANBroadcast(QObject *parent) : QObject(parent)
{
    Socket=new QUdpSocket(this);
    //Socket->bind(BROADCAST_PORT,QUdpSocket::ShareAddress);
      Socket->bind(BROADCAST_PORT,QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    connect(Socket,SIGNAL(readyRead()),this,SLOT(onReadyRead()));

    //broadcast host online every sec
    QTimer *Timer=new QTimer(this);
    QObject::connect(Timer,&QTimer::timeout,

                     [this]()
    {

        if(!bIsEnable)return;

        if(""==LocalHost)return;

        QString Info=QString("ONLINE BROADCAST:%1").arg(LocalHost);

        WriteInfo(Info);

    }
    );
    Timer->start(1000);
}

void LANBroadcast::WriteInfo(QString Data)
{
    QByteArray ByteData=Data.toUtf8();


   Socket->writeDatagram(ByteData,QHostAddress::Broadcast,BROADCAST_PORT);



   // some host deny broadcast message, we need to iterate all the subnet
    QStringList LocalHostAddr=LocalHost.split(".");
    for(int i=1;i<=255;++i)
    {
        QString HostIp=QString("%1.%2.%3.%4").arg(LocalHostAddr[0]).arg(LocalHostAddr[1]).arg(LocalHostAddr[2]).arg(i);
        Socket->writeDatagram(ByteData,QHostAddress(HostIp),BROADCAST_PORT);
    }

}

void LANBroadcast::SetLocalHost(QString InLocalHost)
{

    if(LocalHost!=InLocalHost)
    {
        //detect interface changed, the cached onlinehost should be cleared;
        OnlineHosts.clear();
        ReceivedSendToLocalRequest.clear();
    }
    this->LocalHost=InLocalHost;
  //  qDebug()<<"the localhost is "<<LocalHost;
}

bool LANBroadcast::IsInSameLan(QString IpAddr)
{
    if(""==LocalHost)return false;
    QStringList LocalHostAddr=LocalHost.split(".");
    QStringList AnotherHostAddr=IpAddr.split(".");
    return  (LocalHostAddr[0]==AnotherHostAddr[0])&&
            (LocalHostAddr[1]==AnotherHostAddr[1])&&
            (LocalHostAddr[2]==AnotherHostAddr[2]);
}

void LANBroadcast::BroadcastFileTransReply(QString SenderHost)
{
    if(!bIsEnable)return;
    if(""==LocalHost)return;
    QString Info=QString("FILE TRANSF REPLY:%1 %2").arg(SenderHost).arg(LocalHost);
    QByteArray ByteData=Info.toUtf8();

    qDebug()<<"client send file send broadcast"<<Info;
    Socket->writeDatagram(ByteData,QHostAddress(SenderHost),BROADCAST_PORT);
}

QString LANBroadcast::GetFileSendToLocalRequestDetail(QString IpAddr, bool& OutHasRequest)
{
    OutHasRequest=ReceivedSendToLocalRequest.contains(IpAddr);
    return OutHasRequest?ReceivedSendToLocalRequest[IpAddr].toQString():QString("No File Send Request Broadcast Received");

}

qint64 LANBroadcast::GetTotalRecvFilesSizeFromQuest(QString IpAddr)
{
    return ReceivedSendToLocalRequest.contains(IpAddr)? ReceivedSendToLocalRequest[IpAddr].FilesTotalSize:-1;
}







void LANBroadcast::BroadcastFileTransRequire(QString ReceiverHost, QString FileDesc)
{
    if(!bIsEnable)return;
    if(""==LocalHost)return;
    QString Info=QString("FILE TRANSF REQUEST:%1 %2 %3").arg(LocalHost).arg(ReceiverHost).arg(FileDesc);
    QByteArray ByteData=Info.toUtf8();

   // qDebug()<<"client send file send broadcast"<<Info;
  //  Socket->writeDatagram(ByteData,QHostAddress(ReceiverHost),BROADCAST_PORT);
     Socket->writeDatagram(ByteData,QHostAddress(ReceiverHost),BROADCAST_PORT);
}

void LANBroadcast::SetBroadcastEnable(bool val)
{


        if(!val)
        {
          this->OnlineHosts.clear();
        }

        this->bIsEnable=val;


}




void LANBroadcast::onReadyRead()
{
   // qDebug()<<"call on ready read and "<<bIsEnable;

    while(Socket->hasPendingDatagrams()  )
    {

        QByteArray Datagram;
        Datagram.resize(Socket->pendingDatagramSize());
        Socket->readDatagram(Datagram.data(),Datagram.size());

        // QDataStream InStream(Datagram);

        //Currently use sscanf to extract broadcast message
        //further will use factory instead
        QString Message(Datagram);
        QString MessageInfo;



        if(!bIsEnable)continue;

        if( "" != (MessageInfo=GetMessageInfo(Message,"FILE TRANSF REQUEST")))
        {

            QTextStream OutStream(&MessageInfo);
            QString Sender,Receiver,FilesName,FilesCount,FilesSize;

            OutStream >> Sender >> Receiver >> FilesName>> FilesCount >> FilesSize;
            qDebug()<<"s:"<<Sender<<" r:"<<Receiver<<" name:"<<FilesName<<" count:"<<FilesCount<<" FilesSize:"<<FilesSize;

            if(Receiver==LocalHost)
            {
                //update the map info
                FSendFileRequest Request;
                Request.Name=FilesName;
                Request.FilesCount=FilesCount.toLongLong();
                Request.FilesTotalSize=FilesSize.toLongLong();
                ReceivedSendToLocalRequest.insert(Sender,Request);

                emit OnReceiveFileSendToLocalRequest(Sender,Request);
            }

        }else if("" != (MessageInfo=GetMessageInfo(Message,"FILE TRANSF REPLY")))
        {
            QTextStream OutStream(&MessageInfo);
            QString Sender,Receiver;
            OutStream >> Sender >> Receiver;
            if(Sender==LocalHost)
            {
                emit OnReceiveReceiverReplyLocal(Receiver);
            }

        }

        else if("" != (MessageInfo=GetMessageInfo(Message,"ONLINE BROADCAST")))
        {

          //  qDebug()<<MessageInfo;
            if(IsInSameLan(MessageInfo))
            {
                if(!OnlineHosts.contains(MessageInfo))
                {
                    emit OnOnlineHostFound(MessageInfo);
                }
                OnlineHosts.insert(MessageInfo);
            }

        }




    }

}








