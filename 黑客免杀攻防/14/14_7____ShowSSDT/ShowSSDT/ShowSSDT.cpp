/*
*  ���ڿ���ɱ���������Ӵ���
*
*  ��    �ߣ���������A1Pass��
*  ��    �ڣ�2012-12-12
*  ���������http://book.hackav.com  �� http://www.hzbook.com
*  ����΢����http://weibo.com/a1pass �� http://t.qq.com/a1pass
*  ���߲��ͣ�http://a1pass.blog.163.com
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
extern "C" NTSYSAPI SSDTEntry KeServiceDescriptorTable; // ����SSDT
extern "C" NTSYSAPI NTSTATUS NTAPI ZwDeleteKey(HANDLE KeyHandle);

/* ���ݺ�����ַ��ȡ����� */
#define SYSCALL_INDEX(_function) \
	(ULONG)*(PULONG)((PUCHAR)_function+1)





VOID DriverUnload(PDRIVER_OBJECT objDriver)
{
	UNREFERENCED_PARAMETER(objDriver);
	DbgPrint("My Dirver is unloading...");
}

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT objDriver, PUNICODE_STRING strRegPath)
{
	// �������������δ���ò����ľ���
	UNREFERENCED_PARAMETER(strRegPath);
	DbgPrint("My First Dirver!\r\n");

	PULONG pSSDT_Base  = KeServiceDescriptorTable.ServiceTableBase;
	ULONG  uSSDT_Count = KeServiceDescriptorTable.NumberOfServices;

	// 1. ����ZwDeleteKey������ַ��ȡ��ϵͳ���ñ��
	ULONG uIndex = SYSCALL_INDEX(ZwDeleteKey);
	DbgPrint("ZwDeleteKey Index:%d \r\n",uIndex);

	// 2. ����ZwDeleteKey��ϵͳ��Ż�ȡ���ַ
	DbgPrint("ZwDeleteKey Address:0x%08X \r\n",pSSDT_Base[uIndex]);

	// 3. ����ϵͳSSDT��ϸ��Ϣ
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