//////////////////////////////////////////////////////////////////////////
//FileName:	Sniffer.cpp
//Data:		2009-04-11
//Remark:	嗅探器核心函数实现
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "Header.h"

//////////////////////////////////////////////////////////////////////////
//全局变量定义部分
//////////////////////////////////////////////////////////////////////////
//线程句柄
HANDLE hThread;
//数据包存储向量
vector<Packet> packets;
//数据包暂存结构体
Packet p;
//pcap_t句柄
pcap_t *pcapHandle;
//线程参数
ThreadArg arg;
//数据包数量
unsigned int PacketCount;


//////////////////////////////////////////////////////////////////////////
//核心函数实现
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//初始化wpcap
//////////////////////////////////////////////////////////////////////////
int InitWpcap(void)
{
	//网络设备指针
	pcap_if_t *pcapDev;
	//存储错误信息
	char errContent[PCAP_ERRBUF_SIZE];
	//BPF过滤规则
	bpf_program filter;
	//过滤规则字符串
	char bpfFilterString[] = "ip";
	//掩码
	bpf_u_int32	netMask;
	//网络地址
	bpf_u_int32 netIp = 0;


	// 获得网络设备指针
	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &pcapDev, errContent) == -1)
	{
		MessageBoxA(NULL,"获取网络设备失败","警告",MB_OK);
		return -1;
	}
	//选取合适网卡
	while (pcapDev->addresses==NULL)
	{
		pcapDev = pcapDev->next;
	}

	//打开网卡
	if ((pcapHandle = pcap_open(pcapDev->name,65536,PCAP_OPENFLAG_PROMISCUOUS,1000,NULL,errContent)) == NULL )
	{
		MessageBoxA(NULL,"打开网卡失败","警告",MB_OK);
		return -1;
	}

	// 检查链路层，只支持以太网
	if (pcap_datalink(pcapHandle) != DLT_EN10MB)
	{
		MessageBoxA(NULL,"只支持以太网","警告",MB_OK);
		pcap_freealldevs(pcapDev);
		return -1;
	}
	if (pcapDev->addresses != NULL)
	{
		//取得网络接口子网掩码
		netMask = ((struct sockaddr_in *)(pcapDev->addresses->netmask))->sin_addr.S_un.S_addr;
	}
	else
	{
		netMask = 0xFFFFFFFF;
	}

	//编译BPF过滤规则
	pcap_compile(pcapHandle,&filter,bpfFilterString,0,netIp);

	//设置过滤规则
	pcap_setfilter(pcapHandle,&filter);

	arg.pcapHandle = pcapHandle;

	return 0;
}

