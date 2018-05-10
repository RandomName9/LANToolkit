#ifndef SYNFLOODATTACKER_H
#define SYNFLOODATTACKER_H

#include <LANAttacker/netattacker.h>


class SynFloodAttacker:public NetAttacker
{

public:
     using NetAttacker::NetAttacker;

protected:

   virtual void AttackBehaveImpl(const class LANHostInfo &TargetHost) override;




};

#endif // SYNFLOODATTACKER_H
