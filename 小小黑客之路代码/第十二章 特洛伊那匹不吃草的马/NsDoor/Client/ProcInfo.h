#include "Header.h"
#include "MainFunc.h"


extern HWND hProcList;

//��ʼ���б�
BOOL InitProcList(HWND hList);
//PROCVIEW��װ����
BOOL ProcView(SOCKET sockfd,int dwSize);
//����PIDɾ��Զ�̽��̣�PID=0ʱ����ˢ�½���
BOOL ProcDeleteA(SOCKET sockfd,HWND hDlg);
//����ɾ�����
BOOL ProcReDel(SOCKET sockfd,int dwRecvSize);
//����PIDɾ������
BOOL ProcDelete(SOCKET sockfd,int pid);