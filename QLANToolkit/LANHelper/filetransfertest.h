#ifndef FILETRANSFERTEST_H
#define FILETRANSFERTEST_H

#include <QThread>

class FileTransferTest : public QThread
{
public:
    FileTransferTest(class LANFileTransfer* FileTransfer, QString TestFilePath):_FileTransfer(FileTransfer),FilePath(TestFilePath){}

    static void RunTest(class LANFileTransfer* FileTransfer, QString TestFilePath);

protected:
    virtual void run() Q_DECL_OVERRIDE;
private:
    class LANFileTransfer* _FileTransfer;
    QString FilePath;
};

#endif // FILETRANSFERTEST_H
