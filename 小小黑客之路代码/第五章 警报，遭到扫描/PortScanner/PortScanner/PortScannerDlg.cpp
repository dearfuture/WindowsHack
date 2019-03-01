//////////////////////////////////////////////////////////////////////////
//FileName:	PortScannerDlg.cpp
//Data:		2009-04-18
//Remark��	UI���ֺ��Ĵ���
//////////////////////////////////////////////////////////////////////////
#pragma once


#include "Header.h"
#include "resource.h"

#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"WS2_32.lib")


//////////////////////////////////////////////////////////////////////////
//ȫ�ֱ���
//////////////////////////////////////////////////////////////////////////

//List Box �ؼ����
HWND hList = NULL;
//�����ھ��
HWND hDlg = NULL;

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

	//��ֹIP
	DWORD StartIp;
	DWORD EndIp;

	//��ֹ�˿�
	DWORD StartPort;
	DWORD EndPort;

	//�ж�ɨ��״̬
	static BOOL Flag = TRUE;

	switch (uMsg)
	{
		//��ʼ��������Ϣ
	case WM_INITDIALOG:
		//����ͼ��
		SendMessageA(hDlg,WM_SETICON,ICON_SMALL, (LPARAM) LoadIcon(hInst,MAKEINTRESOURCE(IDI_ICON)));
		SendMessageA(hDlg,WM_SETICON, ICON_BIG,(LPARAM) LoadIcon(hInst,MAKEINTRESOURCE(IDI_ICON)));
		break;
		//������Ϣ
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
			//����ʼɨ�衱��ť������
		case IDC_BUTTON_START:
			if (Flag)
			{
				//���List Box
				SendMessage(GetDlgItem(hDlg,IDC_LIST_RST),LB_RESETCONTENT,NULL,NULL);
				//��ȡ��ֹIP
				SendMessage(GetDlgItem(hDlg,IDC_IP_STARTIP),IPM_GETADDRESS,0,(LPARAM)&StartIp);
				SendMessage(GetDlgItem(hDlg,IDC_IP_ENDIP),IPM_GETADDRESS,0,(LPARAM)&EndIp);
				//��ȡ��ֹ�˿�
				StartPort = GetDlgItemInt(hDlg,IDC_EDIT_STARTPORT,NULL,FALSE);
				EndPort = GetDlgItemInt(hDlg,IDC_EDIT_ENDPORT,NULL,FALSE);

				//��ʼɨ��
				StartScanner(StartIp,EndIp,StartPort,EndPort);
			}
			break;
		default:
			break;
		}
		break;
		//�رմ�����Ϣ
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
//��List Box �������Ϣ
//////////////////////////////////////////////////////////////////////////
BOOL InsertInfo(char *buff)
{
	SendMessage(GetDlgItem(hDlg,IDC_LIST_RST),LB_ADDSTRING,NULL,(LPARAM)buff);
	return TRUE;
}