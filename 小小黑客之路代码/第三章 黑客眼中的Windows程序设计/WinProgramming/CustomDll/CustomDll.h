#ifdef CUSTOMDLL_EXPORTS                    //Ԥ�����
#define CUSTOMDLL_API __declspec(dllexport)
#else
#define CUSTOMDLL_API __declspec(dllimport)
#endif
#define EXTERNC extern "C"
//�����Զ��嵼������
EXTERNC CUSTOMDLL_API int DLLFuncAdd(int a,int b);
