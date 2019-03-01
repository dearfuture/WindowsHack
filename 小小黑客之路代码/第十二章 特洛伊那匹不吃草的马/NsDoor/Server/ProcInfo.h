#include "Header.h"
#include "RatProto.h"

#define PROCNUMS	100
//PROCVIEW函数
BOOL ProcView(SOCKET sockfd,int dwRecvSize);
//删除PID指定进程
BOOL ProcDelete(SOCKET sockfd,int dwRecvSize);