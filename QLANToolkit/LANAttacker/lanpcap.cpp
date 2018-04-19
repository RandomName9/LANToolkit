#include "lanpcap.h"
#include <QtNetwork/QNetworkInterface>
#include <QDebug>
#include <QTcpSocket>
#include <stdlib.h>

#include <pcap.h>
#include <winsock2.h>
#include <Iphlpapi.h>
#include <stdio.h>

typedef struct ip_address{
    u_char byte1;
    u_char byte2;
    u_char byte3;
    u_char byte4;
}ip_address;


/* IPv4 header */
typedef struct ip_header{
    u_char ver_ihl; // Version (4 bits) + Internet header length (4 bits)
    u_char tos; // Type of service
    u_short tlen; // Total length
    u_short identification; // Identification
    u_short flags_fo; // Flags (3 bits) + Fragment offset (13 bits)
    u_char ttl; // Time to live
    u_char proto; // Protocol
    u_short crc; // Header checksum
    ip_address saddr; // Source address
    ip_address daddr; // Destination address
}ip_header;



typedef struct tcp_header {
    u_short sport;
    u_short dport;
    u_int sequence;
    u_int ack;
    u_char reserved : 4, offset : 4;
    u_char flags;
    u_short windows;
    u_short checksum;			// checksum should be recalc using psd_header
    u_short urgent_pointer;
    u_int pad;
}tcp_header;


typedef struct arp_header
{
    u_short hardwareType;
    u_short protocolType;
    u_char hardwareAddrLen;
    u_char protocolAddrLen;
    u_short operationField; //arp rarp
    u_char ether_shost[6];
    ip_address saddr;
    u_char ether_dhost[6];
    ip_address daddr;
}arp_header;


typedef struct ether_header {
    u_char ether_dhost[6];		// 6 byte mac address
    u_char ether_shost[6];
    u_short ether_type;
}ether_header;


//checksum algorithm got from internet
static USHORT checksum(USHORT *buf,int size)
{
    ULONG cksum = 0;
    while(size > 1)
    {
        cksum += *buf++;
        size -= sizeof(USHORT);
    }
    if(size)
    {
        cksum += *(USHORT*)buf;
    }
    cksum = (cksum >> 16) + (cksum & 0xffff);
    return (USHORT)(~cksum);
}


#pragma pack(1)
typedef struct tcp_packet{
    ether_header eth;
    ip_header ip;
    tcp_header tcpHeader;
}tcp_packet;


typedef struct arp_packet
{
    ether_header eth;
    arp_header  arpHeader;
    u_char padding[18];
}arp_packet;

struct psd_header {
    unsigned long   sourceip;
    unsigned long   destip;
    u_char      mbz;            //set as zero
    u_char      ptcl;           //protocol type
    unsigned short   plen;           //datagram's length(from the header to end of the packet)
    tcp_header psd_tcp;
};
#pragma pack()

//todo,might have error
void buildSynPacket(tcp_packet& tcpPacket,const unsigned char* source_mac,const uchar* source_ip,
                    const uchar* dest_ip,const unsigned char* dest_mac,
                    ushort source_port,ushort dest_port)
{
    memcpy(tcpPacket.eth.ether_dhost,dest_mac,6);
    memcpy(tcpPacket.eth.ether_shost,source_mac,6);
    tcpPacket.eth.ether_type = 8; //indicate next is ip protocol
    //-------------------ether 6+6+4=14 bytes

    //-------------------start of ip protocol=20bytes
    tcpPacket.ip.ver_ihl=0x45;  //version 4 ipv4 and header length 5*4=20 length bytes
    tcpPacket.ip.tos=0;//type of service

    tcpPacket.ip.tlen=0x2c00;//ip+tcp=0x2c=20+24 total length ,pad 0x00 for network format

    //useless parameters
    tcpPacket.ip.identification=13504;  //a random number
    tcpPacket.ip.flags_fo=0;


    tcpPacket.ip.ttl=64;

    //will be calc later,currently set as zero for further calculation
    tcpPacket.ip.crc=0;

    tcpPacket.ip.proto=6; //indicate next layer is tcp



    memcpy(&(tcpPacket.ip.saddr.byte1),source_ip,4);

    memcpy(&(tcpPacket.ip.daddr.byte1),dest_ip,4);
    //end of Ip layer

    //-------------------start of Tcp layer 24bytes
    //temp=htons(source_port);
    tcpPacket.tcpHeader.sport=htons(source_port);
    tcpPacket.tcpHeader.dport=htons(dest_port);

    //useless
    tcpPacket.tcpHeader.sequence=3268703835;   //random number
    tcpPacket.tcpHeader.ack=0;      //no ack since we use a random Ip address


    tcpPacket.tcpHeader.reserved=0; //needed by standard
    tcpPacket.tcpHeader.windows=4; //pad size=4byte
    tcpPacket.tcpHeader.flags=2; //syn
    tcpPacket.tcpHeader.urgent_pointer=0;
    tcpPacket.tcpHeader.pad=3020227586;//random data
    tcpPacket.tcpHeader.checksum=0;
    tcpPacket.tcpHeader.offset=6; //6*4=24=header_length

    //process to calculation the checksum
    uchar ipBuffer[20];
    memcpy(ipBuffer,&(tcpPacket.ip),20);
    uchar tcpBuffer[24];
    memcpy(tcpBuffer,&(tcpPacket.tcpHeader),24);

    //calc the checksum of ip
    tcpPacket.ip.crc=checksum((unsigned short*)ipBuffer,20);


    //calc the checksum of tcp
    struct psd_header psd;
    memcpy(&psd.sourceip,ipBuffer+12,4); //source ip address
    memcpy(&psd.destip,ipBuffer+16,4);   //dest ip address
    psd.mbz=0;   //must be zero
    psd.plen=htons(24);    //tcp header length
    psd.ptcl=6;       //tcp protocol
    memcpy(&psd.psd_tcp,tcpBuffer,24);  //the whole tcp header
    psd.psd_tcp.checksum=0; //psd's checksum must set zero for further calculation
    tcpPacket.tcpHeader.checksum=checksum((u_short*)&psd,36);
}


