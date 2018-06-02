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

    bool Result=Server->listen(QHostAddress::AnyIPv4, CONNECT_PORT);
    if(!Result)
    {
        qDebug()<<Server->errorString();
    }

    connect(Server, &QTcpServer::acceptError, [](QAbstractSocket::SocketError Error)
    {

        qDebug()<<"accept error";
    });
}


//when have a NewConnection,using the socket the recv the file info
void LANFileServer::OnNewConnection()
{
    Socket=Server->nextPendingConnection();


    connect(Socket, SIGNAL(readyRead()), this, SLOT(OnReadyRead()));

    qDebug()<<"server establish new connection";
}

void LANFileServer::OnReadyRead()
{

  //  qDebug()<<"call on ready read";

    //a payload might contain multi files info, so we incessantly read it until remainsize zero
    qint64 RemainedSize=1;

    while(0 != RemainedSize)
    {

        QDataStream InStream(Socket);

        //if we detect we havn't create file, read the fileinfo(desc and totalFileSize) from the socket
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

        //now read the file date and write it into file
        qint64 SizeToRead=qMin(TotalFileSize-ProcessedFileSize,Socket->bytesAvailable());
        RemainedSize=Socket->bytesAvailable()-SizeToRead;
        Buf=Socket->read(SizeToRead);
        File->write(Buf);
        ProcessedFileSize+=Buf.size();

        RecvFilesProceedSize+=Buf.size();


        emit OnUpdateRecvProgress((float)ProcessedFileSize/TotalFileSize,(float)RecvFilesProceedSize/RecvFilesTotalSize);


        if(ProcessedFileSize==TotalFileSize)
        {

           qDebug()<<"Finish Receive File"<<File->fileName()<<" with"<<ProcessedFileSize-TotalFileSize<<" and RemainedSize is "<<RemainedSize;


            //Socket->read(RemainedSize);


           //when write a file is over, we reset it to recv the fileInfo and restart recv a new file
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

//giver file info and return a relative path,this is for absolute file path in different host
//for example, client:d:/a/b/c/d.txt and we get /c/d.txt if we want to send b dir
//and server recv the /c/d.txt and merge the path to d:/recvpath/c/d.txt
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

//return the desc for display usage, when the client select a dir/file,we need to get files info to display
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

    this->TotalFileSize=FilesSize;
    QString SFilesSize=Utility::GetSizeDescription(FilesSize);

    return QString("Dir:%1 FileCount:%2 FileSize:%3").arg(RootDir).arg(FilesCount).arg(SFilesSize);

}


//return the desc for network decode usage,format as [desc filecount totalsize(in byte)]
QString LANFileClient::GetSendNetworkInfo() const
{
    if(""==RootDir)
    {
        const QFileInfo &FileInfo=Files[0];
        return QString("%1 1 %2").arg(FileInfo.fileName()).arg(FileInfo.size());
    }
    else
    {
        quint64 FilesSize=0;
        quint32 FilesCount=0;

        for(const QFileInfo &FileInfo:Files)
        {
            FilesSize+=FileInfo.size();
            FilesCount++;
        }

        this->FilesTotalSize=FilesSize;
        this->FilesProcessSize=0;
        return QString("%1 %2 %3").arg(RootDir).arg(FilesCount).arg(FilesSize);
    }
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

  //  FilesSize=Files.size();
}


QString LANFileClient::ConnectToReceiver(QString TargetHost)
{


    if(Files.isEmpty())
    {
      return QString("File Not Set");
    }

    CurrentSendIndex=0;

    ResetFile();




    //make connect to send the first file
    SetupSendInfo(Files[CurrentSendIndex].absoluteFilePath(),CurrentTargetHost=TargetHost);

    return QString("Connecting");
}



//iterate the file by filename(could be a dir),and save it to Files
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




//client send the file
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

    FilesProcessSize+=Buf.size();
  //  qDebug()<<"FilesProcessSize:"<<FilesProcessSize<<" TotalSize"<<FilesTotalSize;
   emit OnUpdateSend((float)ProcessedFileSize/TotalFileSize,(float)FilesProcessSize/FilesTotalSize);


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

    if(FilesProcessSize==FilesTotalSize)
    {
        FilesProcessSize=0;
    }

}




