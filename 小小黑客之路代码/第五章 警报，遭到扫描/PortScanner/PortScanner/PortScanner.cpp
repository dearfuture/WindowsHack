//////////////////////////////////////////////////////////////////////////
//FileName:	PortScanner.cpp
//Data:		2009-04-18
//Remark��	ɨ����Ĵ���
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "Header.h"

#include "resource.h"
//////////////////////////////////////////////////////////////////////////
//�˿���ɨ���ʼ��
//////////////////////////////////////////////////////////////////////////
BOOL InitPortScan()
{
	WSADATA WsaData;

	//����socket�汾��Ϣ
	WORD WsaVersion=MAKEWORD(2,2);

	//��ʼ������
	if(WSAStartup(WsaVersion,&WsaData)!=0)
	{
		MessageBoxA(NULL,"WSAStartup fail;",NULL,NULL);
		return FALSE;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//connect�̺߳���
//////////////////////////////////////////////////////////////////////////
DWORD WINAPI PortScanthread(LPVOID LpParam)
{
	ThreadParam Param;
	//����������
	MoveMemory(&Param,LpParam,sizeof(Param));
	//��hCopyOkEvent��Ϊ���ź�״̬��֪ͨɨ�����߳̽�����һ��ѭ��
	SetEvent(Param.hCopyOkEvent);

	SOCKET Sock;
	SOCKADDR_IN SockAddr = {0};

		//����socket
	Sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (Sock==INVALID_SOCKET)
	{
		MessageBoxA(NULL,"INVALID_SOCKET",NULL,NULL);
	}
	
	//���IP��ַ���˿���Ϣ
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_addr.s_addr = htonl(Param.Ip);
	SockAddr.sin_port = htons(Param.Port);

	//��IP��ַת��Ϊ�ַ���
	char *IpChar = inet_ntoa(SockAddr.sin_addr);

	char str[200];

	if(connect(Sock,(SOCKADDR *)&SockAddr,sizeof(SockAddr))==0)
	{
		//���ӳɹ���
		sprintf(str,"%s : %d ���ӳɹ�\n",IpChar,Param.Port);
	}
	else
	{
		//����ʧ��
		sprintf(str,"%s : %d ����ʧ��\n",IpChar,Param.Port);
	}

	//�����ʾ��Ϣ
	InsertInfo(str);

	//�ͷ�һ���ź�������
	ReleaseSemaphore(Param.hThreadNum,1,NULL);

	//�ر�socket
	closesocket(Sock);

	return 0;
}


//////////////////////////////////////////////////////////////////////////
//ɨ�����߳�
//////////////////////////////////////////////////////////////////////////
DWORD WINAPI MainThread(LPVOID LpParam)
{

	MainThreadParam Param;
	//����������
	MoveMemory(&Param,LpParam,sizeof(Param));
	//��Param.hCopyEvent����Ϊ���ź�״̬
	SetEvent(Param.hCopyEvent);

	ThreadParam threadparam = {0};

	//�������̵߳ġ�����������ɡ��¼����󣬲���Ϊ��������PortScanthread()
	HANDLE hThreadCopyOkEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	threadparam.hCopyOkEvent=hThreadCopyOkEvent;

	//����һ���ź����������������̵߳�������PortScanthread()
	HANDLE hThreadNum = CreateSemaphore(NULL,256,256,NULL);
	threadparam.hThreadNum = hThreadNum;


	//ѭ��connect
	for (DWORD Ip = Param.StartIp;Ip<=Param.EndIp;Ip++)
	{
		for (DWORD Port = Param.StartPort;Port<=Param.EndPort;Port++)
		{

			//�ȴ�hThreadNum�����źţ���ʾ�п����̣߳�
			DWORD WaitRes =WaitForSingleObject(hThreadNum,200);

			if (WaitRes==WAIT_OBJECT_0)
			{
				threadparam.Ip=Ip;
				threadparam.Port=Port;

				CreateThread(NULL,0,PortScanthread,&threadparam,0,NULL);

				//�ȴ������̷߳���������������ϡ����ź�
				WaitForSingleObject(threadparam.hCopyOkEvent,INFINITE);
				//����threadparam.hCopyOkEventΪ���ź�״̬
				ResetEvent(threadparam.hCopyOkEvent);

			}
			else if(WaitRes==WAIT_TIMEOUT)
			{
				Port--;
				continue;
			}
		}
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
//��ʼɨ��
//////////////////////////////////////////////////////////////////////////
BOOL StartScanner(DWORD StartIp,DWORD EndIp,DWORD StartPort,DWORD EndPort)
{
	//��ʼ���˿�ɨ����
	InitPortScan();

	//��ɨ���̲߳���
	MainThreadParam param;
	//��ɨ���̵߳ġ�����������ϡ��¼�����
	HANDLE hMainCopyEvent = CreateEvent(NULL,TRUE,FALSE,NULL);

	//�������ṹ��
	param.hCopyEvent = hMainCopyEvent;
	param.StartIp = StartIp;
	param.EndIp = EndIp;
	param.StartPort = StartPort;
	param.EndPort = EndPort;

	//������ɨ���̲߳���
	CreateThread(NULL,0,MainThread,(LPVOID *)&param,0,NULL);

	//�ȴ�hMainCopyEvent��Ϊ���ź�״̬
	WaitForSingleObject(hMainCopyEvent,INFINITE);
	//����hMainCopyEventΪ���ź�״̬
	ResetEvent(hMainCopyEvent);
	return TRUE;
}


//DWORD WINAPI SynRecvThread(LPVOID Param)
//{
//	MainThreadParam RecvParam = *((MainThreadParam *)(Param));
//	
//
//	SOCKET Sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
//
//	SOCKADDR_IN Addr;
//
//	Addr.sin_family = AF_INET;
//
//	Addr.sin_port = INADDR_ANY;
//
//	Addr.sin_addr.s_addr = inet_addr("10.253.113.46");
//	
//	if(bind(&Sock,&Addr,sizeof(Addr))!=0)
//	{
//		closesocket(Sock);
//		MessageBoxA(NULL,"bind failed",NULL,NULL);
//		return -1;
//	}
//	
//	DWORD dwIn = 1,dwRet;
//
//	if (WSAIoctl(Sock,SIO_RCVALL,&dwIn,sizeof(dwIn),NULL,0,&dwRet,NULL,NULL))
//	{
//		closesocket(Sock);
//		MessageBoxA(NULL,"set WSA failed",NULL,NULL);
//		return -1;
//	}
//
//	char RecvBuff[256];
//
//	SetEvent(RecvParam.hCopyEvent);		//??????????
//
//	while (WaitForSingleObject(RecvParam.hRecvStopEvent,0)==WAIT_TIMEOUT)
//	{
//		if (recv(Sock,RecvBuff,sizeof(RecvBuff),0)<=0)
//		{
//			continue;
//		}
//
//		IP_HEADER *pIpHeader = (IP_HEADER *)RecvBuff;
//		if (inet_ntoa() < RecvParam.StartIp||htonl(pIpHeader->SrcIp)>RecvParam.EndIp)
//		{
//			continue;
//		}
//
//		TCP_HEADER *pTcpHeader = (TCP_HEADER *)(pIpHeader+1);
//		if (htons(pTcpHeader->SrcPort)<RecvParam.StartPort||htons(pTcpHeader->SrcPort)>RecvParam.EndPort)
//		{
//			continue;
//		}
//
//		in_addr ShowIp;
//		ShowIp = pIpHeader->SrcIp;
//		char str[256] = {0};
//
//		sprintf(str,"%s : %d �˿ڿ���",ShowIp,pTcpHeader->SrcPort);
//		
//		cout<<str<<endl;
//
//	}
//
//	closesocket(Sock);
//
//	return 0;
//}


