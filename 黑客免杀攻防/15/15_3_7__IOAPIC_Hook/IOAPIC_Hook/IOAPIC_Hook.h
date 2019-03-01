/*
*  《黑客免杀攻防》例子代码
*
*  作    者：任晓珲（A1Pass）
*  日    期：2013-01-15
*  本书官网：http://book.hackav.com  或 http://www.hzbook.com
*  作者微博：http://weibo.com/a1pass 或 http://t.qq.com/a1pass
*  作者博客：http://a1pass.blog.163.com
*/
#pragma once


/************************************************************************/
/* 自定义设备名称及符号链接名称                                         */
/************************************************************************/
#define DEVICE_NAME                L"\\Device\\IOAPIC_Hook"
#define SYMBOLIC_LINK_NAME         L"\\DosDevices\\IOAPIC_Hook_SymLink"
#define SYMBOLIC_LINK_GLOBAL_NAME  L"\\DosDevices\\Global\\IOAPIC_Hook_SymLink"
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
#define IOCTL_HOOK_IDT     MY_CTL_CODE(1)
#define IOCTL_UNHOOK_IDT   MY_CTL_CODE(2)





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
	// IDT结构
	typedef struct _IDT_INFO 
	{
		UINT16 uIdtLimit;    // IDT范围
		UINT16 uLowIdtBase;  // IDT低基址
		UINT16 uHighIdtBase; // IDT高基址
	}IDT_INFO,*PIDT_INFO;

	// GateType所代表的中断类型
	// 0x05 : 32位的任务门（80386）
	// 0x06 : 16位的中断门（80286）
	// 0x07 : 16位的陷阱门（80286）
	// 0x0E : 32位的中断门（80386）
	// 0x0F : 32位的陷阱门（80386）
	typedef struct _IDT_ENTRY
	{
		UINT16 uOffsetLow;       // 处理程序低地址偏移
		UINT16 uSelector;        // 段选择器
		UINT8  uReserved;        // 保留
		UINT8  GateType:4;       // 中断类型
		UINT8  StorageSegment:1; // 为0是中断门
		UINT8  DPL:2;            // 特权级
		UINT8  Present:1;        // 如未使用中断可置为0
		UINT16 uOffsetHigh;      // 处理程序高地址偏移
	}IDT_ENTRY,*PIDT_ENTRY;

	// IOREGSEL寄存器结构
	typedef struct _IOREGSEL_INFO
	{
		UINT8 uAPICR;     // APIC寄存器地址
		UINT8 Reserved_0; // 保留
		UINT8 Reserved_1; // 保留
		UINT8 Reserved_2; // 保留
	}IOREGSEL_INFO,*PIOREGSEL_INFO;
#pragma pack()

/************************************************************************/
/* 全局变量及声明                                                       */
/************************************************************************/
PVOID            g_pVisualAddr     = 0;             // IOAPIC控制器的关键寄存器映射后的虚拟地址
PIOREGSEL_INFO   g_pIOREGSEL       = 0;             // IOAPIC控制器的选择寄存器
PUINT32          g_pIOWIN          = 0;             // IOAPIC控制器的窗口寄存器
UINT32           g_uOrigIRQ        = 0;             // 被保存的原IRQ
UINT8            g_uVector         = 0;             // 指向一个空IDT的中断向量
ULONG            g_uOrigISRPointer = 0;             // 原始ISR
PIDT_ENTRY       g_pIdtEntry       = nullptr;       // IDT入口指针
UCHAR            g_cLastScanCode   = 0;             // 上一次获取到的键盘扫描码
PUCHAR           g_pKeyboardPort60 = (PUCHAR)0x60;  // 键盘扫描码端口
PUCHAR           g_pKeyboardPort64 = (PUCHAR)0x64;  // 键盘状态端口
/************************************************************************/
/* 函数声明                                                         */
/************************************************************************/
NTSTATUS DriverEntry(IN PDRIVER_OBJECT  objDriver, IN PUNICODE_STRING pRegistryString);
NTSTATUS DispatchCreateClose(IN PDEVICE_OBJECT objDeivce, IN PIRP pIrp);
NTSTATUS DispatchDeviceControl(IN PDEVICE_OBJECT objDeivce, IN PIRP pIrp);
VOID     DriverUnload(IN PDRIVER_OBJECT objDriver);
//----------------------------------------------------------------------//
VOID P2C_WaitForKeyboard(UCHAR cType);
VOID PrintScanCode();
VOID MyInterruptHook();
UINT8 GetNullIDT();
NTSTATUS HookIDT();
NTSTATUS UnHookIDT();
/************************************************************************/
/* 函数分页属性                                                         */
/************************************************************************/
#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)  // 初始化完成后，允许DriverEntry例程中被丢弃
#pragma alloc_text(PAGE, DispatchCreateClose)
#pragma alloc_text(PAGE, DispatchDeviceControl)
#pragma alloc_text(PAGE, DriverUnload)

#pragma alloc_text(PAGE, HookIDT)
#pragma alloc_text(PAGE, UnHookIDT)
#endif
/************************************************************************/
/* 宏定义                                                               */
/************************************************************************/

/* 根据函数地址获取其序号 */
#define MAKE_LONG(_Low,_High)  ( (UINT32)( ((UINT16)(_Low)) | ((UINT32)((UINT16)(_High))) <<16) )
#define READ_PORT(_Port) _asm{_asm in al,_Port;_asm };

#define IOREGSEL 0xFEC00000
//#define IOWIN    0xFEC00010

// 键盘中断号
//#define NT_INT_KEYBD  0xB3
//#define NT_INT_KEYBD  0x31
//#define NT_INT_KEYBD  0x93
#define NT_INT_KEYBD  0x81 /*Windows 7*/

#define KEYBOARD_IRQ  0x01

#define IBUFFER_FULL  0x02
#define OBUFFER_FULL  0x01

#ifdef __cplusplus
}
#endif