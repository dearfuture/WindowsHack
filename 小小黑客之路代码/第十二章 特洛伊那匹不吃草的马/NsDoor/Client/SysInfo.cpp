//////////////////////////////////////////////////////////////////////////
//File:	SysInfo.cpp
//REM:	Remote System Information
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "SysInfo.h"
HWND hSysList = NULL;
//HWND hSysList;
//////////////////////////////////////////////////////////////////////////
//初始化Sysinfo列表框
//////////////////////////////////////////////////////////////////////////
BOOL InitSysList(HWND hList)
{
	//全局IDC_LIST1
	hSysList = hList;
	LVCOLUMN lvCol;
	char *szColumn[]= {"Name", "Value"};
	int width[]= {100,350};

	ZeroMemory(&lvCol, sizeof(LVCOLUMN));

	lvCol.mask= LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH | LVCF_FMT;
	lvCol.fmt= LVCFMT_LEFT;
	ListView_SetExtendedListViewStyleEx(hList,0 , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP);

	for(int i=0; i<2; i++ )
	{
		lvCol.iSubItem= i;
		lvCol.cx= width[i];
		lvCol.pszText= szColumn[i];

		ListView_InsertColumn(hList, i, &lvCol);
	}

	return TRUE;
}



//////////////////////////////////////////////////////////////////////////
//显示远程主机信息
//////////////////////////////////////////////////////////////////////////
BOOL ViewSysInfoItem(HWND hList,LPVOID pData)
{
	LVITEM lvItem;
	lvItem.mask =  LVIF_TEXT;
	char temp[1024] = {0};
	SysInfo si = *(SysInfo*)(pData);

	int count = ListView_GetItemCount(hList);
	lvItem.iItem = count;
	//Name
	lvItem.iSubItem = 0;
	lvItem.pszText = si.Name;
	ListView_InsertItem(hList,&lvItem);
	//Value
	lvItem.iSubItem = 1;
	lvItem.pszText = si.Value;
	ListView_SetItem(hList,&lvItem);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//处理信息
//////////////////////////////////////////////////////////////////////////
BOOL DecodeSysCmd(HWND hList,RATID ratId,LPVOID pData)
{
	int count = lstrlen((char*)pData)/sizeof(SysInfo);
	for (int i=0;i<count;i++)
	{
		SysInfo* psi = (SysInfo*)pData;
		ViewSysInfoItem(hList,psi++);
	}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//SYSINFO封装函数
//////////////////////////////////////////////////////////////////////////
BOOL SysView(SOCKET sockfd,int dwRecvSize)
{
	ListView_DeleteAllItems(hSysList);
	SysInfo* si = (SysInfo*)(szRecvCmd+sizeof(RatProto));
	int count = (dwRecvSize-sizeof(RatProto))/sizeof(SysInfo);
	LVITEM lvItem;
	lvItem.mask =  LVIF_TEXT;
	for (int i=0;i<count;i++)
	{
		lvItem.iItem = i;
		//Name
		lvItem.iSubItem = 0;
		lvItem.pszText = si->Name;
		ListView_InsertItem(hSysList,&lvItem);
		//Value
		lvItem.iSubItem = 1;
		lvItem.pszText = si->Value;
		ListView_SetItem(hSysList,&lvItem);
		//下一条记录
		si++;
	}
	
	return TRUE;
}