#ifndef LANFILERECEIVER_H
#define LANFILERECEIVER_H

#include <QObject>

class LANFileReceiver : public QObject
{
    Q_OBJECT
public:
    explicit LANFileReceiver(QObject *parent = nullptr);

    void SetupReceiveInfo(QString SenderHost);

private:
   class  QTcpServer *Server=nullptr;
    class QTcpSocket *Socket=nullptr;
    qint64 ReadFileSize=0;
    qint64 TotalFileSize=0;
    QString FileDesc;
    class QFile *File=nullptr;
    QByteArray ReceiveBuf;

signals:

public slots:
    void OnNewConnection();
    void  OnReadyRead();
};

#endif // LANFILERECEIVER_H
