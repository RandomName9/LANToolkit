#ifndef LANFILE_H
#define LANFILE_H

#include <QObject>

class LANFile: public QObject
{
     Q_OBJECT
public:

    constexpr static qint64 PAYLOAD_SIZE=4048; //send 4kb every time

    LANFile();

    void Reset();

private:
    class QFile *File;
    bool bHasSendFileInfo=false;
    QByteArray Buf;
    qint64 ProcessedFileSize;
    qint64 TotalFileSize;
    QString FileDesc;

};

#endif // LANFILE_H
