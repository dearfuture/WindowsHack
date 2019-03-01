/*
*  ���ڿ���ɱ���������Ӵ���
*
*  ��    �ߣ���������A1Pass��
*  ��    �ڣ�2012-12-25
*  ���������http://book.hackav.com  �� http://www.hzbook.com
*  ����΢����http://weibo.com/a1pass �� http://t.qq.com/a1pass
*  ���߲��ͣ�http://a1pass.blog.163.com
*/
#pragma once


/************************************************************************/
/* �Զ����豸���Ƽ�������������                                         */
/************************************************************************/
#define DEVICE_NAME                L"\\Device\\InlineHook"
#define SYMBOLIC_LINK_NAME         L"\\DosDevices\\InlineHook_SymLink"
#define SYMBOLIC_LINK_GLOBAL_NAME  L"\\DosDevices\\Global\\InlineHook_SymLink"
/************************************************************************/
/* �Զ��������                                                         */
/************************************************************************/
#define IOCTL_BASE        0x800
#define MY_CTL_CODE(i)                                              \
	CTL_CODE                                                        \
	(                                                               \
	FILE_DEVICE_UNKNOWN,  /* �����Ƶ��������� */                    \
	IOCTL_BASE + i,       /* 0x800~0xFFF�ǿ��ɳ���Ա�Զ���Ĳ��� */ \
	METHOD_BUFFERED,      /* ����ģʽ��ʹ�û�������ʽ���� */        \
	FILE_ANY_ACCESS       /* ����Ȩ�ޣ�ȫ�� */                      \
	)
#define IOCTL_HELLO_WORLD      MY_CTL_CODE(0)
#define IOCTL_HOOK_SYSENTER    MY_CTL_CODE(1)
#define IOCTL_UNHOOK_SYSENTER  MY_CTL_CODE(2)





#ifdef __cplusplus
extern "C" {
#endif
/************************************************************************/
/* ͷ�ļ�                                                               */
/************************************************************************/
#include <ntddk.h>
#include <devioctl.h>
#include <wdmsec.h> // ��Ҫ�ڹ����а�����$(DDK_LIB_PATH)\wdmsec.lib��
#include <ntstrsafe.h>
/************************************************************************/
/* ȫ�ֱ���������                                                       */
/************************************************************************/
PMDL               g_pMDL                 = nullptr; // �ڴ���������ָ��
PVOID              g_fnMappedNtCreateFile = 0;       // NtCreateFile����ָ��
ULONG              g_fnNtCreateFile_5     = 0;       // NtCreateFile+5�ĵ�ַ
POBJECT_ATTRIBUTES g_pObjectAttributes    = nullptr; // ����NtCreateFile�Ķ������ԣ��������Ҫ�������ļ�����
/************************************************************************/
/* ��������                                                         */
/************************************************************************/
NTSTATUS DriverEntry(IN PDRIVER_OBJECT  objDriver, IN PUNICODE_STRING pRegistryString);
NTSTATUS DispatchCreateClose(IN PDEVICE_OBJECT objDeivce, IN PIRP pIrp);
NTSTATUS DispatchDeviceControl(IN PDEVICE_OBJECT objDeivce, IN PIRP pIrp);
VOID     DriverUnload(IN PDRIVER_OBJECT objDriver);
//----------------------------------------------------------------------//
ULONG    Detour(POBJECT_ATTRIBUTES pObjectAttributes);
void     Trampline();
NTSTATUS InlineHook();
NTSTATUS UnInlineHook();
/************************************************************************/
/* ������ҳ����                                                         */
/************************************************************************/
#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)  // ��ʼ����ɺ�����DriverEntry�����б�����
#pragma alloc_text(PAGE, DispatchCreateClose)
#pragma alloc_text(PAGE, DispatchDeviceControl)
#pragma alloc_text(PAGE, DriverUnload)
//----------------------------------------------------------------------//
#pragma alloc_text(PAGE, InlineHook)
#pragma alloc_text(PAGE, UnInlineHook)
#endif
/************************************************************************/
/* �궨��                                                               */
/************************************************************************/
// ���̷���Ȩ�ޣ�����رս���
#define PROCESS_TERMINATE (0x0001)

#ifdef __cplusplus
}
#endif