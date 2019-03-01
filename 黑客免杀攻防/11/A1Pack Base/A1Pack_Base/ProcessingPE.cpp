/************************************************************************
*
* ----------------
* ProcessingPE.CPP 
* ----------------
* 功能描述：
*     此文件中包含有PE处理类的实现部分。
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
#include "StdAfx.h"
#include "ProcessingPE.h"





CProcessingPE::CProcessingPE(void)
{
	ZeroMemory(&m_stcPeInfo, sizeof(PE_INFO));
}
CProcessingPE::~CProcessingPE(void)
{
}





/*********************************************************************
* 相对虚拟地址(RVA)转文件偏移(Offset)
*     此函数负责将传入的RVA转换为Offset。
*
* 注意：此转换函数并未考虑到所有细节，但是在绝大多数情况可以正常运转。
*
* 参数：
* ULONG uRvaAddr：RVA地址值
* 
* 返回值：
* DWORD：成功返回Offset，失败则返回0
*********************************************************************/
DWORD CProcessingPE::RVAToOffset(ULONG uRvaAddr)
{
	PIMAGE_SECTION_HEADER pSectionHeader = IMAGE_FIRST_SECTION(m_pNt_Header);

	for (DWORD i=0; i<m_pNt_Header->FileHeader.NumberOfSections; i++)
	{
		if((pSectionHeader[i].VirtualAddress <= uRvaAddr) && (pSectionHeader[i].VirtualAddress + pSectionHeader[i].SizeOfRawData > uRvaAddr))
		{
			return (pSectionHeader[i].PointerToRawData + (uRvaAddr - pSectionHeader[i].VirtualAddress));
		}
	}

	return 0;
}





/*********************************************************************
* 文件偏移(Offset)转相对虚拟地址(RVA)
*     此函数负责将传入的Offset转换为RVA。
*
* 注意：此转换函数并未考虑到所有细节，但是在绝大多数情况可以正常运转。
*
* 参数：
* ULONG uOffsetAddr：Offset地址值
* 
* 返回值：
* DWORD：成功返回RVA地址，失败则返回0
*********************************************************************/
DWORD CProcessingPE::OffsetToRVA(ULONG uOffsetAddr)
{
	PIMAGE_SECTION_HEADER pSectionHeader = IMAGE_FIRST_SECTION(m_pNt_Header);

	for (DWORD i=0; i<m_pNt_Header->FileHeader.NumberOfSections; i++)
	{
		if((pSectionHeader[i].PointerToRawData <= uOffsetAddr) && (pSectionHeader[i].PointerToRawData + pSectionHeader[i].SizeOfRawData > uOffsetAddr))
		{
			return (pSectionHeader[i].VirtualAddress + (uOffsetAddr - pSectionHeader[i].PointerToRawData));
		}
	}

	return 0;
}





