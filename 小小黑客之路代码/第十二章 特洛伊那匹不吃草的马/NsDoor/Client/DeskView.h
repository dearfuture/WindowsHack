#include "Header.h"
#include "MainFunc.h"

//����Զ����������
BOOL DeskViewRequest(SOCKET sockfd,HWND hDlg);
//��������
BOOL RecvBmpView(SOCKET sockfd,HWND hDlg);
//Bmp��ʾ
BOOL DisplayDeskView(HWND hDlg,char* pData);
//���ƽ��չ̶���������
BOOL RecvData(SOCKET sockfd,char* pData,int dwSize);
