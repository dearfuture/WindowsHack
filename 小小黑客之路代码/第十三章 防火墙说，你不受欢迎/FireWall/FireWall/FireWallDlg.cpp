#include "Header.h"

#define WM_NOTIFY_ICON WM_USER+100
#define IDC_BUTTON_EXIT WM_USER+101
HWND hDlg = NULL;
HWND hPortDlg = NULL;
HWND hIpDlg = NULL;
HWND hPathDlg = NULL;

HINSTANCE hInsTance = NULL;
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
	//窗口函数
	hInsTance = hInstance;
	InitCommonControls();
	DialogBox(hInstance,MAKEINTRESOURCE(IDD_DIALOG),NULL,(DLGPROC)DlgProc);

	return 0;
}

LRESULT CALLBACK PortDlgProc(HWND hDlgMain,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	hPortDlg = hDlgMain;
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
		LVCOLUMN LvCol;
		HWND hList = GetDlgItem(hDlgMain,IDC_LIST_PORT);
		memset(&LvCol,0,sizeof(LvCol));									
		LvCol.mask= LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM ;
		LvCol.fmt=  LVCFMT_LEFT;
		LvCol.cx= 50;
		//初始化列表
		LvCol.pszText= L"端口号";
		SendMessage(hList,LVM_INSERTCOLUMN,0,(LPARAM)&LvCol);
		LvCol.pszText=L"放行/禁止";
		LvCol.cx= 70;
		SendMessage(hList,LVM_INSERTCOLUMN,1,(LPARAM)&LvCol);
		ListView_SetExtendedListViewStyleEx(GetDlgItem(hPortDlg,IDC_LIST_PORT),0 , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP);
		ShowPortList();
		}
		break;
	case WM_NOTIFY:
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_ADDPORT:
			AddPort();
			break;
		case IDC_BUTTON_SHOWPORTLIST:
			ShowPortList();
			break;
		case IDC_BUTTON_DELETPORT:
			DeletPort();
			break;
		default:
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hPortDlg,NULL);
		DestroyWindow(hPortDlg);
		break;
	default:
		break;
	}
	return 0;
}


LRESULT CALLBACK IpDlgProc(HWND hDlgMain,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	hIpDlg = hDlgMain;
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			LVCOLUMN LvCol;
			HWND hList = GetDlgItem(hIpDlg,IDC_LIST_IP);
			memset(&LvCol,0,sizeof(LvCol));									
			LvCol.mask= LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM ;
			LvCol.fmt=  LVCFMT_LEFT;
			LvCol.cx= 120;
			//初始化列表
			LvCol.pszText= L"IP";
			SendMessage(hList,LVM_INSERTCOLUMN,0,(LPARAM)&LvCol);
			LvCol.pszText=L"放行/禁止";
			LvCol.cx= 70;
			SendMessage(hList,LVM_INSERTCOLUMN,1,(LPARAM)&LvCol);
			ListView_SetExtendedListViewStyleEx(GetDlgItem(hIpDlg,IDC_LIST_IP),0 , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP);
			ShowIpList();
		}
		break;
	case WM_NOTIFY:
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_ADDIP:
			AddIp();
			break;
		case IDC_BUTTON_SHOWIPLIST:
			ShowIpList();
			break;
		case IDC_BUTTON_DELETIP:
			DeletIp();
			break;
		default:
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hIpDlg,NULL);
		DestroyWindow(hIpDlg);
		break;
	default:
		break;
	}
	return 0;
}

