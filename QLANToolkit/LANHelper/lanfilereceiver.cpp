#include "lanfilereceiver.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <LANHelper/lanfilesender.h>
#include <QDataStream>
#include <QFile>
#include <QDebug>
#include <QFileInfo>

LANFileReceiver::LANFileReceiver(QObject *parent) : QObject(parent)
{

    Server = new QTcpServer(this);
    connect(Server, SIGNAL(newConnection()), this, SLOT(OnNewConnection()));
}

void LANFileReceiver::SetupReceiveInfo(QString SenderHost)
{
    Server->listen(QHostAddress(SenderHost), LANFileSender::FILE_SEND_PORT);

}

void LANFileReceiver::OnNewConnection()
{
    Socket=Server->nextPendingConnection();
    ReadFileSize=0;
    connect(Socket, SIGNAL(readyRead()), this, SLOT(OnReadyRead()));

    if(File)
    {
        File->close();
        delete File;
        File=nullptr;
    }


    qDebug()<<"establish new connection";




}

void LANFileReceiver::OnReadyRead()
{

    //if file havn't been created,we create one
    if(nullptr==File)
    {
        QDataStream InStream(Socket);
        InStream>>TotalFileSize>>FileDesc;
        QString FilePath="ReceivedFile/"+FileDesc;
        QFileInfo Info(FilePath);
        File=new QFile("D:\\test");
        File->open(QFile::WriteOnly);
        ReadFileSize=sizeof(TotalFileSize)+FileDesc.capacity()+1;
        qDebug()<<"File Path is"<<Info.absoluteFilePath()<<" and size is "<<TotalFileSize;

    }

    ReceiveBuf=Socket->readAll();

    File->write(ReceiveBuf);
    ReadFileSize+=ReceiveBuf.size();
    File->flush();



    if(ReadFileSize>=TotalFileSize)
    {
        qDebug()<<"Finish Receive File";
        File->close();
    }
}
