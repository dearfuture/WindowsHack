#include "Header.h"
#include "resource.h"
#include "Protocol.h"

arp_hdr ArpHead;
ether_hdr EtherHead;

//////////////////////////////////////////////////////////////////////////
//���ͺ���
//////////////////////////////////////////////////////////////////////////
BOOL SendArpPacket(void)
{

	//winpcap���
	pcap_t *pcapHandle;

	//�����豸ָ��
	pcap_if_t *pcapDev;

	//�洢������Ϣ
	char errContent[PCAP_ERRBUF_SIZE];

	//���ݰ�������
	unsigned char buff[64] = {0};

	//���Э���ײ�
	FillHeaders();
	
	//��̫��Э���ײ���ARPЭ���ײ���䵽������
	memcpy(buff,&EtherHead,sizeof(ether_hdr));
	memcpy(buff+sizeof(ether_hdr),&ArpHead,sizeof(arp_hdr));

	// ��������豸ָ��
	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &pcapDev, errContent) == -1)
	{
		MessageBoxA(NULL,"��ȡ�����豸ʧ��","����",MB_OK);
		return FALSE;
	}
	//ѡȡ��������
	while (pcapDev->addresses==NULL)
	{
		pcapDev = pcapDev->next;
	}

	//������
	if ((pcapHandle = pcap_open(pcapDev->name,65536,PCAP_OPENFLAG_PROMISCUOUS,1000,NULL,errContent)) == NULL )
	{
		MessageBoxA(NULL,"������ʧ��","����",MB_OK);
		return FALSE;
	}
	//ѭ�����ͣ�SendingΪȫ�ֱ���
	while(Sending)
	{
		pcap_sendpacket(pcapHandle,buff,64/*size*/);
		Sleep(10);
		SetDlgItemText(hDlg,IDC_STATIC_LIGHT,L"������...");
		Sleep(10);
		SetDlgItemText(hDlg,IDC_STATIC_LIGHT,L"");
	}
	pcap_freealldevs(pcapDev);
	pcap_close(pcapHandle);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//���Э��ͷ����
//////////////////////////////////////////////////////////////////////////
BOOL FillHeaders()
{
	//Դ��Ŀ��IP��ַ
	unsigned long IpSrc = 0;
	unsigned long IpDest = 0;
	//Դ��Ŀ��MAC��ַ
	WCHAR MacSrc[18] = {0};
	WCHAR MacDest[18] = {0};

	//��UI��ȡIP��MAC
	SendMessage(GetDlgItem(hDlg,IDC_IP_SRC),IPM_GETADDRESS,0,(LPARAM)&IpSrc);
	SendMessage(GetDlgItem(hDlg,IDC_IP_DEST),IPM_GETADDRESS,0,(LPARAM)&IpDest);
	GetDlgItemText(hDlg,IDC_MAC_SRC,MacSrc,18);
	GetDlgItemText(hDlg,IDC_MAC_DEST,MacDest,18);

	//��С����ת��
	IpSrc = htonl(IpSrc);
	//��䣬ArpHead��ȫ�ֱ���
	memcpy(ArpHead.SrcIp,&IpSrc,sizeof(char)*4);
	//��С����ת��
	IpDest = htonl(IpDest);
	//���
	memcpy(ArpHead.DestIp,&IpDest,sizeof(char)*4);

	//MAC�ַ���ת����ʵ��MAC��ַ����䣬EtherHeadΪȫ�ֱ���
	MacStrToMac(MacSrc,ArpHead.EtherSrcHost);
	MacStrToMac(MacSrc,EtherHead.EtherSrcHost);
	MacStrToMac(MacDest,ArpHead.EtherDestHost);
	MacStrToMac(MacDest,EtherHead.EtherDestHost);
	
	//��������ֶ�
	//�ϲ�Э�����ͣ�0x0806ΪARPЭ��
	EtherHead.EtherType = htons(0x0806);
	//Ӳ����ַ���ͣ�0x0001Ϊ��̫������
	ArpHead.HardwareType = htons(0x0001);
	//�ϲ�Э�����ͣ�0x0800ΪIPЭ��
	ArpHead.ProtocolType = htons(0x0800);
	//Ӳ����ַ���ȣ�MAC��ַ����Ϊ6�ֽ�
	ArpHead.HardwareLength = 0x06;
	//IP��ַ���ȣ�4�ֽ�
	ArpHead.ProtocolLength = 0x04;
	//���Ϊ0x0001��ʾARP��Ӧ
	ArpHead.OperationCode = htons(0x0002);

	return TRUE;

}

//////////////////////////////////////////////////////////////////////////
//MAC�ַ���ת��
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