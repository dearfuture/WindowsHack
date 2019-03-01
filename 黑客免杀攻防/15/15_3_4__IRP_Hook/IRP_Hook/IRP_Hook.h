/*
*  ���ڿ���ɱ���������Ӵ���
*
*  ��    �ߣ���������A1Pass��
*  ��    �ڣ�2012-12-28
*  ���������http://book.hackav.com  �� http://www.hzbook.com
*  ����΢����http://weibo.com/a1pass �� http://t.qq.com/a1pass
*  ���߲��ͣ�http://a1pass.blog.163.com
*/
#pragma once


/************************************************************************/
/* �Զ����豸���Ƽ�������������                                         */
/************************************************************************/
#define DEVICE_NAME                L"\\Device\\IRPHook"     
#define SYMBOLIC_LINK_NAME         L"\\DosDevices\\IRPHook_SymLink"
#define SYMBOLIC_LINK_GLOBAL_NAME  L"\\DosDevices\\Global\\IRPHook_SymLink"
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

#define MICRO_SECOND  (-10)               // 1΢��
#define MILLI_SECOND  (MICRO_SECOND*1000) // 1����
#define SECOND        (MILLI_SECOND*1000) // 1��



#ifdef __cplusplus
extern "C" {
#endif
/************************************************************************/
/* ͷ�ļ�                                                               */
/************************************************************************/
#include <ntddk.h>
#include <Ntddkbd.h>
#include <devioctl.h>

#include <wdmsec.h> // ��Ҫ�ڹ����а�����;$(DDK_LIB_PATH)\wdmsec.lib��
#include <ntstrsafe.h>


NTSTATUS ObReferenceObjectByName (
	_In_ PUNICODE_STRING ObjectName,
	_In_ ULONG Attributes,
	_In_opt_ PACCESS_STATE AccessState,
	_In_opt_ ACCESS_MASK DesiredAccess,
	_In_ POBJECT_TYPE ObjectType,
	_In_ KPROCESSOR_MODE AccessMode,
	_Inout_opt_ PVOID ParseContext,
	_Out_ PDRIVER_OBJECT *Object );
//extern POBJECT_TYPE IoDriverObjectType;  // Windows NT 5.X
extern POBJECT_TYPE *IoDriverObjectType;   // Windows NT 6.X
/************************************************************************/
/* ȫ�ֱ���������                                                       */
/************************************************************************/
PDRIVER_DISPATCH g_OrigDispatchRead  = nullptr; // ԭ��ǲ����
ULONG            g_uPendingIrpsCount = 0;       // ����δ����IRP��Ŀ
PIRP             g_pPendingIrp       = nullptr; // ���浱ǰpending��IRP
PDRIVER_OBJECT   g_objKbdDriver      = nullptr; // ������������豸����
/************************************************************************/
/* ��������                                                         */
/************************************************************************/
NTSTATUS DriverEntry(IN PDRIVER_OBJECT  objDriver, IN PUNICODE_STRING pRegistryString);
NTSTATUS DispatchCreateClose(IN PDEVICE_OBJECT objDeivce, IN PIRP pIrp);
NTSTATUS DispatchDeviceControl(IN PDEVICE_OBJECT objDeivce, IN PIRP pIrp);
VOID     DriverUnload(IN PDRIVER_OBJECT objDriver);
//----------------------------------------------------------------------//
NTSTATUS IRPHook();
NTSTATUS UnIRPHook();
/************************************************************************/
/* ������ҳ����                                                         */
/************************************************************************/
#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)  // ��ʼ����ɺ�����DriverEntry�����б�����
#pragma alloc_text(PAGE, DispatchCreateClose)
#pragma alloc_text(PAGE, DispatchDeviceControl)
#pragma alloc_text(PAGE, DriverUnload)
//----------------------------------------------------------------------//
#pragma alloc_text(PAGE, IRPHook)
#pragma alloc_text(PAGE, UnIRPHook)
#endif
/************************************************************************/
/* �궨��                                                               */
/************************************************************************/
// ���̷���Ȩ�ޣ�����رս���
#define PROCESS_TERMINATE (0x0001)



#ifdef __cplusplus
}
#endif