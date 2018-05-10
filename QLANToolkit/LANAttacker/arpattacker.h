#ifndef ARPATTACKER_H
#define ARPATTACKER_H

#include "LANAttacker/netattacker.h"


class ArpAttacker:public NetAttacker
{
public:

    using NetAttacker::NetAttacker;


protected:
    virtual void AttackBehaveImpl(const class LANHostInfo &TargetHost) override;



private:






  //  void SendArpAttackPacketDebug3(unsigned char LANIndex);

};

#endif // ARPATTACKER_H
