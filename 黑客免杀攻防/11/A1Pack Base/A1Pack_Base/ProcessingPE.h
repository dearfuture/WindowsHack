/************************************************************************
*
* ----------------
*  ProcessingPE.h 
* ----------------
* 功能描述：
*     此文件中包含有PE处理类的声明部分。
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
#pragma once
#include <Windows.h>
#include <string.h>
#include <stdlib.h>


// 关键PE信息
typedef struct _PE_INFO 
{
	DWORD                 dwOEP;          // 入口点
	DWORD                 dwImageBase;    // 映像基址
	PIMAGE_DATA_DIRECTORY pDataDir;       // 数据目录指针
	IMAGE_DATA_DIRECTORY  stcExport;      // 导出目录
	PIMAGE_SECTION_HEADER pSectionHeader; // 区段表头部指针
}PE_INFO,*PPE_INFO;





class CProcessingPE
{
public:
	CProcessingPE(void);
	~CProcessingPE(void);

public:
	DWORD RVAToOffset(ULONG uRvaAddr);                                        // RVA转文件偏移
	DWORD OffsetToRVA(ULONG uOffsetAddr);                                     // 文件偏移转RVA
	BOOL  GetPeInfo(LPVOID lpImageData, DWORD dwImageSize, PPE_INFO pPeInfo); // 获取PE文件的信息
	void  FixReloc(DWORD dwLoadImageAddr);                                    // 修复重定位信息
	PVOID GetExpVarAddr(LPCTSTR strVarName);                                  // 获取导出全局变量的文件偏移
	void  SetOEP(DWORD dwOEP);                                                // 设置新OEP
	PVOID AddSection(LPCTSTR strName, DWORD dwSize, DWORD dwChara, PIMAGE_SECTION_HEADER pNewSection, PDWORD lpSize); // 添加区段

private:
	DWORD             m_dwFileDataAddr; // 目标文件所在缓存区的地址
	DWORD             m_dwFileDataSize; // 目标文件大小
	PIMAGE_DOS_HEADER m_pDos_Header;    // DOS头指针
	PIMAGE_NT_HEADERS m_pNt_Header;     // NT头指针

	PE_INFO           m_stcPeInfo;      // PE关键信息

};

