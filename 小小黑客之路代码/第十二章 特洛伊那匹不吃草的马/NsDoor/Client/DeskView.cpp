#include "DeskView.h"



//////////////////////////////////////////////////////////////////////////
//发送远程桌面命令
//////////////////////////////////////////////////////////////////////////
BOOL DeskViewRequest(SOCKET sockfd,HWND hDlg)
{
	RatProto ratPro;
	ratPro.RatId = DESK_VIEW;
	ratPro.RatLen = sizeof(RatProto);
	SendCmd(sockfd,(char*)&ratPro,sizeof(RatProto));

	RecvBmpView(sockfd,hDlg);


	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//接收数据
//////////////////////////////////////////////////////////////////////////
BOOL RecvBmpView(SOCKET sockfd,HWND hDlg)
{
	RatProto ratPro;
	//接收数据包头
	BOOL flag = RecvData(sockfd,(char*)(&ratPro),sizeof(RatProto));
	if (!flag)
	{
		return FALSE;
	}
	else if (sizeof(RatProto) > ratPro.RatLen)
	{
		return FALSE;
	}
	//接收内容
	int dwSize = ratPro.RatLen-sizeof(RatProto);
	if (dwSize)
	{
		char* pData = (char*)malloc(dwSize);
		flag = RecvData(sockfd,pData,dwSize);
		if (!flag)
		{
			return FALSE;
		}
		else
		{
			DisplayDeskView(hDlg,pData);
		}
	}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//Bmp显示
//////////////////////////////////////////////////////////////////////////
BOOL DisplayDeskView(HWND hDlg,char* pData)
{
	LPBITMAPFILEHEADER   lpBmfHdr = (LPBITMAPFILEHEADER)pData;
	LPBITMAPINFOHEADER   lpBmiHdr = (LPBITMAPINFOHEADER)(pData+sizeof(BITMAPFILEHEADER));

	HDC hDC = GetDC(hDlg);
	HDC hDC2 = CreateCompatibleDC(hDC);
	HBITMAP hBitmap = CreateCompatibleBitmap(hDC,lpBmiHdr->biWidth,lpBmiHdr->biHeight);
	SelectObject(hDC2,hBitmap);
	ReleaseDC(hDlg,hDC);
	char* pBytes = (char*)lpBmfHdr + lpBmfHdr->bfOffBits;
	SetDIBitsToDevice(hDC2,0,0,lpBmiHdr->biWidth,lpBmiHdr->biHeight,0,0,0,lpBmiHdr->biHeight,pBytes,(BITMAPINFO*)lpBmiHdr,DIB_RGB_COLORS);
	DeleteDC(hDC2);


	HDC dDlgDC = GetDC(hDlg);
	HDC hMemDC = CreateCompatibleDC(dDlgDC);
	HWND hScreen = GetDesktopWindow();
	RECT stRect;
	GetWindowRect(hScreen,&stRect);
	HGDIOBJ hOldBMP = SelectObject(hMemDC,hBitmap);
	//600 400
	StretchBlt(dDlgDC,0,0,600,400,hMemDC,0,0,stRect.right-stRect.left,stRect.bottom-stRect.top,SRCCOPY);
	SelectObject(hMemDC,hOldBMP);
	DeleteObject(hMemDC);
	ReleaseDC(hDlg,dDlgDC);

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//控制接收固定长度数据
//////////////////////////////////////////////////////////////////////////
BOOL RecvData(SOCKET sockfd,char* pData,int dwSize)
{
	while (TRUE)
	{
		fd_set fdset;
		fdset.fd_count = 1;
		fdset.fd_array[0] = sockfd;
		int nRet = select(NULL,&fdset,NULL,NULL,NULL);
		if (nRet == SOCKET_ERROR)
		{
			return FALSE;
		}
		else
		{
			nRet = recv(sockfd,pData,dwSize,NULL);
			if (nRet==SOCKET_ERROR || !nRet)
			{
				return FALSE;
			}
		}
		if (nRet < dwSize)
		{
			pData += nRet;
			dwSize -= nRet;
		}
		else
		{
			break;
		}
	}

	return TRUE;
}