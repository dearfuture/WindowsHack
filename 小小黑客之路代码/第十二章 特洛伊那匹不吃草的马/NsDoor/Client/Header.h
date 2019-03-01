//////////////////////////////////////////////////////////////////////////
//File:	Header.h
//Rem:	包含所有系统头文件
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//头文件包含
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

//全局收发缓冲区
#define MAXLENGTH  1024*10
extern char szSendCmd[MAXLENGTH];
extern char szRecvCmd[MAXLENGTH];
extern int  dwSendSize;
extern int  dwRecvSize;
extern int	dwSelected;

