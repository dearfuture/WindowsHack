/*
*  《黑客免杀攻防》例子代码
*
*  作    者：任晓珲（A1Pass）
*  日    期：2012-12-12
*  本书官网：http://book.hackav.com  或 http://www.hzbook.com
*  作者微博：http://weibo.com/a1pass 或 http://t.qq.com/a1pass
*  作者博客：http://a1pass.blog.163.com
*/
#include <ntddk.h>





#pragma pack(1)
typedef struct _ServiceDesriptorEntry
{
	ULONG *ServiceTableBase;
	ULONG *ServiceCounterTableBase;
	ULONG NumberOfServices;
	UCHAR *ParamTableBase;
}SSDTEntry,*PSSDTEntry;
#pragma pack()
extern "C" NTSYSAPI SSDTEntry KeServiceDescriptorTable; // 导入SSDT
extern "C" NTSYSAPI NTSTATUS NTAPI ZwDeleteKey(HANDLE KeyHandle);

/* 根据函数地址获取其序号 */
#define SYSCALL_INDEX(_function) \
	(ULONG)*(PULONG)((PUCHAR)_function+1)





VOID DriverUnload(PDRIVER_OBJECT objDriver)
{
	UNREFERENCED_PARAMETER(objDriver);
	DbgPrint("My Dirver is unloading...");
}

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT objDriver, PUNICODE_STRING strRegPath)
{
	// 避免编译器关于未引用参数的警告
	UNREFERENCED_PARAMETER(strRegPath);
	DbgPrint("My First Dirver!\r\n");

	PULONG pSSDT_Base  = KeServiceDescriptorTable.ServiceTableBase;
	ULONG  uSSDT_Count = KeServiceDescriptorTable.NumberOfServices;

	// 1. 根据ZwDeleteKey函数地址获取其系统调用编号
	ULONG uIndex = SYSCALL_INDEX(ZwDeleteKey);
	DbgPrint("ZwDeleteKey Index:%d \r\n",uIndex);

	// 2. 根据ZwDeleteKey的系统编号获取其地址
	DbgPrint("ZwDeleteKey Address:0x%08X \r\n",pSSDT_Base[uIndex]);

	// 3. 遍历系统SSDT详细信息
	DbgPrint("SSDT Address:0x%08X \r\n",pSSDT_Base);
	DbgPrint("SSDT Count:%d \r\n",uSSDT_Count);
	for (UINT32 uCount=0; uCount <= uSSDT_Count; uCount++)
	{
		DbgPrint("Index:%04d SSDT_Addr:0x%08X Fun_Addr:0x%08X\r\n", 
			uCount, 
			&pSSDT_Base[uCount],
			pSSDT_Base[uCount]);
	}

	objDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}