/*********************************************************************
* 获取PE文件信息
*     此函数负责获取目标文件的关键PE信息。
*
* 参数：
* LPVOID   lpImageData：目标文件所在缓存区的指针
* DWORD    dwImageSize：目标文件的大小
* PPE_INFO pPeInfo    ：[OUT]用于传出目标文件的关键PE信息
* 
* 返回值：
* BOOL：成功返回true，失败则返回false
*********************************************************************/
BOOL CProcessingPE::GetPeInfo(LPVOID lpImageData, DWORD dwImageSize, PPE_INFO pPeInfo)
{
	// 1、判断映像指针是否有效
	if ( m_stcPeInfo.dwOEP )
	{
		CopyMemory(pPeInfo, &m_stcPeInfo, sizeof(PE_INFO));
		return true;
	} 
	else
	{
		if ( !lpImageData )  return false;
		m_dwFileDataAddr = (DWORD)lpImageData;
		m_dwFileDataSize = dwImageSize;
	}

	// 2. 获取基本信息
	// 2.1 获取DOS头、NT头
	m_pDos_Header = (PIMAGE_DOS_HEADER)lpImageData;
	m_pNt_Header  = (PIMAGE_NT_HEADERS)((DWORD)lpImageData+m_pDos_Header->e_lfanew);
	// 2.2 获取OEP
	m_stcPeInfo.dwOEP = m_pNt_Header->OptionalHeader.AddressOfEntryPoint;
	// 2.3 获取映像基址
	m_stcPeInfo.dwImageBase = m_pNt_Header->OptionalHeader.ImageBase;
	// 2.4 获取关键数据目录表的内容
	PIMAGE_DATA_DIRECTORY lpDataDir = m_pNt_Header->OptionalHeader.DataDirectory;
	m_stcPeInfo.pDataDir = lpDataDir;
	CopyMemory(&m_stcPeInfo.stcExport, lpDataDir+IMAGE_DIRECTORY_ENTRY_EXPORT,    sizeof(IMAGE_DATA_DIRECTORY));
	// 2.5 获取区段表与其他详细信息
	m_stcPeInfo.pSectionHeader = IMAGE_FIRST_SECTION(m_pNt_Header);

	// 3. 检查PE文件是否有效
	if ( (m_pDos_Header->e_magic!=IMAGE_DOS_SIGNATURE) || (m_pNt_Header->Signature!=IMAGE_NT_SIGNATURE) )
	{
		// 这不是一个有效的PE文件
		return false;
	}

	// 4. 传出处理结果
	CopyMemory(pPeInfo, &m_stcPeInfo, sizeof(PE_INFO));

	return true;
}





/*********************************************************************
* 修复重定位项
*     此函数负责修复映像的重定位项，此函数依赖于RVAToOffset函数。
* 
* 注意：
* 1. dwLoadImageAddr指的并非是其本身ImageBase的值，而是其被加载后的预
*    计模块基址。
* 2. 此重定位函数并未考虑到修复类型问题，如果要提高兼容性，应该分别对
*    三种重定位类型进行区别对待。
*
* 参数：
* DWORD dwLoadImageAddr：此映像被加载后的预计模块基址
* 
* 返回值：无
*********************************************************************/
void CProcessingPE::FixReloc(DWORD dwLoadImageAddr)
{
	// 1. 获取映像基址与代码段指针
	DWORD             dwImageBase;
	PVOID             lpCode;
	dwImageBase = m_pNt_Header->OptionalHeader.ImageBase;
	lpCode      = (PVOID)( (DWORD)m_dwFileDataAddr + RVAToOffset(m_pNt_Header->OptionalHeader.BaseOfCode) );

	// 2. 获取重定位表在内存中的地址
	PIMAGE_DATA_DIRECTORY  pDataDir;
	PIMAGE_BASE_RELOCATION pReloc;
	pDataDir = m_pNt_Header->OptionalHeader.DataDirectory+IMAGE_DIRECTORY_ENTRY_BASERELOC;
	pReloc   = (PIMAGE_BASE_RELOCATION)((DWORD)m_dwFileDataAddr + RVAToOffset(pDataDir->VirtualAddress));

	// 3. 遍历重定位表，并对目标代码进行重定位
	while ( pReloc->SizeOfBlock && pReloc->SizeOfBlock < 0x100000 )
	{
		// 3.1 取得重定位项TypeOffset与其数量
		PWORD  pTypeOffset = (PWORD)((DWORD)pReloc+sizeof(IMAGE_BASE_RELOCATION));
		DWORD  dwCount     = (pReloc->SizeOfBlock-sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);

		// 3.2 循环检查重定位项
		for ( DWORD i=0; i<dwCount; i++ )
		{
			if ( !*pTypeOffset ) continue;

			// 3.2.1 获取此重定位项指向的指针
			DWORD  dwPointToRVA  = (*pTypeOffset&0x0FFF)+pReloc->VirtualAddress;
			PDWORD pPtr          = (PDWORD)(RVAToOffset(dwPointToRVA)+(DWORD)m_dwFileDataAddr);
			// 3.2.2 计算重定位增量值
			DWORD dwIncrement = dwLoadImageAddr - dwImageBase;
			// 3.2.3 修复需重定位的地址数据
			*((PDWORD)pPtr) += dwIncrement;
			pTypeOffset++;
		}

		// 3.3 指向下一个重定位块，开始另一次循环
		pReloc = (PIMAGE_BASE_RELOCATION)((DWORD)pReloc + pReloc->SizeOfBlock);
	}
}





