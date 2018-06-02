#ifndef LANANALYZER_H
#define LANANALYZER_H

#include <QObject>
#include <QThread>
#include <QStringList>
#include <QMap>
#include <utility.h>

class LANPcap;

//design flaw,using hardcode to define checked ports



//static constexpr  unsigned short CheckVulerablePorts[]={22,45,80,135,137,139,445};
//static constexpr  unsigned short CheckVulerablePorts[]={902,912,5357,7518,49664,8000,8080};
static constexpr  unsigned short CheckVulerablePorts[]={22,45,80,135,139,8000,443};
static constexpr  unsigned char CheckVulerablePortsSize=sizeof(CheckVulerablePorts)/sizeof(CheckVulerablePorts[0]);



//info to represent a LANHost
class LANHostInfo
{
public:
    unsigned char Ipv4Addr[4];
    unsigned char MacAddr[6];

    QString GetIpv4Addr() const
    {
        return QString("%1.%2.%3.%4").arg(Ipv4Addr[0]).arg(Ipv4Addr[1]).arg(Ipv4Addr[2]).arg(Ipv4Addr[3]);
    }


    QString GetMacAddr()
    {
        QString Result;
        Result.sprintf("%02X-%02X-%02X-%02X-%02X-%02X",MacAddr[0],MacAddr[1],MacAddr[2],MacAddr[3],MacAddr[4],MacAddr[5]);
        return Result;
    }

    QString GetVulerableInfo()
    {
        QString Result;
        for(int Index=0;Index<CheckVulerablePortsSize;++Index)
        {
            Result+=QString("%1:%2 ").arg(CheckVulerablePorts[Index]).arg(bPortVulerable[Index]);
        }
        return Result;
    }

    QString GetOpenPortsInfo()
    {
        QString Result;
        bool bHasOpenPort=false;
        for(int Index=0;Index<CheckVulerablePortsSize;++Index)
        {
            if(bPortVulerable[Index])
            {
                 Result+=QString("%1 ").arg(CheckVulerablePorts[Index]);
                 bHasOpenPort=true;
            }

        }
        return bHasOpenPort?Result:"syn_flood invulnerable";
    }

    LANHostInfo CreateRandomMacAddrHost() const
    {
        LANHostInfo CloneHost=*this;
        for(int i=0;i<6;++i)CloneHost.MacAddr[i]=Utility::RandomInteger(0,0xff);
        return CloneHost;

    }

    LANHostInfo CreateRandomIpAddrHost() const
    {
        LANHostInfo CloneHost=*this;
        for(int i=0;i<4;++i)CloneHost.Ipv4Addr[i]=Utility::RandomInteger(0,0xff);
        return CloneHost;

    }

    static  LANHostInfo CreateRandomHost()
    {
        LANHostInfo Host;
        //set a random ip,don't use ip address in the same LAN
        for(int i=0;i<4;++i)Host.Ipv4Addr[i]=Utility::RandomInteger(0,0xff);
        //fill the mac address with random value
        for(int i=0;i<6;++i)Host.MacAddr[i]=Utility::RandomInteger(0,0xff);
        return Host;
    }

    bool bIsHostVulerable=false; //any of the port is vulerable will mark this host as vulerable
    bool bPortVulerable[CheckVulerablePortsSize];
};


//info to represent a deriveinterface (wlan or ethernet...)
class DeviceInterfaceInfo
{
public:

    QString InterfaceName;
    unsigned char Ipv4Addr[4];
    unsigned char MacAddr[6];
    class pcap_if* PcapInterface=nullptr;

    QString GetIpv4Addr() const
    {
        return QString("%1.%2.%3.%4").arg(Ipv4Addr[0]).arg(Ipv4Addr[1]).arg(Ipv4Addr[2]).arg(Ipv4Addr[3]);
    }


    QString GetMacAddr()
    {
        QString Result;
        Result.sprintf("%02X-%02X-%02X-%02X-%02X-%02X",MacAddr[0],MacAddr[1],MacAddr[2],MacAddr[3],MacAddr[4],MacAddr[5]);
        return Result;
    }

    QString toQString()
    {
         return QString("%1 | %2.%3.%4.%5").arg(InterfaceName).arg(Ipv4Addr[0]).arg(Ipv4Addr[1]).arg(Ipv4Addr[2]).arg(Ipv4Addr[3]);
    }

};

//deprecated
struct FPacketBatch
{
    const LANHostInfo& SrcHost;
    const LANHostInfo& DstHost;
    unsigned short SrcPort;
    unsigned short DstPort;
};



// a  wrapper class for pcap,provide basic analyze,capture and send packet
//design flaw, heavy,should be replaced by ECS Model for further dev
class LANPcap:public QThread
{
    //we need to use qt slot so Q_OBJECT should be added
    Q_OBJECT

signals:

    void OnUpdateHostInfo(int HostIndex);

    void OnAttackSpeedChanged(QString AttackSpeedDescription);

public:
    explicit LANPcap(QObject *parent);
    ~LANPcap();

    bool SendArpRequestPacket(const LANHostInfo& SrcHost,const LANHostInfo& DstHost);
    bool SendArpReplyPacket(const LANHostInfo& SrcHost,const LANHostInfo& DstHost);

    bool SendTcpSynPacket(const LANHostInfo& SrcHost,const LANHostInfo& DstHost,unsigned short SrcPort,unsigned short DstPort);

    //deprecated
    int SendTcpSynPackets(QList<FPacketBatch> &Packets);

    const  LANHostInfo& GetHostInfo(unsigned char Index){return CacheLANHostInfo[Index];}
    void StartAnalyzeLAN();
    void SetCurrentNetInterface(int Index);

    unsigned char GetCurrentInterfaceLANIndex();

    void SetAttackPower(int Val)
    {
        this->AttackPower=Val;
    }

    void UpdateHostVulerableInfo(int HostIndex);

    QStringList GetInterfaceDescriptionLists();


    LANHostInfo GetGateWayHostInfo();
protected:
    virtual void run() Q_DECL_OVERRIDE;



private:

    void UpdateNetTable();
    void UpdateHostMacAddress();

private:


    QStringList NetInterfaceIpAddr;
    class pcap_if* DevLists=nullptr;
    struct pcap* PcapHandle=nullptr;


    //deprecated,design flaw, using thread join and struct(TArray -> fixed size Queue) to sync
    struct pcap_send_queue *PacketQueue;

    QList<DeviceInterfaceInfo> CacheDevs;

    unsigned char GateWayIndex=254;
    unsigned char LANFormat[3]; //format like 172.168.21

    int CurrentSelectDevIndex=-1;

    qint64 NetworkSendSize=0;
    qint64 LastNetworkSendSize=0;

    //design flaw:netmask could change
    LANHostInfo CacheLANHostInfo[256];

    //Ip address map to MAC address

    //design flaw:should be intergrate to ECS's attack comp
    int AttackPower=1;

};

#endif // LANANALYZER_H
