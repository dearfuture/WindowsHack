/*
*  ���ڿ���ɱ���������Ӵ���
*
*  ��    �ߣ���������A1Pass��
*  ��    �ڣ�2012-12-16
*  ���������http://book.hackav.com  �� http://www.hzbook.com
*  ����΢����http://weibo.com/a1pass �� http://t.qq.com/a1pass
*  ���߲��ͣ�http://a1pass.blog.163.com
*/
#include "DDK_DriverExample.h"


NTSTATUS DriverEntry(IN PDRIVER_OBJECT  objDriver,
					 IN PUNICODE_STRING strRegPath)
{
	DbgPrint("DDK_DriverExample Compiled %s %s\r\nIn DriverEntry : %wZ\r\n", __DATE__, __TIME__, strRegPath);


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
		DbgPrint("Error, IoCreateDevice = 0x%x\r\n", nStatus);
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
		DbgPrint("Error, IoCreateSymbolicLink = 0x%x\r\n", nStatus);
		IoDeleteDevice(objDev);
		return nStatus;
	}


	// 4. ������ǲ����
	objDriver->MajorFunction[IRP_MJ_CREATE]         = (PDRIVER_DISPATCH)DispatchCreateClose;
	objDriver->MajorFunction[IRP_MJ_CLOSE]          = (PDRIVER_DISPATCH)DispatchCreateClose;
	objDriver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = (PDRIVER_DISPATCH)DispatchDeviceControl;


	// 5. ����ж�غ���
	objDriver->DriverUnload = DDK_DriverExampleUnload;

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
	CHAR  szDataToAPP[]          = "Hello World from Driver!";
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
	case IOCTL_HELLO_WORLD:   // ��ӡ�ַ���
		DbgPrint("Hello World!\r\n");
		nStatus = STATUS_SUCCESS;
		break;
	case IOCTL_REC_FROM_APP: // ��APP��������
		if( uInSize > 0 )
			DbgPrint("Get Data from App: %s\r\n", pIoBuffer);
		nStatus = STATUS_SUCCESS;
		break;
	case IOCTL_SEND_TO_APP:  // �������ݵ�APP
		if( uOutSize < strlen(szDataToAPP)+1 )  break;
		RtlCopyMemory(pIoBuffer, szDataToAPP, strlen(szDataToAPP)+1);
		pIrp->IoStatus.Information = strlen(szDataToAPP) + 1;
		nStatus = STATUS_SUCCESS;
		break;
	default:
		DbgPrint("Unknown IOCTL: 0x%X (%04X,%04X)\r\n", 
			uIoControlCode,
			DEVICE_TYPE_FROM_CTL_CODE(uIoControlCode),
			IoGetFunctionCodeFromCtlCode(uIoControlCode));
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




VOID DDK_DriverExampleUnload(IN PDRIVER_OBJECT objDriver)
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