/*********************************************************************
* 获取PE文件信息
*     此函数负责获取目标文件的关键PE信息。
*
* 参数：
* LPVOID   lpImageData：目标文件所在缓存区的指针
* DWORD    dwImageSize：目标文件的大小
* PPE_INFO pPeInfo    ：[OUT]用于传出目标文件的关键PE信息
* 
* 返回值：
* BOOL：成功返回true，失败则返回false
*********************************************************************/
PVOID CProcessingPE::GetExpVarAddr(LPCTSTR strVarName)
{
	// 1、获取导出表地址，并将参数strVarName转为ASCII形式，方便对比查找
	CHAR szVarName[MAX_PATH] = {0};
	PIMAGE_EXPORT_DIRECTORY lpExport = (PIMAGE_EXPORT_DIRECTORY)(m_dwFileDataAddr + RVAToOffset(m_stcPeInfo.stcExport.VirtualAddress));
	WideCharToMultiByte(CP_ACP, NULL, strVarName, -1, szVarName, _countof(szVarName), NULL, FALSE);

	// 2、循环读取导出表输出项的输出函数，并依次与szVarName做比对，如果相同，则取出相对应的函数地址
	for (DWORD i=0; i<lpExport->NumberOfNames; i++)
	{
		PDWORD pNameAddr  = (PDWORD)(m_dwFileDataAddr+RVAToOffset(lpExport->AddressOfNames+i));
		PCHAR strTempName = (PCHAR)(m_dwFileDataAddr + RVAToOffset(*pNameAddr));
		if ( !strcmp(szVarName, strTempName) )
		{
			PDWORD pFunAddr = (PDWORD)(m_dwFileDataAddr+RVAToOffset(lpExport->AddressOfFunctions+i));
			return (PVOID)(m_dwFileDataAddr + RVAToOffset(*pFunAddr));
		}
	}
	return 0;
}





