#include "CustomDll.h"
#include <windows.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	//��̬���ӿⱻӳ�䵽ĳ�����̵ĵ�ַ�ռ�
    case DLL_PROCESS_ATTACH:  
    //Ӧ�ó��򴴽��µ��߳�
	case DLL_THREAD_ATTACH:
    //Ӧ�ó���ĳ���߳�������ֹ
	case DLL_THREAD_DETACH:
    //��̬���ӿ⽫��ж��
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

//�Զ��嵼�������������������ĺ�
int DLLFuncAdd(int a,int b)
{

	return a+b;  
}
