/*
*  ���ڿ���ɱ���������Ӵ���
*
*  ��    �ߣ���������A1Pass��
*  ��    �ڣ�2011-04-28
*  ���������http://book.hackav.com  �� http://www.hzbook.com
*  ����΢����http://weibo.com/a1pass �� http://t.qq.com/a1pass
*  ���߲��ͣ�http://a1pass.blog.163.com
*/
#include "stdafx.h"
#include "PEDemo.h"


// exported variable
int nPEDemo=0;

// exported function.
int fnPEDemoFun(void)
{
    Sleep(0x10);
    return 1;
}

int fnPEDemoFunA(void)
{
    Sleep(0x20);
    return 2;
}

int fnPEDemoFunB(void)
{
    Sleep(0x30);
    return 3;
}

