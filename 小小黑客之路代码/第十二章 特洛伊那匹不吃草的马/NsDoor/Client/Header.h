//////////////////////////////////////////////////////////////////////////
//File:	Header.h
//Rem:	��������ϵͳͷ�ļ�
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//ͷ�ļ�����
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "resource.h"
#include "RatProto.h"
#include <stdio.h>
#include <WinSock2.h>
#include <Windows.h>
#include <commctrl.h>
#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"WS2_32.lib")




#define ID_STATUSBAR	1
extern HWND hSBarDlg;
extern HWND hDlgMain;
extern SOCKET sockfd;

//ȫ���շ�������
#define MAXLENGTH  1024*10
extern char szSendCmd[MAXLENGTH];
extern char szRecvCmd[MAXLENGTH];
extern int  dwSendSize;
extern int  dwRecvSize;
extern int	dwSelected;

