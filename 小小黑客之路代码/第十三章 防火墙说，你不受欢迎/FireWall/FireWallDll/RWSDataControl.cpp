//////////////////////////////////////////////////////////////////////////
//�ļ����� RWSDataControl.cpp
//˵�����Կɶ�д�������ݵĿ��ƺ������ṩ��UI����
//////////////////////////////////////////////////////////////////////////
#include "Header.h"


//////////////////////////////////////////////////////////////////////////
//��ȡһ���˿��б���Ŀ
//////////////////////////////////////////////////////////////////////////
BOOL GetPortListItem(int i,PORTLISTITEM *Item)
{
	//�����ٽ���
	EnterCriticalSection(&RWSCriticalSection);
	if (i<PortListSize)
	{
		*Item = PortList[i];
		LeaveCriticalSection(&RWSCriticalSection);
		return TRUE;
	}
	LeaveCriticalSection(&RWSCriticalSection);
	return FALSE;
}


//////////////////////////////////////////////////////////////////////////
//���һ���˿���Ŀ
//////////////////////////////////////////////////////////////////////////
BOOL AddPortListItem(PORTLISTITEM Item)
{
	EnterCriticalSection(&RWSCriticalSection);
	PortList[PortListSize] = Item;
	PortListSize++;
	LeaveCriticalSection(&RWSCriticalSection);
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//ɾ��һ���˿���Ŀ
//////////////////////////////////////////////////////////////////////////
BOOL DeletPortListItem(int i)
{
	EnterCriticalSection(&RWSCriticalSection);
	if (i<PortListSize)
	{
		for (int j = PortListSize-1;j>i;j--)
		{
			PortList[j-1]=PortList[j];
		}
		PortListSize--;
		LeaveCriticalSection(&RWSCriticalSection);
		return TRUE;
	}

	LeaveCriticalSection(&RWSCriticalSection);
	return FALSE;

}


//////////////////////////////////////////////////////////////////////////
//��ȡһ��IP��Ŀ
//////////////////////////////////////////////////////////////////////////
BOOL GetIpListItem(int i,IPLISTITEM *Item)
{
	EnterCriticalSection(&RWSCriticalSection);
	if (i<IpListSize)
	{
		*Item = IpList[i];
		LeaveCriticalSection(&RWSCriticalSection);
		return TRUE;
	}
	LeaveCriticalSection(&RWSCriticalSection);
	return FALSE;
}


//////////////////////////////////////////////////////////////////////////
//���һ��IP��Ŀ
//////////////////////////////////////////////////////////////////////////
BOOL AddIpListItem(IPLISTITEM Item)
{
	EnterCriticalSection(&RWSCriticalSection);
	IpList[IpListSize] = Item;
	IpListSize++;
	LeaveCriticalSection(&RWSCriticalSection);
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//ɾ��һ��IP��Ŀ
//////////////////////////////////////////////////////////////////////////
BOOL DeletIpListItem(int i)
{
	EnterCriticalSection(&RWSCriticalSection);
	if (i<IpListSize)
	{
		for (int j = IpListSize-1;j>i;j--)
		{
			IpList[j-1] = IpList[j];
		}
		IpListSize--;
		LeaveCriticalSection(&RWSCriticalSection);
		return TRUE;
	}

	LeaveCriticalSection(&RWSCriticalSection);
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
//��ȡһ��Ӧ�ó�����Ŀ
//////////////////////////////////////////////////////////////////////////
BOOL GetPathListItem(int i,PATHLISTITEM *Item)
{
	EnterCriticalSection(&RWSCriticalSection);
	if (i<PathListSize)
	{
		*Item = PathList[i];
		LeaveCriticalSection(&RWSCriticalSection);
		return TRUE;
	}
	LeaveCriticalSection(&RWSCriticalSection);
	return FALSE;
}


//////////////////////////////////////////////////////////////////////////
//���һ��Ӧ�ó�����Ŀ
//////////////////////////////////////////////////////////////////////////
BOOL AddPathListItem(PATHLISTITEM Item)
{
	EnterCriticalSection(&RWSCriticalSection);
	PathList[PathListSize] = Item;
	PathListSize++;
	LeaveCriticalSection(&RWSCriticalSection);
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//ɾ��һ��Ӧ�ó�����Ŀ
//////////////////////////////////////////////////////////////////////////
BOOL DeletPathListItem(int i)
{
	EnterCriticalSection(&RWSCriticalSection);
	if (i<PathListSize)
	{
		for (int j=PathListSize-1;j>i;j--)
		{
			PathList[j-1] = PathList[j];
		}
		PathListSize--;
		LeaveCriticalSection(&RWSCriticalSection);
		return TRUE;
	}
	LeaveCriticalSection(&RWSCriticalSection);
	return FALSE;
}



BOOL InitRWSCriticalSection()
{
	InitializeCriticalSection(&RWSCriticalSection);
	return TRUE;
}