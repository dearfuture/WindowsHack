#include "Header.h"
#include "MainFunc.h"

extern HWND hCmdDlg;

//发送远程CMD命令要求
BOOL CmdRequest(SOCKET sockfd,HWND hDlg);
//接收CMD命令反馈信息
BOOL CmdReply(SOCKET sockfd,int dwRecvSize);