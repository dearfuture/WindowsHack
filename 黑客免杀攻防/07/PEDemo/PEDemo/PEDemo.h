/*
*  ���ڿ���ɱ���������Ӵ���
*
*  ��    �ߣ���������A1Pass��
*  ��    �ڣ�2011-04-28
*  ���������http://book.hackav.com  �� http://www.hzbook.com
*  ����΢����http://weibo.com/a1pass �� http://t.qq.com/a1pass
*  ���߲��ͣ�http://a1pass.blog.163.com
*/
#ifdef PEDEMO_EXPORTS
#define PEDEMO_API extern "C" __declspec(dllexport)
#else
#define PEDEMO_API __declspec(dllimport)
#endif

//#pragma comment(linker,"/EXPORT:?fnPEDemoFun@@YAHXZ,@1,NONAME")

PEDEMO_API int nPEDemo;
PEDEMO_API int fnPEDemoFun();
PEDEMO_API int fnPEDemoFunA();
PEDEMO_API int fnPEDemoFunB();

