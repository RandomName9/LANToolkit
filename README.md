#LANToolkit
##functions
- use qt with c++ to code, and use qss to design dark theme ui
- use qt socket to scan LAN information 
- use winpcap to achieve raw socket programming 
- able to send syn packet to perform syn_flood attack
- able to send arp packet to perform arp_sproof attack
##functions in develop
- replace QTcpSocket three-time handshakes connection with programming using winpcap to send and listen arp,icmp,half-open syn packet to speed up the scan process
- intergrate file transfer function into ui
- intergrate udp broadcast function into ui


# Use with LANToolkit
- you mus intstall winpcap before run the binary file,you can found here or download form winpcap'homepage
<https://github.com/RandomName9/LANToolkit/blob/master/Binary/WinPcap_4_1_3.exe>
- the LANToolKit binary download page
<https://github.com/RandomName9/LANToolkit/blob/master/Binary/QLANToolkit.exe>

#Code inside
- LANAttacker provide class to perform attack 
 - lanpcap provide a wrapper for winpcap, include capture,build,send packet
 - netattacker provide a thread to send packet, derive class override the attack interface
  	- arpattacker derived from netattack  to send arp_request and arp_reply
 	 - synfloodattacker derived from netattack to send syn packet
- LANHelper provide class to perform broadcast,file transfer, voip(in develop) 
  - lanfileinfo include client and server class, both derived from LANFileContext
  - lanfile transfer make client and server as class member, wrote as a interface class
  - lanbroadcast provide udp broadcast and listen function to communicate with other host in the LAN