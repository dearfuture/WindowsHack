/*
*  《黑客免杀攻防》例子代码
*
*  作    者：任晓珲（A1Pass）
*  日    期：2012-12-31
*  本书官网：http://book.hackav.com  或 http://www.hzbook.com
*  作者微博：http://weibo.com/a1pass 或 http://t.qq.com/a1pass
*  作者博客：http://a1pass.blog.163.com
*/
#include "LADDR_Hook.h"

/*
* 常规IRP派遣函数
*
*     此例程负责将一些不被我们关注的常规IRP直接发送到真实物理设备。
*/
NTSTATUS LADDR_GeneralDispatch( IN PDEVICE_OBJECT objDev, IN PIRP pIrp ) 
{ 
    // 判断此IRP是来自于过滤设备，还是来自于通讯设备
    if ( pIrp->CurrentLocation == 1 )
    {
        KdPrint(("通讯设备的常规IRP派遣请求！\r\n")); 
        pIrp->IoStatus.Information = 0;
        pIrp->IoStatus.Status      = IO_NO_INCREMENT;
        IoCompleteRequest(pIrp, IO_NO_INCREMENT);
        return STATUS_SUCCESS;
    } 
    else
    {
        KdPrint(("过滤设备的常规IRP派遣请求！\r\n")); 
        IoSkipCurrentIrpStackLocation(pIrp); 
        return IoCallDriver(((PLADDR_DEV_EXT)objDev->DeviceExtension)->objLowerDev, pIrp); 
    }
} 





