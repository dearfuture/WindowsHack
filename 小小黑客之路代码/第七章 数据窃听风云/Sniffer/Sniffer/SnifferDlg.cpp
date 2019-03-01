//////////////////////////////////////////////////////////////////////////
//FileName:	SnifferDlg.cpp
//Data:		2009-04-11
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
HWND hList = NULL;
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
	hInst= hInstance;
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
	//列结构
	LVCOLUMN	LvCol;
	LVITEM		LvItem;	
	LPNMITEMACTIVATE lpnmitem;
	LPNMHDR lpnmhdr;
	//判断抓包状态
	static BOOL Flag = TRUE;

	switch (uMsg)
	{
	case WM_INITDIALOG:
        //设置图标
     	SendMessageA(hDlg,WM_SETICON,ICON_SMALL, (LPARAM) LoadIcon(hInst,MAKEINTRESOURCE(IDI_ICON)));
		SendMessageA(hDlg,WM_SETICON, ICON_BIG,(LPARAM) LoadIcon(hInst,MAKEINTRESOURCE(IDI_ICON)));
		InitCommonControls();
		hList = GetDlgItem(hDlg,IDC_LIST);

		memset(&LvCol,0,sizeof(LvCol));									
		LvCol.mask= LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM ;
		LvCol.fmt=  LVCFMT_LEFT;
		LvCol.cx= 0x25;
		//初始化列表
		LvCol.pszText= "编号";
		SendMessage(hList,LVM_INSERTCOLUMN,0,(LPARAM)&LvCol);
		LvCol.pszText="协议";
		LvCol.cx= 0x25;
		SendMessage(hList,LVM_INSERTCOLUMN,1,(LPARAM)&LvCol);
		LvCol.pszText="源MAC";
		LvCol.cx= 0x80;
		SendMessage(hList,LVM_INSERTCOLUMN,2,(LPARAM)&LvCol);
		LvCol.pszText="源IP";
		LvCol.cx= 0x60;
		SendMessage(hList,LVM_INSERTCOLUMN,3,(LPARAM)&LvCol);
		LvCol.pszText="源Port";
		LvCol.cx= 0x30;
		SendMessage(hList,LVM_INSERTCOLUMN,4,(LPARAM)&LvCol);
		LvCol.pszText="目的MAC";
		LvCol.cx= 0x80;
		SendMessage(hList,LVM_INSERTCOLUMN,5,(LPARAM)&LvCol);
		LvCol.pszText="目的IP";
		LvCol.cx= 0x60;
		SendMessage(hList,LVM_INSERTCOLUMN,7,(LPARAM)&LvCol);
		LvCol.pszText="目的Port";
		LvCol.cx= 0x40;
		SendMessage(hList,LVM_INSERTCOLUMN,7,(LPARAM)&LvCol);
		LvCol.pszText="数据长度";
		LvCol.cx= 0x40;
		SendMessage(hList,LVM_INSERTCOLUMN,8,(LPARAM)&LvCol);
		//设置为正行选取
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

				//显示字符的16进制
				char Buf[65535*3] = {0};
				memset(Buf,0,sizeof(Buf));
				for (unsigned int i=0;i<p.Length;i++)
				{
					//数据字符串指针
					char *ptr = p.pContent;
					//每行显示16个字符
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
				//显示字符
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
				//删除所有显示数据
				ListView_DeleteAllItems(hList);
				//开始嗅探
				StartSniffer();
				SetWindowText(GetDlgItem(hDlg,IDC_EDIT1),"");
				SetWindowText(GetDlgItem(hDlg,IDC_EDIT2),"");
				SetWindowText(GetDlgItem(hDlg,IDC_BUTTON),"停止嗅探");

				Flag = !Flag;
			}
			else
			{
				//停止嗅探
				StopSniffer(pcapHandle);
				SetWindowText(GetDlgItem(hDlg,IDC_BUTTON),"开始抓包");

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
//绑定一个数据包到List Control
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