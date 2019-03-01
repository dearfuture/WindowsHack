/*
*  《黑客免杀攻防》例子代码
*
*  作    者：任晓珲（A1Pass）
*  日    期：2013-01-05
*  本书官网：http://book.hackav.com  或 http://www.hzbook.com
*  作者微博：http://weibo.com/a1pass 或 http://t.qq.com/a1pass
*  作者博客：http://a1pass.blog.163.com
*/
#include "IDT_Hook.h"


VOID P2C_WaitForKeyboard(UCHAR cType)
{
	UCHAR cScanCode = 0;
	for (ULONG i=0; i<100; i++)
	{	
		// 读取状态码
		cScanCode = READ_PORT_UCHAR(g_pKeyboardPort64);
		// 等待一段时间
		KeStallExecutionProcessor(200);
		// 检查键盘端口是否已经准备好
		if( !(cScanCode&cType) )  break;
	}
}


VOID PrintScanCode()
{
	// 1. 取出键盘扫描码（此操作会将端口中的扫描码取出并删除）
	P2C_WaitForKeyboard(OBUFFER_FULL);
	UCHAR cScanCode = READ_PORT_UCHAR(g_pKeyboardPort60);

	// 2. 判断此扫描码是否为此函数发送的，是的话直接返回
	if ( g_cLastScanCode==cScanCode )  return;

	// 3. 重新写入键盘扫描码，以方便其他系统模块使用
	g_cLastScanCode = cScanCode;
	WRITE_PORT_UCHAR(g_pKeyboardPort64, 0xD2);
	P2C_WaitForKeyboard(IBUFFER_FULL);
	WRITE_PORT_UCHAR(g_pKeyboardPort60, cScanCode);

	// 4. 打印此扫描码
	KdPrint(("0x%02X\r\n",cScanCode));
}


__declspec(naked) void MyInterruptHook()
{
	// 1. 保存所有寄存器信息
	_asm pushad;
	_asm pushfd;

	// 2. 调用我们的函数
	PrintScanCode();
	
	// 3. 恢复所有寄存器信息
	_asm popfd;
	_asm popad;

	// 4. 跳转到原ISR函数
	_asm jmp g_uOrigISRPointer; 
}


NTSTATUS HookIDT()
{
	// 1. 获取IDT信息
	IDT_INFO stcIdtInfo = {0};
	_asm sidt stcIdtInfo;
	g_pIdtEntry = (PIDT_ENTRY)MAKE_LONG(stcIdtInfo.uLowIdtBase, stcIdtInfo.uHighIdtBase);

	// 2. 获取IDT指定中断号的ISR
	g_uOrigISRPointer = MAKE_LONG(g_pIdtEntry[NT_INT_KEYBD].uOffsetLow, g_pIdtEntry[NT_INT_KEYBD].uOffsetHigh);

	// 3. 使用我们的函数替换原有ISR
	__asm cli;
	g_pIdtEntry[NT_INT_KEYBD].uOffsetLow  = (unsigned short)MyInterruptHook;
	g_pIdtEntry[NT_INT_KEYBD].uOffsetHigh = (unsigned short)((unsigned long)MyInterruptHook >> 16);
	__asm sti;

	return STATUS_SUCCESS;
}


NTSTATUS UnHookIDT()
{ 
	__asm cli
	g_pIdtEntry[NT_INT_KEYBD].uOffsetLow  = (unsigned short) g_uOrigISRPointer;
	g_pIdtEntry[NT_INT_KEYBD].uOffsetHigh = (unsigned short)((unsigned long)g_uOrigISRPointer >> 16);
	__asm sti

	return STATUS_SUCCESS;
}





