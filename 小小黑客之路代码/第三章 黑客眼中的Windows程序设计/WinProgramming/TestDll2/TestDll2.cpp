//隐式动态链接
#include <windows.h>
#include <iostream>
#include "CustomDll.h"
using namespace std;

#pragma comment(lib,"CustomDll")

int main()
{
	int c = 12,d = 20;
	int result;
	result = DLLFuncAdd(c,d);

	cout<<"如果输出32，则表示成功调用了DLL。输出结果是： "<<result<<endl;
	return 0;
	
}
