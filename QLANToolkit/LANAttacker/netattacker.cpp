#include "netattacker.h"
#include "LANAttacker/lanpcap.h"
#include <QDebug>
NetAttacker::NetAttacker(LANPcap *LANPcap):QThread(LANPcap),_LANPcap(LANPcap)
{

}

NetAttacker::~NetAttacker()
{
    this->requestInterruption();
    this->quit();
    this->wait();
}

void NetAttacker::AddTarget(QString IpAddr)
{
    QString IndexStr=IpAddr.split(".")[3];
    AddTarget(IndexStr.toInt());
}

void NetAttacker::RemoveTarget(QString IpAddr)
{
    QString IndexStr=IpAddr.split(".")[3];
    RemoveTarget(IndexStr.toInt());
}

void NetAttacker::StartAttackTargets()
{
    if(this->isRunning())
    {
        qDebug()<<"NetAttack Thread is already in running";
        return;
    }

    this->start();
    bIsAttacking=true;
}

void NetAttacker::StopAttackTargets()
{
    QMutexLocker Locker(&Mutex);
    this->requestInterruption();
    this->terminate();
    bIsAttacking=false;
}

void NetAttacker::run()
{
    while(!isInterruptionRequested())
    {

        if(_LANPcap)
        {

            QMutexLocker Locker(&Mutex);
            for(unsigned char LANIndex:Targets)
            {

                  AttackBehaveImpl(_LANPcap->GetHostInfo(LANIndex));
            }



        }


    }
}

void NetAttacker::AttackBehaveImpl(const LANHostInfo &TargetHost)
{
    //derived class should impl this function for each target
}




