/************************************************************************
*
* ----------------
* ProcessingPE.CPP 
* ----------------
* ����������
*     ���ļ��а�����PE�������ʵ�ֲ��֡�
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
* ��������ַ(RVA)ת�ļ�ƫ��(Offset)
*     �˺������𽫴����RVAת��ΪOffset��
*
* ע�⣺��ת��������δ���ǵ�����ϸ�ڣ������ھ�������������������ת��
*
* ������
* ULONG uRvaAddr��RVA��ֵַ
* 
* ����ֵ��
* DWORD���ɹ�����Offset��ʧ���򷵻�0
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
* �ļ�ƫ��(Offset)ת��������ַ(RVA)
*     �˺������𽫴����Offsetת��ΪRVA��
*
* ע�⣺��ת��������δ���ǵ�����ϸ�ڣ������ھ�������������������ת��
*
* ������
* ULONG uOffsetAddr��Offset��ֵַ
* 
* ����ֵ��
* DWORD���ɹ�����RVA��ַ��ʧ���򷵻�0
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
* ��ȡPE�ļ���Ϣ
*     �˺��������ȡĿ���ļ��Ĺؼ�PE��Ϣ��
*
* ������
* LPVOID   lpImageData��Ŀ���ļ����ڻ�������ָ��
* DWORD    dwImageSize��Ŀ���ļ��Ĵ�С
* PPE_INFO pPeInfo    ��[OUT]���ڴ���Ŀ���ļ��Ĺؼ�PE��Ϣ
* 
* ����ֵ��
* BOOL���ɹ�����true��ʧ���򷵻�false
*********************************************************************/
BOOL CProcessingPE::GetPeInfo(LPVOID lpImageData, DWORD dwImageSize, PPE_INFO pPeInfo)
{
	// 1���ж�ӳ��ָ���Ƿ���Ч
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

	// 2. ��ȡ������Ϣ
	// 2.1 ��ȡDOSͷ��NTͷ
	m_pDos_Header = (PIMAGE_DOS_HEADER)lpImageData;
	m_pNt_Header  = (PIMAGE_NT_HEADERS)((DWORD)lpImageData+m_pDos_Header->e_lfanew);
	// 2.2 ��ȡOEP
	m_stcPeInfo.dwOEP = m_pNt_Header->OptionalHeader.AddressOfEntryPoint;
	// 2.3 ��ȡӳ���ַ
	m_stcPeInfo.dwImageBase = m_pNt_Header->OptionalHeader.ImageBase;
	// 2.4 ��ȡ�ؼ�����Ŀ¼�������
	PIMAGE_DATA_DIRECTORY lpDataDir = m_pNt_Header->OptionalHeader.DataDirectory;
	m_stcPeInfo.pDataDir = lpDataDir;
	CopyMemory(&m_stcPeInfo.stcExport, lpDataDir+IMAGE_DIRECTORY_ENTRY_EXPORT,    sizeof(IMAGE_DATA_DIRECTORY));
	// 2.5 ��ȡ���α���������ϸ��Ϣ
	m_stcPeInfo.pSectionHeader = IMAGE_FIRST_SECTION(m_pNt_Header);

	// 3. ���PE�ļ��Ƿ���Ч
	if ( (m_pDos_Header->e_magic!=IMAGE_DOS_SIGNATURE) || (m_pNt_Header->Signature!=IMAGE_NT_SIGNATURE) )
	{
		// �ⲻ��һ����Ч��PE�ļ�
		return false;
	}

	// 4. ����������
	CopyMemory(pPeInfo, &m_stcPeInfo, sizeof(PE_INFO));

	return true;
}