/*
* IRP_MJ_READ 的自定义完成例程
*
*     此完成例程主要用于打印键盘扫描码，并调用 IRP_MJ_READ 的原完成例程，是完成键盘
* 过滤的最后一步。
*/
NTSTATUS LADDR_ReadCompletion(IN PDEVICE_OBJECT objDev, IN PIRP pIrp, IN PVOID Context )
{
    UNREFERENCED_PARAMETER(objDev);
    UNREFERENCED_PARAMETER(Context);
    g_uPendingIrpsCount--; 

    // 1. 获取键盘缓冲区数据，并打印其中的扫描码
    if( NT_SUCCESS( pIrp->IoStatus.Status ) ) 
    {
        PVOID pBuf       = pIrp->AssociatedIrp.SystemBuffer;        // 按键缓冲区
        ULONG uBufLen    = pIrp->IoStatus.Information;              // 按键缓冲区长度
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
    if( pIrp->PendingReturned )    IoMarkIrpPending( pIrp ); 

    return pIrp->IoStatus.Status;
}





/*
* IRP_MJ_READ 的自定义派遣函数
*
*     此派遣函数用于保留原完成例程，并将其替换为自定义完成例程OnReadCompletion，
* 在结束时调用系统的原派遣函数。
*/
NTSTATUS LADDR_ReadDispatch(IN PDEVICE_OBJECT objDev, IN PIRP pIrp)
{
    if ( pIrp->CurrentLocation == 1 ) 
    {
        KdPrint(("此IRP来自于通讯设备，或上层设备！\r\n")); 
        pIrp->IoStatus.Status      = STATUS_INVALID_DEVICE_REQUEST; 
        pIrp->IoStatus.Information = 0; 
        IoCompleteRequest(pIrp, IO_NO_INCREMENT); 

        return pIrp->IoStatus.Status; 
    }
    else
    {
        // 将本层I/O局部堆栈拷贝到下一层
        IoCopyCurrentIrpStackLocationToNext(pIrp);
        // 设置IRP完成例程
        IoSetCompletionRoutine( pIrp, LADDR_ReadCompletion, objDev, TRUE, TRUE, TRUE );
        // 将未决IRP数量加1，并在保存此未决IRP后，将本层IRP传递到上层设备
        g_uPendingIrpsCount++; 
        g_pPendingIrp = pIrp;
        PLADDR_DEV_EXT pDevExt = (PLADDR_DEV_EXT)objDev->DeviceExtension;
        return  IoCallDriver( pDevExt->objLowerDev, pIrp );
    }
}





/*
* IRP_MJ_POWER 自定义派遣函数
*
*     此派遣函数用于处理电源事件。
*/
NTSTATUS LADDR_Power( IN PDEVICE_OBJECT objDev, IN PIRP pIrp ) 
{ 
    PLADDR_DEV_EXT pDevExt = (PLADDR_DEV_EXT)objDev->DeviceExtension;

    PoStartNextPowerIrp( pIrp ); 
    IoSkipCurrentIrpStackLocation( pIrp ); 
    return PoCallDriver(pDevExt->objLowerDev, pIrp ); 
} 





/*
* IRP_MJ_PNP 自定义派遣函数
*
*     此派遣函数用于处理热插拔事件。
*/
NTSTATUS LADDR_PnP( IN PDEVICE_OBJECT objDev, IN PIRP pIrp ) 
{
    PLADDR_DEV_EXT pDevExt = (PLADDR_DEV_EXT)objDev->DeviceExtension; 
    NTSTATUS       nStatus = STATUS_SUCCESS; 
    
    // 根据IRP类型处理
    switch ( IoGetCurrentIrpStackLocation(pIrp)->MajorFunction ) 
    { 
    case IRP_MN_REMOVE_DEVICE: 
        KdPrint(("移除设备！\r\n")); 

        // 1. 把请求发下去
        IoSkipCurrentIrpStackLocation(pIrp); 
        IoCallDriver(pDevExt->objLowerDev, pIrp); 

        // 2. 然后解除绑定并删除自己生成的虚拟设备
        IoDetachDevice(pDevExt->objLowerDev); 
        IoDeleteDevice(objDev); 
        nStatus = STATUS_SUCCESS; 

        break; 
    default: 
        // 对于其他类型的IRP，全部都直接下发即可。
        IoSkipCurrentIrpStackLocation(pIrp); 
        nStatus = IoCallDriver(pDevExt->objLowerDev, pIrp); 
    } 

    return nStatus; 
}





NTSTATUS LADDRHook()
{
    // 1. 获取驱动设备对象
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
        KdPrint(("获取kbd驱动对象失败！\r\n"));
        return STATUS_UNSUCCESSFUL;
    }
    ObDereferenceObject(objKbdDriver);

    // 2. 遍历“Kbdclass”驱动下的所有设备，并创建对应的过滤设备附加上去。
    PLADDR_DEV_EXT pDevExt      = nullptr;                    // 扩展设备结构指针
    PDEVICE_OBJECT objFilterDev = nullptr;                    // 我们的过滤设备
    PDEVICE_OBJECT objLowerDev  = nullptr;                    // 附加后的设备
    PDEVICE_OBJECT objTargetDev = objKbdDriver->DeviceObject; // 目标设备
     
    while (objTargetDev) 
    {
        // 2.1 生成一个过滤设备
        nStatus = IoCreateDevice( 
            IN g_objLADDR_Driver,             // 由此驱动对象生成设备
            IN sizeof(LADDR_DEV_EXT),         // 设备扩展的大小
            IN NULL,                          // 设备名称
            IN objTargetDev->DeviceType,      // 设备的类型
            IN objTargetDev->Characteristics, // 设备特征信息
            IN FALSE,                         // 设备是否为独占的
            OUT &objFilterDev );              // 输出创建完成的设备对象指针
        if ( !NT_SUCCESS(nStatus) ) 
        { 
            KdPrint(("生成设备过滤设备对象失败！\r\n")); 
            return nStatus; 
        } 

        // 2.2 将刚刚生成的过了设备附加到目标设备上，并返回上一层设备
        nStatus = IoAttachDeviceToDeviceStackSafe(
            objFilterDev,  // 我们的过滤设备
            objTargetDev,  // 目标设备
            &objLowerDev); // 绑定设备的上一层设备对象
        if ( !NT_SUCCESS(nStatus) ) 
        { 
            KdPrint(("无法附加到设备对象！\r\n")); 
            IoDeleteDevice(objFilterDev); 
            objFilterDev = NULL; 
            return nStatus; 
        } 
        
        // 2.3 填充设备扩展，方便后续分离操作及卸载操作
        pDevExt = (PLADDR_DEV_EXT)(objFilterDev->DeviceExtension); 
        pDevExt->Assignment(pDevExt, objFilterDev, objTargetDev, objLowerDev);

        // 2.4 将过滤设备的所有属性都设置的像上层设备一样，避免出现问题
        objFilterDev->DeviceType      = objLowerDev->DeviceType; 
        objFilterDev->Characteristics = objLowerDev->Characteristics; 
        objFilterDev->StackSize       = objLowerDev->StackSize+1; 
        objFilterDev->Flags          |= objLowerDev->Flags & (DO_BUFFERED_IO | DO_DIRECT_IO | DO_POWER_PAGABLE) ; 
        KdPrint(("附加成功！\r\n")); 
        
        // 2.5 将指针指向下一个设备，并开始下一次循环
        objTargetDev = objTargetDev->NextDevice;
    }

    return nStatus; 
}





