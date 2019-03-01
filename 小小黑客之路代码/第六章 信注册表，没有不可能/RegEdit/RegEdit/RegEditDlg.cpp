//////////////////////////////////////////////////////////////////////////
//FileName:	SnifferDlg.cpp
//Data:		2009-04-11
//Remark��	UI���ֺ��Ĵ���
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "Header.h"
#include "resource.h"


#pragma comment(lib,"comctl32.lib")
//////////////////////////////////////////////////////////////////////////
//UI��ر�����������
//////////////////////////////////////////////////////////////////////////
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
	switch (uMsg)
	{
	case WM_INITDIALOG:
		//����ͼ��
		SendMessageA(hDlg,WM_SETICON,ICON_SMALL, (LPARAM) LoadIcon(hInst,MAKEINTRESOURCE(IDI_ICON)));
		SendMessageA(hDlg,WM_SETICON, ICON_BIG,(LPARAM) LoadIcon(hInst,MAKEINTRESOURCE(IDI_ICON)));

		//��ʼ��IDC_COMBO_RKEY
		SendMessage(GetDlgItem(hDlg,IDC_COMBO_RKEY),CB_INSERTSTRING,0,(LPARAM)L"HKEY_USERS");
		SendMessage(GetDlgItem(hDlg,IDC_COMBO_RKEY),CB_INSERTSTRING,0,(LPARAM)L"HKEY_CURRENT_CONFIG");
		SendMessage(GetDlgItem(hDlg,IDC_COMBO_RKEY),CB_INSERTSTRING,0,(LPARAM)L"HKEY_LOCAL_MACHINE");
		SendMessage(GetDlgItem(hDlg,IDC_COMBO_RKEY),CB_INSERTSTRING,0,(LPARAM)L"HKEY_CURRENT_USER");
		SendMessage(GetDlgItem(hDlg,IDC_COMBO_RKEY),CB_INSERTSTRING,0,(LPARAM)L"HKEY_CLASSES_ROOT");

		//��ʼ��IDC_COMBO_VLUTYPE
		SendMessage(GetDlgItem(hDlg,IDC_COMBO_VLUTYPE),CB_INSERTSTRING,0,(LPARAM)L"REG_SZ");
		SendMessage(GetDlgItem(hDlg,IDC_COMBO_VLUTYPE),CB_INSERTSTRING,0,(LPARAM)L"REG_EXPAND_SZ");
		SendMessage(GetDlgItem(hDlg,IDC_COMBO_VLUTYPE),CB_INSERTSTRING,0,(LPARAM)L"REG_MULTI_SZ");
		SendMessage(GetDlgItem(hDlg,IDC_COMBO_VLUTYPE),CB_INSERTSTRING,0,(LPARAM)L"REG_BINARY");
		SendMessage(GetDlgItem(hDlg,IDC_COMBO_VLUTYPE),CB_INSERTSTRING,0,(LPARAM)L"REG_DWORD");
		break;
	case WM_NOTIFY:
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_LIST_KEY:
			switch (HIWORD(wParam))
			{
			//IDC_LIST_KEY��˫��
			case LBN_DBLCLK:
				{
					//��
					WCHAR Buff[1024] = {0};
					WCHAR KeyName[32] = {0};
					unsigned int Index;
					Index = SendMessage(GetDlgItem(hDlg,IDC_LIST_KEY),LB_GETCURSEL,NULL,NULL);
					SendMessage(GetDlgItem(hDlg,IDC_LIST_KEY),LB_GETTEXT,Index,(LPARAM)KeyName);
					GetDlgItemText(hDlg,IDC_EDIT_PATH,Buff,1024);
					if (wcslen(Buff))
						wcscat(Buff,L"\\");
					wcscat(Buff,KeyName);
					SetDlgItemText(hDlg,IDC_EDIT_PATH,Buff);
					OpenKey();
				}
				break;
			default:
				break;
			}
			break;
		case IDC_LIST_VALUE:
			switch (HIWORD(wParam))
			{
			//IDC_LIST_VALUE��˫��
			case LBN_DBLCLK:
				ReadValue();
				break;
			default:
				break;
			}
			break;

			//���Ӽ�
		case IDC_BUTTON_OPEN:
			OpenKey();
			break;

		case IDC_BUTTON_BACK:
			{
				WCHAR Buff[1024] = {0};
				WCHAR *pBuff;
				GetDlgItemText(hDlg,IDC_EDIT_PATH,Buff,1024);
				for (pBuff = Buff+wcslen(Buff)-1;pBuff>=Buff;pBuff--)
				{
					if (*pBuff==L'\\'||pBuff == Buff)
					{
						memset(pBuff,0,32);
						break;
					}
				}
				SetDlgItemText(hDlg,IDC_EDIT_PATH,Buff);
				OpenKey();
			}
			break;

			//��ȡ��ֵ
		case IDC_BUTTON_READ:
			ReadValue();
			break;

			//�����ֵ
		case IDC_BUTTON_SAVE:
			SaveValue();
			break;

			//ɾ����ֵ
		case IDC_BUTTON_DELEVALUE:
			DeletValue();
			break;

			//ɾ���Ӽ�
		case IDC_BUTTON_DELKEY:
			DeletKey();
			break;

			//����Ӽ�
		case IDC_BUTTON_ADDKEY:
			AddKey();
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

