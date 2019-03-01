/************************************************************************
*
* ----------------
*  A1Pack_Base.h
* ----------------
* ����������
*     ���ļ��а����мӿǲ���Ҫ�õ��ĺ���������ṹ��������
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


// ���Ա��洫�ݸ�Stub���ֵĲ���
typedef struct _GLOBAL_PARAM
{
	BOOL  bShowMessage; // �Ƿ���ʾ������Ϣ
	DWORD dwOEP;        // ������ڵ�
	PBYTE lpStartVA;    // ��ʼ�����ַ��������������
	PBYTE lpEndVA;      // ���������ַ��������������
}GLOBAL_PARAM,*PGLOBAL_PARAM;


// ����һ��������API��������������ִ�мӿǲ���
A1PACK_BASE_API bool A1Pack_Base(LPWSTR strPath,bool bShowMsg);

// �ӿ�ʱ���õ��ĺ�������
extern void  Pretreatment(PBYTE lpCodeStart, PBYTE lpCodeEnd, PE_INFO stcPeInfo);                                             // Ԥ������
extern DWORD Implantation(LPVOID &lpFileData, DWORD dwSize, CProcessingPE* pobjPE, PE_INFO stcPeInfo, GLOBAL_PARAM stcParam); // Stubֲ�뺯��