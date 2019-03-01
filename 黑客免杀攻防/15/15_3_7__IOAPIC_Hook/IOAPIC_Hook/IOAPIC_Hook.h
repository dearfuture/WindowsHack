/*
*  ���ڿ���ɱ���������Ӵ���
*
*  ��    �ߣ���������A1Pass��
*  ��    �ڣ�2013-01-15
*  ���������http://book.hackav.com  �� http://www.hzbook.com
*  ����΢����http://weibo.com/a1pass �� http://t.qq.com/a1pass
*  ���߲��ͣ�http://a1pass.blog.163.com
*/
#pragma once


/************************************************************************/
/* �Զ����豸���Ƽ�������������                                         */
/************************************************************************/
#define DEVICE_NAME                L"\\Device\\IOAPIC_Hook"
#define SYMBOLIC_LINK_NAME         L"\\DosDevices\\IOAPIC_Hook_SymLink"
#define SYMBOLIC_LINK_GLOBAL_NAME  L"\\DosDevices\\Global\\IOAPIC_Hook_SymLink"
/************************************************************************/
/* �Զ��������                                                         */
/************************************************************************/
#define IOCTL_BASE        0x800
#define MY_CTL_CODE(i)                                              \
	CTL_CODE                                                        \
	(                                                               \
	FILE_DEVICE_UNKNOWN,  /* �����Ƶ��������� */                    \
	IOCTL_BASE + i,       /* 0x800~0xFFF�ǿ��ɳ���Ա�Զ���Ĳ��� */ \
	METHOD_BUFFERED,      /* ����ģʽ��ʹ�û�������ʽ���� */        \
	FILE_ANY_ACCESS       /* ����Ȩ�ޣ�ȫ�� */                      \
	)
#define IOCTL_HELLO_WORLD  MY_CTL_CODE(0)
#define IOCTL_HOOK_IDT     MY_CTL_CODE(1)
#define IOCTL_UNHOOK_IDT   MY_CTL_CODE(2)





#ifdef __cplusplus
extern "C" {
#endif
/************************************************************************/
/* ͷ�ļ�                                                               */
/************************************************************************/
#include <ntddk.h>
#include <devioctl.h>
#include <wdmsec.h> // ��Ҫ�ڹ����а�����$(DDK_LIB_PATH)\wdmsec.lib��
#include <ntstrsafe.h>
/************************************************************************/
/* ����ȫ������                                                         */
/************************************************************************/
#pragma pack(1)
	// IDT�ṹ
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

	// IOREGSEL�Ĵ����ṹ
	typedef struct _IOREGSEL_INFO
	{
		UINT8 uAPICR;     // APIC�Ĵ�����ַ
		UINT8 Reserved_0; // ����
		UINT8 Reserved_1; // ����
		UINT8 Reserved_2; // ����
	}IOREGSEL_INFO,*PIOREGSEL_INFO;
#pragma pack()

/************************************************************************/
/* ȫ�ֱ���������                                                       */
/************************************************************************/
PVOID            g_pVisualAddr     = 0;             // IOAPIC�������Ĺؼ��Ĵ���ӳ���������ַ
PIOREGSEL_INFO   g_pIOREGSEL       = 0;             // IOAPIC��������ѡ��Ĵ���
PUINT32          g_pIOWIN          = 0;             // IOAPIC�������Ĵ��ڼĴ���
UINT32           g_uOrigIRQ        = 0;             // �������ԭIRQ
UINT8            g_uVector         = 0;             // ָ��һ����IDT���ж�����
ULONG            g_uOrigISRPointer = 0;             // ԭʼISR
PIDT_ENTRY       g_pIdtEntry       = nullptr;       // IDT���ָ��
UCHAR            g_cLastScanCode   = 0;             // ��һ�λ�ȡ���ļ���ɨ����
PUCHAR           g_pKeyboardPort60 = (PUCHAR)0x60;  // ����ɨ����˿�
PUCHAR           g_pKeyboardPort64 = (PUCHAR)0x64;  // ����״̬�˿�
/************************************************************************/
/* ��������                                                         */
/************************************************************************/
NTSTATUS DriverEntry(IN PDRIVER_OBJECT  objDriver, IN PUNICODE_STRING pRegistryString);
NTSTATUS DispatchCreateClose(IN PDEVICE_OBJECT objDeivce, IN PIRP pIrp);
NTSTATUS DispatchDeviceControl(IN PDEVICE_OBJECT objDeivce, IN PIRP pIrp);
VOID     DriverUnload(IN PDRIVER_OBJECT objDriver);
//----------------------------------------------------------------------//
VOID P2C_WaitForKeyboard(UCHAR cType);
VOID PrintScanCode();
VOID MyInterruptHook();
UINT8 GetNullIDT();
NTSTATUS HookIDT();
NTSTATUS UnHookIDT();
/************************************************************************/
/* ������ҳ����                                                         */
/************************************************************************/
#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)  // ��ʼ����ɺ�����DriverEntry�����б�����
#pragma alloc_text(PAGE, DispatchCreateClose)
#pragma alloc_text(PAGE, DispatchDeviceControl)
#pragma alloc_text(PAGE, DriverUnload)

#pragma alloc_text(PAGE, HookIDT)
#pragma alloc_text(PAGE, UnHookIDT)
#endif
/************************************************************************/
/* �궨��                                                               */
/************************************************************************/

/* ���ݺ�����ַ��ȡ����� */
#define MAKE_LONG(_Low,_High)  ( (UINT32)( ((UINT16)(_Low)) | ((UINT32)((UINT16)(_High))) <<16) )
#define READ_PORT(_Port) _asm{_asm in al,_Port;_asm };

#define IOREGSEL 0xFEC00000
//#define IOWIN    0xFEC00010

// �����жϺ�
//#define NT_INT_KEYBD  0xB3
//#define NT_INT_KEYBD  0x31
//#define NT_INT_KEYBD  0x93
#define NT_INT_KEYBD  0x81 /*Windows 7*/

#define KEYBOARD_IRQ  0x01

#define IBUFFER_FULL  0x02
#define OBUFFER_FULL  0x01

#ifdef __cplusplus
}
#endif