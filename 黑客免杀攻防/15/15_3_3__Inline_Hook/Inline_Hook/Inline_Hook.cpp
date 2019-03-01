/*
*  ���ڿ���ɱ���������Ӵ���
*
*  ��    �ߣ���������A1Pass��
*  ��    �ڣ�2012-12-25
*  ���������http://book.hackav.com  �� http://www.hzbook.com
*  ����΢����http://weibo.com/a1pass �� http://t.qq.com/a1pass
*  ���߲��ͣ�http://a1pass.blog.163.com
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
	_asm push eax;                     // ע�⣬����ʹ��ջ�����˱仯
	_asm mov  eax,DWORD PTR[esp+0x10]; // ��NtCreateFile�ĵ�3����������eax
	_asm mov  g_pObjectAttributes,eax; // ���˲�����ַ������g_lpFileName��
	_asm pop  eax;
	if ( Detour(g_pObjectAttributes) ) // ִ�����ǵ����к������ж��ļ����Ƿ�Ϊ��Ҫ������
	{
		_asm mov eax,0xC0000001L;      // �ǵĻ��򽫺�������ֵ��Ϊ��ʧ��
		_asm retn 0x1C;                // ���ص�����
	}
	_asm push ebp;                     // ����ִ��ԭ����ͷ�����滻����ָ�����ת��ԭ����
	_asm mov  ebp,esp;
	_asm push g_fnNtCreateFile_5;
	_asm ret;
}





NTSTATUS InlineHook()
{
	// 1. ���NtCreateFile������ַ
	UNICODE_STRING ustrFunName    = RTL_CONSTANT_STRING(L"NtCreateFile");
	PVOID          fnNtCreateFile = MmGetSystemRoutineAddress(&ustrFunName);
	
	// 2. ����MDL����ΪNtCreateFile�ڴ�������ӿ�д����
	g_pMDL=MmCreateMdl(NULL,fnNtCreateFile,0x10);
	if (!g_pMDL)  return STATUS_UNSUCCESSFUL;
	MmBuildMdlForNonPagedPool(g_pMDL);                                 // �����ڴ�ҳ��MDL����
	g_pMDL->MdlFlags       = g_pMDL->MdlFlags|MDL_MAPPED_TO_SYSTEM_VA; // �ı�MDL���Ϊ��д
	g_fnMappedNtCreateFile = MmMapLockedPages(g_pMDL, KernelMode);     // ӳ��MDL�ռ�
	KdPrint(("1 �ɹ�������ͷ�ڴ�ռ��Ϊ��д״̬��\r\n"));	

	// 3. ��NtCreateFile������ͷ5���ֽ��滻Ϊ��jmp ���ǵ�ģ����!Trampline()��
	if ( *(PUCHAR)((PUCHAR)g_fnMappedNtCreateFile+0)==0x8B && // ������У��NtCreateFile������ͷ5���ֽ��Ƿ��Ѿ���Hook��
		 *(PUCHAR)((PUCHAR)g_fnMappedNtCreateFile+1)==0xFF && // 8B FF     mov  edi,edi
		 *(PUCHAR)((PUCHAR)g_fnMappedNtCreateFile+2)==0x55 && // 55        push ebp
		 *(PUCHAR)((PUCHAR)g_fnMappedNtCreateFile+3)==0x8B && // 8B EC     mov  ebp,esp
		 *(PUCHAR)((PUCHAR)g_fnMappedNtCreateFile+4)==0xEC )
	{
		// �޸�NtCreateFile������ͷ5���ֽ��滻Ϊ��jmp XXXXXXXX��
		_asm cli;
		*(PUCHAR)g_fnMappedNtCreateFile             = 0xE9;
		*(PULONG)((PUCHAR)g_fnMappedNtCreateFile+1) = (ULONG)Trampline - (ULONG)fnNtCreateFile - 5;
		KdPrint(("2 �ɹ��޸ĺ���ͷ��\r\n"));
		g_fnNtCreateFile_5 = (ULONG)fnNtCreateFile + 5;
		_asm sti;

		return STATUS_SUCCESS;
	}

	return STATUS_UNSUCCESSFUL;

	
}





NTSTATUS UnInlineHook()
{ 
	// 1. ���NtCreateFile������ַ
	UNICODE_STRING ustrFunName    = RTL_CONSTANT_STRING(L"NtCreateFile");
	PVOID          fnNtCreateFile = MmGetSystemRoutineAddress(&ustrFunName);

	// 2. �ָ�NtCreateFile����ͷ5���ֽ�
	if ( *(PUCHAR)((PUCHAR)g_fnMappedNtCreateFile+0)==0xE9 &&
		 *(PULONG)((PUCHAR)g_fnMappedNtCreateFile+1)==((ULONG)Trampline-(ULONG)fnNtCreateFile-5) )
	{
		// �ָ�Ϊԭ����ͷ����Ϣ
		_asm cli;
		*(PUCHAR)((PUCHAR)g_fnMappedNtCreateFile+0) = 0x8B;
		*(PUCHAR)((PUCHAR)g_fnMappedNtCreateFile+1) = 0xFF;
		*(PUCHAR)((PUCHAR)g_fnMappedNtCreateFile+2) = 0x55;
		*(PUCHAR)((PUCHAR)g_fnMappedNtCreateFile+3) = 0x8B;
		*(PUCHAR)((PUCHAR)g_fnMappedNtCreateFile+4) = 0xEC;
		_asm sti;
		KdPrint(("3 �ɹ��ָ�����ͷ��\r\n"));
	}

	// 3. �ͷ�MDL����NtCreateFile�ڴ�ռ��Ϊ��ʼ״̬
	if(g_pMDL)
	{
		MmUnmapLockedPages( g_fnMappedNtCreateFile, g_pMDL );
		IoFreeMdl(g_pMDL);
		g_pMDL = nullptr;
		KdPrint(("4 �ɹ�������ͷ�ڴ�ռ��Ϊ��ʼ״̬��\r\n"));
	}

	return STATUS_SUCCESS;
}





NTSTATUS DriverEntry(IN PDRIVER_OBJECT  objDriver, IN PUNICODE_STRING strRegPath)
{
	KdPrint(("DDK_DriverExample Compiled %s %s\r\nIn DriverEntry : %wZ\r\n", __DATE__, __TIME__, strRegPath));

	// 1. ��������ʼ���ؼ��ֲ�����
	NTSTATUS       nStatus       = STATUS_SUCCESS;
	// �豸����
	PDEVICE_OBJECT objDev        = nullptr;
	// �豸����
	UNICODE_STRING strDeviceName = RTL_CONSTANT_STRING(DEVICE_NAME);
	// ������������
	UNICODE_STRING strSymbolicName;
	// �����û��ɶ���дȨ�޵�SDDL
	UNICODE_STRING strSDDL       = RTL_CONSTANT_STRING(L"D:P(A;;GA;;;WD)");
	// �Լ����дһ��GUID����ʹ��Visual Studio�Դ���GUID����������һ��
	const GUID     MY_GUID       = {0x12345678L,0x1234,0x1234,{0x12,0x34,0x56,0x78,0x90,0xAB,0xCB,0xEF}};

	// 2. ��������ȫ���������豸����
	nStatus = IoCreateDeviceSecure( 
		objDriver,                  // ��������
		0,                          // �豸��չ����Ľṹ���С
		&strDeviceName,             // �豸����
		FILE_DEVICE_UNKNOWN,        // �豸����
		FILE_DEVICE_SECURE_OPEN,    // �豸������Ϣ
		FALSE,                      // �豸�Ƿ�Ϊ��ռ��
		&strSDDL,                   // ��ȫ��������������
		&MY_GUID,                   // GUID
		&objDev);                   // ���������ɵ��豸����ָ��
	if(!NT_SUCCESS(nStatus))
	{
		KdPrint(("Error, IoCreateDevice = 0x%x\r\n", nStatus));
		return nStatus;
	}

	// 3. �������ӷ���
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

	// 4. ������ǲ����
	objDriver->MajorFunction[IRP_MJ_CREATE]         = (PDRIVER_DISPATCH)DispatchCreateClose;
	objDriver->MajorFunction[IRP_MJ_CLOSE]          = (PDRIVER_DISPATCH)DispatchCreateClose;
	objDriver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = (PDRIVER_DISPATCH)DispatchDeviceControl;

	// 5. ����ж�غ���
	objDriver->DriverUnload = DriverUnload;


	return STATUS_SUCCESS;
}


NTSTATUS DispatchCreateClose(IN PDEVICE_OBJECT objDeivce, IN PIRP pIrp)
{
	UNREFERENCED_PARAMETER(objDeivce);

	// ����IRP���״̬
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	// ����IRP�����˶����ֽ�
	pIrp->IoStatus.Information = 0;
	// ����IRP
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}


NTSTATUS DispatchDeviceControl(IN PDEVICE_OBJECT objDeivce, IN PIRP pIrp)
{
	// 1. ��������ʼ���ؼ��ֲ�����
	UNREFERENCED_PARAMETER(objDeivce);
	NTSTATUS nStatus             = STATUS_INVALID_DEVICE_REQUEST;
	PIO_STACK_LOCATION pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
	ULONG uIoControlCode         = 0;
	PVOID pIoBuffer				 = NULL;
	ULONG uInSize                = 0;
	ULONG uOutSize               = 0;
	// ��ȡIoCtrl��
	uIoControlCode = pIrpStack->Parameters.DeviceIoControl.IoControlCode;
	// ��ȡ����I/O����
	pIoBuffer      = pIrp->AssociatedIrp.SystemBuffer;
	// ��ȡ����I/O����Ĵ�С
	uInSize        = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
	// ��ȡ����I/O����Ĵ�С
	uOutSize       = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;

	// 2. ��Ӧ�û���Ϣ
	switch(uIoControlCode)
	{
	case IOCTL_HELLO_WORLD:     // ��ӡ�ַ���
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

	// 3. ���I/O����
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
	// 1. ɾ�����ӷ���
	UNICODE_STRING strSymbolicName;
	if(IoIsWdmVersionAvailable(1,0x10))
		RtlInitUnicodeString(&strSymbolicName, SYMBOLIC_LINK_GLOBAL_NAME);
	else
		RtlInitUnicodeString(&strSymbolicName, SYMBOLIC_LINK_NAME);
	IoDeleteSymbolicLink(&strSymbolicName);

	// 2. ɾ���豸����
	if ( objDriver->DeviceObject )
	    IoDeleteDevice(objDriver->DeviceObject);
}