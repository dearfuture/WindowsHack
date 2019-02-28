#include "stdafx.h"
#include "ScreenCapture.h"


BOOL PaintMouse(HDC hdc)
{
	HDC bufdc = NULL;
	CURSORINFO cursorInfo = { 0 };
	ICONINFO iconInfo = { 0 };
	HBITMAP bmpOldMask = NULL;

	bufdc = ::CreateCompatibleDC(hdc);
	::RtlZeroMemory(&iconInfo, sizeof(iconInfo));
	cursorInfo.cbSize = sizeof(cursorInfo);
	// 获取光标信息
	::GetCursorInfo(&cursorInfo);
	// 获取光标图标信息
	::GetIconInfo(cursorInfo.hCursor, &iconInfo);
	// 绘制 白底黑鼠标(AND)
	bmpOldMask = (HBITMAP)::SelectObject(bufdc, iconInfo.hbmMask);     
	::BitBlt(hdc, cursorInfo.ptScreenPos.x, cursorInfo.ptScreenPos.y, 20, 20,
		bufdc, 0, 0, SRCAND);
	// 绘制 黑底彩色鼠标(OR)
	::SelectObject(bufdc, iconInfo.hbmColor);
	::BitBlt(hdc, cursorInfo.ptScreenPos.x, cursorInfo.ptScreenPos.y, 20, 20,
		bufdc, 0, 0, SRCPAINT);

	// 释放资源
	::SelectObject(bufdc, bmpOldMask);
	::DeleteObject(iconInfo.hbmColor);
	::DeleteObject(iconInfo.hbmMask);
	::DeleteDC(bufdc);
	return TRUE;
}


HBITMAP GetScreenBmp()
{
	// 获取屏幕截屏
	// 获取桌面窗口句柄
	HWND hDesktop = ::GetDesktopWindow();
	// 获取桌面窗口DC
	HDC hdc = ::GetDC(hDesktop);
	// 创建兼容DC
	HDC mdc = ::CreateCompatibleDC(hdc);
	// 获取计算机屏幕的宽和高
	DWORD dwWidth = ::GetSystemMetrics(SM_CXSCREEN);
	DWORD dwHeight = ::GetSystemMetrics(SM_CYSCREEN);
	// 创建兼容位图
	HBITMAP bmp = ::CreateCompatibleBitmap(hdc, dwWidth, dwHeight);
	// 选中位图
	HBITMAP holdbmp = (HBITMAP)::SelectObject(mdc, bmp);
	// 将窗口内容绘制到位图上
	::BitBlt(mdc, 0, 0, dwWidth, dwHeight, hdc, 0, 0, SRCCOPY);

	// 绘制鼠标
	PaintMouse(mdc);

	::SelectObject(mdc, holdbmp);

	// 释放内存
	::DeleteDC(mdc);
	::ReleaseDC(hDesktop, hdc);

	return bmp;
}


BOOL ScreenCapture()
{
	// 获取屏幕截屏位图句柄
	HBITMAP hBmp = GetScreenBmp();
	if (NULL == hBmp)
	{
		return FALSE;
	}
	// 保存为图片
	SaveBmp(hBmp);

	// 释放位图句柄
	::DeleteObject(hBmp);

	return TRUE;
}