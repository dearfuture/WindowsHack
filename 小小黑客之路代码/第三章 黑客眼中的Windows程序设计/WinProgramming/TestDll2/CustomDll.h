#ifdef CUSTOMDLL_EXPORTS                    //预定义宏
#define CUSTOMDLL_API __declspec(dllexport)
#else
#define CUSTOMDLL_API __declspec(dllimport)
#endif
//声明自定义导出函数
CUSTOMDLL_API int DLLFuncAdd(int a,int b);
