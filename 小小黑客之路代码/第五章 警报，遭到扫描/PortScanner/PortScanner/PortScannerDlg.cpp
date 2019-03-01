//////////////////////////////////////////////////////////////////////////
//FileName:	PortScannerDlg.cpp
//Data:		2009-04-18
//Remark：	UI部分核心代码
//////////////////////////////////////////////////////////////////////////
#pragma once


#include "Header.h"
#include "resource.h"

#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"WS2_32.lib")


//////////////////////////////////////////////////////////////////////////
//全局变量
//////////////////////////////////////////////////////////////////////////

//List Box 控件句柄
HWND hList = NULL;
//主窗口句柄
HWND hDlg = NULL;

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

	//起止IP
	DWORD StartIp;
	DWORD EndIp;

	//起止端口
	DWORD StartPort;
	DWORD EndPort;

	//判断扫描状态
	static BOOL Flag = TRUE;

	switch (uMsg)
	{
		//初始化窗口消息
	case WM_INITDIALOG:
		//设置图标
		SendMessageA(hDlg,WM_SETICON,ICON_SMALL, (LPARAM) LoadIcon(hInst,MAKEINTRESOURCE(IDI_ICON)));
		SendMessageA(hDlg,WM_SETICON, ICON_BIG,(LPARAM) LoadIcon(hInst,MAKEINTRESOURCE(IDI_ICON)));
		break;
		//命令消息
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
			//“开始扫描”按钮被按下
		case IDC_BUTTON_START:
			if (Flag)
			{
				//清空List Box
				SendMessage(GetDlgItem(hDlg,IDC_LIST_RST),LB_RESETCONTENT,NULL,NULL);
				//获取起止IP
				SendMessage(GetDlgItem(hDlg,IDC_IP_STARTIP),IPM_GETADDRESS,0,(LPARAM)&StartIp);
				SendMessage(GetDlgItem(hDlg,IDC_IP_ENDIP),IPM_GETADDRESS,0,(LPARAM)&EndIp);
				//获取起止端口
				StartPort = GetDlgItemInt(hDlg,IDC_EDIT_STARTPORT,NULL,FALSE);
				EndPort = GetDlgItemInt(hDlg,IDC_EDIT_ENDPORT,NULL,FALSE);

				//开始扫描
				StartScanner(StartIp,EndIp,StartPort,EndPort);
			}
			break;
		default:
			break;
		}
		break;
		//关闭窗口消息
	case WM_CLOSE:
		EndDialog(hDlg,NULL);
		DestroyWindow(hDlg);
		break;
	default:
		break;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
//向List Box 中添加信息
//////////////////////////////////////////////////////////////////////////
BOOL InsertInfo(char *buff)
{
	SendMessage(GetDlgItem(hDlg,IDC_LIST_RST),LB_ADDSTRING,NULL,(LPARAM)buff);
	return TRUE;
}