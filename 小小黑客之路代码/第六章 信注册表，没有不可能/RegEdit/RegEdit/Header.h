#pragma once

//////////////////////////////////////////////////////////////////////////
//包含头文件
//////////////////////////////////////////////////////////////////////////
#include <Windows.h>
#include <commctrl.h>

extern HWND hDlg;
extern HKEY hKey ;
extern HKEY hBaseKey;


BOOL EnumKey();

BOOL EnumValue();

BYTE wChar16Toi(WCHAR *str);

BOOL SaveValue();

BOOL ReadValue();

BOOL DeletValue();

BOOL DeletKey();

BOOL AddKey();

BOOL OpenKey();

BOOL wStr16ToByte(WCHAR *Data,BYTE *bData);