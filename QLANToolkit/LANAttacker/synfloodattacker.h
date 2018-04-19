#ifndef SYNFLOODATTACKER_H
#define SYNFLOODATTACKER_H

#include <QThread>
#include <QSet>


class SynFloodAttacker:public QThread
{

public:
    explicit SynFloodAttacker(QObject *parent,class  LANPcap* LANPcap);

    ~SynFloodAttacker();

    void AddTarget(unsigned char LANIndex)
    {
        Targets.insert( LANIndex);
    }

    void RemoveTarget(unsigned char LANIndex)
    {
        Targets.remove( LANIndex );
    }

    void AddTarget(QString IpAddr);

    void RemoveTarget(QString IpAddr);

    void ClearTargets()
    {
        this->Targets.clear();
    }

    void StartAttackTargets();
    void StopAttackTargets();

protected:
    virtual void run() Q_DECL_OVERRIDE;

private:
    void SendSynFloodPacket(unsigned char LANIndex);

    //func test for attack host outside the LAN
    void SendSynDebug();

private:

    class  LANPcap* _LANPcap;
    QSet<unsigned char> Targets;
};

#endif // SYNFLOODATTACKER_H
