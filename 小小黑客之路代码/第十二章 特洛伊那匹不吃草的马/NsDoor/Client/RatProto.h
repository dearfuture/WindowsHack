//////////////////////////////////////////////////////////////////////////
//File�� RatProto.cpp
//REM:	 NsDoor Ptotocol
//////////////////////////////////////////////////////////////////////////
#pragma once
//�������
#define IDNUM 100
//NAME�ֽ���
#define NAMESIZE 80


enum RATID{
FILE_VIEW,FILE_OPEN,FILE_DELETE,FILE_EXCUTE,FILE_DOWNLOAD,FILE_UPLOAD,FILE_BACK,FILE_REPLY,FILE_DOWNREPLY,FILE_TRANSMIT,
CMD_REQUSET,CMD_REPLY,
PROC_VIEW,PROC_DELETE,
KEY_OK,KEY_CANCEL,KEY_REPLY,
DESK_VIEW,
SYS_VIEW
};



//NsDoor Э��
struct RatProto{
	RATID RatId;		//����ID
	int	  RatCsm;		//��Կ
	int	  RatLen;		//���ݲ��ֳ���
};

//����ϵͳ��Ϣ�ṹ��
struct SysInfo{
	char Name[20];
	char Value[80];
};
//������Ϣ�ṹ��
struct ProcInfo{
	int pid;
	char Name[30];
};
//�ļ���Ϣ�ṹ��
struct FileInfo{
	int Type;
	char Name[NAMESIZE];
};

/*
�Ƚ���Э��ͷ����RatProto����������ID����������ID�ǲ���Ҫ�����ģ�����SYSINFO_VIEW,��ֱ�Ӹ���RatFuncɢ��������
RatFunc[foo_ID],�����Ӧ�����������ַ��
�������Ҫ�����ģ�����RatCsm���Э��ļ������ͣ���base64��������У��ͣ�����CRCУ��ȣ��������ݵĳ�����
RatLen-sizeof(RatProto)������������Щ��Ϣ�����������ݻ��߽��м�У�飬����RatLen<sizeof(RatProto),Ҳ����
��Э����ѳ�����������
������������CopyMemory��һ�����������ѻ�������ַ��ΪRatFunc[foo_ID]��args������
�����ܶ�Ӧ��������C/S����õģ��������ڲ��ٶ���ṹ����н���������


�󲿷�����Ĳ����ֽ��������٣����費����ִ���
����FILE_DOWNLOAD,FILE_UPLOAD��Ϣʱ���� �����ļ��ĳ��ȣ�Ȼ���Ƭ������ÿƬ�ǹ̶����ȣ���RatLenӦ��Ϊһ��ֵ��
������RatLen���ٵ����Ǹ���ֵʱ���ڴ�����ȷ�������Ӧ�����Ѿ��յ��������ļ����ݣ�Ȼ��ѽ��յ������ֽ�������
�����ȱȽϣ����һ�´�����ɹ�����������ļ������ݰ����쳣���ݰ�
*/