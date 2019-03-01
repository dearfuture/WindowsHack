//////////////////////////////////////////////////////////////////////////
//FileName:	Protocol.h
//Data:		2009-04-10
//Remark:	���ļ��������ϸ��ձ�׼����ĸ�������Э��ṹ		
//////////////////////////////////////////////////////////////////////////
#pragma once
#include <WinSock2.h>

//��̫��Э��ṹ��
typedef struct ether_hdr{
	unsigned char	EtherDestHost[6];	//��̫��Ŀ��MAC��ַ
	unsigned char	EtherSrcHost[6];	//��̫��ԴMAC��ַ
	unsigned short	EtherType;			//��̫��Э������
}ETHER_HEADER;
//IPv4Э��ṹ��
typedef struct ip_hdr{
	unsigned char	Verlen;				//�汾
	unsigned char	Tos;				//��������
	unsigned short	TotalLen;			//����
	unsigned short	Ident;				//��ʶ
	unsigned short	Offset;				//ƫ��
	unsigned char	ttl;				//����ʱ��
	unsigned char	proto;				//Э������(����TCP��UDP��ICMP)
	unsigned short	checksum;			//У���
	in_addr			SrcIp;				//ԴIP��ַ
	in_addr			DestIp;				//Ŀ��IP��ַ
}IP_HEADER;
//TCPЭ��ṹ��
typedef struct tcp_hdr{
	unsigned short	SrcPort;			//Դ�˿�
	unsigned short	DestPort;			//Դ�˿�
	unsigned int	Seq;				//���к�
	unsigned int	Ack;				//ȷ�����к�
	unsigned char	Lenres;				//�ײ�����+����λ
	unsigned char	Flag;				//���
	unsigned short	Win;				//���ڴ�С
	unsigned short	Sum;				//У���
	unsigned short	Urp;				//����ָ��
}TCP_HEADER;
//UDPЭ��ṹ��
typedef struct udp_hdr{
	unsigned short	SrcPort;			//Դ�˿�
	unsigned short	DestPort;			//Ŀ�ĵ�ַ
	unsigned short	Length;				//���ݰ�����
	unsigned short	Checksum;			//У���
}UDP_HEADER;
//ARPЭ��ṹ��
typedef struct	arp_hdr{
	unsigned short	HardwareType;		//Ӳ������
	unsigned short	ProtocolType;		//Э������
	unsigned char	HardwareLength;		//Ӳ����ַ����
	unsigned char	ProtocolLength;		//Э���ַ����
	unsigned short	OperationCode;		//������
	unsigned char	EtherSrcHost[6];	//ԴMAC��ַ
	unsigned char	SrcIp[4];			//ԴIP��ַ
	unsigned char	EtherDestHost[6];	//Ŀ��MAC��ַ
	unsigned char	DestIp[4];			//Ŀ��IP��ַ
}ARP_HEADER;
