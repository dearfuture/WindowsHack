/*
*  《黑客免杀攻防》例子代码
*
*  作    者：任晓珲（A1Pass）
*  日    期：2012-12-21
*  本书官网：http://book.hackav.com  或 http://www.hzbook.com
*  作者微博：http://weibo.com/a1pass 或 http://t.qq.com/a1pass
*  作者博客：http://a1pass.blog.163.com
*/
#include "SSDT_Hook.h"





NTSTATUS MyZwOpenProcess(OUT PHANDLE ProcessHandle,IN ACCESS_MASK DesiredAccess,IN POBJECT_ATTRIBUTES ObjectName,IN PCLIENT_ID ClientId OPTIONAL)
{
	// 如果欲打开的进程是
	if((ULONG)ClientId->UniqueProcess == g_uProtectPID)
	{
		KdPrint(("禁止打开已经保护的进程,PID:%d，返回无权限!\n",g_uProtectPID));
		return STATUS_ACCESS_DENIED;
	}

	// 正常的结束则交给真实的函数
	return g_funOrigZwOpenProcess(ProcessHandle,DesiredAccess,ObjectName,ClientId);;
}


NTSTATUS HookSSDT()
{
	g_pMdlSystemCall = MmCreateMdl(                   /* 创建MDL */
		NULL,                                         // 内存描述符列表
		KeServiceDescriptorTable.ServiceTableBase,    // 内存地址
		KeServiceDescriptorTable.NumberOfServices*4); // 长度

	if(g_pMdlSystemCall)
	{
		MmBuildMdlForNonPagedPool(g_pMdlSystemCall);                                         // 建立内存页的MDL描述
		g_pMdlSystemCall->MdlFlags = g_pMdlSystemCall->MdlFlags|MDL_MAPPED_TO_SYSTEM_VA;     // 改变MDL标记为可写
		g_pMappedSystemCallTable   = (PVOID*)MmMapLockedPages(g_pMdlSystemCall, KernelMode); // 映射MDL空间
		KdPrint(("成功将SSDT内存空间改为可写状态！\n"));	

		HOOK_SYSCALL(ZwOpenProcess, MyZwOpenProcess, g_funOrigZwOpenProcess, ZWOPENPROCESS);
		KdPrint(("成功修改SSDT指定函数地址！\n"));

		return STATUS_SUCCESS;
	}

	return STATUS_UNSUCCESSFUL;
}


NTSTATUS UnHookSSDT()
{ 
	// 恢复SSDT地址
	UNHOOK_SYSCALL(ZwOpenProcess,g_funOrigZwOpenProcess);

	// 释放MDL，将SSDT内存空间改为初始状态
	if(g_pMdlSystemCall)
	{
		MmUnmapLockedPages( g_pMappedSystemCallTable, g_pMdlSystemCall );
		IoFreeMdl(g_pMdlSystemCall);
		KdPrint(("成功将SSDT内存空间改为初始状态！\n"));
	}

	return STATUS_SUCCESS;
}





NTSTATUS DriverEntry(IN PDRIVER_OBJECT  objDriver,
					 IN PUNICODE_STRING strRegPath)
{
	KdPrint(("DDK_DriverExample Compiled %s %s\r\n In DriverEntry : %wZ\r\n", __DATE__, __TIME__, strRegPath));

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
	objDriver->DriverUnload = SSDTHookUnload;

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
	case IOCTL_HOOK_SSDT: // 从APP接收数据
		if( uInSize > 0 )
		{
			KdPrint(("Get Data from App: %s(%08X)\r\n", pIoBuffer,pIoBuffer));
			UNICODE_STRING strPID;
			ANSI_STRING    strTemp;
			RtlInitAnsiString(&strTemp,(PCSZ)pIoBuffer);
			RtlAnsiStringToUnicodeString(&strPID,&strTemp,true);
			RtlUnicodeStringToInteger(&strPID,10,&g_uProtectPID);
		}
		HookSSDT();
		nStatus = STATUS_SUCCESS;
		break;
	case IOCTL_UNHOOK_SSDT:  // 发送数据到APP
		UnHookSSDT();
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


VOID SSDTHookUnload(IN PDRIVER_OBJECT objDriver)
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
