/*
*  ���ڿ���ɱ���������Ӵ���
*
*  ��    �ߣ���������A1Pass��
*  ��    �ڣ�2012-12-21
*  ���������http://book.hackav.com  �� http://www.hzbook.com
*  ����΢����http://weibo.com/a1pass �� http://t.qq.com/a1pass
*  ���߲��ͣ�http://a1pass.blog.163.com
*/
/************************************************************************/
/* �Զ����豸���Ƽ�������������                                         */
/************************************************************************/
#define DEVICE_NAME                L"\\Device\\SSDTHook"
#define SYMBOLIC_LINK_NAME         L"\\DosDevices\\SSDTHook_SymLink"
#define SYMBOLIC_LINK_GLOBAL_NAME  L"\\DosDevices\\Global\\SSDTHook_SymLink"
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
#define IOCTL_HELLO_WORLD  MY_CTL_CODE(0)
#define IOCTL_HOOK_SSDT    MY_CTL_CODE(1)
#define IOCTL_UNHOOK_SSDT  MY_CTL_CODE(2)





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
/* ����ȫ������                                                         */
/************************************************************************/
#pragma pack(1)
typedef struct _ServiceDesriptorEntry
{
	ULONG *ServiceTableBase;
	ULONG *ServiceCounterTableBase;
	ULONG NumberOfServices;
	UCHAR *ParamTableBase;
}SSDTEntry,*PSSDTEntry;
#pragma pack()
typedef NTSTATUS (*_ZwOpenProcess)(OUT PHANDLE, IN ACCESS_MASK, IN POBJECT_ATTRIBUTES, IN PCLIENT_ID OPTIONAL);
/************************************************************************/
/* ȫ�ֱ���������                                                       */
/************************************************************************/
ULONG              g_uProtectPID = 0;        // ��Ҫ�������̵�PID
PMDL               g_pMdlSystemCall;         // SSDT��MDL��ָ��
PVOID*             g_pMappedSystemCallTable; // SSDT��MDL�����ַָ��
_ZwOpenProcess     g_funOrigZwOpenProcess;   // ���庯��ԭ��
NTSYSAPI SSDTEntry KeServiceDescriptorTable; // ����SSDT
/************************************************************************/
/* ��������                                                         */
/************************************************************************/
NTSTATUS DriverEntry(IN PDRIVER_OBJECT  objDriver, IN PUNICODE_STRING pRegistryString);
NTSTATUS DispatchCreateClose(IN PDEVICE_OBJECT objDeivce, IN PIRP pIrp);
NTSTATUS DispatchDeviceControl(IN PDEVICE_OBJECT objDeivce, IN PIRP pIrp);
VOID     SSDTHookUnload(IN PDRIVER_OBJECT objDriver);
//----------------------------------------------------------------------//
NTSYSAPI NTSTATUS NTAPI ZwOpenProcess(OUT PHANDLE ProcessHandle,IN ACCESS_MASK DesiredAccess,IN POBJECT_ATTRIBUTES ObjectName,IN PCLIENT_ID ClientId OPTIONAL);
NTSTATUS MyZwOpenProcess(OUT PHANDLE ProcessHandle,IN ACCESS_MASK DesiredAccess,IN POBJECT_ATTRIBUTES ObjectName,IN PCLIENT_ID ClientId OPTIONAL);
NTSTATUS HookSSDT();
NTSTATUS UnHookSSDT();
/************************************************************************/
/* ������ҳ����                                                         */
/************************************************************************/
#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)  // ��ʼ����ɺ�����DriverEntry�����б�����
#pragma alloc_text(PAGE, DispatchCreateClose)
#pragma alloc_text(PAGE, DispatchDeviceControl)
#pragma alloc_text(PAGE, SSDTHookUnload)

#pragma alloc_text(PAGE, MyZwOpenProcess)
#pragma alloc_text(PAGE, HookSSDT)
#pragma alloc_text(PAGE, UnHookSSDT)
#endif
/************************************************************************/
/* �궨��                                                               */
/************************************************************************/

/* ���ݺ�����ַ��ȡ����� */
#define SYSCALL_INDEX(_function)   \
	(ULONG)*(PULONG)((PUCHAR)_function+1)

/* ��ӳ���ַ��ȡ����Ӧ�ĺ�����ַ */
#define GET_FUN_FOR_MAPPED(_function)  \
	(PLONG)&g_pMappedSystemCallTable[SYSCALL_INDEX(_function)]

/* ��סϵͳ���� */
#define HOOK_SYSCALL(_function,    /* δ���滻��ϵͳ���� */ \
	                 _MyFun,       /* ���ǵ��滻���� */     \
	                 _OrigFun,     /* ����ϵͳ����ָ�� */   \
	                 _OrigFunType) /* ϵͳ����ָ������ */   \
	_OrigFun = (_OrigFunType)InterlockedExchange(GET_FUN_FOR_MAPPED(_function), (ULONG)_MyFun)

/* �ѹ�ϵͳ���� */
#define UNHOOK_SYSCALL(_function,  /* �ѱ��滻��ϵͳ���� */ \
	                   _OrigFun )  /* ����ϵͳ����ָ�� */   \
	InterlockedExchange(GET_FUN_FOR_MAPPED(_function), (ULONG)_OrigFun)


#ifdef __cplusplus
}
#endif