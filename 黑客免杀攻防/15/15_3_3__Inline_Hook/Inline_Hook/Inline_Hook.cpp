/*
*  《黑客免杀攻防》例子代码
*
*  作    者：任晓珲（A1Pass）
*  日    期：2012-12-25
*  本书官网：http://book.hackav.com  或 http://www.hzbook.com
*  作者微博：http://weibo.com/a1pass 或 http://t.qq.com/a1pass
*  作者博客：http://a1pass.blog.163.com
*/
#include "Inline_Hook.h"


ULONG Detour(POBJECT_ATTRIBUTES pObjectAttributes)
{
	if ( wcsstr(pObjectAttributes->ObjectName->Buffer, L"A1Pass") )
		return 1;
	return 0;
}


__declspec(naked) void Trampline()
{
	_asm push eax;                     // 注意，这里使堆栈发生了变化
	_asm mov  eax,DWORD PTR[esp+0x10]; // 将NtCreateFile的第3个参数传给eax
	_asm mov  g_pObjectAttributes,eax; // 将此参数地址保存在g_lpFileName中
	_asm pop  eax;
	if ( Detour(g_pObjectAttributes) ) // 执行我们的绕行函数，判断文件名是否为需要保护的
	{
		_asm mov eax,0xC0000001L;      // 是的话则将函数返回值改为打开失败
		_asm retn 0x1C;                // 返回到调用
	}
	_asm push ebp;                     // 否则执行原函数头部被替换掉的指令，并跳转到原函数
	_asm mov  ebp,esp;
	_asm push g_fnNtCreateFile_5;
	_asm ret;
}





NTSTATUS InlineHook()
{
	// 1. 获得NtCreateFile函数地址
	UNICODE_STRING ustrFunName    = RTL_CONSTANT_STRING(L"NtCreateFile");
	PVOID          fnNtCreateFile = MmGetSystemRoutineAddress(&ustrFunName);
	
	// 2. 创建MDL，并为NtCreateFile内存属性添加可写属性
	g_pMDL=MmCreateMdl(NULL,fnNtCreateFile,0x10);
	if (!g_pMDL)  return STATUS_UNSUCCESSFUL;
	MmBuildMdlForNonPagedPool(g_pMDL);                                 // 建立内存页的MDL描述
	g_pMDL->MdlFlags       = g_pMDL->MdlFlags|MDL_MAPPED_TO_SYSTEM_VA; // 改变MDL标记为可写
	g_fnMappedNtCreateFile = MmMapLockedPages(g_pMDL, KernelMode);     // 映射MDL空间
	KdPrint(("1 成功将函数头内存空间改为可写状态！\r\n"));	

	// 3. 将NtCreateFile函数的头5个字节替换为“jmp 我们的模块名!Trampline()”
	if ( *(PUCHAR)((PUCHAR)g_fnMappedNtCreateFile+0)==0x8B && // 这里是校验NtCreateFile函数的头5个字节是否已经被Hook了
		 *(PUCHAR)((PUCHAR)g_fnMappedNtCreateFile+1)==0xFF && // 8B FF     mov  edi,edi
		 *(PUCHAR)((PUCHAR)g_fnMappedNtCreateFile+2)==0x55 && // 55        push ebp
		 *(PUCHAR)((PUCHAR)g_fnMappedNtCreateFile+3)==0x8B && // 8B EC     mov  ebp,esp
		 *(PUCHAR)((PUCHAR)g_fnMappedNtCreateFile+4)==0xEC )
	{
		// 修改NtCreateFile函数的头5个字节替换为“jmp XXXXXXXX”
		_asm cli;
		*(PUCHAR)g_fnMappedNtCreateFile             = 0xE9;
		*(PULONG)((PUCHAR)g_fnMappedNtCreateFile+1) = (ULONG)Trampline - (ULONG)fnNtCreateFile - 5;
		KdPrint(("2 成功修改函数头！\r\n"));
		g_fnNtCreateFile_5 = (ULONG)fnNtCreateFile + 5;
		_asm sti;

		return STATUS_SUCCESS;
	}

	return STATUS_UNSUCCESSFUL;

	
}





NTSTATUS UnInlineHook()
{ 
	// 1. 获得NtCreateFile函数地址
	UNICODE_STRING ustrFunName    = RTL_CONSTANT_STRING(L"NtCreateFile");
	PVOID          fnNtCreateFile = MmGetSystemRoutineAddress(&ustrFunName);

	// 2. 恢复NtCreateFile函数头5个字节
	if ( *(PUCHAR)((PUCHAR)g_fnMappedNtCreateFile+0)==0xE9 &&
		 *(PULONG)((PUCHAR)g_fnMappedNtCreateFile+1)==((ULONG)Trampline-(ULONG)fnNtCreateFile-5) )
	{
		// 恢复为原函数头的信息
		_asm cli;
		*(PUCHAR)((PUCHAR)g_fnMappedNtCreateFile+0) = 0x8B;
		*(PUCHAR)((PUCHAR)g_fnMappedNtCreateFile+1) = 0xFF;
		*(PUCHAR)((PUCHAR)g_fnMappedNtCreateFile+2) = 0x55;
		*(PUCHAR)((PUCHAR)g_fnMappedNtCreateFile+3) = 0x8B;
		*(PUCHAR)((PUCHAR)g_fnMappedNtCreateFile+4) = 0xEC;
		_asm sti;
		KdPrint(("3 成功恢复函数头！\r\n"));
	}

	// 3. 释放MDL，将NtCreateFile内存空间改为初始状态
	if(g_pMDL)
	{
		MmUnmapLockedPages( g_fnMappedNtCreateFile, g_pMDL );
		IoFreeMdl(g_pMDL);
		g_pMDL = nullptr;
		KdPrint(("4 成功将函数头内存空间改为初始状态！\r\n"));
	}

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
	case IOCTL_HELLO_WORLD:     // 打印字符串
		KdPrint(("Hello World!\r\n"));
		nStatus = STATUS_SUCCESS;
		break;
	case IOCTL_HOOK_SYSENTER:   // InlineHook
		InlineHook();
		nStatus = STATUS_SUCCESS;
		break;
	case IOCTL_UNHOOK_SYSENTER: // UnInlineHook
		UnInlineHook();
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