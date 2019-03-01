//////////////////////////////////////////////////////////////////////////
//FileName:	ArpAttackDlg.cpp
//Data:		2009-04-21
//Remark��	UI���ֺ��Ĵ���
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "Header.h"
#include "resource.h"

#pragma comment(lib,"wpcap.lib")
#pragma comment(lib,"WS2_32.lib")
#pragma comment(lib,"comctl32.lib")
//////////////////////////////////////////////////////////////////////////
//UI��ر�����������
//////////////////////////////////////////////////////////////////////////
HWND hDlg = NULL;
BOOL Sending = FALSE;
HINSTANCE hInst = NULL;
//////////////////////////////////////////////////////////////////////////
//UI��غ�����������
//////////////////////////////////////////////////////////////////////////
//���ڹ��̻ص�����
LRESULT CALLBACK DlgProc(HWND hDlgMain,UINT uMsg,WPARAM wParam,LPARAM lParam);

//////////////////////////////////////////////////////////////////////////
//UI������
//////////////////////////////////////////////////////////////////////////
int WINAPI	WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd )
{
	hInst = hInstance;
	InitCommonControls();
	//���ں���
	DialogBox(hInstance,MAKEINTRESOURCE(IDD_DIALOG),NULL,(DLGPROC)DlgProc);
	return 0;
}


//////////////////////////////////////////////////////////////////////////
//���ڹ��̻ص�����
//////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK DlgProc(HWND hDlgMain,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	hDlg = hDlgMain;
	switch (uMsg)
	{
	case WM_INITDIALOG:
		        //����ͼ��
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
				SetDlgItemText(hDlg,IDC_BUTTON,L"ֹͣ����");
			}
			else
			{
				Sending = FALSE;
				SetDlgItemText(hDlg,IDC_BUTTON,L"��ʼ����");
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

