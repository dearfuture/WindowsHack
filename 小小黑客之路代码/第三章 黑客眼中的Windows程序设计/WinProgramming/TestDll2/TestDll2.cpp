//��ʽ��̬����
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

	cout<<"������32�����ʾ�ɹ�������DLL���������ǣ� "<<result<<endl;
	return 0;
	
}
