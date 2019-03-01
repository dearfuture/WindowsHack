/************************************************************************
*
* ----------------
*  Stub.h
* ----------------
* ����������
*     ���ļ��а�����Stub�Ĺ��ܺ���������������������������������Stub.cpp
* ��dllmain.cpp�໥���õ��������������������Stub���漰���ĸ��ֺ�����ṹ
* ���������
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
#include <Windows.h>


// ����һ��������ȫ�ֱ��������Ա��洫�ݸ�Stub���ֵĲ���
typedef struct _GLOBAL_PARAM
{
	BOOL  bShowMessage; // �Ƿ���ʾ������Ϣ
	DWORD dwOEP;        // ������ڵ�
	PBYTE lpStartVA;    // ��ʼ�����ַ��������������
	PBYTE lpEndVA;      // ���������ַ��������������
}GLOBAL_PARAM,*PGLOBAL_PARAM;
extern "C"__declspec(dllexport) GLOBAL_PARAM g_stcParam;


// ����API��������
typedef DWORD (WINAPI *LPGETPROCADDRESS)(HMODULE,LPCSTR);        // GetProcAddress
typedef HMODULE (WINAPI *LPLOADLIBRARYEX)(LPCTSTR,HANDLE,DWORD); // LoadLibaryEx
extern LPGETPROCADDRESS g_funGetProcAddress;
extern LPLOADLIBRARYEX  g_funLoadLibraryEx;


// ����API��������
typedef VOID (WINAPI *LPEXITPROCESS)(UINT);                          // ExitProcess
typedef int (WINAPI *LPMESSAGEBOX)(HWND,LPCTSTR,LPCTSTR,UINT);       // MessageBox
typedef HMODULE (WINAPI *LPGETMODULEHANDLE)(LPCWSTR);                // GetModuleHandle
typedef BOOL (WINAPI *LPVIRTUALPROTECT)(LPVOID,SIZE_T,DWORD,PDWORD); // VirtualProtect
extern LPEXITPROCESS     g_funExitProcess;
extern LPMESSAGEBOX      g_funMessageBox;
extern LPGETMODULEHANDLE g_funGetModuleHandle;
extern LPVIRTUALPROTECT  g_funVirtualProtect;


// �������ܺ���
extern DWORD GetKernel32Base();   // ��ȡKernel32.dll��ģ���ַ
extern DWORD GetGPAFunAddr();     // ��ȡGetProcAddress�ĺ�����ַ
extern bool  InitializationAPI(); // ��ʼ������API
extern void  Decrypt();           // ���ܺ���
