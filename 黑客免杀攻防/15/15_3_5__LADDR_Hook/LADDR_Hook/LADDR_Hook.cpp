/*
*  ���ڿ���ɱ���������Ӵ���
*
*  ��    �ߣ���������A1Pass��
*  ��    �ڣ�2012-12-31
*  ���������http://book.hackav.com  �� http://www.hzbook.com
*  ����΢����http://weibo.com/a1pass �� http://t.qq.com/a1pass
*  ���߲��ͣ�http://a1pass.blog.163.com
*/
#include "LADDR_Hook.h"

/*
* ����IRP��ǲ����
*
*     �����̸���һЩ�������ǹ�ע�ĳ���IRPֱ�ӷ��͵���ʵ�����豸��
*/
NTSTATUS LADDR_GeneralDispatch( IN PDEVICE_OBJECT objDev, IN PIRP pIrp ) 
{ 
    // �жϴ�IRP�������ڹ����豸������������ͨѶ�豸
    if ( pIrp->CurrentLocation == 1 )
    {
        KdPrint(("ͨѶ�豸�ĳ���IRP��ǲ����\r\n")); 
        pIrp->IoStatus.Information = 0;
        pIrp->IoStatus.Status      = IO_NO_INCREMENT;
        IoCompleteRequest(pIrp, IO_NO_INCREMENT);
        return STATUS_SUCCESS;
    } 
    else
    {
        KdPrint(("�����豸�ĳ���IRP��ǲ����\r\n")); 
        IoSkipCurrentIrpStackLocation(pIrp); 
        return IoCallDriver(((PLADDR_DEV_EXT)objDev->DeviceExtension)->objLowerDev, pIrp); 
    }
} 