LRESULT CALLBACK PathDlgProc(HWND hDlgMain,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	hPathDlg = hDlgMain;

	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			LVCOLUMN LvCol;
			HWND hList = GetDlgItem(hPathDlg,IDC_LIST_PATH);
			memset(&LvCol,0,sizeof(LvCol));									
			LvCol.mask= LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM ;
			LvCol.fmt=  LVCFMT_LEFT;
			LvCol.cx= 400;
			//初始化列表
			LvCol.pszText= L"程序路径";
			SendMessage(hList,LVM_INSERTCOLUMN,0,(LPARAM)&LvCol);
			LvCol.pszText=L"放行/禁止";
			LvCol.cx= 70;
			SendMessage(hList,LVM_INSERTCOLUMN,1,(LPARAM)&LvCol);
			ListView_SetExtendedListViewStyleEx(GetDlgItem(hPathDlg,IDC_LIST_PATH),0 , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP);
			ShowPathList();
		}
		break;
	case WM_NOTIFY:
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_BROWSE:
			{
				WCHAR FileBuff[MAX_PATH] = {0};
				OPENFILENAME ofn;
				memset(&ofn,0,sizeof(OPENFILENAME));
				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.hwndOwner = hDlg;
				ofn.lpstrFile = FileBuff;
				ofn.nMaxFile = MAX_PATH;
				ofn.lpstrFilter = L"应用程序(*.exe)\0*.exe\0All(*.*)\0*.*\0";
				ofn.lpstrTitle = L"打开文件";
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = NULL;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = NULL;
				if(GetOpenFileName(&ofn))
				{
					SetDlgItemText(hPathDlg,IDC_EDIT_ADDPATH,FileBuff);
				}
			}
			break;
		case IDC_BUTTON_ADDPATH:
			AddPath();
			break;
		case IDC_BUTTON_SHOWPATHLIST:
			ShowPathList();
			break;
		case IDC_BUTTON_DELETPATH:
			DeletPath();
			break;
		default:
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hPathDlg,NULL);
		DestroyWindow(hPathDlg);
		break;
	default:
		break;
	}

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
		break;
	case WM_PAINT:
		{
			NOTIFYICONDATA IconData;
			IconData.hWnd = hDlgMain;
			IconData.uID = IDI_ICON_NOTIFY;
			IconData.hIcon = LoadIcon(hInsTance,MAKEINTRESOURCE(IDI_ICON_NOTIFY));
			IconData.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
			IconData.uFlags = NIF_MESSAGE|NIF_ICON;
			IconData.uCallbackMessage = WM_NOTIFY_ICON;
			Shell_NotifyIcon(NIM_ADD,&IconData);
			ShowWindow(hDlgMain,SW_HIDE);
		}
		break;
	case WM_NOTIFY:
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_INSTALL:
			InstallProviders();
			break;
		case IDC_BUTTON_REMOVE:
			RemoveProviders();
			break;
		case IDC_BUTTON_SHOWPORTDLG:
			DialogBox(hInsTance,MAKEINTRESOURCE(IDD_DIALOG_PORT),hDlg,(DLGPROC)PortDlgProc);
			break;
		case IDC_BUTTON_SHOWIPDLG:
			DialogBox(hInsTance,MAKEINTRESOURCE(IDD_DIALOG_IP),hDlg,(DLGPROC)IpDlgProc);
			break;
		case IDC_BUTTON_SHOWPATHDLG:
			DialogBox(hInsTance,MAKEINTRESOURCE(IDD_DIALOG_PATH),hDlg,(DLGPROC)PathDlgProc);
			break;
		case IDC_BUTTON_NOTIFY:
			{
				NOTIFYICONDATA IconData;
				IconData.hWnd = hDlgMain;
				IconData.uID = IDI_ICON_NOTIFY;
				IconData.hIcon = LoadIcon(hInsTance,MAKEINTRESOURCE(IDI_ICON_NOTIFY));
				IconData.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
				IconData.uFlags = NIF_MESSAGE|NIF_ICON;
				IconData.uCallbackMessage = WM_NOTIFY_ICON;
				Shell_NotifyIcon(NIM_ADD,&IconData);
				ShowWindow(hDlgMain,SW_HIDE);
			}
			break;
		case IDC_BUTTON_EXIT:
			{
				NOTIFYICONDATA IconData;
				IconData.hWnd = hDlgMain;
				IconData.uID = IDI_ICON_NOTIFY;
				IconData.hIcon = LoadIcon(hInsTance,MAKEINTRESOURCE(IDI_ICON_NOTIFY));
				IconData.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
				IconData.uFlags = NIF_MESSAGE|NIF_ICON;
				IconData.uCallbackMessage = WM_NOTIFY_ICON;
				Shell_NotifyIcon(NIM_DELETE,&IconData);
				RemoveProviders();
				EndDialog(hDlg,NULL);
				DestroyWindow(hDlg);
			}
			break;
		default:
			break;
		}
		break;
	case WM_NOTIFY_ICON:
		switch (lParam)
		{
		case WM_RBUTTONUP:
			{
				tagPOINT Point;
				GetCursorPos(&Point);
				HMENU hMenu;
				hMenu = CreatePopupMenu();
				AppendMenu(hMenu,MF_STRING,IDC_BUTTON_SHOWPORTDLG,L"显示端口列表");
				AppendMenu(hMenu,MF_STRING,IDC_BUTTON_SHOWIPDLG,L"显示IP列表");
				AppendMenu(hMenu,MF_STRING,IDC_BUTTON_SHOWPATHDLG,L"显示应用程序列表");
				AppendMenu(hMenu,MF_SEPARATOR,0,NULL);
				AppendMenu(hMenu,MF_STRING,IDC_BUTTON_INSTALL,L"开启保护");
				AppendMenu(hMenu,MF_STRING,IDC_BUTTON_REMOVE,L"关闭保护");
				AppendMenu(hMenu,MF_SEPARATOR,0,NULL);
				AppendMenu(hMenu,MF_STRING,0,L"关闭菜单");
				AppendMenu(hMenu,MF_STRING,IDC_BUTTON_EXIT,L"退出");
				TrackPopupMenu(hMenu,TPM_LEFTALIGN,Point.x,Point.y,NULL,hDlgMain,NULL);
				
			}
			break;
		case WM_LBUTTONDBLCLK:
			ShowWindow(hDlgMain,SW_SHOW);
			break;
		}
		break;
	case WM_CLOSE:
		RemoveProviders();
		EndDialog(hDlg,NULL);
		DestroyWindow(hDlg);
		break;
	default:
		break;
	}
	return 0;
}