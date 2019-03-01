/*
*  《黑客免杀攻防》例子代码
*
*  作    者：任晓珲（A1Pass）
*  日    期：2012-12-18
*  本书官网：http://book.hackav.com  或 http://www.hzbook.com
*  作者微博：http://weibo.com/a1pass 或 http://t.qq.com/a1pass
*  作者博客：http://a1pass.blog.163.com
*/
#include "SYSENTER_Hook.h"





__declspec(naked) void MyKiFastCallEntry()
{
	// 1. 在EDX指向的用户空间栈中取出对应参数
	// +00 用户空间Call返回地址
	// +04 用户空间Call返回地址
	// +08 参数1 ( ProcessHandle )
	// +12 参数2 ( DesiredAccess )
	// +16 参数3 ( ObjectName )
	// +20 参数4 ( ClientId )
	_asm push DWORD PTR[EDX+4*5]; // 4*5 = 参数4
	_asm pop  g_pClientPID;       // 保存参数4到g_pClientPID
	_asm push EDX;
	_asm add  DWORD PTR[ESP],4*3; // 4*3 = 参数2
	_asm pop  g_pAccessMask;      // 保存参数2到g_pAccessMask
	
	// 2. 保护指定PID的进程不被结束
	_asm pushad;
	// 2.1 判断否为ZwOpenProcess的SSDT调用号0xBE，是的话则执行保护
	_asm mov g_uSSDT_Index,eax;
	if ( g_uSSDT_Index == 0xBE )
	{
		// 2.2 判断所操作进程是否为受保护进程，并同时判断访问权限中是否包含
		//     可结束进程权限PROCESS_TERMINATE，是的话则修改其参数，去掉此
		//     权限
		if ( ((ULONG)g_pClientPID->UniqueProcess==g_uProtectPID)  &&
			 (*g_pAccessMask&PROCESS_TERMINATE)                   )
		{
			KdPrint(("------------------------------------------\r\n"));
			KdPrint(("Get Access Mask               : 0x%08x\r\n",*g_pAccessMask));
			*g_pAccessMask = (*g_pAccessMask)&(~PROCESS_TERMINATE);
			KdPrint(("Removal PROCESS_TERMINATE Mask: 0x%08x\r\n",*g_pAccessMask));
			KdPrint(("------------------------------------------\r\n"));
		}
	}
	_asm popad;

	// 3. 跳转到系统原KiFastCallEntry函数中执行剩余操作
	_asm jmp g_uOrigKiFastCallEntry;
}





NTSTATUS HookSYSENTER()
{
	_asm mov ecx,0x176; // IA32_SYSENTER_EIP 在MSR的偏移为0x176
	_asm rdmsr;         // 将ECX指定的MSR加载到 EDX:EAX
	_asm mov g_uOrigKiFastCallEntry,eax;
	_asm mov eax,MyKiFastCallEntry;
	_asm wrmsr;         // 将 EDX:EAX 中的值写入ECX指定的MSR

	return STATUS_SUCCESS;
}





NTSTATUS UnHookSYSENTER()
{ 
	_asm mov ecx,0x176; // IA32_SYSENTER_EIP 在MSR的偏移为0x176
	_asm xor edx,edx;
	_asm mov eax,g_uOrigKiFastCallEntry;
	_asm wrmsr;         // 将 EDX:EAX 中的值写入ECX指定的MSR

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
	case IOCTL_HOOK_SYSENTER:   // HOOK_IDT
		if( uInSize > 0 )
		{
			KdPrint(("Get Data from App: %s(%08X)\r\n", pIoBuffer,pIoBuffer));
			UNICODE_STRING strPID;
			ANSI_STRING    strTemp;
			RtlInitAnsiString(&strTemp,(PCSZ)pIoBuffer);
			RtlAnsiStringToUnicodeString(&strPID,&strTemp,true);
			RtlUnicodeStringToInteger(&strPID,10,&g_uProtectPID);
		}
		HookSYSENTER();
		nStatus = STATUS_SUCCESS;
		break;
	case IOCTL_UNHOOK_SYSENTER: // UNHOOK_IDT
		UnHookSYSENTER();
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