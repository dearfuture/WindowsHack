//////////////////////////////////////////////////////////////////////////
//FileName:	ArpAttackDlg.cpp
//Data:		2009-04-21
//Remark：	UI部分核心代码
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "Header.h"
#include "resource.h"

#pragma comment(lib,"wpcap.lib")
#pragma comment(lib,"WS2_32.lib")
#pragma comment(lib,"comctl32.lib")
//////////////////////////////////////////////////////////////////////////
//UI相关变量声明部分
//////////////////////////////////////////////////////////////////////////
HWND hDlg = NULL;
BOOL Sending = FALSE;
HINSTANCE hInst = NULL;
//////////////////////////////////////////////////////////////////////////
//UI相关函数声明部分
//////////////////////////////////////////////////////////////////////////
//窗口过程回调函数
LRESULT CALLBACK DlgProc(HWND hDlgMain,UINT uMsg,WPARAM wParam,LPARAM lParam);

//////////////////////////////////////////////////////////////////////////
//UI主函数
//////////////////////////////////////////////////////////////////////////
int WINAPI	WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd )
{
	hInst = hInstance;
	InitCommonControls();
	//窗口函数
	DialogBox(hInstance,MAKEINTRESOURCE(IDD_DIALOG),NULL,(DLGPROC)DlgProc);
	return 0;
}


//////////////////////////////////////////////////////////////////////////
//窗口过程回调函数
//////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK DlgProc(HWND hDlgMain,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	hDlg = hDlgMain;
	switch (uMsg)
	{
	case WM_INITDIALOG:
		        //设置图标
     	SendMessageA(hDlg,WM_SETICON,ICON_SMALL, (LPARAM) LoadIcon(hInst,MAKEINTRESOURCE(IDI_ICON)));
		SendMessageA(hDlg,WM_SETICON, ICON_BIG,(LPARAM) LoadIcon(hInst,MAKEINTRESOURCE(IDI_ICON)));
		break;
	case WM_NOTIFY:
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON:
			if (!Sending)
			{
				Sending = TRUE;
				CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)SendArpPacket,NULL,0,NULL);
				SetDlgItemText(hDlg,IDC_BUTTON,L"停止发送");
			}
			else
			{
				Sending = FALSE;
				SetDlgItemText(hDlg,IDC_BUTTON,L"开始发送");
			}
			break;
		default:
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg,NULL);
		DestroyWindow(hDlg);
		break;
	default:
		break;
	}

	return 0;
}

