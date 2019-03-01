//////////////////////////////////////////////////////////////////////////
//FileName:	Sniffer.cpp
//Data:		2009-04-11
//Remark:	��̽�����ĺ���ʵ��
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "Header.h"

//////////////////////////////////////////////////////////////////////////
//ȫ�ֱ������岿��
//////////////////////////////////////////////////////////////////////////
//�߳̾��
HANDLE hThread;
//���ݰ��洢����
vector<Packet> packets;
//���ݰ��ݴ�ṹ��
Packet p;
//pcap_t���
pcap_t *pcapHandle;
//�̲߳���
ThreadArg arg;
//���ݰ�����
unsigned int PacketCount;


//////////////////////////////////////////////////////////////////////////
//���ĺ���ʵ��
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//��ʼ��wpcap
//////////////////////////////////////////////////////////////////////////
int InitWpcap(void)
{
	//�����豸ָ��
	pcap_if_t *pcapDev;
	//�洢������Ϣ
	char errContent[PCAP_ERRBUF_SIZE];
	//BPF���˹���
	bpf_program filter;
	//���˹����ַ���
	char bpfFilterString[] = "ip";
	//����
	bpf_u_int32	netMask;
	//�����ַ
	bpf_u_int32 netIp = 0;


	// ��������豸ָ��
	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &pcapDev, errContent) == -1)
	{
		MessageBoxA(NULL,"��ȡ�����豸ʧ��","����",MB_OK);
		return -1;
	}
	//ѡȡ��������
	while (pcapDev->addresses==NULL)
	{
		pcapDev = pcapDev->next;
	}

	//������
	if ((pcapHandle = pcap_open(pcapDev->name,65536,PCAP_OPENFLAG_PROMISCUOUS,1000,NULL,errContent)) == NULL )
	{
		MessageBoxA(NULL,"������ʧ��","����",MB_OK);
		return -1;
	}

	// �����·�㣬ֻ֧����̫��
	if (pcap_datalink(pcapHandle) != DLT_EN10MB)
	{
		MessageBoxA(NULL,"ֻ֧����̫��","����",MB_OK);
		pcap_freealldevs(pcapDev);
		return -1;
	}
	if (pcapDev->addresses != NULL)
	{
		//ȡ������ӿ���������
		netMask = ((struct sockaddr_in *)(pcapDev->addresses->netmask))->sin_addr.S_un.S_addr;
	}
	else
	{
		netMask = 0xFFFFFFFF;
	}

	//����BPF���˹���
	pcap_compile(pcapHandle,&filter,bpfFilterString,0,netIp);

	//���ù��˹���
	pcap_setfilter(pcapHandle,&filter);

	arg.pcapHandle = pcapHandle;

	return 0;
}

