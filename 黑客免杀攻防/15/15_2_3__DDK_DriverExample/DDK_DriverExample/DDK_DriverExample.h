/*
*  《黑客免杀攻防》例子代码
*
*  作    者：任晓珲（A1Pass）
*  日    期：2012-12-16
*  本书官网：http://book.hackav.com  或 http://www.hzbook.com
*  作者微博：http://weibo.com/a1pass 或 http://t.qq.com/a1pass
*  作者博客：http://a1pass.blog.163.com
*/
#ifdef __cplusplus
extern "C" {
#endif

#include <ntddk.h>
#include <devioctl.h>
#include <wdmsec.h> // 需要在工程中包含“$(DDK_LIB_PATH)\wdmsec.lib”

NTSTATUS DriverEntry(IN PDRIVER_OBJECT  objDriver, IN PUNICODE_STRING pRegistryString);
NTSTATUS DispatchCreateClose(IN PDEVICE_OBJECT objDeivce, IN PIRP pIrp);
NTSTATUS DispatchDeviceControl(IN PDEVICE_OBJECT objDeivce, IN PIRP pIrp);
VOID     DDK_DriverExampleUnload(IN PDRIVER_OBJECT objDriver);
#ifdef __cplusplus
}
#endif

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)  // 初始化完成后，允许DriverEntry例程中被丢弃
#pragma alloc_text(PAGE, DispatchCreateClose)
#pragma alloc_text(PAGE, DispatchDeviceControl)
#pragma alloc_text(PAGE, DDK_DriverExampleUnload)
#endif

#define DEVICE_NAME                L"\\Device\\DriverExample"
#define SYMBOLIC_LINK_NAME         L"\\DosDevices\\DriverExample_SymLink"
#define SYMBOLIC_LINK_GLOBAL_NAME  L"\\DosDevices\\Global\\DriverExample_SymLink"

#define IOCTL_BASE        0x800
#define MY_CTL_CODE(i)    \
	CTL_CODE              \
	(                     \
	FILE_DEVICE_UNKNOWN,  \
	IOCTL_BASE + i,       \
	METHOD_BUFFERED,      \
	FILE_ANY_ACCESS       \
	)
     
#define IOCTL_HELLO_WORLD   MY_CTL_CODE(0)
#define IOCTL_REC_FROM_APP  MY_CTL_CODE(1)
#define IOCTL_SEND_TO_APP   MY_CTL_CODE(2)


