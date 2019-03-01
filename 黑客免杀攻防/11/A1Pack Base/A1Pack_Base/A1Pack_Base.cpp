/************************************************************************
*
* ----------------
* A1Pack_Base.cpp
* ----------------
* ����������
*     ���ļ��а����мӿǲ��ֵ���Ҫ�߼����루���漰PE��������
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
#include "A1Pack_Base.h"





/*********************************************************************
* ִ�мӿǲ�����������
*     �˺�����ִ�мӿǲ�������������������ݴ���Ĳ������ز���ȡĿ��
* �ļ���Ϣ����������ش�������Ŀ���ļ����мӿǲ�����
*
* ������
* LPWSTR strPath �����ӿ��ļ���Ŀ���ļ���·��
* bool   bShowMsg�����ƼӿǺ��Ƿ���ʾ���ܳɹ�����Ϣ��
* 
* ����ֵ��
* bool���ɹ�����true��ʧ���򷵻�false��
*********************************************************************/
A1PACK_BASE_API bool A1Pack_Base(LPWSTR strPath,bool bShowMsg)
{
	CProcessingPE objProcPE; // PE�������
	PE_INFO       stcPeInfo; // PE��Ϣ

	HANDLE  hFile_In;
	HANDLE  hFile_Out;
	DWORD   dwFileSize;
	LPVOID  lpFileImage;
	WCHAR   szOutPath[MAX_PATH] = {0};

	// 1. ��������ļ�·��
	LPWSTR strSuffix = PathFindExtension(strPath);         // ��ȡ�ļ��ĺ�׺��
	wcsncpy_s(szOutPath,MAX_PATH,strPath,wcslen(strPath)); // ����Ŀ���ļ�·����szOutPath
	PathRemoveExtension(szOutPath);                        // ��szOutPath�б���·���ĺ�׺��ȥ��
	wcscat_s(szOutPath,MAX_PATH,L"_Pack");                 // ��·����󸽼ӡ�_Pack��
	wcscat_s(szOutPath,MAX_PATH,strSuffix);                // ��·����󸽼Ӹոձ���ĺ�׺��
	
	// 2. ��ȡ�ļ���Ϣ����ӳ����ڴ���
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

	// 3. ��ȡPE�ļ���Ϣ
	objProcPE.GetPeInfo(lpFileImage,dwFileSize,&stcPeInfo);

	// 4. ��ȡĿ���ļ�����ε���ʼ������Ϣ
	//    ��ȡ��һ�����ε������Ϣ����������ܣ�Ĭ�ϵ�һ������Ϊ����Σ�
	PBYTE lpStart   = (PBYTE)(stcPeInfo.pSectionHeader->PointerToRawData+(DWORD)lpFileImage);
	PBYTE lpEnd     = (PBYTE)((DWORD)lpStart+stcPeInfo.pSectionHeader->SizeOfRawData);
	PBYTE lpStartVA = (PBYTE)(stcPeInfo.pSectionHeader->VirtualAddress+stcPeInfo.dwImageBase);
	PBYTE lpEndVA   = (PBYTE)((DWORD)lpStartVA+stcPeInfo.pSectionHeader->SizeOfRawData);

	// 5. ���ļ�����Ԥ����
	Pretreatment(lpStart,lpEnd,stcPeInfo);

	// 6. ֲ��Stub
	DWORD        dwStubSize = 0;
	GLOBAL_PARAM stcParam   = {0};
	stcParam.bShowMessage   = bShowMsg;
	stcParam.dwOEP          = stcPeInfo.dwOEP + stcPeInfo.dwImageBase;
	stcParam.lpStartVA      = lpStartVA;
	stcParam.lpEndVA        = lpEndVA;
	dwStubSize = Implantation(lpFileImage,dwFileSize,&objProcPE,stcPeInfo,stcParam);

	// 7. ��������ɺ�Ľ��д�뵽���ļ���
	if ( INVALID_HANDLE_VALUE != (hFile_Out=CreateFile(szOutPath, GENERIC_WRITE|GENERIC_READ, FILE_SHARE_WRITE|FILE_SHARE_READ, NULL, OPEN_ALWAYS,0,NULL)) )
	{
		DWORD dwRet = 0;
		WriteFile(hFile_Out, lpFileImage, dwStubSize+dwFileSize,&dwRet,NULL);
	}

	// 8. �ͷ������Դ������
	CloseHandle(hFile_In);
	CloseHandle(hFile_Out);
	VirtualFree(lpFileImage,0,MEM_RELEASE);
	return true;
}





/*********************************************************************
* Ŀ���ļ�Ԥ������
*     �˺��������Ŀ���ļ���һЩԤ����������������μ�����������
* ���޸ĵȡ�
*
* ������
* PBYTE   lpCodeStart������������ڴ��е���ʼ��ַ
* PBYTE   lpCodeEnd  ������������ڴ��еĽ�����ַ
* PE_INFO stcPeInfo  ��Ŀ���ļ���PE��Ϣ
* 
* ����ֵ����
*********************************************************************/
void Pretreatment(PBYTE lpCodeStart, PBYTE lpCodeEnd, PE_INFO stcPeInfo)
{
	// 1. ����ָ������
	while ( lpCodeStart<lpCodeEnd )
	{
		*lpCodeStart ^= 0xA1;
		*lpCodeStart += 0x88;
		lpCodeStart++;
	}

    // 2. ����һ�����θ����Ͽ�д����
	PDWORD pChara = &(stcPeInfo.pSectionHeader->Characteristics);
	*pChara = *pChara|IMAGE_SCN_MEM_WRITE;
}





