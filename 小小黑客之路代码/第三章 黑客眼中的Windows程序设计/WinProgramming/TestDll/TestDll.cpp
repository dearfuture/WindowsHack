/////////////////////////////////////////////////////////////////////////////
//��ʽ��̬����
//////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <iostream>
using namespace std;
//����DLL��ĺ���ԭ��
typedef int (*PFNEXPORTFUNC)(int,int);
int main()
{
	int c = 12,d = 20;
	int result;
	HMODULE hModule = LoadLibrary("CustomDll.dll");
	if(hModule != NULL)
	{
		PFNEXPORTFUNC mDLLFuncAdd = (PFNEXPORTFUNC) GetProcAddress(hModule,"DLLFuncAdd");
		if(mDLLFuncAdd != NULL)
		{
			result = mDLLFuncAdd(c,d);
			cout<<"������32�����ʾ�ɹ�������DLL���������ǣ� "<<result<<endl;
		}
		FreeLibrary(hModule);
	}

	system("pause");
	return 0;
}
