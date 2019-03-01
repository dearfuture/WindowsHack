#pragma once
#include "Header.h"
#include "MainFunc.h"


#define PERSENDBYTES 10000

extern HWND hFmgrList;
extern HWND hFmgrSBar;
extern char szTempPath[MAX_PATH];
extern int dwFileSize;
extern char* pFileData;
extern char szFname[MAX_PATH];


//获得远程主机盘符
BOOL SendDriveList(SOCKET sockfd);

//初始化ListControl
BOOL InitFmgrList(HWND hList);
//初始化状态栏
BOOL UpdataSBar(HWND hDlg);

//打开文件
BOOL FileOpen(SOCKET sockfd,char* szFile,char* szType);
//删除文件
BOOL FileDelete(SOCKET sockfd,char* szFile);
//返回上一级
BOOL FolderBack(SOCKET sockfd);
//下载文件
BOOL FileDownLoad(SOCKET sockfd,char* szFile);

//刷新文件列表
BOOL FileView(SOCKET sockfd,int dwRecvSize);
//接收文件操作反馈信息
BOOL FileReply(SOCKET sockfd,int dwRecvSize);
//获得文件类型
char* GetFileType(int type);
//为文件传输做准备
BOOL FileDownReply(SOCKET sockfd,int dwRecvSize);
//文件传输
BOOL FileTransmit(SOCKET sockfd,int dwRecvSize);