void buildArpReplyPacket(arp_packet& arpPacket,const unsigned char* source_mac,const uchar* source_ip,
                         const uchar* dest_ip,const unsigned char* dest_mac)
{
    memcpy(arpPacket.eth.ether_dhost,dest_mac,6);
    memcpy(arpPacket.eth.ether_shost,source_mac,6);
    arpPacket.eth.ether_type=htons(0x0806); //indicate arp layer

    arpPacket.arpHeader.hardwareType=htons(1);
    arpPacket.arpHeader.protocolType=htons(0x0800); //indicate ip protocol
    arpPacket.arpHeader.hardwareAddrLen=6;
    arpPacket.arpHeader.protocolAddrLen=4;
    arpPacket.arpHeader.operationField=htons(0x0002); //reply
    memcpy(&(arpPacket.arpHeader.saddr.byte1),source_ip,4);
    memcpy(&(arpPacket.arpHeader.daddr.byte1),dest_ip,4);
    memcpy(arpPacket.arpHeader.ether_shost,source_mac,6);
    memcpy(arpPacket.arpHeader.ether_dhost,dest_mac,6);
    memset(arpPacket.padding,0,18);
}


void buildArpRequestPacket(arp_packet& arpPacket,const unsigned char* source_mac,const uchar* source_ip,
                           const uchar* dest_ip,const unsigned char* dest_mac)
{

    memset(arpPacket.eth.ether_dhost,0xff,6);
    memcpy(arpPacket.eth.ether_shost,source_mac,6);
    arpPacket.eth.ether_type=htons(0x0806); //indicate arp layer

    arpPacket.arpHeader.hardwareType=htons(1);
    arpPacket.arpHeader.protocolType=htons(0x0800); //indicate ip protocol
    arpPacket.arpHeader.hardwareAddrLen=6;
    arpPacket.arpHeader.protocolAddrLen=4;
    arpPacket.arpHeader.operationField=htons(0x0001); //request
    memcpy(&(arpPacket.arpHeader.saddr.byte1),source_ip,4);
    memcpy(&(arpPacket.arpHeader.daddr.byte1),dest_ip,4);
    memcpy(arpPacket.arpHeader.ether_shost,source_mac,6);
    //  memcpy(arpPacket.arpHeader.ether_dhost,dest_mac,6);
    memset(arpPacket.arpHeader.ether_dhost,0,6);
    memset(arpPacket.padding,0,18);
}

LANPcap::LANPcap(QObject *parent):QThread(parent)
{



    char ErrBuf[255];
    //find all device
    if (-1==pcap_findalldevs(&DevLists, ErrBuf))
    {
        qDebug()<<"pcap_findalldevs error "<<ErrBuf;
        return;
    }
    else if(nullptr==DevLists)
    {
        qDebug()<<"find no dev interface";
        return;
    }

    //iterate through the device interface,and find their ipv4 address
    for(pcap_if *CurrentDev=DevLists ; nullptr!=CurrentDev ; CurrentDev=CurrentDev->next)
    {

        if(CurrentDev->addresses)
        {

            //CurrentDev->addresses,may have ipv4 or ipv6,  only use Ipv4 currently

            for(pcap_addr *DevIpAddr=CurrentDev->addresses;DevIpAddr;DevIpAddr=DevIpAddr->next)
            {

                SOCKADDR_IN* SocketAddr=(SOCKADDR_IN*)DevIpAddr->addr;
                ulong IpAddr=SocketAddr->sin_addr.s_addr;
                if(0 != IpAddr)
                {
                    uchar* Ip4vAddr=(uchar*)(&IpAddr);


                    DeviceInterfaceInfo CurrentDevInfo;

                    CurrentDevInfo.PcapInterface=CurrentDev;
                    CurrentDevInfo.InterfaceName=QString(CurrentDev->description);
                    memcpy(CurrentDevInfo.Ipv4Addr,Ip4vAddr,4);

                    //Copy the device interface info and save it to cache
                    CacheDevs.append(CurrentDevInfo);

                }

            }
        }
    }


    //debug print all avaliable devinfo
    for(auto& DevInfo:CacheDevs)
    {

        qDebug()<<DevInfo.toQString();
    }

}

