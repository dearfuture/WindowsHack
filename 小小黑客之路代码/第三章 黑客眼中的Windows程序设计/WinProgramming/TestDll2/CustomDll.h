#ifdef CUSTOMDLL_EXPORTS                    //Ԥ�����
#define CUSTOMDLL_API __declspec(dllexport)
#else
#define CUSTOMDLL_API __declspec(dllimport)
#endif
//�����Զ��嵼������
CUSTOMDLL_API int DLLFuncAdd(int a,int b);
