#include "Header.h"
#include "resource.h"
#include "Protocol.h"

arp_hdr ArpHead;
ether_hdr EtherHead;

//////////////////////////////////////////////////////////////////////////
//发送函数
//////////////////////////////////////////////////////////////////////////
BOOL SendArpPacket(void)
{

	//winpcap句柄
	pcap_t *pcapHandle;

	//网络设备指针
	pcap_if_t *pcapDev;

	//存储错误信息
	char errContent[PCAP_ERRBUF_SIZE];

	//数据包缓冲区
	unsigned char buff[64] = {0};

	//填充协议首部
	FillHeaders();
	
	//以太网协议首部和ARP协议首部填充到缓冲区
	memcpy(buff,&EtherHead,sizeof(ether_hdr));
	memcpy(buff+sizeof(ether_hdr),&ArpHead,sizeof(arp_hdr));

	// 获得网络设备指针
	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &pcapDev, errContent) == -1)
	{
		MessageBoxA(NULL,"获取网络设备失败","警告",MB_OK);
		return FALSE;
	}
	//选取合适网卡
	while (pcapDev->addresses==NULL)
	{
		pcapDev = pcapDev->next;
	}

	//打开网卡
	if ((pcapHandle = pcap_open(pcapDev->name,65536,PCAP_OPENFLAG_PROMISCUOUS,1000,NULL,errContent)) == NULL )
	{
		MessageBoxA(NULL,"打开网卡失败","警告",MB_OK);
		return FALSE;
	}
	//循环发送，Sending为全局变量
	while(Sending)
	{
		pcap_sendpacket(pcapHandle,buff,64/*size*/);
		Sleep(10);
		SetDlgItemText(hDlg,IDC_STATIC_LIGHT,L"发送中...");
		Sleep(10);
		SetDlgItemText(hDlg,IDC_STATIC_LIGHT,L"");
	}
	pcap_freealldevs(pcapDev);
	pcap_close(pcapHandle);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//填充协议头函数
//////////////////////////////////////////////////////////////////////////
BOOL FillHeaders()
{
	//源、目的IP地址
	unsigned long IpSrc = 0;
	unsigned long IpDest = 0;
	//源、目的MAC地址
	WCHAR MacSrc[18] = {0};
	WCHAR MacDest[18] = {0};

	//从UI获取IP和MAC
	SendMessage(GetDlgItem(hDlg,IDC_IP_SRC),IPM_GETADDRESS,0,(LPARAM)&IpSrc);
	SendMessage(GetDlgItem(hDlg,IDC_IP_DEST),IPM_GETADDRESS,0,(LPARAM)&IpDest);
	GetDlgItemText(hDlg,IDC_MAC_SRC,MacSrc,18);
	GetDlgItemText(hDlg,IDC_MAC_DEST,MacDest,18);

	//大小定基转换
	IpSrc = htonl(IpSrc);
	//填充，ArpHead是全局变量
	memcpy(ArpHead.SrcIp,&IpSrc,sizeof(char)*4);
	//大小定基转换
	IpDest = htonl(IpDest);
	//填充
	memcpy(ArpHead.DestIp,&IpDest,sizeof(char)*4);

	//MAC字符串转换到实际MAC地址并填充，EtherHead为全局变量
	MacStrToMac(MacSrc,ArpHead.EtherSrcHost);
	MacStrToMac(MacSrc,EtherHead.EtherSrcHost);
	MacStrToMac(MacDest,ArpHead.EtherDestHost);
	MacStrToMac(MacDest,EtherHead.EtherDestHost);
	
	//填充其它字段
	//上层协议类型，0x0806为ARP协议
	EtherHead.EtherType = htons(0x0806);
	//硬件地址类型，0x0001为以太网类型
	ArpHead.HardwareType = htons(0x0001);
	//上层协议类型，0x0800为IP协议
	ArpHead.ProtocolType = htons(0x0800);
	//硬件地址长度，MAC地址长度为6字节
	ArpHead.HardwareLength = 0x06;
	//IP地址长度，4字节
	ArpHead.ProtocolLength = 0x04;
	//填充为0x0001表示ARP响应
	ArpHead.OperationCode = htons(0x0002);

	return TRUE;

}

//////////////////////////////////////////////////////////////////////////
//MAC字符串转换
//////////////////////////////////////////////////////////////////////////
BOOL MacStrToMac(WCHAR *MacStr,unsigned char *Mac)
{
	WCHAR *str = MacStr;
	int i;
	int low,high;
	char temp;
	for(i=0;i<6;i++)
	{
		if (str[0]-L'0'>=0&&str[0]-L'0'<=9)
		{
			high = str[0] - L'0';
		}else if(str[0]-L'a'>=0&&str[0]-L'a'<=5)
		{
			high = str[0] - L'a'+10;
		}else if(str[0]-L'A'>=0&&str[0]-L'A'<=5)
		{
			high = str[0] - L'A'+10;
		}

		if (str[1]-L'0'>=0&&str[1]-L'0'<=9)
		{
			low = str[1] - L'0';
		}else if(str[1]-L'a'>=0&&str[1]-L'a'<=5)
		{
			low = str[1] - L'a'+10;
		}else if(str[1]-L'A'>=0&&str[1]-L'A'<=5)
		{
			low = str[1] - L'A'+10;
		}
		Mac[i]=high*16+low;
		str+=3;
	}
	return TRUE;
}