/************************************************************************
*
* ----------------
*  Stub.cpp
* ----------------
* 功能描述：
*     此文件中包含有Stub的功能函数实现，剧集里整个Stub项目的核心逻辑代码。
* 
* ----------
*  版本信息
* ----------
* V1.0.0 Base 2012-07-20 (AI-32)
* 
* ----------
*  版权声明
* ----------
*     此源码为任晓珲(网名:A1Pass)创作，任晓珲(网名:A1Pass)对此享有完全著
* 作权。任何人在未经授权的情况下不得使用此源码，任何人取得使用权后不得删除
* 或更改此版权信息。所有由此代码衍生出来的产品、代码、文件、多媒体，都应注
* 明其应用了本源码中的全部或部分技术信息。
*                      CM. Ren Xiaohui(A1Pass), 2012. All rights reserved.
* 
* ----------
*  提示信息
* ----------
*     此代码隶属于黑客反病毒组织支持的开源项目“A1Pack”，此项目由A1Pass为
* 撰写《黑客免杀攻防》而创立，这个项目的核心思想是创建一个用最简单、最容易
* 维护的代码实现一个功能全面，兼容性良好的加壳产品。
*     此项目共有“A1Pack Base”、“A1Pack Intermediate”与“A1Pack Advanced”
* 三种版本，其中“A1Pack Base”的全部代码与“A1Pack Intermediate”的部分代码
* 将分别以全部开源与半开源的方式用于《黑客免杀攻防》，“A1Pack Advanced”则
* 属于软件项目，不开放源代码。
*     如果您想更轻松更快速的理解本项目的中心逻辑及核心思想，建议您购买
* 《黑客免杀攻防》配合阅读，书中详细的讲解了此项目的核心思想与一些代码的逻辑
* 细节。
*
* ----------
*  免责声明
* ----------
*     此代码虽然经过了详细的测试与分析，但作者本人并不为由于使用此代码对
* 使用者造成的任何直接或间接损失负责。
*     使用者应该具备有一定的公民素质与技术判别能力，作者对于其他人使用此
* 代码进行的任何违法行为不负任何责任；作者对其他人由于技术能力不足而无法
* 正确使用此代码产生的任何后果不负任何责任。
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
* 获取Kernel32.dll的模块基址
*     此函数会通过读取PEB相关信息的方式获取Kernel32.dll的模块基址。但
* 需要注意的是，此函数在NT 6.1内核中获取的将会是KernelBase.dll的模块
* 基址，但是并不会对我们的使用产生太多影响。
*     在NT 6.1中获取基础API函数地址时，我们只需要注意KernelBase.dll
* 没有LoadLibrary，只有LoadLibraryEx这一特点就可以了。
*
* 参数：无
* 
* 返回值：
* DWORD：Kernel32.dll的模块基址。
*********************************************************************/
DWORD GetKernel32Base()
{
	DWORD dwKernel32Addr = 0;
	__asm
	{
		push eax
		mov eax,dword ptr fs:[0x30] // eax = PEB的地址
		mov eax,[eax+0x0C]          // eax = 指向PEB_LDR_DATA结构的指针
		mov eax,[eax+0x1C]          // eax = 模块初始化链表的头指针InInitializationOrderModuleList
		mov eax,[eax]               // eax = 列表中的第二个条目
		mov eax,[eax+0x08]          // eax = 获取到的Kernel32.dll基址（Win7下获取的是KernelBase.dll的基址）
		mov dwKernel32Addr,eax
		pop eax
	}

	return dwKernel32Addr;
}





/*********************************************************************
* 获取GetProcAddress的函数地址
*     此函数会通过读取Kernel32.dll的模块的相关映像信息读取其输出表。然
* 后通过遍历输出表查找“GetProcAddress”，并由此计算得出其函数地址。
*
* 参数：无
* 
* 返回值：
* DWORD：GetProcAddress的函数基址，返回0则代表获取失败。
*********************************************************************/
DWORD GetGPAFunAddr()
{
	DWORD dwAddrBase = GetKernel32Base();

	// 1. 获取DOS头、NT头
	PIMAGE_DOS_HEADER pDos_Header;
	PIMAGE_NT_HEADERS pNt_Header;
	pDos_Header = (PIMAGE_DOS_HEADER)dwAddrBase;
	pNt_Header  = (PIMAGE_NT_HEADERS)(dwAddrBase + pDos_Header->e_lfanew);

	// 2. 获取导出表项
	PIMAGE_DATA_DIRECTORY   pDataDir;
	PIMAGE_EXPORT_DIRECTORY pExport;
	pDataDir = pNt_Header->OptionalHeader.DataDirectory+IMAGE_DIRECTORY_ENTRY_EXPORT;
	pExport  = (PIMAGE_EXPORT_DIRECTORY)(dwAddrBase + pDataDir->VirtualAddress);

	// 3. 获取导出表详细信息
	PDWORD pAddrOfFun      = (PDWORD)(pExport->AddressOfFunctions    + dwAddrBase);
	PDWORD pAddrOfNames    = (PDWORD)(pExport->AddressOfNames        + dwAddrBase);
	PWORD  pAddrOfOrdinals = (PWORD) (pExport->AddressOfNameOrdinals + dwAddrBase);

	// 4. 处理以函数名查找函数地址的请求，循环获取ENT中的函数名，并与传入值对比对，如能匹配上
	//    则在EAT中以指定序号作为索引，并取出其地址值。
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
* 初始化将要用到的API
*     此函数会初始化将要使用的API信息。由于Stub部分在被植入到其他程序中
* 后会丢弃掉导入表信息，因此只能通过动态获取API地址的方式调用API，而这
* 里的“初始化”的意思就是获取各个简要使用的API地址。
*
* 参数：无
* 
* 返回值：
* bool：成功返回true，失败则返回false。
*********************************************************************/
bool InitializationAPI()
{
	HMODULE hModule;

	// 1. 初始化基础API
	g_funGetProcAddress = (LPGETPROCADDRESS)GetGPAFunAddr();
	g_funLoadLibraryEx  = (LPLOADLIBRARYEX)g_funGetProcAddress((HMODULE)GetKernel32Base(),"LoadLibraryExW");

	// 2. 初始化其他API
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
* 解密函数
*     此函数会将加壳程序加密的信息解密，从而使得宿主程序可以正常运行。
*
* 参数：无
* 
* 返回值：无
*********************************************************************/
void Decrypt()
{
	// 在导出的全局变量中读取需解密区域的起始于结束VA
	PBYTE lpStart = g_stcParam.lpStartVA;
	PBYTE lpEnd   = g_stcParam.lpEndVA;

	// 循环解密
	while ( lpStart<lpEnd )
	{
		*lpStart -= 0x88;
		*lpStart ^= 0xA1;
		lpStart++;
	}
}