//////////////////////////////////////////////////////////////////////////
//IP分析回调函数
//////////////////////////////////////////////////////////////////////////
BOOL IpPacketCallback(u_char *argument,const struct pcap_pkthdr *packetHdr,const u_char *packetContent)
{
	//IP协议变量
	IP_HEADER *IpProtocol;

	//获得IP协议内容
	IpProtocol = (IP_HEADER *)(packetContent+sizeof(ETHER_HEADER));

	//填充源IP和目的IP
	p.SrcIp = IpProtocol->SrcIp;
	p.DestIp = IpProtocol->DestIp;

	//分析IP协议类型
	switch(IpProtocol->proto)
	{
	case IPPROTO_TCP:
		p.Protocol = TCP;
		//进一步TCP协议
		TcpPacketCallback(argument,packetHdr,packetContent);
		break;
	case IPPROTO_UDP:
		p.Protocol = UDP;
		//进一步分析UDP协议
		UdpPacketCallback(argument,packetHdr,packetContent);
		break;
	default :
		break;
	}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//TCP分析回调函数
//////////////////////////////////////////////////////////////////////////
BOOL TcpPacketCallback(u_char *argument,const struct pcap_pkthdr *packetHdr,const u_char *packetContent)
{
	//TCP协议变量
	TCP_HEADER	*TcpProtocol;

	//获得TCP协议内容
	TcpProtocol = (TCP_HEADER *)(packetContent+14+20);

	//分别填充TCP协议的源端口、目的端口及数据长度
	p.SrcPort = TcpProtocol->SrcPort;
	p.DestPort = TcpProtocol->DestPort;
	//计算TCP包的数据长度
	IP_HEADER *IpHdr = (IP_HEADER *)(packetContent+14);
	int totallength = ntohs(IpHdr->TotalLen);
	int HeaderLength = totallength-40;
	p.Length = HeaderLength;

	//填充数据内容
	char *pBuf = new char[HeaderLength+1];
	memset(pBuf,0,HeaderLength+1);
	char *ptr = (char *)((PBYTE)(TcpProtocol)+sizeof(TCP_HEADER));
	memcpy(pBuf,ptr,HeaderLength);

	p.pContent = pBuf;

	//将捕获到的数据包信息加入向量列表
	packets.push_back(p);

	InsertPacket(p);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//UDP分析回调函数
//////////////////////////////////////////////////////////////////////////
BOOL UdpPacketCallback(u_char *argument,const struct pcap_pkthdr *packetHdr,const u_char *packetContent)
{
	//UDP 协议变量
	UDP_HEADER *UdpProtocol;

	//获得UDP协议内容
	UdpProtocol = (UDP_HEADER *)(packetContent+14+20);

	//分别填充UDP协议的源端口、目的端口及数据长度
	p.SrcPort = UdpProtocol->SrcPort;
	p.DestPort = UdpProtocol->DestPort;
	int	HeaderLength = ntohs(UdpProtocol->Length)-sizeof(UDP_HEADER);
	p.Length = HeaderLength;

	//填充数据内容
	char *pBuf = new char[HeaderLength+1];
	memset(pBuf,0,HeaderLength+1);
	char *ptr = (char *)((PBYTE)(UdpProtocol)+sizeof(UDP_HEADER));
	memcpy(pBuf,ptr,HeaderLength);
	p.pContent = pBuf;

	//将捕获到的数据包信息加入向量列表
	packets.push_back(p);

	InsertPacket(p);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//以太网分析回调函数
//////////////////////////////////////////////////////////////////////////
void EtherPacketCallback(u_char *argument,const struct pcap_pkthdr *packetHdr,const u_char *packetContent)
{
	//以太网协议变量
	ETHER_HEADER *EtherProtocol;

	//获得以太网协议内容
	EtherProtocol=(ETHER_HEADER *)(packetContent);

	//填充源MAC和目的MAC
	memcpy(p.SrcMac,EtherProtocol->EtherSrcHost,6);
	memcpy(p.DestMac,EtherProtocol->EtherDestHost,6);

	//IP协议分析
	IpPacketCallback(argument,packetHdr,packetContent);
}

//////////////////////////////////////////////////////////////////////////
//线程回调函数
//////////////////////////////////////////////////////////////////////////
BOOL Func(LPVOID p)
{
	ThreadArg *arg = (ThreadArg *)p;

	//注册回调函数，循环捕获数据包，每捕获到一个数据包就自动调用pFun()进行分析
	pcap_loop((pcap_t*)arg->pcapHandle,-1,(pcap_handler)arg->pFunc,NULL);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//开始嗅探
//////////////////////////////////////////////////////////////////////////
BOOL StartSniffer()
{
	//初始化wpcap
	InitWpcap();

	//数目清零
	PacketCount = 0;

	//清空数据包信息
	for (unsigned int i=0;i<packets.size();i++)
	{
		delete packets[i].pContent;
	}
	packets.clear();

	arg.pcapHandle = pcapHandle;
	arg.pFunc = EtherPacketCallback;

	//利用回调函数捕获数据包
	hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)Func,(LPVOID)&arg,NULL,NULL);
	if (hThread==NULL)
	{
		MessageBox(NULL,"嗅探失败",NULL,NULL);
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//停止嗅探
//////////////////////////////////////////////////////////////////////////
BOOL StopSniffer(pcap_t *pcapHandle)
{
	//结束嗅探线程
	if (TerminateThread(hThread,NULL)==FALSE)
	{
		MessageBox(NULL,"线程无法结束!",NULL,NULL);
		return FALSE;
	}
	pcap_close(pcapHandle);

	return TRUE;
}