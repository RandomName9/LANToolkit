#include "lanfiletransfer.h"
#include <LANHelper/lanfileinfo.h>
#include <QTimer>


LANFileTransfer::LANFileTransfer(QObject *parent):LANBroadcast(parent)
{
     Client=new LANFileClient(this);
     Server=new LANFileServer(this);


     connect(this,&LANBroadcast::OnReceiveReceiverReplyLocal,
             [this](QString Receiver)
     {
         //qDebug()<<"receive reply from receiver";
         ConnectToReceiver(Receiver);
     });


     connect(Client,&LANFileClient::OnUpdateSend,[this](float CurProgress,float TotalProgress)
     {
         //qDebug()<<"the curPrgress "<<CurProgress<<" and the TotalProgress"<<TotalProgress;

         //signal need to retransmit as interface for outter
         emit OnSendProgressUpdate(CurProgress,TotalProgress);
     });


     connect(Server,&LANFileServer::OnUpdateRecvProgress,[this](float CurProgress,float TotalProgress)
     {
       //  qDebug()<<"the curPrgress "<<CurProgress<<" and the TotalProgress"<<TotalProgress;

         //signal need to retransmit as interface for outter
         emit OnRecvProgressUpdate(CurProgress,TotalProgress);
     });
}

void LANFileTransfer::SetupSendFileInfo(QString Filename, QString TargetHost)
{
    Client->SetupSendInfo(Filename,TargetHost);
}


void LANFileTransfer::SetSenderHost(QString SenderHost)
{



   Server->SetRecvTotalSize(GetTotalRecvFilesSizeFromQuest(SenderHost));

   //send udp reply wait for callback(client making connecting),then wait to listen to that connecting
   BroadcastFileTransReply(SenderHost);

}

void LANFileTransfer::SetReceiveFileDirPath(QString DirPath)
{
    Server->SetReceiveFileDirPath(DirPath);
}

QString LANFileTransfer::SetFilesToSend(QString FileName)
{
    Client->SetFilesToSend(FileName);
    return Client->GetFilesDescription();
}

QString LANFileTransfer::ConnectToReceiver(QString TargetHost)
{
    return Client->ConnectToReceiver(TargetHost);
}

void LANFileTransfer::BroadcastSendFileRequire(QString ReceiverIpAddr)
{
    BroadcastFileTransRequire(ReceiverIpAddr,Client->GetSendNetworkInfo());
}





