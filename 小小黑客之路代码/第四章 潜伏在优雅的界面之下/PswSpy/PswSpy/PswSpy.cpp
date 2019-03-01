#include "resource.h"
#include <Windows.h>


//ȫ�ֹ��Ӿ��
HHOOK hHook = NULL;

//////////////////////////////////////////////////////////////////////////
//������������
//////////////////////////////////////////////////////////////////////////
//��깳�ӹ���
LRESULT  CALLBACK HookProc(int code,WPARAM wparam,LPARAM lparam);
//���ڹ���
int CALLBACK DlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );



//������
int APIENTRY WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd )
{
	return DialogBox(hInstance,MAKEINTRESOURCE(IDD_DIALOG),NULL,DlgProc);
}


//////////////////////////////////////////////////////////////////////////
//���ڹ���
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
//��깳�ӹ���
//////////////////////////////////////////////////////////////////////////
LRESULT  CALLBACK HookProc(int code,WPARAM wparam,LPARAM lparam)
{
	HWND  hwnd;
	POINT point;
	//������λ��
	GetCursorPos(&point);
	//�õ����Ĵ��ھ��
	hwnd = WindowFromPoint (point);
	//�õ����ڷ��
	long nStyle = GetWindowLong (hwnd,GWL_STYLE);
	EVENTMSG * event = (EVENTMSG * )lparam;
	//�Ƿ�Ϊ�������
	if(event->message == WM_LBUTTONDOWN)
	{
		//�Ƿ�Ϊ�����
		if(nStyle&ES_PASSWORD)
		{
			PostMessage (hwnd,EM_SETPASSWORDCHAR,0,0);
		}

	}

	return CallNextHookEx(hHook,code,wparam,lparam);
}
