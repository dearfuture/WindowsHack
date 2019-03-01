#include "resource.h"
#include <Windows.h>


//全局钩子句柄
HHOOK hHook = NULL;

//////////////////////////////////////////////////////////////////////////
//函数声明部分
//////////////////////////////////////////////////////////////////////////
//鼠标钩子过程
LRESULT  CALLBACK HookProc(int code,WPARAM wparam,LPARAM lparam);
//窗口过程
int CALLBACK DlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );



//主函数
int APIENTRY WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd )
{
	return DialogBox(hInstance,MAKEINTRESOURCE(IDD_DIALOG),NULL,DlgProc);
}


//////////////////////////////////////////////////////////////////////////
//窗口过程
//////////////////////////////////////////////////////////////////////////
int CALLBACK DlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( uMsg )
	{
	case WM_COMMAND:
		{
			switch ( LOWORD( wParam ) )
			{
			case IDCANCEL:
				{
					UnhookWindowsHookEx(hHook);
					EndDialog(hDlg,wParam);
				}
				break;
			case IDOK:
				{
					hHook = SetWindowsHookEx(WH_JOURNALRECORD,HookProc,GetModuleHandle(NULL),0);
				}
				break;
			}
		}
		break;
	case WM_CLOSE:
		{
			EndDialog( hDlg, 0 );
		}
		break;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
//鼠标钩子过程
//////////////////////////////////////////////////////////////////////////
LRESULT  CALLBACK HookProc(int code,WPARAM wparam,LPARAM lparam)
{
	HWND  hwnd;
	POINT point;
	//获得鼠标位置
	GetCursorPos(&point);
	//得到鼠标的窗口句柄
	hwnd = WindowFromPoint (point);
	//得到窗口风格　
	long nStyle = GetWindowLong (hwnd,GWL_STYLE);
	EVENTMSG * event = (EVENTMSG * )lparam;
	//是否为密码左键
	if(event->message == WM_LBUTTONDOWN)
	{
		//是否为密码框
		if(nStyle&ES_PASSWORD)
		{
			PostMessage (hwnd,EM_SETPASSWORDCHAR,0,0);
		}

	}

	return CallNextHookEx(hHook,code,wparam,lparam);
}