/*********************************************************************
* �޸��ض�λ��
*     �˺��������޸�ӳ����ض�λ��˺���������RVAToOffset������
* 
* ע�⣺
* 1. dwLoadImageAddrָ�Ĳ������䱾��ImageBase��ֵ�������䱻���غ��Ԥ
*    ��ģ���ַ��
* 2. ���ض�λ������δ���ǵ��޸��������⣬���Ҫ��߼����ԣ�Ӧ�÷ֱ��
*    �����ض�λ���ͽ�������Դ���
*
* ������
* DWORD dwLoadImageAddr����ӳ�񱻼��غ��Ԥ��ģ���ַ
* 
* ����ֵ����
*********************************************************************/
void CProcessingPE::FixReloc(DWORD dwLoadImageAddr)
{
	// 1. ��ȡӳ���ַ������ָ��
	DWORD             dwImageBase;
	PVOID             lpCode;
	dwImageBase = m_pNt_Header->OptionalHeader.ImageBase;
	lpCode      = (PVOID)( (DWORD)m_dwFileDataAddr + RVAToOffset(m_pNt_Header->OptionalHeader.BaseOfCode) );

	// 2. ��ȡ�ض�λ�����ڴ��еĵ�ַ
	PIMAGE_DATA_DIRECTORY  pDataDir;
	PIMAGE_BASE_RELOCATION pReloc;
	pDataDir = m_pNt_Header->OptionalHeader.DataDirectory+IMAGE_DIRECTORY_ENTRY_BASERELOC;
	pReloc   = (PIMAGE_BASE_RELOCATION)((DWORD)m_dwFileDataAddr + RVAToOffset(pDataDir->VirtualAddress));

	// 3. �����ض�λ������Ŀ���������ض�λ
	while ( pReloc->SizeOfBlock && pReloc->SizeOfBlock < 0x100000 )
	{
		// 3.1 ȡ���ض�λ��TypeOffset��������
		PWORD  pTypeOffset = (PWORD)((DWORD)pReloc+sizeof(IMAGE_BASE_RELOCATION));
		DWORD  dwCount     = (pReloc->SizeOfBlock-sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);

		// 3.2 ѭ������ض�λ��
		for ( DWORD i=0; i<dwCount; i++ )
		{
			if ( !*pTypeOffset ) continue;

			// 3.2.1 ��ȡ���ض�λ��ָ���ָ��
			DWORD  dwPointToRVA  = (*pTypeOffset&0x0FFF)+pReloc->VirtualAddress;
			PDWORD pPtr          = (PDWORD)(RVAToOffset(dwPointToRVA)+(DWORD)m_dwFileDataAddr);
			// 3.2.2 �����ض�λ����ֵ
			DWORD dwIncrement = dwLoadImageAddr - dwImageBase;
			// 3.2.3 �޸����ض�λ�ĵ�ַ����
			*((PDWORD)pPtr) += dwIncrement;
			pTypeOffset++;
		}

		// 3.3 ָ����һ���ض�λ�飬��ʼ��һ��ѭ��
		pReloc = (PIMAGE_BASE_RELOCATION)((DWORD)pReloc + pReloc->SizeOfBlock);
	}
}





