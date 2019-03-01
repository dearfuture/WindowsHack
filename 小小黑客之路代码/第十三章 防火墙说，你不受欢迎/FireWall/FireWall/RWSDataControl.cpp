//////////////////////////////////////////////////////////////////////////
//文件名：RWSdataControl.cpp
//说明：动态链接库的读写共享区段的数据控制
//////////////////////////////////////////////////////////////////////////


#include "Header.h"


//////////////////////////////////////////////////////////////////////////
// 为端口名单添加一个条目
//////////////////////////////////////////////////////////////////////////
BOOL AddPort()
{
	PORTLISTITEM ItemBuff;
	ItemBuff.Port = htons((USHORT)GetDlgItemInt(hPortDlg,IDC_EDIT_ADDPORT,NULL,FALSE));

	if (SendDlgItemMessage(hPortDlg,IDC_RADIO_PORTPASS,BM_GETCHECK,NULL,NULL)==BST_CHECKED)
	{
		ItemBuff.Pass = TRUE;
	}
	if (SendDlgItemMessage(hPortDlg,IDC_RADIO_PORTFORBID,BM_GETCHECK,NULL,NULL)==BST_CHECKED)
	{
		ItemBuff.Pass = FALSE;
	}

	AddPortListItem(ItemBuff);
	ShowPortList();
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//在UI的端口列表里显示湖所有条目
//////////////////////////////////////////////////////////////////////////
BOOL ShowPortList()
{
	HWND hList = GetDlgItem(hPortDlg,IDC_LIST_PORT);
	LVITEM LvItem;
	WCHAR Buff[32] = {0};

	int i = 0;

	PORTLISTITEM ItemBuff;

	SendDlgItemMessage(hPortDlg,IDC_LIST_PORT,LVM_DELETEALLITEMS,NULL,NULL);
	while (GetPortListItem(i,&ItemBuff)==TRUE)
	{
		LvItem.iItem = i;
		LvItem.mask= LVIF_TEXT;
		LvItem.iSubItem = 0;
		wsprintfW(Buff,L"%d",htons(ItemBuff.Port));
		LvItem.pszText = Buff;
		SendDlgItemMessage(hPortDlg,IDC_LIST_PORT,LVM_INSERTITEM,(WPARAM)0,(LPARAM)&LvItem);

		memset(Buff,0,sizeof(Buff));
		LvItem.iSubItem = 1;
		if (ItemBuff.Pass==TRUE)
		{
			wcscat(Buff,L"放行");
		}
		else
		{
			wcscat(Buff,L"禁止");
		}
		LvItem.pszText = Buff;
		SendDlgItemMessage(hPortDlg,IDC_LIST_PORT,LVM_SETITEM,(WPARAM)0,(LPARAM)&LvItem);
		i++;

	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// 删除一个端口条目
//////////////////////////////////////////////////////////////////////////
BOOL DeletPort()
{
	int index;
	index = SendDlgItemMessage(hPortDlg,IDC_LIST_PORT,LVM_GETSELECTIONMARK,NULL,NULL);
	if (index!=-1)
	{
		DeletPortListItem(index);
	}
	ShowPortList();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// 为IP名单添加一个条目
//////////////////////////////////////////////////////////////////////////
BOOL AddIp()
{
	CHAR Buff[32] = {0};
	IPLISTITEM ItemBuff;
	GetDlgItemTextA(hIpDlg,IDC_EDIT_ADDIP,Buff,32);

	ItemBuff.Ip.S_un.S_addr = inet_addr(Buff);

	if (SendDlgItemMessage(hIpDlg,IDC_RADIO_IPPASS,BM_GETCHECK,NULL,NULL)==BST_CHECKED)
	{
		ItemBuff.Pass = TRUE;
	}
	if (SendDlgItemMessage(hIpDlg,IDC_RADIO_IPFORBID,BM_GETCHECK,NULL,NULL)==BST_CHECKED)
	{
		ItemBuff.Pass = FALSE;
	}
	AddIpListItem(ItemBuff);
	ShowIpList();
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
// 在UI中显示IP列表
//////////////////////////////////////////////////////////////////////////
BOOL ShowIpList()
{
	LVITEM LvItem;
	WCHAR Buff[32] = {0};

	int i = 0;

	IPLISTITEM ItemBuff;

	SendDlgItemMessage(hIpDlg,IDC_LIST_IP,LVM_DELETEALLITEMS,NULL,NULL);
	while (GetIpListItem(i,&ItemBuff)==TRUE)
	{
		LvItem.iItem = i;
		LvItem.mask= LVIF_TEXT;
		LvItem.iSubItem = 0;
		CHAR *IpBuff = inet_ntoa(ItemBuff.Ip);
		SetDlgItemTextA(hIpDlg,IDC_EDIT_ADDIP,IpBuff);
		GetDlgItemText(hIpDlg,IDC_EDIT_ADDIP,Buff,32);
		LvItem.pszText = Buff;
		SendDlgItemMessage(hIpDlg,IDC_LIST_IP,LVM_INSERTITEM,(WPARAM)0,(LPARAM)&LvItem);

		memset(Buff,0,sizeof(Buff));
		LvItem.iSubItem = 1;
		if (ItemBuff.Pass==TRUE)
		{
			wcscat(Buff,L"放行");
		}
		else
		{
			wcscat(Buff,L"禁止");
		}
		LvItem.pszText = Buff;
		SendDlgItemMessage(hIpDlg,IDC_LIST_IP,LVM_SETITEM,(WPARAM)0,(LPARAM)&LvItem);
		i++;

	}
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//删除一个IP条目
//////////////////////////////////////////////////////////////////////////
BOOL DeletIp()
{
	int index;
	index = SendDlgItemMessage(hIpDlg,IDC_LIST_IP,LVM_GETSELECTIONMARK,NULL,NULL);
	DeletIpListItem(index);
	ShowIpList();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// 为应用程序名单添加一个条目
//////////////////////////////////////////////////////////////////////////
BOOL AddPath()
{
	PATHLISTITEM ItemBuff;
	GetDlgItemText(hPathDlg,IDC_EDIT_ADDPATH,ItemBuff.Path,MAX_PATH);
	if (wcslen(ItemBuff.Path)==0)
	{
		return FALSE;
	}
	if (SendDlgItemMessage(hPathDlg,IDC_RADIO_PATHPASS,BM_GETCHECK,NULL,NULL)==BST_CHECKED)
	{
		ItemBuff.Pass = TRUE;
	}
	if (SendDlgItemMessage(hPathDlg,IDC_RADIO_PATHFORBID,BM_GETCHECK,NULL,NULL)==BST_CHECKED)
	{
		ItemBuff.Pass = FALSE;
	}

	AddPathListItem(ItemBuff);
	ShowPathList();
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//在UI中的应用程序列表中显示信息
//////////////////////////////////////////////////////////////////////////
BOOL ShowPathList()
{
	LVITEM LvItem;
	WCHAR Buff[MAX_PATH] = {0};
	int i = 0;
	PATHLISTITEM ItemBuff;
	SendDlgItemMessage(hPathDlg,IDC_LIST_PATH,LVM_DELETEALLITEMS,NULL,NULL);
	while (GetPathListItem(i,&ItemBuff)==TRUE)
	{
		LvItem.iItem = i;
		LvItem.mask= LVIF_TEXT;
		LvItem.iSubItem = 0;
		LvItem.pszText = ItemBuff.Path;
		SendDlgItemMessage(hPathDlg,IDC_LIST_PATH,LVM_INSERTITEM,(WPARAM)0,(LPARAM)&LvItem);

		memset(Buff,0,sizeof(Buff));
		LvItem.iSubItem = 1;
		if (ItemBuff.Pass==TRUE)
		{
			wcscat(Buff,L"放行");
		}
		else
		{
			wcscat(Buff,L"禁止");
		}
		LvItem.pszText = Buff;
		SendDlgItemMessage(hPathDlg,IDC_LIST_PATH,LVM_SETITEM,(WPARAM)0,(LPARAM)&LvItem);
		i++;

	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//删除一个应用程序条目
//////////////////////////////////////////////////////////////////////////
BOOL DeletPath()
{
	int index;
	index = SendDlgItemMessage(hPathDlg,IDC_LIST_PATH,LVM_GETSELECTIONMARK,NULL,NULL);
	DeletPathListItem(index);
	ShowPathList();
	return TRUE;
}