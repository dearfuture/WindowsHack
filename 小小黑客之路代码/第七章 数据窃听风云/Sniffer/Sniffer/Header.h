//////////////////////////////////////////////////////////////////////////
//FileName:	Header.h
//Data:		2009-04-11
//Remark:	��Ŀ����ͷ�ļ���ȫ�ֱ�������������
//////////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////////////////////////////////////////////
//����ͷ�ļ�
//////////////////////////////////////////////////////////////////////////
#include <WinSock2.h>
#include <commctrl.h>
#include <stdlib.h>
#include <vector>
using std::vector;
//Winpcapͷ�ļ�
#include <pcap.h>
#include "remote-ext.h"
//��������Э������
#include "Protocol.h"

//////////////////////////////////////////////////////////////////////////
//�궨�岿��
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
//�ṹ����������
//////////////////////////////////////////////////////////////////////////
//IPЭ��ö������ 
enum Proto	{TCP=0,UDP};
//��Ҫ�����ݰ���Ϣ�ṹ��
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

//�̲߳����ṹ��
struct ThreadArg{
	LPVOID pcapHandle;
	LPVOID pFunc;	
};

//////////////////////////////////////////////////////////////////////////
//ȫ�ֱ�����������
//////////////////////////////////////////////////////////////////////////
//�߳̾��
extern HANDLE hThread;
//���ݰ��洢����
extern vector<Packet> packets;
//���ݰ��ݴ�ṹ��
extern Packet p;
//pcap_t���
extern pcap_t *pcapHandle;
//�̲߳���
extern ThreadArg arg;
//���ݰ�����
extern unsigned int PacketCount;


//////////////////////////////////////////////////////////////////////////
//������������
//////////////////////////////////////////////////////////////////////////
//��̫�������ص�����
void EtherPacketCallback(u_char *argument,const struct pcap_pkthdr *packetHdr,const u_char *packetContent);
//IP�����ص�����
BOOL IpPacketCallback(u_char *argument,const struct pcap_pkthdr *packetHdr,const u_char *packetContent);
//UDP�����ص�����
BOOL UdpPacketCallback(u_char *argument,const struct pcap_pkthdr *packetHdr,const u_char *packetContent);
//Tcp�����ص�����
BOOL TcpPacketCallback(u_char *argument,const struct pcap_pkthdr *packetHdr,const u_char *packetContent);
BOOL InsertPacket(Packet p);
//�̻߳ص�����
BOOL Func(LPVOID p);
//��ʼ��̽
BOOL StartSniffer();
//ֹͣ��̽
BOOL StopSniffer(pcap_t *pcapHandle);
//Winpcap��ʼ��
int InitWpcap(void);