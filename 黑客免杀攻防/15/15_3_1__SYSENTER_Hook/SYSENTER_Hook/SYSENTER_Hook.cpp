/*
*  ���ڿ���ɱ���������Ӵ���
*
*  ��    �ߣ���������A1Pass��
*  ��    �ڣ�2012-12-18
*  ���������http://book.hackav.com  �� http://www.hzbook.com
*  ����΢����http://weibo.com/a1pass �� http://t.qq.com/a1pass
*  ���߲��ͣ�http://a1pass.blog.163.com
*/
#include "SYSENTER_Hook.h"





__declspec(naked) void MyKiFastCallEntry()
{
	// 1. ��EDXָ����û��ռ�ջ��ȡ����Ӧ����
	// +00 �û��ռ�Call���ص�ַ
	// +04 �û��ռ�Call���ص�ַ
	// +08 ����1 ( ProcessHandle )
	// +12 ����2 ( DesiredAccess )
	// +16 ����3 ( ObjectName )
	// +20 ����4 ( ClientId )
	_asm push DWORD PTR[EDX+4*5]; // 4*5 = ����4
	_asm pop  g_pClientPID;       // �������4��g_pClientPID
	_asm push EDX;
	_asm add  DWORD PTR[ESP],4*3; // 4*3 = ����2
	_asm pop  g_pAccessMask;      // �������2��g_pAccessMask
	
	// 2. ����ָ��PID�Ľ��̲�������
	_asm pushad;
	// 2.1 �жϷ�ΪZwOpenProcess��SSDT���ú�0xBE���ǵĻ���ִ�б���
	_asm mov g_uSSDT_Index,eax;
	if ( g_uSSDT_Index == 0xBE )
	{
		// 2.2 �ж������������Ƿ�Ϊ�ܱ������̣���ͬʱ�жϷ���Ȩ�����Ƿ����
		//     �ɽ�������Ȩ��PROCESS_TERMINATE���ǵĻ����޸��������ȥ����
		//     Ȩ��
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

	// 3. ��ת��ϵͳԭKiFastCallEntry������ִ��ʣ�����
	_asm jmp g_uOrigKiFastCallEntry;
}





NTSTATUS HookSYSENTER()
{
	_asm mov ecx,0x176; // IA32_SYSENTER_EIP ��MSR��ƫ��Ϊ0x176
	_asm rdmsr;         // ��ECXָ����MSR���ص� EDX:EAX
	_asm mov g_uOrigKiFastCallEntry,eax;
	_asm mov eax,MyKiFastCallEntry;
	_asm wrmsr;         // �� EDX:EAX �е�ֵд��ECXָ����MSR

	return STATUS_SUCCESS;
}





NTSTATUS UnHookSYSENTER()
{ 
	_asm mov ecx,0x176; // IA32_SYSENTER_EIP ��MSR��ƫ��Ϊ0x176
	_asm xor edx,edx;
	_asm mov eax,g_uOrigKiFastCallEntry;
	_asm wrmsr;         // �� EDX:EAX �е�ֵд��ECXָ����MSR

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