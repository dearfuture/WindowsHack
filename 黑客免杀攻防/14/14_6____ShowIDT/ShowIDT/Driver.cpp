/*
*  《黑客免杀攻防》例子代码
*
*  作    者：任晓珲（A1Pass）
*  日    期：2012-11-14
*  本书官网：http://book.hackav.com  或 http://www.hzbook.com
*  作者微博：http://weibo.com/a1pass 或 http://t.qq.com/a1pass
*  作者博客：http://a1pass.blog.163.com
*/
#include <ntddk.h>

#define MAKE_LONG(Low,High) ( (UINT32)( ((UINT16)(Low)) | ((UINT32)((UINT16)(High))) <<16) )

typedef struct _IDT_INFO 
{
	UINT16 uIdtLimit;    // IDT范围
	UINT16 uLowIdtBase;  // IDT低基址
	UINT16 uHighIdtBase; // IDT高基址
}IDT_INFO,*PIDT_INFO;

// GateType所代表的中断类型
// 0x05 : 32位的任务门（80386）
// 0x06 : 16位的中断门（80286）
// 0x07 : 16位的陷阱门（80286）
// 0x0E : 32位的中断门（80386）
// 0x0F : 32位的陷阱门（80386）
typedef struct _IDT_ENTRY
{
	UINT16 uOffsetLow;       // 处理程序低地址偏移
	UINT16 uSelector;        // 段选择器
	UINT8  uReserved;        // 保留
	UINT8  GateType:4;       // 中断类型
	UINT8  StorageSegment:1; // 为0是中断门
	UINT8  DPL:2;            // 特权级
	UINT8  Present:1;        // 如未使用中断可置为0
	UINT16 uOffsetHigh;      // 处理程序高地址偏移
}IDT_ENTRY,*PIDT_ENTRY;

VOID DriverUnload(PDRIVER_OBJECT objDriver)
{
	UNREFERENCED_PARAMETER(objDriver);
	DbgPrint("My Dirver is unloading...");
}

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT objDriver, PUNICODE_STRING strRegPath)
{
	// 避免编译器关于未引用参数的警告
	UNREFERENCED_PARAMETER(strRegPath);

	DbgPrint("My First Dirver!");

	IDT_INFO   stcIdtInfo = {0};
	PIDT_ENTRY pIdtEntry  = nullptr;

	_asm sidt stcIdtInfo;
	pIdtEntry = (PIDT_ENTRY)MAKE_LONG(stcIdtInfo.uLowIdtBase, stcIdtInfo.uHighIdtBase);

	for (UINT32 uCount=0; uCount <= 256; uCount++)
	{
		PIDT_ENTRY pIndex = &pIdtEntry[uCount];
		UINT32     uAddr  = MAKE_LONG(pIndex->uOffsetLow, pIndex->uOffsetHigh);
		
		DbgPrint("Interrupt %d: ISR 0x%08X\r\n", uCount, uAddr);
	}

	objDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}