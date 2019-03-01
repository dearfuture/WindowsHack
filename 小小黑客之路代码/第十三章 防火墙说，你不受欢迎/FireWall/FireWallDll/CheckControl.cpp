//////////////////////////////////////////////////////////////////////////
//�ļ�����CheckControl.cpp
//˵��������Ӧ�ó���IP�Ͷ˿ڵļ��
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
	wcscat(Buff,L"��Ҫ�����������ӣ��Ƿ���ֹ��������ǡ�������ֹ�������������������񡰽����ֹ�������������");
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