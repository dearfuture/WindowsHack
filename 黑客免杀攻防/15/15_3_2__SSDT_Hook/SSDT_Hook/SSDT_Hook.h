/*
*  《黑客免杀攻防》例子代码
*
*  作    者：任晓珲（A1Pass）
*  日    期：2012-12-21
*  本书官网：http://book.hackav.com  或 http://www.hzbook.com
*  作者微博：http://weibo.com/a1pass 或 http://t.qq.com/a1pass
*  作者博客：http://a1pass.blog.163.com
*/
/************************************************************************/
/* 自定义设备名称及符号链接名称                                         */
/************************************************************************/
#define DEVICE_NAME                L"\\Device\\SSDTHook"
#define SYMBOLIC_LINK_NAME         L"\\DosDevices\\SSDTHook_SymLink"
#define SYMBOLIC_LINK_GLOBAL_NAME  L"\\DosDevices\\Global\\SSDTHook_SymLink"
/************************************************************************/
/* 自定义控制码                                                         */
/************************************************************************/
#define IOCTL_BASE        0x800
#define MY_CTL_CODE(i)                                              \
	CTL_CODE                                                        \
	(                                                               \
	FILE_DEVICE_UNKNOWN,  /* 欲控制的驱动类型 */                    \
	IOCTL_BASE + i,       /* 0x800~0xFFF是可由程序员自定义的部分 */ \
	METHOD_BUFFERED,      /* 操作模式：使用缓冲区方式操作 */        \
	FILE_ANY_ACCESS       /* 访问权限：全部 */                      \
	)
#define IOCTL_HELLO_WORLD  MY_CTL_CODE(0)
#define IOCTL_HOOK_SSDT    MY_CTL_CODE(1)
#define IOCTL_UNHOOK_SSDT  MY_CTL_CODE(2)





#ifdef __cplusplus
extern "C" {
#endif
/************************************************************************/
/* 头文件                                                               */
/************************************************************************/
#include <ntddk.h>
#include <devioctl.h>
#include <wdmsec.h> // 需要在工程中包含“$(DDK_LIB_PATH)\wdmsec.lib”
#include <ntstrsafe.h>
/************************************************************************/
/* 定义全局类型                                                         */
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
/* 全局变量及声明                                                       */
/************************************************************************/
ULONG              g_uProtectPID = 0;        // 需要保护进程的PID
PMDL               g_pMdlSystemCall;         // SSDT的MDL的指针
PVOID*             g_pMappedSystemCallTable; // SSDT的MDL虚拟地址指针
_ZwOpenProcess     g_funOrigZwOpenProcess;   // 定义函数原型
NTSYSAPI SSDTEntry KeServiceDescriptorTable; // 导入SSDT
/************************************************************************/
/* 函数声明                                                         */
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
/* 函数分页属性                                                         */
/************************************************************************/
#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)  // 初始化完成后，允许DriverEntry例程中被丢弃
#pragma alloc_text(PAGE, DispatchCreateClose)
#pragma alloc_text(PAGE, DispatchDeviceControl)
#pragma alloc_text(PAGE, SSDTHookUnload)

#pragma alloc_text(PAGE, MyZwOpenProcess)
#pragma alloc_text(PAGE, HookSSDT)
#pragma alloc_text(PAGE, UnHookSSDT)
#endif
/************************************************************************/
/* 宏定义                                                               */
/************************************************************************/

/* 根据函数地址获取其序号 */
#define SYSCALL_INDEX(_function)   \
	(ULONG)*(PULONG)((PUCHAR)_function+1)

/* 在映射地址里取出相应的函数地址 */
#define GET_FUN_FOR_MAPPED(_function)  \
	(PLONG)&g_pMappedSystemCallTable[SYSCALL_INDEX(_function)]

/* 勾住系统函数 */
#define HOOK_SYSCALL(_function,    /* 未被替换的系统函数 */ \
	                 _MyFun,       /* 我们的替换函数 */     \
	                 _OrigFun,     /* 备份系统函数指针 */   \
	                 _OrigFunType) /* 系统函数指针类型 */   \
	_OrigFun = (_OrigFunType)InterlockedExchange(GET_FUN_FOR_MAPPED(_function), (ULONG)_MyFun)

/* 脱钩系统函数 */
#define UNHOOK_SYSCALL(_function,  /* 已被替换的系统函数 */ \
	                   _OrigFun )  /* 备份系统函数指针 */   \
	InterlockedExchange(GET_FUN_FOR_MAPPED(_function), (ULONG)_OrigFun)


#ifdef __cplusplus
}
#endif