LANPcap::~LANPcap()
{
    this->requestInterruption();
    this->quit();
    this->wait();
    if(PcapHandle)
    {
        pcap_close(PcapHandle);
    }
    PcapHandle=nullptr;

    pcap_freealldevs(DevLists);
    DevLists=nullptr;
}

bool LANPcap::SendArpRequestPacket(const LANHostInfo &SrcHost, const LANHostInfo &DstHost)
{
    arp_packet Packet;
    buildArpRequestPacket(Packet,SrcHost.MacAddr,SrcHost.Ipv4Addr,DstHost.Ipv4Addr,DstHost.MacAddr);


    if(0!=(pcap_sendpacket(PcapHandle,(const uchar*)(&Packet),sizeof(arp_packet))))
    {
        qDebug()<<"send arp packet fail"<<pcap_geterr(PcapHandle);


        return false;
    }

    return true;
}

bool LANPcap::SendArpReplyPacket(const LANHostInfo &SrcHost, const LANHostInfo &DstHost)
{
    arp_packet Packet;
    buildArpReplyPacket(Packet,SrcHost.MacAddr,SrcHost.Ipv4Addr,DstHost.Ipv4Addr,DstHost.MacAddr);
    if(0!=(pcap_sendpacket(PcapHandle,(const uchar*)(&Packet),sizeof(arp_packet))))
    {
        qDebug()<<"send arp packet fail"<<pcap_geterr(PcapHandle);

        return false;
    }

    return true;
}

bool LANPcap::SendTcpSynPacket(const LANHostInfo &SrcHost,const LANHostInfo &DstHost,unsigned short SrcPort,unsigned short DstPort)
{
    tcp_packet Packet;
    buildSynPacket(Packet,SrcHost.MacAddr,SrcHost.Ipv4Addr,DstHost.Ipv4Addr,DstHost.MacAddr,SrcPort,DstPort);

    if(0!=(pcap_sendpacket(PcapHandle,(const uchar*)(&Packet),sizeof(tcp_packet))))
    {
        qDebug()<<"send tcp syn packet fail"<<pcap_geterr(PcapHandle);

        emit OnSendTcpSynPacket(false);
        return false;
    }

    emit OnSendTcpSynPacket(true);
    return true;

}

void LANPcap::StartAnalyzeLAN()
{
    if(!this->isRunning())
    {

        this->start();
    }else
    {
        qDebug()<<"Thread is already in running";
    }
}

void LANPcap::SetCurrentNetInterface(int Index)
{
    if(Index<CacheDevs.size())
    {
        this->CurrentSelectDevIndex=Index;
        const DeviceInterfaceInfo &SelectInterface=CacheDevs[CurrentSelectDevIndex];
        memcpy(LANFormat,SelectInterface.Ipv4Addr,3);

        char ErrBuf[PCAP_ERRBUF_SIZE];





        if(PcapHandle=pcap_open_live(SelectInterface.PcapInterface->name,
                                     65535, //max packet size
                                     0,
                                     1000, //time out 1s
                                     ErrBuf)
                )
        {






            u_int NetMask=0xffffff00;
            struct bpf_program CompileProgram; //will be used for further compiler
            if(pcap_compile(PcapHandle, &CompileProgram, "tcp", 1, NetMask) <0)
            {
                qDebug()<<"compile error";
                return;
            }else
            {
                if( pcap_setfilter(PcapHandle, &CompileProgram)<0)
                {
                    qDebug()<<"set filter error";
                    return;
                }
            }
        }
        else
        {
            qDebug()<<"open interface fail"<<ErrBuf;
        }

    }else
    {
        qDebug()<<"set dev index out of range";
    }




}

unsigned char LANPcap::GetCurrentInterfaceLANIndex()
{
    const DeviceInterfaceInfo &SelectInterface=CacheDevs[CurrentSelectDevIndex];
    return SelectInterface.Ipv4Addr[3];
}

