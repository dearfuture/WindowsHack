/*
*  ���ڿ���ɱ���������Ӵ���
*
*  ��    �ߣ���������A1Pass��
*  ��    �ڣ�2012-11-02
*  ���������http://book.hackav.com  �� http://www.hzbook.com
*  ����΢����http://weibo.com/a1pass �� http://t.qq.com/a1pass
*  ���߲��ͣ�http://a1pass.blog.163.com
*/
#include <ntddk.h>

VOID DriverUnload(PDRIVER_OBJECT objDriver)
{
	// �������������δ���ò����ľ���
	UNREFERENCED_PARAMETER(objDriver);

	// ʲôҲ������ֻ��ӡһ���ַ���
	DbgPrint("My Dirver is unloading...");
}

NTSTATUS DriverEntry(PDRIVER_OBJECT objDriver, PUNICODE_STRING strRegPath)
{
	// �������������δ���ò����ľ���
	UNREFERENCED_PARAMETER(strRegPath);

	// ������뷽ʽΪDebug�������һ��INT 3ָ��������ǵ���
#ifdef DBG
	_asm int 3;
#endif

	// ��ӡһ���ַ�������ע������ж�غ������Ա�������ж��
	DbgPrint("My First Dirver!");
	objDriver->DriverUnload = DriverUnload;

	return STATUS_SUCCESS;
}