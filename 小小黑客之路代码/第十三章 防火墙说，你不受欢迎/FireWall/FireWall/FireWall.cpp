//////////////////////////////////////////////////////////////////////////
//文件名：FireWall.cpp
//说明：LSP的安装和拆卸
//////////////////////////////////////////////////////////////////////////


#define  UNICODE     
#define  _UNICODE         

#include "Header.h"

//////////////////////////////////////////////////////////////////////////
//自己定义的GUID，作为LSP的唯一标示
//////////////////////////////////////////////////////////////////////////
GUID  UDPguid={0x4d1e91fd,0x116a,0x44aa,{0x8f,0xd4,0x1d,0x2c,0xf2,0x7b,0xd9,0xa9}};

GUID  UDPchainguid={0xd3c21121,0x85e1,0x48f3,{0x9a,0xb6,0x23,0xd9,0x0c,0x73,0x07,0xef}};

GUID  TCPguid={0x4d1e91fd,0x116a,0x44aa,{0x8f,0xd4,0x1d,0x2c,0xf2,0x7b,0xd9,0xaa}};

GUID  TCPchainguid={0xd3c21121,0x85e1,0x48f3,{0x9a,0xb6,0x23,0xd9,0x0c,0x73,0x07,0xe0}};

//////////////////////////////////////////////////////////////////////////
//几个函数说明
//////////////////////////////////////////////////////////////////////////

BOOL  GetProviders();
void  FreeProviders();
void  InstallProviders();
void  RemoveProviders();
void  start();
void  usage();

//总的服务提供者数量
int                   totalprotos=0;
DWORD                 protoinfosize=0;
//服务提供者的数组头指针
LPWSAPROTOCOL_INFOW   protoinfo=NULL;



