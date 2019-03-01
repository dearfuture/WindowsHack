//////////////////////////////////////////////////////////////////////////
//文件名： RWSDataControl.cpp
//说明：对可读写共享数据的控制函数，提供给UI调用
//////////////////////////////////////////////////////////////////////////
#include "Header.h"


//////////////////////////////////////////////////////////////////////////
//获取一个端口列表条目
//////////////////////////////////////////////////////////////////////////
BOOL GetPortListItem(int i,PORTLISTITEM *Item)
{
	//进入临界区
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
//添加一个端口条目
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
//删除一个端口条目
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
//获取一个IP条目
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
//添加一个IP条目
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
//删除一个IP条目
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
//获取一个应用程序条目
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
//添加一个应用程序条目
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
//删除一个应用程序条目
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