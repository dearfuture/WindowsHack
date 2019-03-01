#include "Header.h"

//////////////////////////////////////////////////////////////////////////
//定义可读写共享数据段
//////////////////////////////////////////////////////////////////////////
#pragma data_seg(".FWRWS")
//IP列表
IPLISTITEM IpList[1024] = {0};
int IpListSize = 0;
//端口列表
PORTLISTITEM PortList[1024] = {0};
int PortListSize = 0;
//应用程序列表
PATHLISTITEM PathList[1024] = {0};
int PathListSize = 0;
//临界区变量，每次对该区段进行读写都要先进入临界区
CRITICAL_SECTION RWSCriticalSection;
//状态
#pragma data_seg()
//将数据段设为共享的可读写的
#pragma comment(linker,"/section:.FWRWS,RWS")

//自己定义的GUID 与UI中的GUID相同
GUID  UDPguid={0x4d1e91fd,0x116a,0x44aa,{0x8f,0xd4,0x1d,0x2c,0xf2,0x7b,0xd9,0xa9}};

GUID  TCPguid={0x4d1e91fd,0x116a,0x44aa,{0x8f,0xd4,0x1d,0x2c,0xf2,0x7b,0xd9,0xaa}};


//服务提供者信息数据组头指针
LPWSAPROTOCOL_INFOW  protoinfo=NULL;

//下一个服务提供者的函数指针列表
WSPPROC_TABLE        nextproctable;

DWORD                protoinfosize=0;
int                  totalprotos=0;


