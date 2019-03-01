//////////////////////////////////////////////////////////////////////////
//FileName:	PortScanner.cpp
//Data:		2009-04-18
//Remark：	扫描核心代码
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "Header.h"

#include "resource.h"
//////////////////////////////////////////////////////////////////////////
//端口器扫描初始化
//////////////////////////////////////////////////////////////////////////
BOOL InitPortScan()
{
	WSADATA WsaData;

	//构建socket版本信息
	WORD WsaVersion=MAKEWORD(2,2);

	//初始化网络
	if(WSAStartup(WsaVersion,&WsaData)!=0)
	{
		MessageBoxA(NULL,"WSAStartup fail;",NULL,NULL);
		return FALSE;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//connect线程函数
//////////////////////////////////////////////////////////////////////////
DWORD WINAPI PortScanthread(LPVOID LpParam)
{
	ThreadParam Param;
	//将参数复制
	MoveMemory(&Param,LpParam,sizeof(Param));
	//将hCopyOkEvent设为有信号状态来通知扫描主线程进行下一次循环
	SetEvent(Param.hCopyOkEvent);

	SOCKET Sock;
	SOCKADDR_IN SockAddr = {0};

		//创建socket
	Sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (Sock==INVALID_SOCKET)
	{
		MessageBoxA(NULL,"INVALID_SOCKET",NULL,NULL);
	}
	
	//填充IP地址及端口信息
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_addr.s_addr = htonl(Param.Ip);
	SockAddr.sin_port = htons(Param.Port);

	//将IP地址转换为字符串
	char *IpChar = inet_ntoa(SockAddr.sin_addr);

	char str[200];

	if(connect(Sock,(SOCKADDR *)&SockAddr,sizeof(SockAddr))==0)
	{
		//连接成功，
		sprintf(str,"%s : %d 连接成功\n",IpChar,Param.Port);
	}
	else
	{
		//连接失败
		sprintf(str,"%s : %d 连接失败\n",IpChar,Param.Port);
	}

	//添加显示信息
	InsertInfo(str);

	//释放一个信号量计数
	ReleaseSemaphore(Param.hThreadNum,1,NULL);

	//关闭socket
	closesocket(Sock);

	return 0;
}


//////////////////////////////////////////////////////////////////////////
//扫描主线程
//////////////////////////////////////////////////////////////////////////
DWORD WINAPI MainThread(LPVOID LpParam)
{

	MainThreadParam Param;
	//将参数复制
	MoveMemory(&Param,LpParam,sizeof(Param));
	//将Param.hCopyEvent设置为有信号状态
	SetEvent(Param.hCopyEvent);

	ThreadParam threadparam = {0};

	//创建子线程的“参数复制完成”事件对象，并作为参数传入PortScanthread()
	HANDLE hThreadCopyOkEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	threadparam.hCopyOkEvent=hThreadCopyOkEvent;

	//创建一个信号量对象来控制子线程的总数量PortScanthread()
	HANDLE hThreadNum = CreateSemaphore(NULL,256,256,NULL);
	threadparam.hThreadNum = hThreadNum;


	//循环connect
	for (DWORD Ip = Param.StartIp;Ip<=Param.EndIp;Ip++)
	{
		for (DWORD Port = Param.StartPort;Port<=Param.EndPort;Port++)
		{

			//等待hThreadNum发出信号（表示有可有线程）
			DWORD WaitRes =WaitForSingleObject(hThreadNum,200);

			if (WaitRes==WAIT_OBJECT_0)
			{
				threadparam.Ip=Ip;
				threadparam.Port=Port;

				CreateThread(NULL,0,PortScanthread,&threadparam,0,NULL);

				//等待其子线程发出“参数复制完毕”的信号
				WaitForSingleObject(threadparam.hCopyOkEvent,INFINITE);
				//重置threadparam.hCopyOkEvent为无信号状态
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
//开始扫描
//////////////////////////////////////////////////////////////////////////
BOOL StartScanner(DWORD StartIp,DWORD EndIp,DWORD StartPort,DWORD EndPort)
{
	//初始化端口扫描器
	InitPortScan();

	//主扫描线程参数
	MainThreadParam param;
	//主扫描线程的“参数复制完毕”事件变量
	HANDLE hMainCopyEvent = CreateEvent(NULL,TRUE,FALSE,NULL);

	//填充参数结构体
	param.hCopyEvent = hMainCopyEvent;
	param.StartIp = StartIp;
	param.EndIp = EndIp;
	param.StartPort = StartPort;
	param.EndPort = EndPort;

	//创建主扫描线程参数
	CreateThread(NULL,0,MainThread,(LPVOID *)&param,0,NULL);

	//等待hMainCopyEvent变为有信号状态
	WaitForSingleObject(hMainCopyEvent,INFINITE);
	//重置hMainCopyEvent为无信号状态
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
//		sprintf(str,"%s : %d 端口开放",ShowIp,pTcpHeader->SrcPort);
//		
//		cout<<str<<endl;
//
//	}
//
//	closesocket(Sock);
//
//	return 0;
//}