void LANPcap::UpdateHostVulerableInfo(int HostIndex)
{

    if(-1==CurrentSelectDevIndex)return;



    LANHostInfo& HostInfo=this->CacheLANHostInfo[HostIndex];

    //fill the lan format
    memcpy(HostInfo.Ipv4Addr,LANFormat,3);
    HostInfo.Ipv4Addr[3]=HostIndex;

    QString IpAddr=HostInfo.GetIpv4Addr();

    QTcpSocket TcpSocket(0); //use tcp socket to directly check vulerability,but will expose this machine itself

    HostInfo.bIsHostVulerable=false;//reset vulerablity to false to update

    for(int Index=0;Index<CheckVulerablePortsSize;++Index)
    {

        unsigned short Port=CheckVulerablePorts[Index];
        TcpSocket.abort();
        TcpSocket.connectToHost(IpAddr,Port);

        static constexpr int TCP_WAIT_TIME=10;
        if(TcpSocket.waitForConnected(TCP_WAIT_TIME))
        {
            HostInfo.bPortVulerable[Index]=true;
            HostInfo.bIsHostVulerable=true;
        }
        else
        {
            HostInfo.bPortVulerable[Index]=false;
        }
    }
    TcpSocket.abort();
    if(HostInfo.bIsHostVulerable)
    {
        qDebug()<<IpAddr<<"|Port State"<<HostInfo.GetVulerableInfo();
    }




}

QStringList LANPcap::GetInterfaceDescriptionLists()
{
    QStringList Result;
    for(auto& DevInfo:CacheDevs)
    {

        Result+=DevInfo.toQString();
    }
    return Result;

}

void LANPcap::run()
{
    if(-1==CurrentSelectDevIndex)return;



    for(int Index=1;Index<256;++Index) //loop through the lan with netmask 24
    {
        this->UpdateHostVulerableInfo(Index);
        emit OnUpdateHostInfo(Index);
    }

    //after tcp connect we need to copy the mac address from arp table into cache hostinfo
    //SendArp also work but take longer time
    UpdateNetTable();
    return;

}

void LANPcap::UpdateNetTable()
{
    PMIB_IPNETTABLE NetTable = nullptr;
    ULONG TableSize  = 0;

    //get the table size for create the buffer
    GetIpNetTable(NetTable, &TableSize, TRUE);

    //create and save table to buffer
    NetTable = new MIB_IPNETTABLE[TableSize];
    GetIpNetTable(NetTable, &TableSize, TRUE);

    PMIB_IPNETTABLE pNetTable = NetTable;

    //iterate through the NetTable
    for (int i = 0; i < pNetTable->dwNumEntries; ++i)
    {
        const auto& Info=pNetTable->table[i];

        QString DebugInfo;

        const unsigned char* IpAddr= reinterpret_cast<const unsigned char*>(&Info.dwAddr);
        const unsigned char* MacAddr= reinterpret_cast<const unsigned char*>(Info.bPhysAddr);

        //only update the host in the LAN
        if(0==memcmp(IpAddr,LANFormat,3))
        {
            DebugInfo.sprintf("%d.%d.%d.%d | %02x-%02x-%02x-%02x-%02x-%02x",IpAddr[0],IpAddr[1],IpAddr[2],IpAddr[3],
                    MacAddr[0],MacAddr[1],MacAddr[2],MacAddr[3],MacAddr[4],MacAddr[5]
                    );

            qDebug()<<DebugInfo;

            //copy the mac address to HostInfo
            memcpy(CacheLANHostInfo[IpAddr[3]].MacAddr,MacAddr,6);
        }



    }
    delete [] NetTable;


    IP_ADAPTER_INFO Interface[20];
    PIP_ADAPTER_INFO NetInterface = NULL;
    DWORD Result = 0;
    unsigned long Length = sizeof(Interface);
    Result = GetAdaptersInfo(Interface, &Length);
    if (Result != NO_ERROR)
    {
        printf("GetAdaptersInfo failed error.\n");
    }
    else
    {
        NetInterface = Interface;
        while (NetInterface)
        {
            static int number;
            number++;
            QString IpAddr(NetInterface->IpAddressList.IpAddress.String);

            DeviceInterfaceInfo &SelectInterface=CacheDevs[CurrentSelectDevIndex];

            if(0==SelectInterface.GetIpv4Addr().compare(IpAddr))
            {
                memcpy(SelectInterface.MacAddr,NetInterface->Address,6);
                memcpy(CacheLANHostInfo[SelectInterface.Ipv4Addr[3]].MacAddr,NetInterface->Address,6);
                qDebug()<<"Current Mac"<<SelectInterface.GetMacAddr();
            }

            NetInterface = NetInterface->Next;
        }
    }

}