/*
* IRP_MJ_READ ���Զ����������
*
*     �����������Ҫ���ڴ�ӡ����ɨ���룬������ IRP_MJ_READ ��ԭ������̣�����ɼ���
* ���˵����һ����
*/
NTSTATUS LADDR_ReadCompletion(IN PDEVICE_OBJECT objDev, IN PIRP pIrp, IN PVOID Context )
{
    UNREFERENCED_PARAMETER(objDev);
    UNREFERENCED_PARAMETER(Context);
    g_uPendingIrpsCount--; 

    // 1. ��ȡ���̻��������ݣ�����ӡ���е�ɨ����
    if( NT_SUCCESS( pIrp->IoStatus.Status ) ) 
    {
        PVOID pBuf       = pIrp->AssociatedIrp.SystemBuffer;        // ����������
        ULONG uBufLen    = pIrp->IoStatus.Information;              // ��������������
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
    if( pIrp->PendingReturned )    IoMarkIrpPending( pIrp ); 

    return pIrp->IoStatus.Status;
}





/*
* IRP_MJ_READ ���Զ�����ǲ����
*
*     ����ǲ�������ڱ���ԭ������̣��������滻Ϊ�Զ����������OnReadCompletion��
* �ڽ���ʱ����ϵͳ��ԭ��ǲ������
*/
NTSTATUS LADDR_ReadDispatch(IN PDEVICE_OBJECT objDev, IN PIRP pIrp)
{
    if ( pIrp->CurrentLocation == 1 ) 
    {
        KdPrint(("��IRP������ͨѶ�豸�����ϲ��豸��\r\n")); 
        pIrp->IoStatus.Status      = STATUS_INVALID_DEVICE_REQUEST; 
        pIrp->IoStatus.Information = 0; 
        IoCompleteRequest(pIrp, IO_NO_INCREMENT); 

        return pIrp->IoStatus.Status; 
    }
    else
    {
        // ������I/O�ֲ���ջ��������һ��
        IoCopyCurrentIrpStackLocationToNext(pIrp);
        // ����IRP�������
        IoSetCompletionRoutine( pIrp, LADDR_ReadCompletion, objDev, TRUE, TRUE, TRUE );
        // ��δ��IRP������1�����ڱ����δ��IRP�󣬽�����IRP���ݵ��ϲ��豸
        g_uPendingIrpsCount++; 
        g_pPendingIrp = pIrp;
        PLADDR_DEV_EXT pDevExt = (PLADDR_DEV_EXT)objDev->DeviceExtension;
        return  IoCallDriver( pDevExt->objLowerDev, pIrp );
    }
}





/*
* IRP_MJ_POWER �Զ�����ǲ����
*
*     ����ǲ�������ڴ����Դ�¼���
*/
NTSTATUS LADDR_Power( IN PDEVICE_OBJECT objDev, IN PIRP pIrp ) 
{ 
    PLADDR_DEV_EXT pDevExt = (PLADDR_DEV_EXT)objDev->DeviceExtension;

    PoStartNextPowerIrp( pIrp ); 
    IoSkipCurrentIrpStackLocation( pIrp ); 
    return PoCallDriver(pDevExt->objLowerDev, pIrp ); 
} 





/*
* IRP_MJ_PNP �Զ�����ǲ����
*
*     ����ǲ�������ڴ����Ȳ���¼���
*/
NTSTATUS LADDR_PnP( IN PDEVICE_OBJECT objDev, IN PIRP pIrp ) 
{
    PLADDR_DEV_EXT pDevExt = (PLADDR_DEV_EXT)objDev->DeviceExtension; 
    NTSTATUS       nStatus = STATUS_SUCCESS; 
    
    // ����IRP���ʹ���
    switch ( IoGetCurrentIrpStackLocation(pIrp)->MajorFunction ) 
    { 
    case IRP_MN_REMOVE_DEVICE: 
        KdPrint(("�Ƴ��豸��\r\n")); 

        // 1. ��������ȥ
        IoSkipCurrentIrpStackLocation(pIrp); 
        IoCallDriver(pDevExt->objLowerDev, pIrp); 

        // 2. Ȼ�����󶨲�ɾ���Լ����ɵ������豸
        IoDetachDevice(pDevExt->objLowerDev); 
        IoDeleteDevice(objDev); 
        nStatus = STATUS_SUCCESS; 

        break; 
    default: 
        // �����������͵�IRP��ȫ����ֱ���·����ɡ�
        IoSkipCurrentIrpStackLocation(pIrp); 
        nStatus = IoCallDriver(pDevExt->objLowerDev, pIrp); 
    } 

    return nStatus; 
}





NTSTATUS LADDRHook()
{
    // 1. ��ȡ�����豸����
    NTSTATUS       nStatus      = 0; 
    UNICODE_STRING strKbdName   = RTL_CONSTANT_STRING(L"\\Driver\\Kbdclass");
    PDRIVER_OBJECT objKbdDriver = nullptr;

    nStatus = ObReferenceObjectByName( 
        &strKbdName, 
        OBJ_CASE_INSENSITIVE, 
        NULL, 
        0, 
        *IoDriverObjectType, 
        KernelMode,
        NULL, 
        &objKbdDriver);
    if (!NT_SUCCESS(nStatus))
    {
        KdPrint(("��ȡkbd��������ʧ�ܣ�\r\n"));
        return STATUS_UNSUCCESSFUL;
    }
    ObDereferenceObject(objKbdDriver);

    // 2. ������Kbdclass�������µ������豸����������Ӧ�Ĺ����豸������ȥ��
    PLADDR_DEV_EXT pDevExt      = nullptr;                    // ��չ�豸�ṹָ��
    PDEVICE_OBJECT objFilterDev = nullptr;                    // ���ǵĹ����豸
    PDEVICE_OBJECT objLowerDev  = nullptr;                    // ���Ӻ���豸
    PDEVICE_OBJECT objTargetDev = objKbdDriver->DeviceObject; // Ŀ���豸
     
    while (objTargetDev) 
    {
        // 2.1 ����һ�������豸
        nStatus = IoCreateDevice( 
            IN g_objLADDR_Driver,             // �ɴ��������������豸
            IN sizeof(LADDR_DEV_EXT),         // �豸��չ�Ĵ�С
            IN NULL,                          // �豸����
            IN objTargetDev->DeviceType,      // �豸������
            IN objTargetDev->Characteristics, // �豸������Ϣ
            IN FALSE,                         // �豸�Ƿ�Ϊ��ռ��
            OUT &objFilterDev );              // ���������ɵ��豸����ָ��
        if ( !NT_SUCCESS(nStatus) ) 
        { 
            KdPrint(("�����豸�����豸����ʧ�ܣ�\r\n")); 
            return nStatus; 
        } 

        // 2.2 ���ո����ɵĹ����豸���ӵ�Ŀ���豸�ϣ���������һ���豸
        nStatus = IoAttachDeviceToDeviceStackSafe(
            objFilterDev,  // ���ǵĹ����豸
            objTargetDev,  // Ŀ���豸
            &objLowerDev); // ���豸����һ���豸����
        if ( !NT_SUCCESS(nStatus) ) 
        { 
            KdPrint(("�޷����ӵ��豸����\r\n")); 
            IoDeleteDevice(objFilterDev); 
            objFilterDev = NULL; 
            return nStatus; 
        } 
        
        // 2.3 ����豸��չ������������������ж�ز���
        pDevExt = (PLADDR_DEV_EXT)(objFilterDev->DeviceExtension); 
        pDevExt->Assignment(pDevExt, objFilterDev, objTargetDev, objLowerDev);

        // 2.4 �������豸���������Զ����õ����ϲ��豸һ���������������
        objFilterDev->DeviceType      = objLowerDev->DeviceType; 
        objFilterDev->Characteristics = objLowerDev->Characteristics; 
        objFilterDev->StackSize       = objLowerDev->StackSize+1; 
        objFilterDev->Flags          |= objLowerDev->Flags & (DO_BUFFERED_IO | DO_DIRECT_IO | DO_POWER_PAGABLE) ; 
        KdPrint(("���ӳɹ���\r\n")); 
        
        // 2.5 ��ָ��ָ����һ���豸������ʼ��һ��ѭ��
        objTargetDev = objTargetDev->NextDevice;
    }

    return nStatus; 
}





/*
* ��������豸
*
*     �˺������ڽ������豸��Ŀ���豸�Ϸ��룬��ɾ��Ϊ���˶��������豸���ԣ��Ӷ�ʹ��
* �����������ж�ء�
*/
VOID LADDR_Detach(IN PDEVICE_OBJECT objDev) 
{ 
    PLADDR_DEV_EXT pDevExt = (PLADDR_DEV_EXT)objDev->DeviceExtension;

    __try 
    { 
        __try 
        {
            if ( pDevExt->objTargetDev )
            {
                IoDetachDevice(pDevExt->objTargetDev);
                pDevExt->objTargetDev = nullptr; 
                KdPrint(("����ɹ���\r\n")); 
            }
            IoDeleteDevice(objDev); 
            pDevExt->objFilterDev = nullptr;
        } 
        __except (EXCEPTION_EXECUTE_HANDLER){} 
    } 
    __finally{} 

    return; 
}





/*
* ȡ������IRP
*
*     �˺������ڳ���ȡ������IRP����ʹ�ó����������ж�ء�
*/
BOOLEAN CancelKeyboardIrp(IN PIRP pIrp)
{
    // 1. ���IRP��Ч��ֱ�ӷ���
    if ( !pIrp )  return false;

    // 2. �����IRPδ��ȡ������ȡ��������ֵ����ִ��ȡ��IRP�Ĳ���
    if ( pIrp->Cancel=false||pIrp->CancelRoutine )
    {
        if ( !IoCancelIrp(pIrp) )
        {
            KdPrint(( "ȡ��IRP(0x%X)ʧ�ܣ�\r\n", pIrp ));
            return false;
        }
        KdPrint(( "�ɹ�ȡ��IRP(0x%X)��\r\n", pIrp ));
    }

    // 2. ȡ����������������Ϊ��
    IoSetCancelRoutine( pIrp, NULL );
    return true; 
}





NTSTATUS UnLADDRHook()
{
    // 1. �ѵ�ǰ�߳�����Ϊ��ʵʱģʽ���Ա����������о�����Ӱ����������
    KeSetPriorityThread(KeGetCurrentThread(), LOW_REALTIME_PRIORITY);

    // 2. ���������豸��һ�ɽ����
    PDEVICE_OBJECT objDev = g_objLADDR_Driver->DeviceObject; 
    while (objDev)
    {
        // ����󶨲�ɾ�����е��豸
        LADDR_Detach(objDev);
        objDev = objDev->NextDevice;
    } 

    // 3. �������IRPδ����ҵ�ǰIRP��Ч������ȡ�����IRP
    if ( g_uPendingIrpsCount>0 && g_pPendingIrp )
    {
        if ( !CancelKeyboardIrp(g_pPendingIrp) )
        {
            // ѭ���ȴ�IRP���
            LARGE_INTEGER stcDelay = RtlConvertLongToLargeInteger(500*MILLI_SECOND);
            while (g_uPendingIrpsCount)
            {
                KdPrint(("����%d��IRP�ڵȴ���...\r\n",g_uPendingIrpsCount));
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
    g_objLADDR_Driver = objDriver;

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
        sizeof(LADDR_DEV_EXT),      // �豸��չ����Ľṹ���С
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
    // 4.1 ���ڽ�ʱ����������ڼ���������Kbdclass�����ϲ㣬������С�Kbdclass����IRP��
    //     �ᱻ���ص��������д�������һЩ�������ǹ�ע�ĳ���IRP������ֻ��Ҫ����ֱ��
    //     ���͵������豸���ɡ����������Ҫ��д���еķַ�������ָ��Ϊ���ǵĴ�����
    //     LADDR_GeneralDispatch()�ĵ�ַ������LADDR_GeneralDispatch()�������������
    //     �豸������IRPֱ�ӷ��͵������豸�еĲ�����
    for (ULONG i=0; i < IRP_MJ_MAXIMUM_FUNCTION; i++) 
    { 
        objDriver->MajorFunction[i] = LADDR_GeneralDispatch; 
    }
    // 4.2 ��Ҫ�Թؼ��ļ���IRP����������ǲ��������ʹ�����ǵ������ﵽ���˼�����Ϣ����
    //     �ã���֧�ּ��̵��Ȳ�Ρ�
    objDriver->MajorFunction[IRP_MJ_READ]  = (PDRIVER_DISPATCH)LADDR_ReadDispatch; // ָ��һ��Read��ǲ����
    objDriver->MajorFunction[IRP_MJ_POWER] = (PDRIVER_DISPATCH)LADDR_Power;        // ��Ϊ��������Ҫ����PoCallDriver��PoStartNextPowerIrp
    objDriver->MajorFunction[IRP_MJ_PNP]   = (PDRIVER_DISPATCH)LADDR_PnP;          // ������̲��
    // 4.3 ����һЩ��Ҫ����ǲ����
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
    case IOCTL_HOOK_SYSENTER:   // InlineHook
        LADDRHook();
        nStatus = STATUS_SUCCESS;
        break;
    case IOCTL_UNHOOK_SYSENTER: // UnInlineHook
        UnLADDRHook();
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