//////////////////////////////////////////////////////////////////////////
//IP�����ص�����
//////////////////////////////////////////////////////////////////////////
BOOL IpPacketCallback(u_char *argument,const struct pcap_pkthdr *packetHdr,const u_char *packetContent)
{
	//IPЭ�����
	IP_HEADER *IpProtocol;

	//���IPЭ������
	IpProtocol = (IP_HEADER *)(packetContent+sizeof(ETHER_HEADER));

	//���ԴIP��Ŀ��IP
	p.SrcIp = IpProtocol->SrcIp;
	p.DestIp = IpProtocol->DestIp;

	//����IPЭ������
	switch(IpProtocol->proto)
	{
	case IPPROTO_TCP:
		p.Protocol = TCP;
		//��һ��TCPЭ��
		TcpPacketCallback(argument,packetHdr,packetContent);
		break;
	case IPPROTO_UDP:
		p.Protocol = UDP;
		//��һ������UDPЭ��
		UdpPacketCallback(argument,packetHdr,packetContent);
		break;
	default :
		break;
	}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//TCP�����ص�����
//////////////////////////////////////////////////////////////////////////
BOOL TcpPacketCallback(u_char *argument,const struct pcap_pkthdr *packetHdr,const u_char *packetContent)
{
	//TCPЭ�����
	TCP_HEADER	*TcpProtocol;

	//���TCPЭ������
	TcpProtocol = (TCP_HEADER *)(packetContent+14+20);

	//�ֱ����TCPЭ���Դ�˿ڡ�Ŀ�Ķ˿ڼ����ݳ���
	p.SrcPort = TcpProtocol->SrcPort;
	p.DestPort = TcpProtocol->DestPort;
	//����TCP�������ݳ���
	IP_HEADER *IpHdr = (IP_HEADER *)(packetContent+14);
	int totallength = ntohs(IpHdr->TotalLen);
	int HeaderLength = totallength-40;
	p.Length = HeaderLength;

	//�����������
	char *pBuf = new char[HeaderLength+1];
	memset(pBuf,0,HeaderLength+1);
	char *ptr = (char *)((PBYTE)(TcpProtocol)+sizeof(TCP_HEADER));
	memcpy(pBuf,ptr,HeaderLength);

	p.pContent = pBuf;

	//�����񵽵����ݰ���Ϣ���������б�
	packets.push_back(p);

	InsertPacket(p);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//UDP�����ص�����
//////////////////////////////////////////////////////////////////////////
BOOL UdpPacketCallback(u_char *argument,const struct pcap_pkthdr *packetHdr,const u_char *packetContent)
{
	//UDP Э�����
	UDP_HEADER *UdpProtocol;

	//���UDPЭ������
	UdpProtocol = (UDP_HEADER *)(packetContent+14+20);

	//�ֱ����UDPЭ���Դ�˿ڡ�Ŀ�Ķ˿ڼ����ݳ���
	p.SrcPort = UdpProtocol->SrcPort;
	p.DestPort = UdpProtocol->DestPort;
	int	HeaderLength = ntohs(UdpProtocol->Length)-sizeof(UDP_HEADER);
	p.Length = HeaderLength;

	//�����������
	char *pBuf = new char[HeaderLength+1];
	memset(pBuf,0,HeaderLength+1);
	char *ptr = (char *)((PBYTE)(UdpProtocol)+sizeof(UDP_HEADER));
	memcpy(pBuf,ptr,HeaderLength);
	p.pContent = pBuf;

	//�����񵽵����ݰ���Ϣ���������б�
	packets.push_back(p);

	InsertPacket(p);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//��̫�������ص�����
//////////////////////////////////////////////////////////////////////////
void EtherPacketCallback(u_char *argument,const struct pcap_pkthdr *packetHdr,const u_char *packetContent)
{
	//��̫��Э�����
	ETHER_HEADER *EtherProtocol;

	//�����̫��Э������
	EtherProtocol=(ETHER_HEADER *)(packetContent);

	//���ԴMAC��Ŀ��MAC
	memcpy(p.SrcMac,EtherProtocol->EtherSrcHost,6);
	memcpy(p.DestMac,EtherProtocol->EtherDestHost,6);

	//IPЭ�����
	IpPacketCallback(argument,packetHdr,packetContent);
}

//////////////////////////////////////////////////////////////////////////
//�̻߳ص�����
//////////////////////////////////////////////////////////////////////////
BOOL Func(LPVOID p)
{
	ThreadArg *arg = (ThreadArg *)p;

	//ע��ص�������ѭ���������ݰ���ÿ����һ�����ݰ����Զ�����pFun()���з���
	pcap_loop((pcap_t*)arg->pcapHandle,-1,(pcap_handler)arg->pFunc,NULL);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//��ʼ��̽
//////////////////////////////////////////////////////////////////////////
BOOL StartSniffer()
{
	//��ʼ��wpcap
	InitWpcap();

	//��Ŀ����
	PacketCount = 0;

	//������ݰ���Ϣ
	for (unsigned int i=0;i<packets.size();i++)
	{
		delete packets[i].pContent;
	}
	packets.clear();

	arg.pcapHandle = pcapHandle;
	arg.pFunc = EtherPacketCallback;

	//���ûص������������ݰ�
	hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)Func,(LPVOID)&arg,NULL,NULL);
	if (hThread==NULL)
	{
		MessageBox(NULL,"��̽ʧ��",NULL,NULL);
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//ֹͣ��̽
//////////////////////////////////////////////////////////////////////////
BOOL StopSniffer(pcap_t *pcapHandle)
{
	//������̽�߳�
	if (TerminateThread(hThread,NULL)==FALSE)
	{
		MessageBox(NULL,"�߳��޷�����!",NULL,NULL);
		return FALSE;
	}
	pcap_close(pcapHandle);

	return TRUE;
}