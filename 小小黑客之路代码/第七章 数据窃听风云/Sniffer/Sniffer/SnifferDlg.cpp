//////////////////////////////////////////////////////////////////////////
//FileName:	SnifferDlg.cpp
//Data:		2009-04-11
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
HWND hList = NULL;
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
	hInst= hInstance;
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
	//�нṹ
	LVCOLUMN	LvCol;
	LVITEM		LvItem;	
	LPNMITEMACTIVATE lpnmitem;
	LPNMHDR lpnmhdr;
	//�ж�ץ��״̬
	static BOOL Flag = TRUE;

	switch (uMsg)
	{
	case WM_INITDIALOG:
        //����ͼ��
     	SendMessageA(hDlg,WM_SETICON,ICON_SMALL, (LPARAM) LoadIcon(hInst,MAKEINTRESOURCE(IDI_ICON)));
		SendMessageA(hDlg,WM_SETICON, ICON_BIG,(LPARAM) LoadIcon(hInst,MAKEINTRESOURCE(IDI_ICON)));
		InitCommonControls();
		hList = GetDlgItem(hDlg,IDC_LIST);

		memset(&LvCol,0,sizeof(LvCol));									
		LvCol.mask= LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM ;
		LvCol.fmt=  LVCFMT_LEFT;
		LvCol.cx= 0x25;
		//��ʼ���б�
		LvCol.pszText= "���";
		SendMessage(hList,LVM_INSERTCOLUMN,0,(LPARAM)&LvCol);
		LvCol.pszText="Э��";
		LvCol.cx= 0x25;
		SendMessage(hList,LVM_INSERTCOLUMN,1,(LPARAM)&LvCol);
		LvCol.pszText="ԴMAC";
		LvCol.cx= 0x80;
		SendMessage(hList,LVM_INSERTCOLUMN,2,(LPARAM)&LvCol);
		LvCol.pszText="ԴIP";
		LvCol.cx= 0x60;
		SendMessage(hList,LVM_INSERTCOLUMN,3,(LPARAM)&LvCol);
		LvCol.pszText="ԴPort";
		LvCol.cx= 0x30;
		SendMessage(hList,LVM_INSERTCOLUMN,4,(LPARAM)&LvCol);
		LvCol.pszText="Ŀ��MAC";
		LvCol.cx= 0x80;
		SendMessage(hList,LVM_INSERTCOLUMN,5,(LPARAM)&LvCol);
		LvCol.pszText="Ŀ��IP";
		LvCol.cx= 0x60;
		SendMessage(hList,LVM_INSERTCOLUMN,7,(LPARAM)&LvCol);
		LvCol.pszText="Ŀ��Port";
		LvCol.cx= 0x40;
		SendMessage(hList,LVM_INSERTCOLUMN,7,(LPARAM)&LvCol);
		LvCol.pszText="���ݳ���";
		LvCol.cx= 0x40;
		SendMessage(hList,LVM_INSERTCOLUMN,8,(LPARAM)&LvCol);
		//����Ϊ����ѡȡ
		ListView_SetExtendedListViewStyleEx(GetDlgItem(hDlg,IDC_LIST),0 , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP);
		memset(&LvItem,0,sizeof(LvItem));

		break;
	case WM_NOTIFY:
		lpnmhdr = (LPNMHDR)lParam;
		if( lpnmhdr->hwndFrom==hList ){
			if(lpnmhdr->code == NM_CLICK ) {
				lpnmitem = (LPNMITEMACTIVATE)lParam;
				int iSelect = ListView_GetSelectionMark(hList);
				if (iSelect>packets.size())
				{
					break;
				}
				Packet p = packets[iSelect];

				//��ʾ�ַ���16����
				char Buf[65535*3] = {0};
				memset(Buf,0,sizeof(Buf));
				for (unsigned int i=0;i<p.Length;i++)
				{
					//�����ַ���ָ��
					char *ptr = p.pContent;
					//ÿ����ʾ16���ַ�
					if (i%16==0&&i!=0)
					{
						strcat(Buf,"\r\n");
					}
					if (ptr[i]=='\0')
					{
						strcat(Buf,"00 ");
					}
					else
					{
						char temp[10] = {0};
						sprintf(temp,"%02x ",ptr[i]);
						if (strstr(temp,"ffffff")!=NULL)
						{
							strcpy(temp,temp+6);
						}
						strcat(Buf,temp);
					}
				}
				HWND hEdit1 = GetDlgItem(hDlg,IDC_EDIT1);
				SetWindowText(hEdit1,Buf);
				//��ʾ�ַ�
				memset(Buf,0,sizeof(Buf));
				for (unsigned int i=0;i<p.Length;i++)
				{
					char *ptr = p.pContent;
					if (i%16==0&&i!=0)
					{
						strcat(Buf,"\r\n");
					}
					char ch = ptr[i];
					char temp[10] = {0};
					sprintf(temp,"%c",ch);
					strcat(Buf,temp);
				}
				HWND hEdit2 = GetDlgItem(hDlg,IDC_EDIT2);
				SetWindowText(hEdit2,Buf);
			}
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON:
			if (Flag)
			{
				//ɾ��������ʾ����
				ListView_DeleteAllItems(hList);
				//��ʼ��̽
				StartSniffer();
				SetWindowText(GetDlgItem(hDlg,IDC_EDIT1),"");
				SetWindowText(GetDlgItem(hDlg,IDC_EDIT2),"");
				SetWindowText(GetDlgItem(hDlg,IDC_BUTTON),"ֹͣ��̽");

				Flag = !Flag;
			}
			else
			{
				//ֹͣ��̽
				StopSniffer(pcapHandle);
				SetWindowText(GetDlgItem(hDlg,IDC_BUTTON),"��ʼץ��");

				Flag = !Flag;
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

//////////////////////////////////////////////////////////////////////////
//��һ�����ݰ���List Control
//////////////////////////////////////////////////////////////////////////
BOOL InsertPacket(Packet p)
{
	char Buf[1024];
	LVITEM LvItem;
	LvItem.mask= LVIF_TEXT;
	LvItem.iItem= PacketCount++;
	LvItem.iSubItem= 0;
	memset(Buf,0,sizeof(Buf));
	sprintf(Buf,"%d",PacketCount);
	LvItem.pszText= Buf;
	SendMessage(hList, LVM_INSERTITEM, (WPARAM)0, (LPARAM)&LvItem);
	LvItem.iSubItem = 1;
	memset(Buf,0,sizeof(Buf));
	if (p.Protocol==TCP)
	{
		memcpy(Buf,"TCP",sizeof("TCP"));
	}
	else
	{
		memcpy(Buf,"UDP",sizeof("TCP"));
	}
	SendMessage(hList,LVM_SETITEM,(WPARAM)0,(LPARAM)&LvItem);
	LvItem.iSubItem = 2;
	memset(Buf,0,sizeof(Buf));
	sprintf(Buf,"%02x:%02x:%02x:%02x:%02x:%02x",p.SrcMac[0],p.SrcMac[1],p.SrcMac[2],p.SrcMac[3],p.SrcMac[4],p.SrcMac[5]);
	LvItem.pszText= strupr(Buf);
	SendMessage(hList,LVM_SETITEM,(WPARAM)0,(LPARAM)&LvItem);
	LvItem.iSubItem = 3;
	memset(Buf,0,sizeof(Buf));
	sprintf(Buf,"%s",inet_ntoa(p.SrcIp));
	LvItem.pszText= Buf;
	SendMessage(hList,LVM_SETITEM,(WPARAM)0,(LPARAM)&LvItem);
	LvItem.iSubItem = 4;
	memset(Buf,0,sizeof(Buf));
	sprintf(Buf,"%d",ntohs(p.SrcPort));
	LvItem.pszText= Buf;
	SendMessage(hList,LVM_SETITEM,(WPARAM)0,(LPARAM)&LvItem);
	LvItem.iSubItem = 5;
	memset(Buf,0,sizeof(Buf));
	sprintf(Buf,"%02x:%02x:%02x:%02x:%02x:%02x",p.DestMac[0],p.DestMac[1],p.DestMac[2],p.SrcMac[3],p.DestMac[4],p.DestMac[5]);
	LvItem.pszText= strupr(Buf);
	SendMessage(hList,LVM_SETITEM,(WPARAM)0,(LPARAM)&LvItem);
	LvItem.iSubItem = 6;
	memset(Buf,0,sizeof(Buf));
	sprintf(Buf,"%s",inet_ntoa(p.DestIp));
	LvItem.pszText= Buf;
	SendMessage(hList,LVM_SETITEM,(WPARAM)0,(LPARAM)&LvItem);
	LvItem.iSubItem = 7;
	memset(Buf,0,sizeof(Buf));
	sprintf(Buf,"%d",ntohs(p.DestPort));
	LvItem.pszText= Buf;
	SendMessage(hList,LVM_SETITEM,(WPARAM)0,(LPARAM)&LvItem);
	LvItem.iSubItem = 8;
	memset(Buf,0,sizeof(Buf));
	sprintf(Buf,"%d",p.Length);
	LvItem.pszText= Buf;
	SendMessage(hList,LVM_SETITEM,(WPARAM)0,(LPARAM)&LvItem);

	return TRUE;
}