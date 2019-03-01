/*
*  《黑客免杀攻防》例子代码
*
*  作    者：任晓珲（A1Pass）
*  日    期：2012-12-18
*  本书官网：http://book.hackav.com  或 http://www.hzbook.com
*  作者微博：http://weibo.com/a1pass 或 http://t.qq.com/a1pass
*  作者博客：http://a1pass.blog.163.com
*/
#pragma once


/************************************************************************/
/* 自定义设备名称及符号链接名称                                         */
/************************************************************************/
#define DEVICE_NAME                L"\\Device\\SYSENTER_Hook"
#define SYMBOLIC_LINK_NAME         L"\\DosDevices\\SYSENTER_Hook_SymLink"
#define SYMBOLIC_LINK_GLOBAL_NAME  L"\\DosDevices\\Global\\SYSENTER_Hook_SymLink"
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
#define IOCTL_HELLO_WORLD      MY_CTL_CODE(0)
#define IOCTL_HOOK_SYSENTER    MY_CTL_CODE(1)
#define IOCTL_UNHOOK_SYSENTER  MY_CTL_CODE(2)





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
/* 全局变量及声明                                                       */
/************************************************************************/
ULONG        g_uProtectPID          = 0; // 需要保护进程的PID
ULONG        g_uOrigKiFastCallEntry = 0; // 被保存的原KiFastCallEntry的地址
ULONG        g_uSSDT_Index          = 0; // 需要调用内核函数的SSDT索引号
PCLIENT_ID   g_pClientPID           = 0; // 调用ZwOpenProcess时传入的PID
PACCESS_MASK g_pAccessMask          = 0; // 调用ZwOpenProcess时传入访问权限
/************************************************************************/
/* 函数声明                                                         */
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
/* 函数分页属性                                                         */
/************************************************************************/
#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)  // 初始化完成后，允许DriverEntry例程中被丢弃
#pragma alloc_text(PAGE, DispatchCreateClose)
#pragma alloc_text(PAGE, DispatchDeviceControl)
#pragma alloc_text(PAGE, DriverUnload)
//----------------------------------------------------------------------//
#pragma alloc_text(PAGE, HookSYSENTER)
#pragma alloc_text(PAGE, UnHookSYSENTER)
#endif
/************************************************************************/
/* 宏定义                                                               */
/************************************************************************/
// 进程访问权限，允许关闭进程
#define PROCESS_TERMINATE (0x0001)

#ifdef __cplusplus
}
#endif