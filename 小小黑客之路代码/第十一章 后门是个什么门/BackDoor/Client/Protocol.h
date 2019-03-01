//////////////////////////////////////////////////////////////////////////
//FileName:	Protocol.h
//Data:		2009-04-10
//Remark:	该文件包含了严格按照标准定义的各种网络协议结构		
//////////////////////////////////////////////////////////////////////////
#pragma once
#include <WinSock2.h>

//以太网协议结构体
typedef struct ether_hdr{
	unsigned char	EtherDestHost[6];	//以太网目的MAC地址
	unsigned char	EtherSrcHost[6];	//以太网源MAC地址
	unsigned short	EtherType;			//以太网协议类型
}ETHER_HEADER;
//IPv4协议结构体
typedef struct ip_hdr{
	unsigned char	Verlen;				//版本
	unsigned char	Tos;				//服务质量
	unsigned short	TotalLen;			//长度
	unsigned short	Ident;				//标识
	unsigned short	Offset;				//偏移
	unsigned char	ttl;				//生存时间
	unsigned char	proto;				//协议类型(包含TCP、UDP、ICMP)
	unsigned short	checksum;			//校验和
	in_addr			SrcIp;				//源IP地址
	in_addr			DestIp;				//目的IP地址
}IP_HEADER;
//TCP协议结构体
typedef struct tcp_hdr{
	unsigned short	SrcPort;			//源端口
	unsigned short	DestPort;			//源端口
	unsigned int	Seq;				//序列号
	unsigned int	Ack;				//确认序列号
	unsigned char	Lenres;				//首部长度+保留位
	unsigned char	Flag;				//标记
	unsigned short	Win;				//窗口大小
	unsigned short	Sum;				//校验和
	unsigned short	Urp;				//紧急指针
}TCP_HEADER;
//UDP协议结构体
typedef struct udp_hdr{
	unsigned short	SrcPort;			//源端口
	unsigned short	DestPort;			//目的地址
	unsigned short	Length;				//数据包长度
	unsigned short	Checksum;			//校验和
}UDP_HEADER;
//ARP协议结构体
typedef struct	arp_hdr{
	unsigned short	HardwareType;		//硬件类型
	unsigned short	ProtocolType;		//协议类型
	unsigned char	HardwareLength;		//硬件地址长度
	unsigned char	ProtocolLength;		//协议地址长度
	unsigned short	OperationCode;		//操作码
	unsigned char	EtherSrcHost[6];	//源MAC地址
	unsigned char	SrcIp[4];			//源IP地址
	unsigned char	EtherDestHost[6];	//目的MAC地址
	unsigned char	DestIp[4];			//目的IP地址
}ARP_HEADER;