/*********************************************************************
* ��ȡPE�ļ���Ϣ
*     �˺��������ȡĿ���ļ��Ĺؼ�PE��Ϣ��
*
* ������
* LPVOID   lpImageData��Ŀ���ļ����ڻ�������ָ��
* DWORD    dwImageSize��Ŀ���ļ��Ĵ�С
* PPE_INFO pPeInfo    ��[OUT]���ڴ���Ŀ���ļ��Ĺؼ�PE��Ϣ
* 
* ����ֵ��
* BOOL���ɹ�����true��ʧ���򷵻�false
*********************************************************************/
PVOID CProcessingPE::GetExpVarAddr(LPCTSTR strVarName)
{
	// 1����ȡ�������ַ����������strVarNameתΪASCII��ʽ������ԱȲ���
	CHAR szVarName[MAX_PATH] = {0};
	PIMAGE_EXPORT_DIRECTORY lpExport = (PIMAGE_EXPORT_DIRECTORY)(m_dwFileDataAddr + RVAToOffset(m_stcPeInfo.stcExport.VirtualAddress));
	WideCharToMultiByte(CP_ACP, NULL, strVarName, -1, szVarName, _countof(szVarName), NULL, FALSE);

	// 2��ѭ����ȡ�����������������������������szVarName���ȶԣ������ͬ����ȡ�����Ӧ�ĺ�����ַ
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
* ������κ���
*     �˺���������Ŀ���ļ������һ���Զ�������Ρ�
*
* ע��
*     �˺�����δ���ǵ�Ŀ�꺯�����ڸ������ݵ�ϸ�����⡣
*
* ������
* LPCTSTR               strName    �������ε�����
* DWORD                 dwSize     �������ε���С���
* DWORD                 dwChara    �������ε�����
* PIMAGE_SECTION_HEADER pNewSection��[OUT]�����εĶνṹָ��
* PDWORD                lpSize     ��[OUT]�����ε����մ�С
* 
* ����ֵ��
* PVOID���ɹ�����ָ�����������������ڴ��ָ��
*********************************************************************/
PVOID CProcessingPE::AddSection(LPCTSTR strName, DWORD dwSize, DWORD dwChara, PIMAGE_SECTION_HEADER pNewSection, PDWORD lpSize)
{
	PIMAGE_SECTION_HEADER pSectionHeader = IMAGE_FIRST_SECTION(m_pNt_Header);

	// 1. ��ȡ������Ϣ
	DWORD dwDosSize  = m_pDos_Header->e_lfanew;
	DWORD dwPeSize   = sizeof(IMAGE_NT_HEADERS32);
	DWORD dwStnSize  = m_pNt_Header->FileHeader.NumberOfSections * sizeof(IMAGE_SECTION_HEADER);
	DWORD dwHeadSize = dwDosSize+dwPeSize+dwStnSize;

	// 2. �����α��м��������ε���Ϣ
	// 2.1 ��ȡ������Ϣ
	CHAR  szVarName[7]    = {0};
	DWORD dwFileAlign     = m_pNt_Header->OptionalHeader.FileAlignment;    // �ļ�����
	DWORD dwSectAlign     = m_pNt_Header->OptionalHeader.SectionAlignment; // ��������   
	WORD  dwNumOfsect     = m_pNt_Header->FileHeader.NumberOfSections;     // ������Ŀ

	// 2.2 ��ȡ���һ�����ε���Ϣ
	IMAGE_SECTION_HEADER stcLastSect = {0};
	CopyMemory(&stcLastSect, &pSectionHeader[dwNumOfsect-1], sizeof(IMAGE_SECTION_HEADER));

	// 2.3 �����������ȼ�����Ӧ��ַ��Ϣ
	DWORD dwVStart = 0;                                                        // �����ַ��ʼλ��
	DWORD dwFStart = stcLastSect.SizeOfRawData + stcLastSect.PointerToRawData; // �ļ���ַ��ʼλ��

	if ( stcLastSect.Misc.VirtualSize%dwSectAlign )
		dwVStart = (stcLastSect.Misc.VirtualSize / dwSectAlign+1) * dwSectAlign + stcLastSect.VirtualAddress;
	else
		dwVStart = (stcLastSect.Misc.VirtualSize / dwSectAlign  ) * dwSectAlign + stcLastSect.VirtualAddress;

	DWORD dwVirtualSize   = 0; // ���������С
	DWORD dwSizeOfRawData = 0; // �����ļ���С
	if ( dwSize%dwSectAlign)
		dwVirtualSize   = (dwSize / dwSectAlign+1) * dwSectAlign;
	else
		dwVirtualSize   = (dwSize / dwSectAlign  ) * dwSectAlign;

	if ( dwSize%dwFileAlign )
		dwSizeOfRawData = (dwSize / dwFileAlign+1) * dwFileAlign;
	else
		dwSizeOfRawData = (dwSize / dwFileAlign  ) * dwFileAlign;

	WideCharToMultiByte(CP_ACP, NULL, strName, -1, szVarName, _countof(szVarName), NULL, FALSE);

	// 2.4 ��װһ���µ�����ͷ
	IMAGE_SECTION_HEADER stcNewSect  = {0};
	CopyMemory(stcNewSect.Name, szVarName, 7);     // ��������
	stcNewSect.Misc.VirtualSize = dwVirtualSize;   // �����С
	stcNewSect.VirtualAddress   = dwVStart;        // �����ַ
	stcNewSect.SizeOfRawData    = dwSizeOfRawData; // �ļ���С
	stcNewSect.PointerToRawData = dwFStart;        // �ļ���ַ
	stcNewSect.Characteristics  = dwChara;         // ��������

	// 2.5 д��ָ��λ��
	CopyMemory( (PVOID)((DWORD)m_dwFileDataAddr+dwHeadSize), &stcNewSect, sizeof(IMAGE_SECTION_HEADER) );

	// 3. �޸�������Ŀ�ֶ�NumberOfSections
	m_pNt_Header->FileHeader.NumberOfSections++;

	// 4. �޸�PE�ļ��ľ���ߴ��ֶ�SizeOfImage
	m_pNt_Header->OptionalHeader.SizeOfImage += dwVirtualSize;

	// 5. ���������ε���ϸ��Ϣ����С���Լ���ֱ�ӷ��ʵĵ�ַ
	CopyMemory(pNewSection, &stcNewSect, sizeof(IMAGE_SECTION_HEADER));
	*lpSize     = dwSizeOfRawData;
	return (PVOID)(m_dwFileDataAddr+dwFStart);
}





/*********************************************************************
* �޸�Ŀ���ļ�OEP
*     �˺��������޸�Ŀ���ļ�OEP��
*
* ������
* DWORD dwOEP����OEP
* 
* ����ֵ����
*********************************************************************/
void CProcessingPE::SetOEP(DWORD dwOEP)
{
	m_pNt_Header->OptionalHeader.AddressOfEntryPoint = dwOEP;
}