#include "Header.h"
#include "MainFunc.h"

extern HWND hCmdDlg;

//����Զ��CMD����Ҫ��
BOOL CmdRequest(SOCKET sockfd,HWND hDlg);
//����CMD�������Ϣ
BOOL CmdReply(SOCKET sockfd,int dwRecvSize);