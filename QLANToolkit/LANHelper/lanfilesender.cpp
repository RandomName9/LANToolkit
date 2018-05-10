#include "lanfilesender.h"
#include <QTcpSocket>
#include <QFile>
#include <QFileInfo>
#include <QHostAddress>
#include <QDataStream>
#include <QDebug>

LANFileSender::LANFileSender(QObject *parent) : QObject(parent)
{
    Socket=new QTcpSocket(this);

    //when we connect to the receiver, send it the file info:size name...
    connect(Socket, SIGNAL(connected()), this, SLOT(SendFileInfo()));

    //then we write the raw data of file
    connect(Socket, SIGNAL(bytesWritten(qint64)), this, SLOT(SendFile(qint64)));
}

void LANFileSender::SetupSendInfo(QString Filename, QString TargetHost)
{
    bHasSendFileInfo=false;
    File = new QFile(Filename);
    File->open(QFile::ReadOnly);
    QFileInfo FileInfo(Filename);
    FileDesc = FileInfo.fileName();
    TotalFileSize=File->size();
    SentFileSize=0;
    Socket->abort();

    Socket->connectToHost(QHostAddress(TargetHost), FILE_SEND_PORT);
}

void LANFileSender::SendFile(qint64 Size)
{
    SentFileSize+=Size;
    SendBuf=File->read(qMin(TotalFileSize-SentFileSize, PayLoadSize));
    Socket->write(SendBuf);

    if(SentFileSize==TotalFileSize)
    {
        emit FinishSendFile();
        qDebug()<<"Finish Send File";
    }
}

void LANFileSender::SendFileInfo()
{
    QDataStream OutStream(&SendBuf, QIODevice::WriteOnly);
    OutStream<<TotalFileSize<<FileDesc;

    Socket->write(SendBuf);
    qDebug()<<"Start Send File Info";

}