/*********************************************************************
* 添加区段函数
*     此函数负责在目标文件中添加一个自定义的区段。
*
* 注：
*     此函数并未考虑到目标函数存在附加数据等细节问题。
*
* 参数：
* LPCTSTR               strName    ：新区段的名称
* DWORD                 dwSize     ：新区段的最小体积
* DWORD                 dwChara    ：新区段的属性
* PIMAGE_SECTION_HEADER pNewSection：[OUT]新区段的段结构指针
* PDWORD                lpSize     ：[OUT]新区段的最终大小
* 
* 返回值：
* PVOID：成功返回指向新区段现在所在内存的指针
*********************************************************************/
PVOID CProcessingPE::AddSection(LPCTSTR strName, DWORD dwSize, DWORD dwChara, PIMAGE_SECTION_HEADER pNewSection, PDWORD lpSize)
{
	PIMAGE_SECTION_HEADER pSectionHeader = IMAGE_FIRST_SECTION(m_pNt_Header);

	// 1. 获取基本信息
	DWORD dwDosSize  = m_pDos_Header->e_lfanew;
	DWORD dwPeSize   = sizeof(IMAGE_NT_HEADERS32);
	DWORD dwStnSize  = m_pNt_Header->FileHeader.NumberOfSections * sizeof(IMAGE_SECTION_HEADER);
	DWORD dwHeadSize = dwDosSize+dwPeSize+dwStnSize;

	// 2. 在区段表中加入新区段的信息
	// 2.1 获取基本信息
	CHAR  szVarName[7]    = {0};
	DWORD dwFileAlign     = m_pNt_Header->OptionalHeader.FileAlignment;    // 文件粒度
	DWORD dwSectAlign     = m_pNt_Header->OptionalHeader.SectionAlignment; // 区段粒度   
	WORD  dwNumOfsect     = m_pNt_Header->FileHeader.NumberOfSections;     // 区段数目

	// 2.2 获取最后一个区段的信息
	IMAGE_SECTION_HEADER stcLastSect = {0};
	CopyMemory(&stcLastSect, &pSectionHeader[dwNumOfsect-1], sizeof(IMAGE_SECTION_HEADER));

	// 2.3 根据区段粒度计算相应地址信息
	DWORD dwVStart = 0;                                                        // 虚拟地址起始位置
	DWORD dwFStart = stcLastSect.SizeOfRawData + stcLastSect.PointerToRawData; // 文件地址起始位置

	if ( stcLastSect.Misc.VirtualSize%dwSectAlign )
		dwVStart = (stcLastSect.Misc.VirtualSize / dwSectAlign+1) * dwSectAlign + stcLastSect.VirtualAddress;
	else
		dwVStart = (stcLastSect.Misc.VirtualSize / dwSectAlign  ) * dwSectAlign + stcLastSect.VirtualAddress;

	DWORD dwVirtualSize   = 0; // 区段虚拟大小
	DWORD dwSizeOfRawData = 0; // 区段文件大小
	if ( dwSize%dwSectAlign)
		dwVirtualSize   = (dwSize / dwSectAlign+1) * dwSectAlign;
	else
		dwVirtualSize   = (dwSize / dwSectAlign  ) * dwSectAlign;

	if ( dwSize%dwFileAlign )
		dwSizeOfRawData = (dwSize / dwFileAlign+1) * dwFileAlign;
	else
		dwSizeOfRawData = (dwSize / dwFileAlign  ) * dwFileAlign;

	WideCharToMultiByte(CP_ACP, NULL, strName, -1, szVarName, _countof(szVarName), NULL, FALSE);

	// 2.4 组装一个新的区段头
	IMAGE_SECTION_HEADER stcNewSect  = {0};
	CopyMemory(stcNewSect.Name, szVarName, 7);     // 区段名称
	stcNewSect.Misc.VirtualSize = dwVirtualSize;   // 虚拟大小
	stcNewSect.VirtualAddress   = dwVStart;        // 虚拟地址
	stcNewSect.SizeOfRawData    = dwSizeOfRawData; // 文件大小
	stcNewSect.PointerToRawData = dwFStart;        // 文件地址
	stcNewSect.Characteristics  = dwChara;         // 区段属性

	// 2.5 写入指定位置
	CopyMemory( (PVOID)((DWORD)m_dwFileDataAddr+dwHeadSize), &stcNewSect, sizeof(IMAGE_SECTION_HEADER) );

	// 3. 修改区段数目字段NumberOfSections
	m_pNt_Header->FileHeader.NumberOfSections++;

	// 4. 修改PE文件的景象尺寸字段SizeOfImage
	m_pNt_Header->OptionalHeader.SizeOfImage += dwVirtualSize;

	// 5. 返回新区段的详细信息、大小，以及可直接访问的地址
	CopyMemory(pNewSection, &stcNewSect, sizeof(IMAGE_SECTION_HEADER));
	*lpSize     = dwSizeOfRawData;
	return (PVOID)(m_dwFileDataAddr+dwFStart);
}





/*********************************************************************
* 修改目标文件OEP
*     此函数负责修改目标文件OEP。
*
* 参数：
* DWORD dwOEP：新OEP
* 
* 返回值：无
*********************************************************************/
void CProcessingPE::SetOEP(DWORD dwOEP)
{
	m_pNt_Header->OptionalHeader.AddressOfEntryPoint = dwOEP;
}