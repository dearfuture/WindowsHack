/************************************************************************
*
* ----------------
*  Stub.cpp
* ----------------
* ����������
*     ���ļ��а�����Stub�Ĺ��ܺ���ʵ�֣��缯������Stub��Ŀ�ĺ����߼����롣
* 
* ----------
*  �汾��Ϣ
* ----------
* V1.0.0 Base 2012-07-20 (AI-32)
* 
* ----------
*  ��Ȩ����
* ----------
*     ��Դ��Ϊ������(����:A1Pass)������������(����:A1Pass)�Դ�������ȫ��
* ��Ȩ���κ�����δ����Ȩ������²���ʹ�ô�Դ�룬�κ���ȡ��ʹ��Ȩ�󲻵�ɾ��
* ����Ĵ˰�Ȩ��Ϣ�������ɴ˴������������Ĳ�Ʒ�����롢�ļ�����ý�壬��Ӧע
* ����Ӧ���˱�Դ���е�ȫ���򲿷ּ�����Ϣ��
*                      CM. Ren Xiaohui(A1Pass), 2012. All rights reserved.
* 
* ----------
*  ��ʾ��Ϣ
* ----------
*     �˴��������ںڿͷ�������֧֯�ֵĿ�Դ��Ŀ��A1Pack��������Ŀ��A1PassΪ
* ׫д���ڿ���ɱ�������������������Ŀ�ĺ���˼���Ǵ���һ������򵥡�������
* ά���Ĵ���ʵ��һ������ȫ�棬���������õļӿǲ�Ʒ��
*     ����Ŀ���С�A1Pack Base������A1Pack Intermediate���롰A1Pack Advanced��
* ���ְ汾�����С�A1Pack Base����ȫ�������롰A1Pack Intermediate���Ĳ��ִ���
* ���ֱ���ȫ����Դ��뿪Դ�ķ�ʽ���ڡ��ڿ���ɱ����������A1Pack Advanced����
* ���������Ŀ��������Դ���롣
*     �����������ɸ����ٵ���Ȿ��Ŀ�������߼�������˼�룬����������
* ���ڿ���ɱ����������Ķ���������ϸ�Ľ����˴���Ŀ�ĺ���˼����һЩ������߼�
* ϸ�ڡ�
*
* ----------
*  ��������
* ----------
*     �˴�����Ȼ��������ϸ�Ĳ���������������߱��˲���Ϊ����ʹ�ô˴����
* ʹ������ɵ��κ�ֱ�ӻ�����ʧ����
*     ʹ����Ӧ�þ߱���һ���Ĺ��������뼼���б����������߶���������ʹ�ô�
* ������е��κ�Υ����Ϊ�����κ����Σ����߶����������ڼ�������������޷�
* ��ȷʹ�ô˴���������κκ�������κ����Ρ�
* 
************************************************************************/
#include "stdafx.h"
#include "Stub.h"

extern __declspec(dllexport) GLOBAL_PARAM g_stcParam={0};
LPGETPROCADDRESS  g_funGetProcAddress  = nullptr;
LPLOADLIBRARYEX   g_funLoadLibraryEx   = nullptr;

LPEXITPROCESS     g_funExitProcess     = nullptr;
LPMESSAGEBOX      g_funMessageBox      = nullptr;
LPGETMODULEHANDLE g_funGetModuleHandle = nullptr;
LPVIRTUALPROTECT  g_funVirtualProtect  = nullptr;





/*********************************************************************
* ��ȡKernel32.dll��ģ���ַ
*     �˺�����ͨ����ȡPEB�����Ϣ�ķ�ʽ��ȡKernel32.dll��ģ���ַ����
* ��Ҫע����ǣ��˺�����NT 6.1�ں��л�ȡ�Ľ�����KernelBase.dll��ģ��
* ��ַ�����ǲ���������ǵ�ʹ�ò���̫��Ӱ�졣
*     ��NT 6.1�л�ȡ����API������ַʱ������ֻ��Ҫע��KernelBase.dll
* û��LoadLibrary��ֻ��LoadLibraryEx��һ�ص�Ϳ����ˡ�
*
* ��������
* 
* ����ֵ��
* DWORD��Kernel32.dll��ģ���ַ��
*********************************************************************/
DWORD GetKernel32Base()
{
	DWORD dwKernel32Addr = 0;
	__asm
	{
		push eax
		mov eax,dword ptr fs:[0x30] // eax = PEB�ĵ�ַ
		mov eax,[eax+0x0C]          // eax = ָ��PEB_LDR_DATA�ṹ��ָ��
		mov eax,[eax+0x1C]          // eax = ģ���ʼ�������ͷָ��InInitializationOrderModuleList
		mov eax,[eax]               // eax = �б��еĵڶ�����Ŀ
		mov eax,[eax+0x08]          // eax = ��ȡ����Kernel32.dll��ַ��Win7�»�ȡ����KernelBase.dll�Ļ�ַ��
		mov dwKernel32Addr,eax
		pop eax
	}

	return dwKernel32Addr;
}





