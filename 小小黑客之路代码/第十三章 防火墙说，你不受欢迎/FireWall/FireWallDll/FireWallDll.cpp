#include "Header.h"

//////////////////////////////////////////////////////////////////////////
//����ɶ�д�������ݶ�
//////////////////////////////////////////////////////////////////////////
#pragma data_seg(".FWRWS")
//IP�б�
IPLISTITEM IpList[1024] = {0};
int IpListSize = 0;
//�˿��б�
PORTLISTITEM PortList[1024] = {0};
int PortListSize = 0;
//Ӧ�ó����б�
PATHLISTITEM PathList[1024] = {0};
int PathListSize = 0;
//�ٽ���������ÿ�ζԸ����ν��ж�д��Ҫ�Ƚ����ٽ���
CRITICAL_SECTION RWSCriticalSection;
//״̬
#pragma data_seg()
//�����ݶ���Ϊ����Ŀɶ�д��
#pragma comment(linker,"/section:.FWRWS,RWS")

//�Լ������GUID ��UI�е�GUID��ͬ
GUID  UDPguid={0x4d1e91fd,0x116a,0x44aa,{0x8f,0xd4,0x1d,0x2c,0xf2,0x7b,0xd9,0xa9}};

GUID  TCPguid={0x4d1e91fd,0x116a,0x44aa,{0x8f,0xd4,0x1d,0x2c,0xf2,0x7b,0xd9,0xaa}};


//�����ṩ����Ϣ������ͷָ��
LPWSAPROTOCOL_INFOW  protoinfo=NULL;

//��һ�������ṩ�ߵĺ���ָ���б�
WSPPROC_TABLE        nextproctable;

DWORD                protoinfosize=0;
int                  totalprotos=0;


//////////////////////////////////////////////////////////////////////////
//��ȡ���з����ṩ��
//////////////////////////////////////////////////////////////////////////
BOOL GetProviders()
{
	int    errorcode;

	protoinfo=NULL;
	protoinfosize=0;
	totalprotos=0;

	//�оٷ����ṩ�ߣ���ʵ��Ϊ�˻�ȡ�����ṩ�ߵ�����
	if(WSCEnumProtocols(NULL,protoinfo,&protoinfosize,&errorcode)==SOCKET_ERROR)
	{
		if(errorcode!=WSAENOBUFS)
		{
			return FALSE;
		}
	}


	//�������ṩ�����������ڴ�ռ�
	if((protoinfo=(LPWSAPROTOCOL_INFOW)GlobalAlloc(GPTR,protoinfosize))==NULL)
	{             
		return FALSE;
	}

	//������з����ṩ����Ϣ��������protoinfo
	if((totalprotos=WSCEnumProtocols(NULL,protoinfo,&protoinfosize,&errorcode))==SOCKET_ERROR)
	{
		return FALSE;
	}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//�ͷŷ����ṩ��ָ��
//////////////////////////////////////////////////////////////////////////
void FreeProviders()
{
	GlobalFree(protoinfo);
}


//////////////////////////////////////////////////////////////////////////
//��̬���ӿ�������
//////////////////////////////////////////////////////////////////////////
BOOL WINAPI DllMain(HINSTANCE hmodule,
					DWORD     reason,
					LPVOID    lpreserved)
{

	//�ٽ�����ʼ��
	InitializeCriticalSection(&RWSCriticalSection);

	return TRUE;
}



//////////////////////////////////////////////////////////////////////////
//TCP��LSP��WSPStartup����
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

	//��ȡ�����ṩ����Ϣ
	GetProviders();


	//�ҵ��Լ���LSP�����ID
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

	//��Э�����У��ҵ��Լ���LSP����һ��LSP
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

	//��ȡ��һ��LSP��Dll·��
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

	//������һ��LSP��Dll
	if((hfilter=LoadLibrary(NextDllPath))==NULL)
	{
		return WSAEPROVIDERFAILEDINIT;
	}

	 //��ȡ��һ��LSP��WSPStartup������ַ
	if((wspstartupfunc=(LPWSPSTARTUP)GetProcAddress(hfilter,"WSPStartup"))==NULL)
	{
		return WSAEPROVIDERFAILEDINIT;
	}


	//������һ��LSP��WSPStartup����
	if((errorcode=wspstartupfunc(wversionrequested,lpwspdata,lpprotoinfo,upcalltable,lpproctable))!=ERROR_SUCCESS)
	{
		return errorcode;
	}

	//������һ��LSP�ĸ���WSP������ַ
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
//UDP��LSP��WSPStartup����ͬTCP�ĺ�����
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

	//����Ӧ�ó���
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
//WSPSocket����������һ�������ṩ�ߵ���
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
//WSPBind����������һ�������ṩ�ߵ���
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
	//���Ҫ�󶨵�Port�ĺϷ���
	if(CheckPort(SrcSockAddr.sin_port)==FALSE)
	{
		*lpErrno = WSAECONNABORTED;
		return SOCKET_ERROR;
	}
	return nextproctable.lpWSPBind(s,name,namelen,lpErrno);
}

//////////////////////////////////////////////////////////////////////////
//WSPListen����������һ�������ṩ�ߵ���
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
		//���Ҫ�����Ķ˿ڵĺϷ���
		if (CheckPort(SrcSockAddr.sin_port)==FALSE)
		{

			*lpErrno = WSAECONNABORTED;
			return SOCKET_ERROR;
		}
	}

	return nextproctable.lpWSPListen(s,backlog,lpErrno);
}


//////////////////////////////////////////////////////////////////////////
//WSPConnect����������һ�������ṩ�ߵ���
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
	//���IP
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
			//���Դ�˿�
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
//WSPRecvFrom����������һ�������ṩ�ߵ���
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
		//���Դ�˿�
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

	//���IP
	if(CheckIp(SockAddr.sin_addr)==FALSE)
	{
		*lpErrno = WSAECONNABORTED;
		memset(lpBuffers,0,dwBufferCount);
		return SOCKET_ERROR;
	}

	return RST;
}


//////////////////////////////////////////////////////////////////////////
//WSPSendTo����������һ�������ṩ�ߵ���
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
		//���Դ�˿�
		if(CheckPort(SrcSockAddr.sin_port)==FALSE)
		{
			return SOCKET_ERROR;
		}
	}

	//���IP
	if (CheckIp(DestSockAddr.sin_addr)==FALSE)
	{
		*lpErrno = WSAECONNABORTED;
		return SOCKET_ERROR;
	}

	return nextproctable.lpWSPSendTo(s,lpBuffers,dwBufferCount,lpNumberOfBytesSent,
				dwFlags,lpTo,iTolen,lpOverlapped,lpCompletionRoutine,lpThreadId,lpErrno);

}