//////////////////////////////////////////////////////////////////////////
//获取服务提供者信息
//////////////////////////////////////////////////////////////////////////
BOOL GetProviders()
{
	int  errorcode;

	protoinfo=NULL;
	totalprotos=0;
	protoinfosize=0;

	if(WSCEnumProtocols(NULL,protoinfo,&protoinfosize,&errorcode)==SOCKET_ERROR)
	{
		if(errorcode!=WSAENOBUFS)
		{
			_tprintf(_T("First WSCEnumProtocols Error: %d\n"),errorcode);
			return FALSE;
		}
	}

	if((protoinfo=(LPWSAPROTOCOL_INFOW)GlobalAlloc(GPTR,protoinfosize))==NULL)
	{
		_tprintf(_T("GlobalAlloc in GetProviders Error: %d\n"),GetLastError());
		return FALSE;
	}

	if((totalprotos=WSCEnumProtocols(NULL,protoinfo,&protoinfosize,&errorcode))==SOCKET_ERROR)
	{
		_tprintf(_T("Second WSCEnumProtocols Error: %d\n"),GetLastError());
		return FALSE;
	}

	_tprintf(_T("Found %d protocols!\n"),totalprotos); 
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//释放服务提供者信息
//////////////////////////////////////////////////////////////////////////
void FreeProviders()
{
	GlobalFree(protoinfo);
}

//////////////////////////////////////////////////////////////////////////
//安装TCP的LSP
//////////////////////////////////////////////////////////////////////////
void InstallTCPProviders()
{
	int                i;
	int                provcnt;
	int                CataIndex;
	int                ErrorCode;
	DWORD              LayercataId=0,TCPorigcataid=0;
	WCHAR              DllPath[MAX_PATH];                 
	LPDWORD            CataEntries;
	WSAPROTOCOL_INFOW  LayerInfo,TCPchaininfo,ChainArray[1];

	GetProviders();

	for(i=0;i<totalprotos;i++)
	{

		if(protoinfo[i].iAddressFamily==AF_INET
			&& protoinfo[i].iProtocol==IPPROTO_TCP)
		{
			memcpy(&LayerInfo,&protoinfo[i],sizeof(WSAPROTOCOL_INFOW));
			LayerInfo.dwServiceFlags1=protoinfo[i].dwServiceFlags1 & (~XP1_IFS_HANDLES);
			TCPorigcataid=protoinfo[i].dwCatalogEntryId;
			memcpy(&TCPchaininfo,&protoinfo[i],sizeof(WSAPROTOCOL_INFOW));
			TCPchaininfo.dwServiceFlags1=protoinfo[i].dwServiceFlags1 & (~XP1_IFS_HANDLES);
			break;
		}
	}

	wcscpy(LayerInfo.szProtocol,L"TCPProvider");
	LayerInfo.ProtocolChain.ChainLen=LAYERED_PROTOCOL;

	if(GetCurrentDirectory(MAX_PATH,DllPath)==0)
	{
		return ;
	} 
	wcscat(DllPath,L"\\FireWallDll.dll");

	if(WSCInstallProvider(&TCPguid,DllPath,&LayerInfo,1,&ErrorCode)==SOCKET_ERROR)
	{
		MessageBoxA(NULL,"WSCInstallProvider Error: ",NULL,NULL);
		return ;
	}

	FreeProviders();

	GetProviders();

	for(i=0;i<totalprotos;i++)
	{
		if(memcmp(&protoinfo[i].ProviderId,&TCPguid,sizeof(GUID))==0)
		{
			LayercataId=protoinfo[i].dwCatalogEntryId;
			break;
		}
	}

	provcnt=0;

	wcscpy(TCPchaininfo.szProtocol,L"TCPFW");

	if(TCPchaininfo.ProtocolChain.ChainLen==BASE_PROTOCOL)
	{
		TCPchaininfo.ProtocolChain.ChainEntries[1]=TCPorigcataid;
	}
	else
	{
		for(i=TCPchaininfo.ProtocolChain.ChainLen;i>0;i--)
		{
			TCPchaininfo.ProtocolChain.ChainEntries[i+1]=TCPchaininfo.ProtocolChain.ChainEntries[i];
		}
	}

	TCPchaininfo.ProtocolChain.ChainLen++;
	TCPchaininfo.ProtocolChain.ChainEntries[0]=LayercataId;

	memcpy(&ChainArray[provcnt++],&TCPchaininfo,sizeof(WSAPROTOCOL_INFOW));


	if(WSCInstallProvider(&TCPchainguid,DllPath,ChainArray,provcnt,&ErrorCode)==SOCKET_ERROR)
	{
		return ;
	}

	FreeProviders();

	GetProviders();

	if((CataEntries=(LPDWORD)GlobalAlloc(GPTR,totalprotos*sizeof(WSAPROTOCOL_INFOW)))==NULL)
	{
		return ;
	}

	CataIndex=0;
	for(i=0;i<totalprotos;i++)
	{
		if(memcmp(&protoinfo[i].ProviderId,&TCPguid,sizeof(GUID))==0 
			|| memcmp(&protoinfo[i].ProviderId,&TCPchainguid,sizeof(GUID))==0)
		{
			CataEntries[CataIndex++]=protoinfo[i].dwCatalogEntryId;
		}
	}

	for(i=0;i<totalprotos;i++)
	{
		if(memcmp(&protoinfo[i].ProviderId,&TCPguid,sizeof(GUID))!=0 
			&& memcmp(&protoinfo[i].ProviderId,&TCPchainguid,sizeof(GUID))!=0)
		{
			CataEntries[CataIndex++]=protoinfo[i].dwCatalogEntryId;
		}
	}

	if((ErrorCode==WSCWriteProviderOrder(CataEntries,totalprotos))!=ERROR_SUCCESS)
	{
		return ;
	}

	FreeProviders();

	return ;
}
//////////////////////////////////////////////////////////////////////////
//安装UDP的LSP
//////////////////////////////////////////////////////////////////////////
void InstallUDPProviders()
{
	int                i;
	int                provcnt;
	int                CataIndex;
	int                ErrorCode;
	DWORD              LayercataId=0,UDPorigcataid=0;
	WCHAR              DllPath[MAX_PATH];                 
	LPDWORD            CataEntries;
	WSAPROTOCOL_INFOW  LayerInfo,UDPchaininfo,ChainArray[1];

	GetProviders();

	for(i=0;i<totalprotos;i++)
	{

		if(protoinfo[i].iAddressFamily==AF_INET
			&& protoinfo[i].iProtocol==IPPROTO_UDP)
		{
			memcpy(&LayerInfo,&protoinfo[i],sizeof(WSAPROTOCOL_INFOW));
			LayerInfo.dwServiceFlags1=protoinfo[i].dwServiceFlags1 & (~XP1_IFS_HANDLES);
			UDPorigcataid=protoinfo[i].dwCatalogEntryId;
			memcpy(&UDPchaininfo,&protoinfo[i],sizeof(WSAPROTOCOL_INFOW));
			UDPchaininfo.dwServiceFlags1=protoinfo[i].dwServiceFlags1 & (~XP1_IFS_HANDLES);
			break;
		}
	}

	wcscpy(LayerInfo.szProtocol,L"UDPProvider");
	LayerInfo.ProtocolChain.ChainLen=LAYERED_PROTOCOL;

	if(GetCurrentDirectory(MAX_PATH,DllPath)==0)
	{
		return ;
	} 
	wcscat(DllPath,L"\\FireWallDll.dll");

	if(WSCInstallProvider(&UDPguid,DllPath,&LayerInfo,1,&ErrorCode)==SOCKET_ERROR)
	{
		MessageBoxA(NULL,"WSCInstallProvider Error: ",NULL,NULL);
		return ;
	}

	FreeProviders();

	GetProviders();

	for(i=0;i<totalprotos;i++)
	{
		if(memcmp(&protoinfo[i].ProviderId,&UDPguid,sizeof(GUID))==0)
		{
			LayercataId=protoinfo[i].dwCatalogEntryId;
			break;
		}
	}

	provcnt=0;

	wcscpy(UDPchaininfo.szProtocol,L"UDPFW");

	if(UDPchaininfo.ProtocolChain.ChainLen==BASE_PROTOCOL)
	{
		UDPchaininfo.ProtocolChain.ChainEntries[1]=UDPorigcataid;
	}
	else
	{
		for(i=UDPchaininfo.ProtocolChain.ChainLen;i>0;i--)
		{
			UDPchaininfo.ProtocolChain.ChainEntries[i+1]=UDPchaininfo.ProtocolChain.ChainEntries[i];
		}
	}

	UDPchaininfo.ProtocolChain.ChainLen++;
	UDPchaininfo.ProtocolChain.ChainEntries[0]=LayercataId;

	memcpy(&ChainArray[provcnt++],&UDPchaininfo,sizeof(WSAPROTOCOL_INFOW));


	if(WSCInstallProvider(&UDPchainguid,DllPath,ChainArray,provcnt,&ErrorCode)==SOCKET_ERROR)
	{
		return ;
	}

	FreeProviders();

	GetProviders();

	if((CataEntries=(LPDWORD)GlobalAlloc(GPTR,totalprotos*sizeof(WSAPROTOCOL_INFOW)))==NULL)
	{
		return ;
	}

	CataIndex=0;
	for(i=0;i<totalprotos;i++)
	{
		if(memcmp(&protoinfo[i].ProviderId,&UDPguid,sizeof(GUID))==0 
			|| memcmp(&protoinfo[i].ProviderId,&UDPchainguid,sizeof(GUID))==0)
		{
			CataEntries[CataIndex++]=protoinfo[i].dwCatalogEntryId;
		}
	}

	for(i=0;i<totalprotos;i++)
	{
		if(memcmp(&protoinfo[i].ProviderId,&UDPguid,sizeof(GUID))!=0 
			&& memcmp(&protoinfo[i].ProviderId,&UDPchainguid,sizeof(GUID))!=0)
		{
			CataEntries[CataIndex++]=protoinfo[i].dwCatalogEntryId;
		}
	}

	if((ErrorCode==WSCWriteProviderOrder(CataEntries,totalprotos))!=ERROR_SUCCESS)
	{
		return ;
	}

	FreeProviders();

	return ;
}


//////////////////////////////////////////////////////////////////////////
//安装两个服务提供者
//////////////////////////////////////////////////////////////////////////
void InstallProviders()
{
	InstallUDPProviders();
	InstallTCPProviders();
}


//////////////////////////////////////////////////////////////////////////
//拆卸服务提供者
//////////////////////////////////////////////////////////////////////////
void RemoveProviders()
{
	int  errorcode;
	BOOL signal=TRUE;

	if(WSCDeinstallProvider(&UDPguid,&errorcode)==SOCKET_ERROR)
	{
		signal=FALSE;
	}

	if(WSCDeinstallProvider(&UDPchainguid,&errorcode)==SOCKET_ERROR)
	{
		signal=FALSE;
	}
	if (WSCDeinstallProvider(&TCPguid,&errorcode)==SOCKET_ERROR)
	{
		signal = FALSE;
	}
	if (WSCDeinstallProvider(&TCPchainguid,&errorcode)==SOCKET_ERROR)
	{
		signal = FALSE;
	}

	return;
}
