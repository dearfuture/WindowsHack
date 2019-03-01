/************************************************************************
*
* ----------------
*  Stub.h
* ----------------
* 功能描述：
*     此文件中包含有Stub的功能函数声明与其他各种声明，是用于引导Stub.cpp
* 与dllmain.cpp相互调用的桥梁，这里包含有所有Stub中涉及到的各种函数与结构
* 体的声明。
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
#include <Windows.h>


// 声明一个导出的全局变量，用以保存传递给Stub部分的参数
typedef struct _GLOBAL_PARAM
{
	BOOL  bShowMessage; // 是否显示解密信息
	DWORD dwOEP;        // 程序入口点
	PBYTE lpStartVA;    // 起始虚拟地址（被异或加密区）
	PBYTE lpEndVA;      // 结束虚拟地址（被异或加密区）
}GLOBAL_PARAM,*PGLOBAL_PARAM;
extern "C"__declspec(dllexport) GLOBAL_PARAM g_stcParam;


// 基础API定义声明
typedef DWORD (WINAPI *LPGETPROCADDRESS)(HMODULE,LPCSTR);        // GetProcAddress
typedef HMODULE (WINAPI *LPLOADLIBRARYEX)(LPCTSTR,HANDLE,DWORD); // LoadLibaryEx
extern LPGETPROCADDRESS g_funGetProcAddress;
extern LPLOADLIBRARYEX  g_funLoadLibraryEx;


// 其他API定义声明
typedef VOID (WINAPI *LPEXITPROCESS)(UINT);                          // ExitProcess
typedef int (WINAPI *LPMESSAGEBOX)(HWND,LPCTSTR,LPCTSTR,UINT);       // MessageBox
typedef HMODULE (WINAPI *LPGETMODULEHANDLE)(LPCWSTR);                // GetModuleHandle
typedef BOOL (WINAPI *LPVIRTUALPROTECT)(LPVOID,SIZE_T,DWORD,PDWORD); // VirtualProtect
extern LPEXITPROCESS     g_funExitProcess;
extern LPMESSAGEBOX      g_funMessageBox;
extern LPGETMODULEHANDLE g_funGetModuleHandle;
extern LPVIRTUALPROTECT  g_funVirtualProtect;


// 声明功能函数
extern DWORD GetKernel32Base();   // 获取Kernel32.dll的模块基址
extern DWORD GetGPAFunAddr();     // 获取GetProcAddress的函数地址
extern bool  InitializationAPI(); // 初始化各个API
extern void  Decrypt();           // 解密函数
