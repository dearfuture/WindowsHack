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


//���Զ�������̷�
BOOL SendDriveList(SOCKET sockfd);

//��ʼ��ListControl
BOOL InitFmgrList(HWND hList);
//��ʼ��״̬��
BOOL UpdataSBar(HWND hDlg);

//���ļ�
BOOL FileOpen(SOCKET sockfd,char* szFile,char* szType);
//ɾ���ļ�
BOOL FileDelete(SOCKET sockfd,char* szFile);
//������һ��
BOOL FolderBack(SOCKET sockfd);
//�����ļ�
BOOL FileDownLoad(SOCKET sockfd,char* szFile);

//ˢ���ļ��б�
BOOL FileView(SOCKET sockfd,int dwRecvSize);
//�����ļ�����������Ϣ
BOOL FileReply(SOCKET sockfd,int dwRecvSize);
//����ļ�����
char* GetFileType(int type);
//Ϊ�ļ�������׼��
BOOL FileDownReply(SOCKET sockfd,int dwRecvSize);
//�ļ�����
BOOL FileTransmit(SOCKET sockfd,int dwRecvSize);