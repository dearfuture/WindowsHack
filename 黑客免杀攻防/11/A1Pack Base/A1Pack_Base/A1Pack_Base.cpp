/************************************************************************
*
* ----------------
* A1Pack_Base.cpp
* ----------------
* 功能描述：
*     此文件中包含有加壳部分的主要逻辑代码（不涉及PE操作）。
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
#include "A1Pack_Base.h"





/*********************************************************************
* 执行加壳操作的主函数
*     此函数是执行加壳操作的主函数，它会根据传入的参数加载并读取目标
* 文件信息，并调用相关处理函数对目标文件进行加壳操作。
*
* 参数：
* LPWSTR strPath ：待加壳文件（目标文件）路径
* bool   bShowMsg：控制加壳后是否显示加密成功的消息框
* 
* 返回值：
* bool：成功返回true，失败则返回false。
*********************************************************************/
A1PACK_BASE_API bool A1Pack_Base(LPWSTR strPath,bool bShowMsg)
{
	CProcessingPE objProcPE; // PE处理对象
	PE_INFO       stcPeInfo; // PE信息

	HANDLE  hFile_In;
	HANDLE  hFile_Out;
	DWORD   dwFileSize;
	LPVOID  lpFileImage;
	WCHAR   szOutPath[MAX_PATH] = {0};

	// 1. 生成输出文件路径
	LPWSTR strSuffix = PathFindExtension(strPath);         // 获取文件的后缀名
	wcsncpy_s(szOutPath,MAX_PATH,strPath,wcslen(strPath)); // 备份目标文件路径到szOutPath
	PathRemoveExtension(szOutPath);                        // 将szOutPath中保存路径的后缀名去掉
	wcscat_s(szOutPath,MAX_PATH,L"_Pack");                 // 在路径最后附加“_Pack”
	wcscat_s(szOutPath,MAX_PATH,strSuffix);                // 在路径最后附加刚刚保存的后缀名
	
	// 2. 获取文件信息，并映射进内存中
	if ( INVALID_HANDLE_VALUE == (hFile_In=CreateFile(strPath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL)) )
	{
		return false;
	}
	if ( INVALID_FILE_SIZE == ( dwFileSize=GetFileSize(hFile_In,NULL)) )
	{
		CloseHandle(hFile_In);
		return false;
	}
	if ( !(lpFileImage=VirtualAlloc(NULL,dwFileSize*2,MEM_COMMIT,PAGE_READWRITE)) )
	{
		CloseHandle(hFile_In);
		return false;
	}
	DWORD dwRet;
	if ( !ReadFile(hFile_In,lpFileImage,dwFileSize,&dwRet,NULL) )
	{
		CloseHandle(hFile_In);
		VirtualFree(lpFileImage,0,MEM_RELEASE);
		return false;
	}

	// 3. 获取PE文件信息
	objProcPE.GetPeInfo(lpFileImage,dwFileSize,&stcPeInfo);

	// 4. 获取目标文件代码段的起始结束信息
	//    读取第一个区段的相关信息，并将其加密（默认第一个区段为代码段）
	PBYTE lpStart   = (PBYTE)(stcPeInfo.pSectionHeader->PointerToRawData+(DWORD)lpFileImage);
	PBYTE lpEnd     = (PBYTE)((DWORD)lpStart+stcPeInfo.pSectionHeader->SizeOfRawData);
	PBYTE lpStartVA = (PBYTE)(stcPeInfo.pSectionHeader->VirtualAddress+stcPeInfo.dwImageBase);
	PBYTE lpEndVA   = (PBYTE)((DWORD)lpStartVA+stcPeInfo.pSectionHeader->SizeOfRawData);

	// 5. 对文件进行预处理
	Pretreatment(lpStart,lpEnd,stcPeInfo);

	// 6. 植入Stub
	DWORD        dwStubSize = 0;
	GLOBAL_PARAM stcParam   = {0};
	stcParam.bShowMessage   = bShowMsg;
	stcParam.dwOEP          = stcPeInfo.dwOEP + stcPeInfo.dwImageBase;
	stcParam.lpStartVA      = lpStartVA;
	stcParam.lpEndVA        = lpEndVA;
	dwStubSize = Implantation(lpFileImage,dwFileSize,&objProcPE,stcPeInfo,stcParam);

	// 7. 将处理完成后的结果写入到新文件中
	if ( INVALID_HANDLE_VALUE != (hFile_Out=CreateFile(szOutPath, GENERIC_WRITE|GENERIC_READ, FILE_SHARE_WRITE|FILE_SHARE_READ, NULL, OPEN_ALWAYS,0,NULL)) )
	{
		DWORD dwRet = 0;
		WriteFile(hFile_Out, lpFileImage, dwStubSize+dwFileSize,&dwRet,NULL);
	}

	// 8. 释放相关资源并返回
	CloseHandle(hFile_In);
	CloseHandle(hFile_Out);
	VirtualFree(lpFileImage,0,MEM_RELEASE);
	return true;
}





