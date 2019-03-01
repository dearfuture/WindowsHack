/*
*  ���ڿ���ɱ���������Ӵ���
*
*  ��    �ߣ���������A1Pass��
*  ��    �ڣ�2012-12-18
*  ���������http://book.hackav.com  �� http://www.hzbook.com
*  ����΢����http://weibo.com/a1pass �� http://t.qq.com/a1pass
*  ���߲��ͣ�http://a1pass.blog.163.com
*/
#pragma once


/************************************************************************/
/* �Զ����豸���Ƽ�������������                                         */
/************************************************************************/
#define DEVICE_NAME                L"\\Device\\SYSENTER_Hook"
#define SYMBOLIC_LINK_NAME         L"\\DosDevices\\SYSENTER_Hook_SymLink"
#define SYMBOLIC_LINK_GLOBAL_NAME  L"\\DosDevices\\Global\\SYSENTER_Hook_SymLink"
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
ULONG        g_uProtectPID          = 0; // ��Ҫ�������̵�PID
ULONG        g_uOrigKiFastCallEntry = 0; // �������ԭKiFastCallEntry�ĵ�ַ
ULONG        g_uSSDT_Index          = 0; // ��Ҫ�����ں˺�����SSDT������
PCLIENT_ID   g_pClientPID           = 0; // ����ZwOpenProcessʱ�����PID
PACCESS_MASK g_pAccessMask          = 0; // ����ZwOpenProcessʱ�������Ȩ��
/************************************************************************/
/* ��������                                                         */
/************************************************************************/
NTSTATUS DriverEntry(IN PDRIVER_OBJECT  objDriver, IN PUNICODE_STRING pRegistryString);
NTSTATUS DispatchCreateClose(IN PDEVICE_OBJECT objDeivce, IN PIRP pIrp);
NTSTATUS DispatchDeviceControl(IN PDEVICE_OBJECT objDeivce, IN PIRP pIrp);
VOID     DriverUnload(IN PDRIVER_OBJECT objDriver);
//----------------------------------------------------------------------//
VOID     MyKiFastCallEntry();
NTSTATUS HookSYSENTER();
NTSTATUS UnHookSYSENTER();
/************************************************************************/
/* ������ҳ����                                                         */
/************************************************************************/
#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)  // ��ʼ����ɺ�����DriverEntry�����б�����
#pragma alloc_text(PAGE, DispatchCreateClose)
#pragma alloc_text(PAGE, DispatchDeviceControl)
#pragma alloc_text(PAGE, DriverUnload)
//----------------------------------------------------------------------//
#pragma alloc_text(PAGE, HookSYSENTER)
#pragma alloc_text(PAGE, UnHookSYSENTER)
#endif
/************************************************************************/
/* �궨��                                                               */
/************************************************************************/
// ���̷���Ȩ�ޣ�����رս���
#define PROCESS_TERMINATE (0x0001)

#ifdef __cplusplus
}
#endif