//////////////////////////////////////////////////////////////////////////
//获取所有服务提供者
//////////////////////////////////////////////////////////////////////////
BOOL GetProviders()
{
	int    errorcode;

	protoinfo=NULL;
	protoinfosize=0;
	totalprotos=0;

	//列举服务提供者，其实是为了获取服务提供者的数量
	if(WSCEnumProtocols(NULL,protoinfo,&protoinfosize,&errorcode)==SOCKET_ERROR)
	{
		if(errorcode!=WSAENOBUFS)
		{
			return FALSE;
		}
	}


	//按服务提供者数量分配内存空间
	if((protoinfo=(LPWSAPROTOCOL_INFOW)GlobalAlloc(GPTR,protoinfosize))==NULL)
	{             
		return FALSE;
	}

	//获得所有服务提供者信息，保存在protoinfo
	if((totalprotos=WSCEnumProtocols(NULL,protoinfo,&protoinfosize,&errorcode))==SOCKET_ERROR)
	{
		return FALSE;
	}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//释放服务提供者指针
//////////////////////////////////////////////////////////////////////////
void FreeProviders()
{
	GlobalFree(protoinfo);
}


//////////////////////////////////////////////////////////////////////////
//动态链接库主函数
//////////////////////////////////////////////////////////////////////////
BOOL WINAPI DllMain(HINSTANCE hmodule,
					DWORD     reason,
					LPVOID    lpreserved)
{

	//临界区初始化
	InitializeCriticalSection(&RWSCriticalSection);

	return TRUE;
}



//////////////////////////////////////////////////////////////////////////
//TCP的LSP的WSPStartup函数
//////////////////////////////////////////////////////////////////////////
int WSPAPI TCPWSPStartup(
WORD					wversionrequested,
LPWSPDATA				lpwspdata,
LPWSAPROTOCOL_INFOW		lpprotoinfo,
WSPUPCALLTABLE			upcalltable,
LPWSPPROC_TABLE			lpproctable
)
{
	int           i;
	int           errorcode; 
	int           filterpathlen;
	DWORD         layerid=0; 
	DWORD         nextlayerid=0;
	TCHAR         *NextDllPath;
	HINSTANCE     hfilter;
	LPWSPSTARTUP  wspstartupfunc=NULL;

	if(lpprotoinfo->ProtocolChain.ChainLen<=1)
	{
		return FALSE;
	}

	//获取服务提供者信息
	GetProviders();


	//找到自己的LSP的入口ID
	for(i=0;i<totalprotos;i++)
	{
		if(memcmp(&protoinfo[i].ProviderId,&TCPguid,sizeof(GUID))==0)
		{
			layerid=protoinfo[i].dwCatalogEntryId;
			break;
		}
	}
	if (i>=totalprotos)
	{
		return FALSE;
	}

	//在协议链中，找到自己的LSP的下一个LSP
	for(i=0;i<lpprotoinfo->ProtocolChain.ChainLen;i++)
	{
		if(lpprotoinfo->ProtocolChain.ChainEntries[i]==layerid)
		{
			nextlayerid=lpprotoinfo->ProtocolChain.ChainEntries[i+1];
			break;
		}
	}
	if (i>=lpprotoinfo->ProtocolChain.ChainLen)
	{
		return FALSE;
	}

	//获取下一个LSP的Dll路径
	filterpathlen=MAX_PATH;
	NextDllPath=(TCHAR*)GlobalAlloc(GPTR,filterpathlen);  
	for(i=0;i<totalprotos;i++)
	{
		if(nextlayerid==protoinfo[i].dwCatalogEntryId)
		{
			if(WSCGetProviderPath(&protoinfo[i].ProviderId,NextDllPath,&filterpathlen,&errorcode)==SOCKET_ERROR)
			{
				return WSAEPROVIDERFAILEDINIT;
			}
			break;
		}
	}
	if(!ExpandEnvironmentStrings(NextDllPath,NextDllPath,MAX_PATH))
	{  
		return WSAEPROVIDERFAILEDINIT;
	}

	//加载下一个LSP的Dll
	if((hfilter=LoadLibrary(NextDllPath))==NULL)
	{
		return WSAEPROVIDERFAILEDINIT;
	}

	 //获取下一个LSP的WSPStartup函数地址
	if((wspstartupfunc=(LPWSPSTARTUP)GetProcAddress(hfilter,"WSPStartup"))==NULL)
	{
		return WSAEPROVIDERFAILEDINIT;
	}


	//调用下一个LSP的WSPStartup函数
	if((errorcode=wspstartupfunc(wversionrequested,lpwspdata,lpprotoinfo,upcalltable,lpproctable))!=ERROR_SUCCESS)
	{
		return errorcode;
	}

	//保存下一个LSP的各个WSP函数地址
	nextproctable=*lpproctable;
	lpproctable->lpWSPBind = FWWSPBind;
	lpproctable->lpWSPConnect = FWWSPConnect;
	lpproctable->lpWSPListen = FWWSPListen;
	lpproctable->lpWSPSendTo = FWWSPSendTo;
	lpproctable->lpWSPRecvFrom = FWWSPRecvFrom;
	lpproctable->lpWSPSocket = FWWSPSocket;
	FreeProviders();
	return 0;
}

//////////////////////////////////////////////////////////////////////////
//UDP的LSP的WSPStartup函数同TCP的很类似
//////////////////////////////////////////////////////////////////////////
int WSPAPI UDPWSPStartup(
					  WORD wversionrequested,
					  LPWSPDATA         lpwspdata,
					  LPWSAPROTOCOL_INFOW lpprotoinfo,
					  WSPUPCALLTABLE upcalltable,
					  LPWSPPROC_TABLE lpproctable
					  )
{
	int           i;
	int           errorcode; 
	int           filterpathlen;
	DWORD         layerid=0; 
	DWORD         nextlayerid=0;
	TCHAR         *NextDllPath;
	HINSTANCE     hfilter;
	LPWSPSTARTUP  wspstartupfunc=NULL;

	if(lpprotoinfo->ProtocolChain.ChainLen<=1)
	{
		return FALSE;
	}

	GetProviders();

	for(i=0;i<totalprotos;i++)
	{
		if(memcmp(&protoinfo[i].ProviderId,&UDPguid,sizeof(GUID))==0)
		{
			layerid=protoinfo[i].dwCatalogEntryId;
			break;
		}
	}
	if (i>=totalprotos)
	{
		return FALSE;
	}

	for(i=0;i<lpprotoinfo->ProtocolChain.ChainLen;i++)
	{
		if(lpprotoinfo->ProtocolChain.ChainEntries[i]==layerid)
		{
			nextlayerid=lpprotoinfo->ProtocolChain.ChainEntries[i+1];
			break;
		}
	}

	if (i>=lpprotoinfo->ProtocolChain.ChainLen)
	{
		return FALSE;
	}

	filterpathlen=MAX_PATH;
	NextDllPath=(TCHAR*)GlobalAlloc(GPTR,filterpathlen);  
	for(i=0;i<totalprotos;i++)
	{
		if(nextlayerid==protoinfo[i].dwCatalogEntryId)
		{
			if(WSCGetProviderPath(&protoinfo[i].ProviderId,NextDllPath,&filterpathlen,&errorcode)==SOCKET_ERROR)
			{
				return WSAEPROVIDERFAILEDINIT;
			}
			break;
		}
	}

	if(!ExpandEnvironmentStrings(NextDllPath,NextDllPath,MAX_PATH))
	{  
		return WSAEPROVIDERFAILEDINIT;
	}

	if((hfilter=LoadLibrary(NextDllPath))==NULL)
	{
		return WSAEPROVIDERFAILEDINIT;
	}

	if((wspstartupfunc=(LPWSPSTARTUP)GetProcAddress(hfilter,"WSPStartup"))==NULL)
	{
		return WSAEPROVIDERFAILEDINIT;
	}

	if((errorcode=wspstartupfunc(wversionrequested,lpwspdata,lpprotoinfo,upcalltable,lpproctable))!=ERROR_SUCCESS)
	{
		return errorcode;
	}

	nextproctable=*lpproctable;
	lpproctable->lpWSPBind = FWWSPBind;
	lpproctable->lpWSPConnect = FWWSPConnect;
	lpproctable->lpWSPListen = FWWSPListen;
	lpproctable->lpWSPSendTo = FWWSPSendTo;
	lpproctable->lpWSPRecvFrom = FWWSPRecvFrom;
	lpproctable->lpWSPSocket = FWWSPSocket;
	FreeProviders();
	return 0;

}


//////////////////////////////////////////////////////////////////////////
//WSPStartup
//////////////////////////////////////////////////////////////////////////
int WSPAPI WSPStartup(
					  WORD wversionrequested,
					  LPWSPDATA         lpwspdata,
					  LPWSAPROTOCOL_INFOW lpprotoinfo,
					  WSPUPCALLTABLE upcalltable,
					  LPWSPPROC_TABLE lpproctable
					  )
{
	int RSTUDP,RSTTCP;

	//检查该应用程序
	WCHAR PathBuff[MAX_PATH] = {0};
	GetModuleFileName(NULL,PathBuff,MAX_PATH);
	if (CheckAppPath(PathBuff)==FALSE)
	{
		return SOCKET_ERROR;
	}
	RSTUDP = UDPWSPStartup(wversionrequested,lpwspdata,lpprotoinfo,upcalltable,lpproctable);
	RSTTCP = TCPWSPStartup(wversionrequested,lpwspdata,lpprotoinfo,upcalltable,lpproctable);

	if (RSTTCP!=0&&RSTUDP!=0)
	{
		MessageBoxA(NULL,"startupfailed",NULL,NULL);
		return FALSE;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
//WSPSocket函数，供上一个服务提供者调用
//////////////////////////////////////////////////////////////////////////
SOCKET WSPAPI FWWSPSocket(
int			af,                               
int			type,                             
int			protocol,                         
LPWSAPROTOCOL_INFOW lpProtocolInfo,   
GROUP		g,                              
DWORD		dwFlags,                        
LPINT		lpErrno
)
{
	SOCKET s;

	s = nextproctable.lpWSPSocket(af,type,protocol,lpProtocolInfo,g,dwFlags
		,lpErrno);
	if (s == SOCKET_ERROR)
	{
		return SOCKET_ERROR;
	}

	if(s == INVALID_SOCKET)
		return s;

	if (af == FROM_PROTOCOL_INFO)
		af = lpProtocolInfo->iAddressFamily;

	if (type == FROM_PROTOCOL_INFO)
		type = lpProtocolInfo->iSocketType;

	if (protocol == FROM_PROTOCOL_INFO)
		protocol = lpProtocolInfo->iProtocol;

	return s;

}



//////////////////////////////////////////////////////////////////////////
//WSPBind函数，供上一个服务提供者调用
//////////////////////////////////////////////////////////////////////////
int WINAPI FWWSPBind(
				   __in          SOCKET s,
				   __in          const struct sockaddr* name,
				   __in          int namelen,
				   __out         LPINT lpErrno
				   )
{

	SOCKADDR_IN SrcSockAddr;
	memcpy(&SrcSockAddr,name,sizeof(SrcSockAddr));
	//检查要绑定的Port的合法性
	if(CheckPort(SrcSockAddr.sin_port)==FALSE)
	{
		*lpErrno = WSAECONNABORTED;
		return SOCKET_ERROR;
	}
	return nextproctable.lpWSPBind(s,name,namelen,lpErrno);
}

//////////////////////////////////////////////////////////////////////////
//WSPListen函数，供上一个服务提供者调用
//////////////////////////////////////////////////////////////////////////
int WINAPI FWWSPListen(
__in          SOCKET s,
__in          int backlog,
__out         LPINT lpErrno
)
{
	SOCKADDR_IN SrcSockAddr;
	int SrcSockAddrLen = sizeof(SrcSockAddr);
	if (getsockname(s,(sockaddr*)&SrcSockAddr,&SrcSockAddrLen)==0)
	{
		//检查要监听的端口的合法性
		if (CheckPort(SrcSockAddr.sin_port)==FALSE)
		{

			*lpErrno = WSAECONNABORTED;
			return SOCKET_ERROR;
		}
	}

	return nextproctable.lpWSPListen(s,backlog,lpErrno);
}


//////////////////////////////////////////////////////////////////////////
//WSPConnect函数，供上一个服务提供者调用
//////////////////////////////////////////////////////////////////////////
int WINAPI FWWSPConnect(
__in          SOCKET s,
__in          const struct sockaddr* name,
__in          int namelen,
__in          LPWSABUF lpCallerData,
__out         LPWSABUF lpCalleeData,
__in          LPQOS lpSQOS,
__in          LPQOS lpGQOS,
__out         LPINT lpErrno
)
{
	SOCKADDR_IN DestSockAddr;
	memcpy(&DestSockAddr,name,namelen);
	//检查IP
	if(CheckIp(DestSockAddr.sin_addr)==FALSE)
	{
		*lpErrno = WSAECONNABORTED;
		return SOCKET_ERROR;
	}
	int RST = 0;
	RST = nextproctable.lpWSPConnect(s,name,namelen,lpCallerData,lpCalleeData,lpSQOS,lpGQOS,lpErrno);

	SOCKADDR_IN SrcSockAddr;
	int SrcSockAddrLen = sizeof(SrcSockAddr);
	if (RST==0)
	{
		if (getsockname(s,(sockaddr*)&SrcSockAddr,&SrcSockAddrLen)==0)
		{
			//检查源端口
			if(CheckPort(SrcSockAddr.sin_port)==FALSE)
			{
				*lpErrno = WSAECONNABORTED;
				return SOCKET_ERROR;
			}
		}
	}

	return RST;
}


//////////////////////////////////////////////////////////////////////////
//WSPRecvFrom函数，供上一个服务提供者调用
//////////////////////////////////////////////////////////////////////////
int WINAPI FWWSPRecvFrom(
SOCKET								s,
LPWSABUF							lpBuffers,
DWORD								dwBufferCount,
LPDWORD								lpNumberOfBytesRecvd,
LPDWORD								lpFlags,
struct sockaddr*					lpFrom,
LPINT								lpFromlen,
LPWSAOVERLAPPED						lpOverlapped,
LPWSAOVERLAPPED_COMPLETION_ROUTINE	lpCompletionRoutine,
LPWSATHREADID						lpThreadId,
LPINT								lpErrno
)
{
	SOCKADDR_IN SrcSockAddr;
	int SrcSockAddrLen = sizeof(SrcSockAddr);
	if (getsockname(s,(sockaddr *)&SrcSockAddr,&SrcSockAddrLen)==0)
	{
		//检查源端口
		if (CheckPort(SrcSockAddr.sin_port)==FALSE)
		{
			*lpErrno = WSAECONNABORTED;
			memset(lpBuffers,0,dwBufferCount);
			return SOCKET_ERROR;
		}
	}

	int RST = nextproctable.lpWSPRecvFrom(s,lpBuffers,dwBufferCount,lpNumberOfBytesRecvd,
					lpFlags,lpFrom,lpFromlen,lpOverlapped,lpCompletionRoutine,lpThreadId,lpErrno);
	
	if (RST!=0)
	{
		return RST;
	}

	SOCKADDR_IN SockAddr;
	memcpy(&SockAddr,lpFrom,sizeof(SOCKADDR_IN));

	//检查IP
	if(CheckIp(SockAddr.sin_addr)==FALSE)
	{
		*lpErrno = WSAECONNABORTED;
		memset(lpBuffers,0,dwBufferCount);
		return SOCKET_ERROR;
	}

	return RST;
}


//////////////////////////////////////////////////////////////////////////
//WSPSendTo函数，供上一个服务提供者调用
//////////////////////////////////////////////////////////////////////////
int WINAPI FWWSPSendTo(
__in          SOCKET s,
__in          LPWSABUF lpBuffers,
__in          DWORD dwBufferCount,
__out         LPDWORD lpNumberOfBytesSent,
__in          DWORD dwFlags,
__in          const struct sockaddr* lpTo,
__in          int iTolen,
__in          LPWSAOVERLAPPED lpOverlapped,
__in          LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine,
__in          LPWSATHREADID lpThreadId,
__out         LPINT lpErrno
)
{

	SOCKADDR_IN DestSockAddr;
	SOCKADDR_IN SrcSockAddr;
	int SrcSockAddrLen = sizeof(SrcSockAddr);
	
	memcpy(&DestSockAddr,lpTo,iTolen);
	
	if (getsockname(s,(sockaddr*)&SrcSockAddr,&SrcSockAddrLen)==0)
	{
		//检查源端口
		if(CheckPort(SrcSockAddr.sin_port)==FALSE)
		{
			return SOCKET_ERROR;
		}
	}

	//检查IP
	if (CheckIp(DestSockAddr.sin_addr)==FALSE)
	{
		*lpErrno = WSAECONNABORTED;
		return SOCKET_ERROR;
	}

	return nextproctable.lpWSPSendTo(s,lpBuffers,dwBufferCount,lpNumberOfBytesSent,
				dwFlags,lpTo,iTolen,lpOverlapped,lpCompletionRoutine,lpThreadId,lpErrno);

}
