/////////////////////////////////////////////////////////////////////////////
//显式动态链接
//////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <iostream>
using namespace std;
//声明DLL里的函数原型
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
			cout<<"如果输出32，则表示成功调用了DLL。输出结果是： "<<result<<endl;
		}
		FreeLibrary(hModule);
	}

	system("pause");
	return 0;
}