/*********************************************************************
* Stubֲ�뺯��
*     �˺���������Ŀ���ļ�ֲ��Stub�Ĵ��벿�֡�
*
* ������
* LPVOID         &lpFileData��Ŀ���ļ����ڻ�������ָ��
* DWORD          dwSize     ��Ŀ���ļ��Ĵ�С
* CProcessingPE* pobjPE     ��Ŀ���ļ���PE�������
* PE_INFO        stcPeInfo  ��Ŀ���ļ���PE��Ϣ
* GLOBAL_PARAM   stcParam   �����ݸ�Stub���ֵĲ���
* 
* ����ֵ��
* DWORD���ɹ���������ӽڵĴ�С��ʧ���򷵻�0
*********************************************************************/
DWORD Implantation(LPVOID &lpFileData, DWORD dwSize, CProcessingPE* pobjPE, PE_INFO stcPeInfo, GLOBAL_PARAM stcParam)
{
	// 1. ����Դ�ж�ȡ�ļ�����
	HRSRC   hREC        = NULL; // ��Դ����
	HGLOBAL hREC_Handle = NULL; // ��Դ���
	DWORD   dwStubSize  = NULL; // �ļ���С
	LPVOID  lpResData   = NULL; // ��Դ����ָ��
	HMODULE hModule     = GetModuleHandle(L"A1Pack_Base.dll");
	if ( !(hREC=FindResource(hModule, MAKEINTRESOURCE(IDR_STUB1), L"STUB")) )  return false;
	if ( !(hREC_Handle=LoadResource(hModule, hREC)) )                          return false;
	if ( !(lpResData=LockResource(hREC_Handle)) )                              return false;
	if ( !(dwStubSize=SizeofResource(hModule, hREC)) )                         return false;

	// 2. ��ȡStub���ֵĹؼ���Ϣ
	CProcessingPE objProcPE;
	PE_INFO       stcStubPeInfo;
	PBYTE         lpData = new BYTE[dwStubSize];
	// 2.1 ��Stub���Ƶ���ʱ����������ֹ�ظ�����
	CopyMemory(lpData,lpResData,dwStubSize);
	// 2.2 ��ȡStub��PE��Ϣ
	objProcPE.GetPeInfo(lpData,dwStubSize,&stcStubPeInfo);
	// 2.3 �������ε������Ϣ��Ĭ�ϵ�һ������Ϊ����Σ�
	PBYTE lpText     = (PBYTE)(stcStubPeInfo.pSectionHeader->PointerToRawData+(DWORD)lpData);
	DWORD dwTextSize = stcStubPeInfo.pSectionHeader->SizeOfRawData;

	// 3. �������
	DWORD                 dwNewSectionSize = 0;
	IMAGE_SECTION_HEADER  stcNewSection    = {0};
	PVOID lpNewSectionData = pobjPE->AddSection(L".A1Pass",dwTextSize,IMAGE_SCN_CNT_CODE|IMAGE_SCN_MEM_READ|IMAGE_SCN_MEM_WRITE|IMAGE_SCN_MEM_EXECUTE,&stcNewSection,&dwNewSectionSize);

	// 4. ��Stub���ֽ��е��ض�λ����
	//    �µļ��ص�ַ = (�����ε���ʼRVA - Stub��".Text"���ε���ʼRVA) + ӳ���ַ
	DWORD dwLoadImageAddr = (stcNewSection.VirtualAddress - stcStubPeInfo.pSectionHeader->VirtualAddress) + stcPeInfo.dwImageBase;
	objProcPE.FixReloc(dwLoadImageAddr);

	// 5. д�����ò���
	// 5.1 ��ȡStub�ĵ���������ַ
	PVOID lpPatam = objProcPE.GetExpVarAddr(L"g_stcParam");
	// 5.2 ����������Ϣ��Stub��
	CopyMemory(lpPatam,&stcParam,sizeof(GLOBAL_PARAM));

	// 6. ��Stub���Ƶ���������
	CopyMemory(lpNewSectionData,lpText,dwTextSize);

	// 7. ���㲢������OEP
	DWORD dwNewOEP = 0;
	// 7.1 ������OEP
	DWORD dwStubOEP       = stcStubPeInfo.dwOEP;
	DWORD dwStubTextRVA   = stcStubPeInfo.pSectionHeader->VirtualAddress;
	DWORD dwNewSectionRVA = stcNewSection.VirtualAddress;
	dwNewOEP = (dwStubOEP-dwStubTextRVA) + dwNewSectionRVA;
	// 7.2 ������OEP
	pobjPE->SetOEP(dwNewOEP);

	// 8. �ͷ���Դ����������
	delete[] lpData;
	FreeResource(hREC_Handle);
	return dwNewSectionSize;
}