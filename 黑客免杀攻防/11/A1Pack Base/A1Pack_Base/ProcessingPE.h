/************************************************************************
*
* ----------------
*  ProcessingPE.h 
* ----------------
* ����������
*     ���ļ��а�����PE��������������֡�
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
#pragma once
#include <Windows.h>
#include <string.h>
#include <stdlib.h>


// �ؼ�PE��Ϣ
typedef struct _PE_INFO 
{
	DWORD                 dwOEP;          // ��ڵ�
	DWORD                 dwImageBase;    // ӳ���ַ
	PIMAGE_DATA_DIRECTORY pDataDir;       // ����Ŀ¼ָ��
	IMAGE_DATA_DIRECTORY  stcExport;      // ����Ŀ¼
	PIMAGE_SECTION_HEADER pSectionHeader; // ���α�ͷ��ָ��
}PE_INFO,*PPE_INFO;





class CProcessingPE
{
public:
	CProcessingPE(void);
	~CProcessingPE(void);

public:
	DWORD RVAToOffset(ULONG uRvaAddr);                                        // RVAת�ļ�ƫ��
	DWORD OffsetToRVA(ULONG uOffsetAddr);                                     // �ļ�ƫ��תRVA
	BOOL  GetPeInfo(LPVOID lpImageData, DWORD dwImageSize, PPE_INFO pPeInfo); // ��ȡPE�ļ�����Ϣ
	void  FixReloc(DWORD dwLoadImageAddr);                                    // �޸��ض�λ��Ϣ
	PVOID GetExpVarAddr(LPCTSTR strVarName);                                  // ��ȡ����ȫ�ֱ������ļ�ƫ��
	void  SetOEP(DWORD dwOEP);                                                // ������OEP
	PVOID AddSection(LPCTSTR strName, DWORD dwSize, DWORD dwChara, PIMAGE_SECTION_HEADER pNewSection, PDWORD lpSize); // �������

private:
	DWORD             m_dwFileDataAddr; // Ŀ���ļ����ڻ������ĵ�ַ
	DWORD             m_dwFileDataSize; // Ŀ���ļ���С
	PIMAGE_DOS_HEADER m_pDos_Header;    // DOSͷָ��
	PIMAGE_NT_HEADERS m_pNt_Header;     // NTͷָ��

	PE_INFO           m_stcPeInfo;      // PE�ؼ���Ϣ

};

