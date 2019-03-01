/*
*  ���ڿ���ɱ���������Ӵ���
*
*  ��    �ߣ���������A1Pass��
*  ��    �ڣ�2012-12-21
*  ���������http://book.hackav.com  �� http://www.hzbook.com
*  ����΢����http://weibo.com/a1pass �� http://t.qq.com/a1pass
*  ���߲��ͣ�http://a1pass.blog.163.com
*/
#include "SSDT_Hook.h"





NTSTATUS MyZwOpenProcess(OUT PHANDLE ProcessHandle,IN ACCESS_MASK DesiredAccess,IN POBJECT_ATTRIBUTES ObjectName,IN PCLIENT_ID ClientId OPTIONAL)
{
	// ������򿪵Ľ�����
	if((ULONG)ClientId->UniqueProcess == g_uProtectPID)
	{
		KdPrint(("��ֹ���Ѿ������Ľ���,PID:%d��������Ȩ��!\n",g_uProtectPID));
		return STATUS_ACCESS_DENIED;
	}

	// �����Ľ����򽻸���ʵ�ĺ���
	return g_funOrigZwOpenProcess(ProcessHandle,DesiredAccess,ObjectName,ClientId);;
}


NTSTATUS HookSSDT()
{
	g_pMdlSystemCall = MmCreateMdl(                   /* ����MDL */
		NULL,                                         // �ڴ��������б�
		KeServiceDescriptorTable.ServiceTableBase,    // �ڴ��ַ
		KeServiceDescriptorTable.NumberOfServices*4); // ����

	if(g_pMdlSystemCall)
	{
		MmBuildMdlForNonPagedPool(g_pMdlSystemCall);                                         // �����ڴ�ҳ��MDL����
		g_pMdlSystemCall->MdlFlags = g_pMdlSystemCall->MdlFlags|MDL_MAPPED_TO_SYSTEM_VA;     // �ı�MDL���Ϊ��д
		g_pMappedSystemCallTable   = (PVOID*)MmMapLockedPages(g_pMdlSystemCall, KernelMode); // ӳ��MDL�ռ�
		KdPrint(("�ɹ���SSDT�ڴ�ռ��Ϊ��д״̬��\n"));	

		HOOK_SYSCALL(ZwOpenProcess, MyZwOpenProcess, g_funOrigZwOpenProcess, ZWOPENPROCESS);
		KdPrint(("�ɹ��޸�SSDTָ��������ַ��\n"));

		return STATUS_SUCCESS;
	}

	return STATUS_UNSUCCESSFUL;
}


NTSTATUS UnHookSSDT()
{ 
	// �ָ�SSDT��ַ
	UNHOOK_SYSCALL(ZwOpenProcess,g_funOrigZwOpenProcess);

	// �ͷ�MDL����SSDT�ڴ�ռ��Ϊ��ʼ״̬
	if(g_pMdlSystemCall)
	{
		MmUnmapLockedPages( g_pMappedSystemCallTable, g_pMdlSystemCall );
		IoFreeMdl(g_pMdlSystemCall);
		KdPrint(("�ɹ���SSDT�ڴ�ռ��Ϊ��ʼ״̬��\n"));
	}

	return STATUS_SUCCESS;
}





NTSTATUS DriverEntry(IN PDRIVER_OBJECT  objDriver,
					 IN PUNICODE_STRING strRegPath)
{
	KdPrint(("DDK_DriverExample Compiled %s %s\r\n In DriverEntry : %wZ\r\n", __DATE__, __TIME__, strRegPath));

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
	objDriver->DriverUnload = SSDTHookUnload;

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
	case IOCTL_HELLO_WORLD:  // ��ӡ�ַ���
		KdPrint(("Hello World!\r\n"));
		nStatus = STATUS_SUCCESS;
		break;
	case IOCTL_HOOK_SSDT: // ��APP��������
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
	case IOCTL_UNHOOK_SSDT:  // �������ݵ�APP
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

	// 3. ���I/O����
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