/*********************************************************************
* 目标文件预处理函数
*     此函数负责对目标文件做一些预处理工作，包括代码段加密与代码段属
* 性修改等。
*
* 参数：
* PBYTE   lpCodeStart：代码段在现内存中的起始地址
* PBYTE   lpCodeEnd  ：代码段在现内存中的结束地址
* PE_INFO stcPeInfo  ：目标文件的PE信息
* 
* 返回值：无
*********************************************************************/
void Pretreatment(PBYTE lpCodeStart, PBYTE lpCodeEnd, PE_INFO stcPeInfo)
{
	// 1. 加密指定区域
	while ( lpCodeStart<lpCodeEnd )
	{
		*lpCodeStart ^= 0xA1;
		*lpCodeStart += 0x88;
		lpCodeStart++;
	}

    // 2. 给第一个区段附加上可写属性
	PDWORD pChara = &(stcPeInfo.pSectionHeader->Characteristics);
	*pChara = *pChara|IMAGE_SCN_MEM_WRITE;
}





/*********************************************************************
* Stub植入函数
*     此函数负责向目标文件植入Stub的代码部分。
*
* 参数：
* LPVOID         &lpFileData：目标文件所在缓存区的指针
* DWORD          dwSize     ：目标文件的大小
* CProcessingPE* pobjPE     ：目标文件的PE处理对象
* PE_INFO        stcPeInfo  ：目标文件的PE信息
* GLOBAL_PARAM   stcParam   ：传递给Stub部分的参数
* 
* 返回值：
* DWORD：成功返回新添加节的大小，失败则返回0
*********************************************************************/
DWORD Implantation(LPVOID &lpFileData, DWORD dwSize, CProcessingPE* pobjPE, PE_INFO stcPeInfo, GLOBAL_PARAM stcParam)
{
	// 1. 在资源中读取文件内容
	HRSRC   hREC        = NULL; // 资源对象
	HGLOBAL hREC_Handle = NULL; // 资源句柄
	DWORD   dwStubSize  = NULL; // 文件大小
	LPVOID  lpResData   = NULL; // 资源数据指针
	HMODULE hModule     = GetModuleHandle(L"A1Pack_Base.dll");
	if ( !(hREC=FindResource(hModule, MAKEINTRESOURCE(IDR_STUB1), L"STUB")) )  return false;
	if ( !(hREC_Handle=LoadResource(hModule, hREC)) )                          return false;
	if ( !(lpResData=LockResource(hREC_Handle)) )                              return false;
	if ( !(dwStubSize=SizeofResource(hModule, hREC)) )                         return false;

	// 2. 提取Stub部分的关键信息
	CProcessingPE objProcPE;
	PE_INFO       stcStubPeInfo;
	PBYTE         lpData = new BYTE[dwStubSize];
	// 2.1 将Stub复制到临时缓冲区，防止重复操作
	CopyMemory(lpData,lpResData,dwStubSize);
	// 2.2 获取Stub的PE信息
	objProcPE.GetPeInfo(lpData,dwStubSize,&stcStubPeInfo);
	// 2.3 算出代码段的相关信息（默认第一个区段为代码段）
	PBYTE lpText     = (PBYTE)(stcStubPeInfo.pSectionHeader->PointerToRawData+(DWORD)lpData);
	DWORD dwTextSize = stcStubPeInfo.pSectionHeader->SizeOfRawData;

	// 3. 添加区段
	DWORD                 dwNewSectionSize = 0;
	IMAGE_SECTION_HEADER  stcNewSection    = {0};
	PVOID lpNewSectionData = pobjPE->AddSection(L".A1Pass",dwTextSize,IMAGE_SCN_CNT_CODE|IMAGE_SCN_MEM_READ|IMAGE_SCN_MEM_WRITE|IMAGE_SCN_MEM_EXECUTE,&stcNewSection,&dwNewSectionSize);

	// 4. 对Stub部分进行的重定位操作
	//    新的加载地址 = (新区段的起始RVA - Stub的".Text"区段的起始RVA) + 映像基址
	DWORD dwLoadImageAddr = (stcNewSection.VirtualAddress - stcStubPeInfo.pSectionHeader->VirtualAddress) + stcPeInfo.dwImageBase;
	objProcPE.FixReloc(dwLoadImageAddr);

	// 5. 写入配置参数
	// 5.1 获取Stub的导出变量地址
	PVOID lpPatam = objProcPE.GetExpVarAddr(L"g_stcParam");
	// 5.2 保存配置信息到Stub中
	CopyMemory(lpPatam,&stcParam,sizeof(GLOBAL_PARAM));

	// 6. 将Stub复制到新区段中
	CopyMemory(lpNewSectionData,lpText,dwTextSize);

	// 7. 计算并设置新OEP
	DWORD dwNewOEP = 0;
	// 7.1 计算新OEP
	DWORD dwStubOEP       = stcStubPeInfo.dwOEP;
	DWORD dwStubTextRVA   = stcStubPeInfo.pSectionHeader->VirtualAddress;
	DWORD dwNewSectionRVA = stcNewSection.VirtualAddress;
	dwNewOEP = (dwStubOEP-dwStubTextRVA) + dwNewSectionRVA;
	// 7.2 设置新OEP
	pobjPE->SetOEP(dwNewOEP);

	// 8. 释放资源，函数返回
	delete[] lpData;
	FreeResource(hREC_Handle);
	return dwNewSectionSize;
}