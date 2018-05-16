#include "lanfileinfo.h"

#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QDataStream>
#include <QFile>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <utility.h>

LANFileContext::~LANFileContext()
{
    ResetFile();

    if(Socket)
    {
        Socket->close();
        delete Socket;
        Socket=nullptr;
    }
}

void LANFileContext::ResetFile()
{
    // Socket->setReadBufferSize(PAYLOAD_SIZE);
    ProcessedFileSize=0;
    TotalFileSize=0;
    if(File)
    {
        File->close();
        delete File;
        File=nullptr;
    }
}





LANFileServer::LANFileServer(QObject *parent):LANFileContext(parent)
{
    Server = new QTcpServer(this);
    connect(Server, SIGNAL(newConnection()), this, SLOT(OnNewConnection()));

}

void LANFileServer::SetSenderHost(QString SenderHost)
{
    //listen to the client,when connect, OnNewConnection will be triggered
    Server->listen(QHostAddress(SenderHost), LANFileContext::CONNECT_PORT);

}

void LANFileServer::OnNewConnection()
{
    Socket=Server->nextPendingConnection();
    connect(Socket, SIGNAL(readyRead()), this, SLOT(OnReadyRead()));

    qDebug()<<"server establish new connection";
}

void LANFileServer::OnReadyRead()
{


    qint64 RemainedSize=1;

    while(0 != RemainedSize)
    {

        QDataStream InStream(Socket);
        if(nullptr==File)
        {
            InStream>>TotalFileSize>>FileDesc;
            QString FilePath=DirPath+FileDesc;



            if(!FileDesc.contains(".") || TotalFileSize<0)
            {
           //     qDebug()<<"Break With FileDesc"<<FileDesc<<" and size is "<<TotalFileSize;

                return;

            }
            //create the upper path's dir on disk
            Utility::CreateFilePath(FilePath);


            File=new QFile(FilePath);


            File->open(QFile::WriteOnly);

            qDebug()<<"Received File Path is"<<FilePath<<" and size is "<<TotalFileSize;
        }

        qint64 SizeToRead=qMin(TotalFileSize-ProcessedFileSize,Socket->bytesAvailable());
        RemainedSize=Socket->bytesAvailable()-SizeToRead;
        Buf=Socket->read(SizeToRead);
        File->write(Buf);
        ProcessedFileSize+=Buf.size();

        if(ProcessedFileSize==TotalFileSize)
        {

           qDebug()<<"Finish Receive File"<<File->fileName()<<" with"<<ProcessedFileSize-TotalFileSize<<" and RemainedSize is "<<RemainedSize;


            //Socket->read(RemainedSize);

            this->ResetFile();
        }

    }






}




LANFileClient::LANFileClient(QObject *parent):LANFileContext(parent)
{
    Socket=new QTcpSocket(this);

    //whether connected or write, we always call SendFile
    QObject::connect(Socket, &QTcpSocket::connected,[this](){this->SendFile(0);});


    connect(Socket, SIGNAL(bytesWritten(qint64)), this, SLOT(SendFile(qint64)));
}

void LANFileClient::SetupSendInfo(QString Filename, QString TargetHost)
{



    File = new QFile(Filename);
    File->open(QFile::ReadOnly);
    QFileInfo FileInfo(Filename);
    FileDesc = GetFileRelativePath(FileInfo);
    qDebug()<<"filename "<<Filename<<" FileDesc"<<FileDesc;
    TotalFileSize=File->size();
    ProcessedFileSize=0;

    if(!Socket->isOpen())
    {
        Socket->connectToHost(QHostAddress(TargetHost), LANFileContext::CONNECT_PORT);
        qDebug()<<"Client is making connection";
    }else
    {
        //send the next file info
        SendFile(0);
    }

}

QString LANFileClient::GetFileRelativePath(QFileInfo &FileInfo)
{
    if(""==RootDir)
    {
        return FileInfo.fileName();
    }
    else
    {

        QDir DirInfo(RootDir);
        QString DirName=DirInfo.dirName();

        QStringList Lists=FileInfo.absoluteFilePath().split(DirName);

        return DirName+Lists.last();





    }
}


QString LANFileClient::GetFilesDescription()
{


    if(""==RootDir)
    {
        const QFileInfo &FileInfo=Files[0];
        QString SFileSize=Utility::GetSizeDescription(FileInfo.size());
        return QString("Filename:%1 FileSize:%2").arg(FileInfo.fileName()).arg(SFileSize);

    }


    quint64 FilesSize=0;
    quint32 FilesCount=0;

    for(QFileInfo &FileInfo:Files)
    {
        FilesSize+=FileInfo.size();
        FilesCount++;
    }
    QString SFilesSize=Utility::GetSizeDescription(FilesSize);

    return QString("Dir:%1 FileCount:%2 FileSize:%3").arg(RootDir).arg(FilesCount).arg(SFilesSize);

}




void LANFileClient::SetFilesToSend(QString FileName)
{
    Files.clear();

    QDir Dir(FileName);

    if(Dir.exists())
    {
        RootDir=Dir.absolutePath();
        IterateFiles(FileName);
    }else
    {
        RootDir=QString("");
        QFileInfo FileInfo(FileName);
        Files.append(FileInfo);
    }


}

QString LANFileClient::ConnectToReceiver(QString TargetHost)
{
    if(Files.isEmpty())return QString("File Not Set");

    CurrentSendIndex=0;
    ResetFile();

    //make connect to send the first file
    SetupSendInfo(Files[CurrentSendIndex].absoluteFilePath(),CurrentTargetHost=TargetHost);

    return QString("Connecting");
}




void LANFileClient::IterateFiles(QString FileName)
{
    QDir Dir(FileName);

    foreach(QFileInfo FileInfo ,Dir.entryInfoList())
    {
        if(FileInfo.isFile())
        {
            Files.append(FileInfo);
            qDebug()<<GetFileRelativePath(FileInfo);



        }else
        {
            if(FileInfo.fileName()=="." || FileInfo.fileName() == "..")continue;
            //  qDebug() << "Entry Dir" << FileInfo.absoluteFilePath();
            IterateFiles(FileInfo.absoluteFilePath());
        }
    }
}





void LANFileClient::SendFile(qint64 Size)
{

    qDebug()<<"call SendFile with"<<File->fileName();

    QByteArray HeaderBuf;
    if(0==ProcessedFileSize)
    {

        QDataStream OutStream(&HeaderBuf, QIODevice::WriteOnly);
        OutStream<<TotalFileSize<<FileDesc;
    }



    Buf=File->read(qMin(TotalFileSize-ProcessedFileSize, PAYLOAD_SIZE-HeaderBuf.size()));


    Socket->write(HeaderBuf+Buf);


    ProcessedFileSize+=Buf.size();


//    if(ProcessedFileSize>=PAYLOAD_SIZE)
//    {
//        while(!Socket->flush());
//    }

    if(ProcessedFileSize==TotalFileSize)
    {


       // Socket->flush();
       // QThread::sleep(TotalFileSize/PAYLOAD_SIZE);


        if(CurrentSendIndex< (Files.size()-1) )
        {

            //iterate to send all the files
            SetupSendInfo(Files[++CurrentSendIndex].absoluteFilePath(),CurrentTargetHost);
        }
    }

}




