//////////////////////////////////////////////////////////////////////////
//FileName:	Header.h
//Data:		2009-04-11
//Remark:	项目所需头文件、全局变量及函数声明
//////////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////////////////////////////////////////////
//包含头文件
//////////////////////////////////////////////////////////////////////////
#include <WinSock2.h>
#include <commctrl.h>
#include <stdlib.h>
#include <vector>
using std::vector;
//Winpcap头文件
#include <pcap.h>
#include "remote-ext.h"
//各种网络协议声明
#include "Protocol.h"

//////////////////////////////////////////////////////////////////////////
//宏定义部分
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
//结构体声明部分
//////////////////////////////////////////////////////////////////////////
//IP协议枚举类型 
enum Proto	{TCP=0,UDP};
//必要的数据包信息结构体
struct Packet{
	Proto			Protocol;
	unsigned char	SrcMac[6];
	unsigned char	DestMac[6];
	in_addr			SrcIp;
	in_addr			DestIp;
	unsigned short	SrcPort;
	unsigned short  DestPort;
	unsigned int	Length;
	char			*pContent;
};

//线程参数结构体
struct ThreadArg{
	LPVOID pcapHandle;
	LPVOID pFunc;	
};

//////////////////////////////////////////////////////////////////////////
//全局变量声明部分
//////////////////////////////////////////////////////////////////////////
//线程句柄
extern HANDLE hThread;
//数据包存储向量
extern vector<Packet> packets;
//数据包暂存结构体
extern Packet p;
//pcap_t句柄
extern pcap_t *pcapHandle;
//线程参数
extern ThreadArg arg;
//数据包数量
extern unsigned int PacketCount;


//////////////////////////////////////////////////////////////////////////
//函数声明部分
//////////////////////////////////////////////////////////////////////////
//以太网分析回调函数
void EtherPacketCallback(u_char *argument,const struct pcap_pkthdr *packetHdr,const u_char *packetContent);
//IP分析回调函数
BOOL IpPacketCallback(u_char *argument,const struct pcap_pkthdr *packetHdr,const u_char *packetContent);
//UDP分析回调函数
BOOL UdpPacketCallback(u_char *argument,const struct pcap_pkthdr *packetHdr,const u_char *packetContent);
//Tcp分析回调函数
BOOL TcpPacketCallback(u_char *argument,const struct pcap_pkthdr *packetHdr,const u_char *packetContent);
BOOL InsertPacket(Packet p);
//线程回调函数
BOOL Func(LPVOID p);
//开始嗅探
BOOL StartSniffer();
//停止嗅探
BOOL StopSniffer(pcap_t *pcapHandle);
//Winpcap初始化
int InitWpcap(void);