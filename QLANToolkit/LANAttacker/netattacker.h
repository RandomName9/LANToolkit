#ifndef NETATTACKER_H
#define NETATTACKER_H

#include <QSet>
#include <QThread>
#include <LANAttacker/lanpcap.h>

//class designed for common attack behave

class NetAttacker:public QThread
{
public:

   explicit NetAttacker(QObject *parent,class  LANPcap* LANPcap);

    ~NetAttacker();

   inline void AddTarget(unsigned char LANIndex)
    {
        Targets.insert( LANIndex);
    }

    inline void RemoveTarget(unsigned char LANIndex)
    {
        bool bIsAttackingWhenRemove=IsAttcking();
        if(bIsAttackingWhenRemove)
        {
            StopAttackTargets();
        }


        Targets.remove( LANIndex );

        if(bIsAttackingWhenRemove)
        {
            StartAttackTargets();
        }

    }

    inline bool IsAttcking(){return bIsAttacking;}

    inline int GetTargetCounts(){return Targets.size();}

    void AddTarget(QString IpAddr);

    void RemoveTarget(QString IpAddr);

    inline void ClearTargets()
    {
        this->Targets.clear();
    }

    void StartAttackTargets();

    void StopAttackTargets();




protected:
    virtual void run() Q_DECL_OVERRIDE;

    //each specific target host attack
    virtual void AttackBehaveImpl(const class LANHostInfo &TargetHost);


    inline class LANPcap* GetLANPCap(){return _LANPcap;}



protected:
    QSet<unsigned char> Targets;


private:
    class  LANPcap* _LANPcap;
    bool bIsAttacking=false;
};

#endif // NETATTACKER_H