/*********************************************************************
* ��ȡGetProcAddress�ĺ�����ַ
*     �˺�����ͨ����ȡKernel32.dll��ģ������ӳ����Ϣ��ȡ�������Ȼ
* ��ͨ�������������ҡ�GetProcAddress�������ɴ˼���ó��亯����ַ��
*
* ��������
* 
* ����ֵ��
* DWORD��GetProcAddress�ĺ�����ַ������0������ȡʧ�ܡ�
*********************************************************************/
DWORD GetGPAFunAddr()
{
	DWORD dwAddrBase = GetKernel32Base();

	// 1. ��ȡDOSͷ��NTͷ
	PIMAGE_DOS_HEADER pDos_Header;
	PIMAGE_NT_HEADERS pNt_Header;
	pDos_Header = (PIMAGE_DOS_HEADER)dwAddrBase;
	pNt_Header  = (PIMAGE_NT_HEADERS)(dwAddrBase + pDos_Header->e_lfanew);

	// 2. ��ȡ��������
	PIMAGE_DATA_DIRECTORY   pDataDir;
	PIMAGE_EXPORT_DIRECTORY pExport;
	pDataDir = pNt_Header->OptionalHeader.DataDirectory+IMAGE_DIRECTORY_ENTRY_EXPORT;
	pExport  = (PIMAGE_EXPORT_DIRECTORY)(dwAddrBase + pDataDir->VirtualAddress);

	// 3. ��ȡ��������ϸ��Ϣ
	PDWORD pAddrOfFun      = (PDWORD)(pExport->AddressOfFunctions    + dwAddrBase);
	PDWORD pAddrOfNames    = (PDWORD)(pExport->AddressOfNames        + dwAddrBase);
	PWORD  pAddrOfOrdinals = (PWORD) (pExport->AddressOfNameOrdinals + dwAddrBase);

	// 4. �����Ժ��������Һ�����ַ������ѭ����ȡENT�еĺ����������봫��ֵ�Աȶԣ�����ƥ����
	//    ����EAT����ָ�������Ϊ��������ȡ�����ֵַ��
	DWORD dwFunAddr;
	for (DWORD i=0; i<pExport->NumberOfNames; i++)
	{
		PCHAR lpFunName = (PCHAR)(pAddrOfNames[i]+dwAddrBase);
		if ( !strcmp(lpFunName, "GetProcAddress") )
		{
			dwFunAddr = pAddrOfFun[pAddrOfOrdinals[i]] + dwAddrBase;
			break;
		}
		if ( i == pExport->NumberOfNames-1 )
			return 0; 
	}

	return dwFunAddr;
}





/*********************************************************************
* ��ʼ����Ҫ�õ���API
*     �˺������ʼ����Ҫʹ�õ�API��Ϣ������Stub�����ڱ�ֲ�뵽����������
* ��ᶪ�����������Ϣ�����ֻ��ͨ����̬��ȡAPI��ַ�ķ�ʽ����API������
* ��ġ���ʼ��������˼���ǻ�ȡ������Ҫʹ�õ�API��ַ��
*
* ��������
* 
* ����ֵ��
* bool���ɹ�����true��ʧ���򷵻�false��
*********************************************************************/
bool InitializationAPI()
{
	HMODULE hModule;

	// 1. ��ʼ������API
	g_funGetProcAddress = (LPGETPROCADDRESS)GetGPAFunAddr();
	g_funLoadLibraryEx  = (LPLOADLIBRARYEX)g_funGetProcAddress((HMODULE)GetKernel32Base(),"LoadLibraryExW");

	// 2. ��ʼ������API
	hModule = NULL;
	if ( !(hModule=g_funLoadLibraryEx(L"kernel32.dll",NULL,NULL)) )  return false;
	g_funExitProcess = (LPEXITPROCESS)g_funGetProcAddress(hModule,"ExitProcess");
	hModule = NULL;
	if ( !(hModule=g_funLoadLibraryEx(L"user32.dll",NULL,NULL)) )  return false;
	g_funMessageBox = (LPMESSAGEBOX)g_funGetProcAddress(hModule,"MessageBoxW");
	hModule = NULL;
	if ( !(hModule=g_funLoadLibraryEx(L"kernel32.dll",NULL,NULL)) )  return false;
	g_funGetModuleHandle = (LPGETMODULEHANDLE)g_funGetProcAddress(hModule,"GetModuleHandleW");
	hModule = NULL;
	if ( !(hModule=g_funLoadLibraryEx(L"kernel32.dll",NULL,NULL)) )  return false;
	g_funVirtualProtect = (LPVIRTUALPROTECT)g_funGetProcAddress(hModule,"VirtualProtect");

	return true;
}





/*********************************************************************
* ���ܺ���
*     �˺����Ὣ�ӿǳ�����ܵ���Ϣ���ܣ��Ӷ�ʹ��������������������С�
*
* ��������
* 
* ����ֵ����
*********************************************************************/
void Decrypt()
{
	// �ڵ�����ȫ�ֱ����ж�ȡ������������ʼ�ڽ���VA
	PBYTE lpStart = g_stcParam.lpStartVA;
	PBYTE lpEnd   = g_stcParam.lpEndVA;

	// ѭ������
	while ( lpStart<lpEnd )
	{
		*lpStart -= 0x88;
		*lpStart ^= 0xA1;
		lpStart++;
	}
}
