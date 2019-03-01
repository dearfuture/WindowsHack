/*
*  ���ڿ���ɱ���������Ӵ���
*
*  ��    �ߣ���������A1Pass��
*  ��    �ڣ�2012-11-14
*  ���������http://book.hackav.com  �� http://www.hzbook.com
*  ����΢����http://weibo.com/a1pass �� http://t.qq.com/a1pass
*  ���߲��ͣ�http://a1pass.blog.163.com
*/
#include <ntddk.h>

#define MAKE_LONG(Low,High) ( (UINT32)( ((UINT16)(Low)) | ((UINT32)((UINT16)(High))) <<16) )

typedef struct _IDT_INFO 
{
	UINT16 uIdtLimit;    // IDT��Χ
	UINT16 uLowIdtBase;  // IDT�ͻ�ַ
	UINT16 uHighIdtBase; // IDT�߻�ַ
}IDT_INFO,*PIDT_INFO;

// GateType��������ж�����
// 0x05 : 32λ�������ţ�80386��
// 0x06 : 16λ���ж��ţ�80286��
// 0x07 : 16λ�������ţ�80286��
// 0x0E : 32λ���ж��ţ�80386��
// 0x0F : 32λ�������ţ�80386��
typedef struct _IDT_ENTRY
{
	UINT16 uOffsetLow;       // �������͵�ַƫ��
	UINT16 uSelector;        // ��ѡ����
	UINT8  uReserved;        // ����
	UINT8  GateType:4;       // �ж�����
	UINT8  StorageSegment:1; // Ϊ0���ж���
	UINT8  DPL:2;            // ��Ȩ��
	UINT8  Present:1;        // ��δʹ���жϿ���Ϊ0
	UINT16 uOffsetHigh;      // �������ߵ�ַƫ��
}IDT_ENTRY,*PIDT_ENTRY;

VOID DriverUnload(PDRIVER_OBJECT objDriver)
{
	UNREFERENCED_PARAMETER(objDriver);
	DbgPrint("My Dirver is unloading...");
}

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT objDriver, PUNICODE_STRING strRegPath)
{
	// �������������δ���ò����ľ���
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