/*
*  《黑客免杀攻防》例子代码
*
*  作    者：任晓珲（A1Pass）
*  日    期：2011-04-28
*  本书官网：http://book.hackav.com  或 http://www.hzbook.com
*  作者微博：http://weibo.com/a1pass 或 http://t.qq.com/a1pass
*  作者博客：http://a1pass.blog.163.com
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

