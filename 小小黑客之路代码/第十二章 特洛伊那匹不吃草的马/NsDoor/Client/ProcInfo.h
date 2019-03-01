#include "Header.h"
#include "MainFunc.h"


extern HWND hProcList;

//初始化列表
BOOL InitProcList(HWND hList);
//PROCVIEW封装函数
BOOL ProcView(SOCKET sockfd,int dwSize);
//根据PID删除远程进程，PID=0时代表刷新进程
BOOL ProcDeleteA(SOCKET sockfd,HWND hDlg);
//返回删除结果
BOOL ProcReDel(SOCKET sockfd,int dwRecvSize);
//根据PID删除进程
BOOL ProcDelete(SOCKET sockfd,int pid);