/*
* 分离过滤设备
*
*     此函数用于将过滤设备从目标设备上分离，并删除为过滤而创建的设备对性，从而使得
* 程序可以正常卸载。
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
                KdPrint(("分离成功！\r\n")); 
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
* 取消键盘IRP
*
*     此函数用于尝试取消键盘IRP，以使得程序可以正常卸载。
*/
BOOLEAN CancelKeyboardIrp(IN PIRP pIrp)
{
    // 1. 如果IRP无效则直接返回
    if ( !pIrp )  return false;

    // 2. 如果此IRP未被取消或其取消例程有值，则执行取消IRP的操作
    if ( pIrp->Cancel=false||pIrp->CancelRoutine )
    {
        if ( !IoCancelIrp(pIrp) )
        {
            KdPrint(( "取消IRP(0x%X)失败！\r\n", pIrp ));
            return false;
        }
        KdPrint(( "成功取消IRP(0x%X)！\r\n", pIrp ));
    }

    // 2. 取消后重设此完成例程为空
    IoSetCancelRoutine( pIrp, NULL );
    return true; 
}





NTSTATUS UnLADDRHook()
{
    // 1. 把当前线程设置为低实时模式，以便让它的运行尽量少影响其他程序
    KeSetPriorityThread(KeGetCurrentThread(), LOW_REALTIME_PRIORITY);

    // 2. 遍历所有设备并一律解除绑定
    PDEVICE_OBJECT objDev = g_objLADDR_Driver->DeviceObject; 
    while (objDev)
    {
        // 解除绑定并删除所有的设备
        LADDR_Detach(objDev);
        objDev = objDev->NextDevice;
    } 

    // 3. 如果还有IRP未完成且当前IRP有效则，则尝试取消这个IRP
    if ( g_uPendingIrpsCount>0 && g_pPendingIrp )
    {
        if ( !CancelKeyboardIrp(g_pPendingIrp) )
        {
            // 循环等待IRP完成
            LARGE_INTEGER stcDelay = RtlConvertLongToLargeInteger(500*MILLI_SECOND);
            while (g_uPendingIrpsCount)
            {
                KdPrint(("还有%d个IRP在等待中...\r\n",g_uPendingIrpsCount));
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
        sizeof(LADDR_DEV_EXT),      // 设备扩展对象的结构体大小
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
    // 4.1 由于届时此驱动会绑定在键盘驱动“Kbdclass”的上层，因此所有“Kbdclass”的IRP都
    //     会被拦截到此驱动中处理，对于一些不被我们关注的常规IRP，我们只需要将其直接
    //     发送到物理设备即可。因此我们需要填写所有的分发函数的指针为我们的处理函数
    //     LADDR_GeneralDispatch()的地址，并在LADDR_GeneralDispatch()中完成跳过虚拟
    //     设备处理，将IRP直接发送到物理设备中的操作。
    for (ULONG i=0; i < IRP_MJ_MAXIMUM_FUNCTION; i++) 
    { 
        objDriver->MajorFunction[i] = LADDR_GeneralDispatch; 
    }
    // 4.2 需要对关键的几个IRP类型设置派遣函数，以使得我们的驱动达到过滤键盘信息的作
    //     用，并支持键盘的热插拔。
    objDriver->MajorFunction[IRP_MJ_READ]  = (PDRIVER_DISPATCH)LADDR_ReadDispatch; // 指定一个Read派遣函数
    objDriver->MajorFunction[IRP_MJ_POWER] = (PDRIVER_DISPATCH)LADDR_Power;        // 因为此类请求要调用PoCallDriver和PoStartNextPowerIrp
    objDriver->MajorFunction[IRP_MJ_PNP]   = (PDRIVER_DISPATCH)LADDR_PnP;          // 处理键盘插拔
    // 4.3 设置一些必要的派遣函数
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