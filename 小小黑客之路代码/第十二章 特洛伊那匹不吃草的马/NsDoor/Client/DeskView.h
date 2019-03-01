#include "Header.h"
#include "MainFunc.h"

//发送远程桌面命令
BOOL DeskViewRequest(SOCKET sockfd,HWND hDlg);
//接收数据
BOOL RecvBmpView(SOCKET sockfd,HWND hDlg);
//Bmp显示
BOOL DisplayDeskView(HWND hDlg,char* pData);
//控制接收固定长度数据
BOOL RecvData(SOCKET sockfd,char* pData,int dwSize);
