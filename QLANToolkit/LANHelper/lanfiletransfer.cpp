#include "lanfiletransfer.h"
#include <LANHelper/lanfileinfo.h>



LANFileTransfer::LANFileTransfer(QObject *parent):LANBroadcast(parent)
{
     Client=new LANFileClient(this);
     Server=new LANFileServer(this);
}

void LANFileTransfer::SetupSendFileInfo(QString Filename, QString TargetHost)
{
    Client->SetupSendInfo(Filename,TargetHost);
}


void LANFileTransfer::SetSenderHost(QString SenderHost)
{
    Server->SetSenderHost(SenderHost);
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