NTSTATUS DriverEntry(IN PDRIVER_OBJECT  objDriver, IN PUNICODE_STRING strRegPath)
{
	KdPrint(("DDK_DriverExample Compiled %s %s\r\nIn DriverEntry : %wZ\r\n", __DATE__, __TIME__, strRegPath));

	// 1. 声明并初始化关键局部变量
	NTSTATUS       nStatus       = STATUS_SUCCESS;
	// 设备对象
	PDEVICE_OBJECT objDev        = nullptr;
	// 设备名称
	UNICODE_STRING strDeviceName = RTL_CONSTANT_STRING(DEVICE_NAME);
	// 符号链接名称
	UNICODE_STRING strSymbolicName;
	// 所有用户可读可写权限的SDDL
	UNICODE_STRING strSDDL       = RTL_CONSTANT_STRING(L"D:P(A;;GA;;;WD)");
	// 自己随便写一个GUID，或使用Visual Studio自带的GUID生成器生成一个
	const GUID     MY_GUID       = {0x12345678L,0x1234,0x1234,{0x12,0x34,0x56,0x78,0x90,0xAB,0xCB,0xEF}};

	// 2. 创建带安全描述符的设备对象
	nStatus = IoCreateDeviceSecure( 
		objDriver,                  // 驱动对象
		0,                          // 设备扩展对象的结构体大小
		&strDeviceName,             // 设备名称
		FILE_DEVICE_UNKNOWN,        // 设备类型
		FILE_DEVICE_SECURE_OPEN,    // 设备特征信息
		FALSE,                      // 设备是否为独占的
		&strSDDL,                   // 安全描述符定义语言
		&MY_GUID,                   // GUID
		&objDev);                   // 输出创建完成的设备对象指针
	if(!NT_SUCCESS(nStatus))
	{
		KdPrint(("Error, IoCreateDevice = 0x%x\r\n", nStatus));
		return nStatus;
	}

	// 3. 创建链接符号
	if(IoIsWdmVersionAvailable(1,0x10))
		RtlInitUnicodeString(&strSymbolicName, SYMBOLIC_LINK_GLOBAL_NAME);
	else
		RtlInitUnicodeString(&strSymbolicName, SYMBOLIC_LINK_NAME);

	nStatus = IoCreateSymbolicLink(&strSymbolicName, &strDeviceName);
	if(!NT_SUCCESS(nStatus))
	{
		KdPrint(("Error, IoCreateSymbolicLink = 0x%x\r\n", nStatus));
		IoDeleteDevice(objDev);
		return nStatus;
	}

	// 4. 设置派遣函数
	objDriver->MajorFunction[IRP_MJ_CREATE]         = (PDRIVER_DISPATCH)DispatchCreateClose;
	objDriver->MajorFunction[IRP_MJ_CLOSE]          = (PDRIVER_DISPATCH)DispatchCreateClose;
	objDriver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = (PDRIVER_DISPATCH)DispatchDeviceControl;

	// 5. 设置卸载函数
	objDriver->DriverUnload = DriverUnload;

	
	return STATUS_SUCCESS;
}


NTSTATUS DispatchCreateClose(IN PDEVICE_OBJECT objDeivce, IN PIRP pIrp)
{
	UNREFERENCED_PARAMETER(objDeivce);

	// 设置IRP完成状态
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	// 设置IRP操作了多少字节
	pIrp->IoStatus.Information = 0;
	// 处理IRP
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}


NTSTATUS DispatchDeviceControl(IN PDEVICE_OBJECT objDeivce, IN PIRP pIrp)
{
	// 1. 声明并初始化关键局部变量
	UNREFERENCED_PARAMETER(objDeivce);
	NTSTATUS nStatus             = STATUS_INVALID_DEVICE_REQUEST;
	PIO_STACK_LOCATION pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
	ULONG uIoControlCode         = 0;
	PVOID pIoBuffer				 = NULL;
	ULONG uInSize                = 0;
	ULONG uOutSize               = 0;
	// 获取IoCtrl码
	uIoControlCode = pIrpStack->Parameters.DeviceIoControl.IoControlCode;
	// 获取传入I/O缓存
	pIoBuffer      = pIrp->AssociatedIrp.SystemBuffer;
	// 获取传入I/O缓存的大小
	uInSize        = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
	// 获取传出I/O缓存的大小
	uOutSize       = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;

	// 2. 相应用户信息
	switch(uIoControlCode)
	{
	case IOCTL_HELLO_WORLD:  // 打印字符串
		KdPrint(("Hello World!\r\n"));
		nStatus = STATUS_SUCCESS;
		break;
	case IOCTL_HOOK_IDT:    // HOOK_IDT
		HookIDT();
		nStatus = STATUS_SUCCESS;
		break;
	case IOCTL_UNHOOK_IDT:  // UNHOOK_IDT
		UnHookIDT();
		nStatus = STATUS_SUCCESS;
		break;
	default:
		KdPrint(("Unknown IOCTL: 0x%X (%04X,%04X)\r\n", 
			uIoControlCode,
			DEVICE_TYPE_FROM_CTL_CODE(uIoControlCode),
			IoGetFunctionCodeFromCtlCode(uIoControlCode) ));
		nStatus = STATUS_INVALID_PARAMETER;	
		break;
	}

	// 3. 完成I/O请求
	if(nStatus == STATUS_SUCCESS)
		pIrp->IoStatus.Information = uOutSize;
	else
		pIrp->IoStatus.Information = 0;
	pIrp->IoStatus.Status = nStatus;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return nStatus;
}


VOID DriverUnload(IN PDRIVER_OBJECT objDriver)
{
	// 1. 删除链接符号
	UNICODE_STRING strSymbolicName;
	if(IoIsWdmVersionAvailable(1,0x10))
		RtlInitUnicodeString(&strSymbolicName, SYMBOLIC_LINK_GLOBAL_NAME);
	else
		RtlInitUnicodeString(&strSymbolicName, SYMBOLIC_LINK_NAME);
	IoDeleteSymbolicLink(&strSymbolicName);

	// 2. 删除设备对象
	if ( objDriver->DeviceObject )
	    IoDeleteDevice(objDriver->DeviceObject);
}