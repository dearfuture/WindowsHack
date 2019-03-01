/************************************************************************
*
* ----------------
*  A1Pack_Base.h
* ----------------
* 功能描述：
*     此文件中包含有加壳部分要用到的函数声明与结构体声明。
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
#include "resource.h"
#include "ProcessingPE.h"
#include <stdlib.h>
#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")

#ifdef A1PACK_BASE_EXPORTS
#define A1PACK_BASE_API __declspec(dllexport)
#else
#define A1PACK_BASE_API __declspec(dllimport)
#endif


// 用以保存传递给Stub部分的参数
typedef struct _GLOBAL_PARAM
{
	BOOL  bShowMessage; // 是否显示解密信息
	DWORD dwOEP;        // 程序入口点
	PBYTE lpStartVA;    // 起始虚拟地址（被异或加密区）
	PBYTE lpEndVA;      // 结束虚拟地址（被异或加密区）
}GLOBAL_PARAM,*PGLOBAL_PARAM;


// 声明一个导出的API，共界面程序调用执行加壳操作
A1PACK_BASE_API bool A1Pack_Base(LPWSTR strPath,bool bShowMsg);

// 加壳时会用到的函数声明
extern void  Pretreatment(PBYTE lpCodeStart, PBYTE lpCodeEnd, PE_INFO stcPeInfo);                                             // 预处理函数
extern DWORD Implantation(LPVOID &lpFileData, DWORD dwSize, CProcessingPE* pobjPE, PE_INFO stcPeInfo, GLOBAL_PARAM stcParam); // Stub植入函数