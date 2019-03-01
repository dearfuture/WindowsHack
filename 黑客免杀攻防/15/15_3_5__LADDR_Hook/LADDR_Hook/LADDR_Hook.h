/*
*  《黑客免杀攻防》例子代码
*
*  作    者：任晓珲（A1Pass）
*  日    期：2012-12-31
*  本书官网：http://book.hackav.com  或 http://www.hzbook.com
*  作者微博：http://weibo.com/a1pass 或 http://t.qq.com/a1pass
*  作者博客：http://a1pass.blog.163.com
*/
#pragma once


/************************************************************************/
/* 自定义设备名称及符号链接名称                                         */
/************************************************************************/
#define DEVICE_NAME                L"\\Device\\LADDRHook"     
#define SYMBOLIC_LINK_NAME         L"\\DosDevices\\LADDRHook_SymLink"
#define SYMBOLIC_LINK_GLOBAL_NAME  L"\\DosDevices\\Global\\LADDRHook_SymLink"
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

#define MICRO_SECOND  (-10)               // 1微秒
#define MILLI_SECOND  (MICRO_SECOND*1000) // 1毫秒
#define SECOND        (MILLI_SECOND*1000) // 1秒



#ifdef __cplusplus
extern "C" {
#endif
/************************************************************************/
/* 头文件                                                               */
/************************************************************************/
#include <ntddk.h>
#include <Ntddkbd.h>
#include <devioctl.h>

#include <wdmsec.h> // 需要在工程中包含“;$(DDK_LIB_PATH)\wdmsec.lib”
#include <ntstrsafe.h>


typedef struct _LADDR_DEV_EXT 
{
    ULONG          uSize;        // 此设备扩展结构的大小
    PDEVICE_OBJECT objFilterDev; // 过滤设备对象
    PDEVICE_OBJECT objTargetDev; // 绑定的设备对象
    PDEVICE_OBJECT objLowerDev;  // 绑定设备的上一层设备对象

    NTSTATUS Assignment(         /* 设备扩展赋值函数 */
        _LADDR_DEV_EXT *pDevExt, // 设备扩展对象指针
        PDEVICE_OBJECT objFDev,  // 过滤设备对象
        PDEVICE_OBJECT objTDev,  // 绑定的设备对象
        PDEVICE_OBJECT objLDev)  // 绑定设备的上一层设备对象
    {
        // 如果设备扩展对象指针为空，则直接返回失败
        if ( !pDevExt )  
            return STATUS_UNSUCCESSFUL;

        // 为指定设备扩展赋值
        memset(pDevExt, 0, sizeof(_LADDR_DEV_EXT));
        pDevExt->uSize        = sizeof(_LADDR_DEV_EXT); 
        pDevExt->objFilterDev = objFDev;
        pDevExt->objTargetDev = objTDev;
        pDevExt->objLowerDev  = objLDev;
        return STATUS_SUCCESS;
    }
} LADDR_DEV_EXT, *PLADDR_DEV_EXT;


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
/* 全局变量及声明                                                       */
/************************************************************************/
ULONG          g_uPendingIrpsCount = 0;       // 返回未决的IRP数目
PIRP           g_pPendingIrp       = nullptr; // 保存当前pending的IRP
PDRIVER_OBJECT g_objLADDR_Driver   = nullptr; // 保存当前驱动对象
/************************************************************************/
/* 函数声明                                                         */
/************************************************************************/
NTSTATUS DriverEntry(IN PDRIVER_OBJECT  objDriver, IN PUNICODE_STRING pRegistryString);
NTSTATUS DispatchCreateClose(IN PDEVICE_OBJECT objDeivce, IN PIRP pIrp);
NTSTATUS DispatchDeviceControl(IN PDEVICE_OBJECT objDeivce, IN PIRP pIrp);
VOID     DriverUnload(IN PDRIVER_OBJECT objDriver);
//----------------------------------------------------------------------//
NTSTATUS LADDRHook();
NTSTATUS UnLADDRHook();
/************************************************************************/
/* 函数分页属性                                                         */
/************************************************************************/
#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)  // 初始化完成后，允许DriverEntry例程中被丢弃
#pragma alloc_text(PAGE, DispatchCreateClose)
#pragma alloc_text(PAGE, DispatchDeviceControl)
#pragma alloc_text(PAGE, DriverUnload)
//----------------------------------------------------------------------//
#pragma alloc_text(PAGE, IRPHook)
#pragma alloc_text(PAGE, UnIRPHook)
#endif
/************************************************************************/
/* 宏定义                                                               */
/************************************************************************/
// 进程访问权限，允许关闭进程
#define PROCESS_TERMINATE (0x0001)



#ifdef __cplusplus
}
#endif