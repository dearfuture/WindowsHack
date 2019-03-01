//////////////////////////////////////////////////////////////////////////
//文件名：CheckControl.cpp
//说明：负责应用程序，IP和端口的检查
//////////////////////////////////////////////////////////////////////////
#include "Header.h"


BOOL CheckPort(USHORT Port)
{
	EnterCriticalSection(&RWSCriticalSection);
	for (int i = 0;i<PortListSize;i++)
	{
		if (Port==PortList[i].Port)
		{
			BOOL Buff = PortList[i].Pass;
			LeaveCriticalSection(&RWSCriticalSection);
			return Buff;
		}
	}
	LeaveCriticalSection(&RWSCriticalSection);
	return TRUE;
}

BOOL CheckAppPath(WCHAR * Path)
{
	EnterCriticalSection(&RWSCriticalSection);
	for (int i = 0;i<PathListSize;i++)
	{
		if (wcscmp(PathList[i].Path,Path)==0)
		{
			BOOL Buff = PathList[i].Pass;
			LeaveCriticalSection(&RWSCriticalSection);
			return Buff;
		}
	}
	LeaveCriticalSection(&RWSCriticalSection);

	WCHAR Buff[512] = {0};
	wcscat(Buff,Path);
	wcscat(Buff,L"正要产生网络连接，是否阻止，点击“是”进行阻止并加入黑名单，点击”否“解除阻止并加入白名单。");
	switch (MessageBoxW(NULL,Buff,L"FireWall",MB_YESNOCANCEL))
	{
	case IDYES:
		{
			PATHLISTITEM ItemBuff;
			wcscpy(ItemBuff.Path,Path);
			ItemBuff.Pass = FALSE;
			AddPathListItem(ItemBuff);
		}
		break;
	case IDNO:
		{
			PATHLISTITEM ItemBuff;
			wcscpy(ItemBuff.Path,Path);
			ItemBuff.Pass = TRUE;
			AddPathListItem(ItemBuff);
		}
		break;
	default:
		break;
	}

	return TRUE;
}



BOOL CheckIp(IN_ADDR Ip)
{
	EnterCriticalSection(&RWSCriticalSection);
	for (int i = 0;i<IpListSize;i++)
	{
		if (memcmp(&IpList[i].Ip,&Ip,sizeof(in_addr))==0)
		{
			BOOL Buff = IpList[i].Pass;
			LeaveCriticalSection(&RWSCriticalSection);
			return Buff;
		}
	}
	LeaveCriticalSection(&RWSCriticalSection);
	return TRUE;
}