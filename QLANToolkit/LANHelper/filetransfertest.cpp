#include "filetransfertest.h"
#include "LANHelper/lanfiletransfer.h"



void FileTransferTest::RunTest(LANFileTransfer *FileTransfer, QString TestFilePath)
{
    auto *Test=new FileTransferTest(FileTransfer,TestFilePath);
    Test->start();
}

void FileTransferTest::run()
{
    if(nullptr!=_FileTransfer)
    {
        _FileTransfer->SetFilesToSend(this->FilePath);
        _FileTransfer->SetSenderHost("127.0.0.1");
        _FileTransfer->ConnectToReceiver("127.0.0.1");

    }
}
