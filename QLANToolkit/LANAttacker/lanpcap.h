#ifndef LANANALYZER_H
#define LANANALYZER_H

#include <QObject>
#include <QThread>
#include <QStringList>
#include <QMap>

class LANPcap;

//use hard code to define checked port
//static constexpr  unsigned short CheckVulerablePorts[]={22,45,80,135,137,139,445};
static constexpr  unsigned short CheckVulerablePorts[]={22,45,80,135,139,8000,443};
//static constexpr  unsigned short CheckVulerablePorts[]={902,912,5357,7518,49664,8000,8080};
static constexpr  unsigned char CheckVulerablePortsSize=sizeof(CheckVulerablePorts)/sizeof(CheckVulerablePorts[0]);


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

    bool bIsHostVulerable=false; //any of the port is vulerable will mark this host as vulerable
    bool bPortVulerable[CheckVulerablePortsSize];
};


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
         return QString("%1 | IPv4 %2.%3.%4.%5").arg(InterfaceName).arg(Ipv4Addr[0]).arg(Ipv4Addr[1]).arg(Ipv4Addr[2]).arg(Ipv4Addr[3]);
    }

};



// a  wrapper class for pcap,provide basic analyze,capture and send packet
class LANPcap:public QThread
{
    //we need to use qt slot so Q_OBJECT should be added
    Q_OBJECT

signals:

    void OnUpdateHostInfo(int HostIndex);
    void OnSendTcpSynPacket(bool val);

public:
    explicit LANPcap(QObject *parent);
    ~LANPcap();

    bool SendArpRequestPacket(const LANHostInfo& SrcHost,const LANHostInfo& DstHost);
     bool SendArpReplyPacket(const LANHostInfo& SrcHost,const LANHostInfo& DstHost);

    bool SendTcpSynPacket(const LANHostInfo& SrcHost,const LANHostInfo& DstHost,unsigned short SrcPort,unsigned short DstPort);
    const  LANHostInfo& GetHostInfo(unsigned char Index){return CacheLANHostInfo[Index];}
    void StartAnalyzeLAN();
    void SetCurrentNetInterface(int Index);

    unsigned char GetCurrentInterfaceLANIndex();

    void UpdateHostVulerableInfo(int HostIndex);

    QStringList GetInterfaceDescriptionLists();

protected:
    virtual void run() Q_DECL_OVERRIDE;



private:

    void UpdateNetTable();
    void UpdateHostMacAddress();

private:


    QStringList NetInterfaceIpAddr;
    class pcap_if* DevLists=nullptr;
    struct pcap* PcapHandle=nullptr;

    QList<DeviceInterfaceInfo> CacheDevs;
    unsigned char LANFormat[3]; //format like 172.168.21

    int CurrentSelectDevIndex=-1;



    LANHostInfo CacheLANHostInfo[256];

    //Ip address map to MAC address



};

#endif // LANANALYZER_H
