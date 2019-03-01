//////////////////////////////////////////////////////////////////////////
//�ļ�����RWSdataControl.cpp
//˵������̬���ӿ�Ķ�д�������ε����ݿ���
//////////////////////////////////////////////////////////////////////////


#include "Header.h"


//////////////////////////////////////////////////////////////////////////
// Ϊ�˿��������һ����Ŀ
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
//��UI�Ķ˿��б�����ʾ��������Ŀ
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
			wcscat(Buff,L"����");
		}
		else
		{
			wcscat(Buff,L"��ֹ");
		}
		LvItem.pszText = Buff;
		SendDlgItemMessage(hPortDlg,IDC_LIST_PORT,LVM_SETITEM,(WPARAM)0,(LPARAM)&LvItem);
		i++;

	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// ɾ��һ���˿���Ŀ
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
// ΪIP�������һ����Ŀ
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
// ��UI����ʾIP�б�
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
			wcscat(Buff,L"����");
		}
		else
		{
			wcscat(Buff,L"��ֹ");
		}
		LvItem.pszText = Buff;
		SendDlgItemMessage(hIpDlg,IDC_LIST_IP,LVM_SETITEM,(WPARAM)0,(LPARAM)&LvItem);
		i++;

	}
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//ɾ��һ��IP��Ŀ
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
// ΪӦ�ó����������һ����Ŀ
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
//��UI�е�Ӧ�ó����б�����ʾ��Ϣ
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
			wcscat(Buff,L"����");
		}
		else
		{
			wcscat(Buff,L"��ֹ");
		}
		LvItem.pszText = Buff;
		SendDlgItemMessage(hPathDlg,IDC_LIST_PATH,LVM_SETITEM,(WPARAM)0,(LPARAM)&LvItem);
		i++;

	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//ɾ��һ��Ӧ�ó�����Ŀ
//////////////////////////////////////////////////////////////////////////
BOOL DeletPath()
{
	int index;
	index = SendDlgItemMessage(hPathDlg,IDC_LIST_PATH,LVM_GETSELECTIONMARK,NULL,NULL);
	DeletPathListItem(index);
	ShowPathList();
	return TRUE;
}