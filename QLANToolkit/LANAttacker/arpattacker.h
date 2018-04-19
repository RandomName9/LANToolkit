#ifndef ARPATTACKER_H
#define ARPATTACKER_H

#include <QSet>
#include <QThread>

class ArpAttacker:public QThread
{
public:
    explicit ArpAttacker(QObject *parent, class  LANPcap* LANPcap):QThread(parent),_LANPcap(LANPcap){}


    void StartAttack();

    void AddTarget(unsigned char LANIndex)
    {
        this->Targets.insert(LANIndex);
    }



protected:

   virtual void run() Q_DECL_OVERRIDE;


private:

    void SendArpAttackPacket(unsigned char LANIndex);

    void SendArpAttackPacketDebug(unsigned char LANIndex);
    void SendArpAttackPacketDebug2(unsigned char LANIndex);

    void SendArpAttackPacketDebug3(unsigned char LANIndex);
private:

    class LANPcap* _LANPcap;
     QSet<unsigned char> Targets;
};

#endif // ARPATTACKER_H
