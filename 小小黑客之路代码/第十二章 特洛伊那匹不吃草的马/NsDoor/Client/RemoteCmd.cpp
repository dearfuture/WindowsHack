#include "RemoteCmd.h"

HWND hCmdDlg = NULL;
//////////////////////////////////////////////////////////////////////////
//发送远程CMD命令要求
//////////////////////////////////////////////////////////////////////////
BOOL CmdRequest(SOCKET sockfd,HWND hDlg)
{
	hCmdDlg = hDlg;
	char szMsg[1024] = {0};
	int nRet =GetDlgItemText(hDlg,IDC_EDIT1,szMsg,sizeof(szMsg));
	if (nRet==0)
	{
		MessageBox(hDlg,"命令输入有误！",NULL,NULL);
		return FALSE;
	}
	int dwSize = sizeof(RatProto)+nRet+1;
	RatProto ratPro;
	ratPro.RatId = CMD_REQUSET;
	ratPro.RatLen = dwSize;
	char* pData = (char*)malloc(dwSize);
	if (pData)
	{
		memcpy(pData,&ratPro,sizeof(ratPro));
		pData += sizeof(RatProto);
		sprintf(pData,"%s",szMsg);
		pData -= sizeof(RatProto);
		SendCmd(sockfd,pData,dwSize);
	}

	return TRUE;
}



//////////////////////////////////////////////////////////////////////////
//接收CMD命令反馈信息
//////////////////////////////////////////////////////////////////////////
BOOL CmdReply(SOCKET sockfd,int dwRecvSize)
{
	char szText[MAXLENGTH] = {0};
	int nRet = GetDlgItemText(hCmdDlg,IDC_EDIT2,szText,MAXLENGTH);
	char* pText = NULL;
	if (nRet > MAXLENGTH/2)
	{
		pText = szText+MAXLENGTH/2;
		sprintf(szText,"%s",pText);
		nRet = MAXLENGTH/2;
	}
	pText = szRecvCmd+sizeof(RatProto);
	if (nRet==0)
	{
		sprintf(szText,"%s",pText);
	}
	else
	{
		sprintf(szText,"%s\r\n-------------------------------------------------------\r\n%s",szText,pText);
	}

	SetDlgItemText(hCmdDlg,IDC_EDIT2,szText);

	return TRUE;
}