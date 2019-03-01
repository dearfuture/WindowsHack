

#include "Header.h"
#include "resource.h"



HKEY hKey ;
HKEY hBaseKey;

//////////////////////////////////////////////////////////////////////////
//ö���Ӽ�
//////////////////////////////////////////////////////////////////////////
BOOL EnumKey()
{
	int Index = 0;
	DWORD num = 512;
	//����
	WCHAR Buff[512] = {0};
	SendMessage(GetDlgItem(hDlg,IDC_LIST_KEY),LB_RESETCONTENT,NULL,NULL);
	//ѭ��ö���Ӽ�ֱ��RegEnumKeyEx()����ERROR_NO_MORE_ITEMS
	while (TRUE)
	{
		if (RegEnumKeyEx(hKey,Index,(LPWSTR)Buff,&num,NULL,NULL,NULL,NULL)==ERROR_NO_MORE_ITEMS)
		{
			break;
		}
		SendMessage(GetDlgItem(hDlg,IDC_LIST_KEY),LB_INSERTSTRING,Index,(LPARAM)Buff);
		Index++;
		num = 512;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//ö�ټ�ֵ
//////////////////////////////////////////////////////////////////////////
BOOL EnumValue()
{
	int Index = 0;
	DWORD num = 512;
	//����
	WCHAR Buff[512] = {0};
	SendMessage(GetDlgItem(hDlg,IDC_LIST_VALUE),LB_RESETCONTENT,NULL,NULL);
	//ѭ��ö���Ӽ�ֱ��RegEnumValue()����ERROR_NO_MORE_ITEMS
	while (TRUE)
	{
		if (RegEnumValue(hKey,Index,(LPWSTR)Buff,&num,NULL,NULL,NULL,NULL)==ERROR_NO_MORE_ITEMS)
		{
			break;
		}
		SendMessage(GetDlgItem(hDlg,IDC_LIST_VALUE),LB_INSERTSTRING,Index,(LPARAM)Buff);
		Index++;
		num = 512;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//���Ӽ�
//////////////////////////////////////////////////////////////////////////
BOOL OpenKey()
{
	//����
	WCHAR Buff[1024] = {0};
	//�������ƻ���
	WCHAR BaseKey[32];
	//��UI��ø�������
	GetDlgItemText(hDlg,IDC_COMBO_RKEY,BaseKey,32);
	//���ݸ�������ΪhBaseKey��ȫ��HKEY��������ֵ
	if (wcscmp(BaseKey,L"HKEY_CLASSES_ROOT")==0)
	{
		hBaseKey = HKEY_CLASSES_ROOT;
	}else if (wcscmp(BaseKey,L"HKEY_CURRENT_USER")==0)
	{
		hBaseKey = HKEY_CURRENT_USER;
	}else if(wcscmp(BaseKey,L"HKEY_LOCAL_MACHINE")==0)
	{
		hBaseKey = HKEY_LOCAL_MACHINE;
	}else if (wcscmp(BaseKey,L"HKEY_CURRENT_CONFIG")==0)
	{
		hBaseKey = HKEY_CURRENT_CONFIG;
	}else if (wcscmp(BaseKey,L"HKEY_USERS")==0)
	{
		hBaseKey = HKEY_USERS;
	}
	//��ȡ��·��
	GetDlgItemText(hDlg,IDC_EDIT_PATH,Buff,1024);
	//�򿪸ü�
	if (RegOpenKeyEx(hBaseKey,Buff,NULL,KEY_ALL_ACCESS,&hKey))
	{
		//��ʧ�����IDC_EDIT_PATH������ɾ�����һ��·����
		WCHAR Buff[1024] = {0};
		WCHAR *pBuff;
		GetDlgItemText(hDlg,IDC_EDIT_PATH,Buff,1024);
		for (pBuff = Buff+wcslen(Buff)-1;pBuff>=Buff;pBuff--)
		{
			if (*pBuff==L'\\'||pBuff == Buff)
			{
				memset(pBuff,0,32);
				SetDlgItemText(hDlg,IDC_EDIT_PATH,Buff);
				break;
			}
		}
		MessageBoxA(NULL,"���Ӽ�ʧ��",NULL,NULL);
		return FALSE;
	}
	//ö���Ӽ�����ֵ
	EnumKey();
	EnumValue();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//����Ӽ�
//////////////////////////////////////////////////////////////////////////
BOOL AddKey()
{
	//�Ӽ����ƻ���
	WCHAR Name [512];
	//������
	DWORD Disposition;
	DWORD ret;
	//���Ӽ����
	HKEY hNewKey;
	GetDlgItemText(hDlg,IDC_EDIT_ADD,Name,512);
	ret = RegCreateKeyEx(hKey,Name,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hNewKey,&Disposition);
	if (ret==ERROR_SUCCESS&&Disposition==REG_CREATED_NEW_KEY)
	{
		EnumKey();
		MessageBoxA(NULL,"�����ɹ�",NULL,NULL);
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//ɾ���Ӽ�
//////////////////////////////////////////////////////////////////////////
BOOL DeletKey()
{
	//�Ӽ����ƻ���
	WCHAR Name [512] = {0};
	//��LIST�ؼ����������
	int Index = 0;
	//��ȡ��ǰLIST��ѡ�����������
	Index = SendDlgItemMessage(hDlg,IDC_LIST_KEY,LB_GETCURSEL,0,0);
	//��ȡ��ֵ����
	SendDlgItemMessage(hDlg,IDC_LIST_KEY,LB_GETTEXT,(WPARAM)Index,(LPARAM)Name);
	if(RegDeleteKey(hKey,Name)==ERROR_SUCCESS)
	{
		EnumKey();
		MessageBoxA(NULL,"ɾ���ɹ�",NULL,NULL);
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//ɾ����ֵ
//////////////////////////////////////////////////////////////////////////
BOOL DeletValue()
{
	//��ֵ���ƻ���
	WCHAR Name[512];
	//��ȡ��ֵ����
	GetDlgItemText(hDlg,IDC_EDIT_VALUENAME,Name,512);
	if(RegDeleteValue(hKey,Name)==ERROR_SUCCESS)
	{
		MessageBoxA(NULL,"ɾ���ɹ�",NULL,NULL);
		SetDlgItemText(hDlg,IDC_EDIT_VALUENAME,L"");
		SetDlgItemText(hDlg,IDC_EDIT_VALUEDATA,L"");
		SetDlgItemText(hDlg,IDC_COMBO_VLUTYPE,L"");
		EnumValue();
	}
	else
	{
		MessageBoxA(NULL,"ɾ��ʧ��",NULL,NULL);
		return FALSE;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//��ȡ��ֵ
//////////////////////////////////////////////////////////////////////////
BOOL ReadValue()
{
	//��ֵ��LIST�е�������
	int Index;
	//��ֵ������
	WCHAR Name[512];
	//���ݻ���
	BYTE Data[1024] = {0};
	//��������
	DWORD Type = REG_SZ;
	//�������Ĵ�С
	DWORD DataLen = 1024;
	//��ȡ������
	Index = SendDlgItemMessage(hDlg,IDC_LIST_VALUE,LB_GETCURSEL,0,0);
	//��ȡ��ֵ����
	SendDlgItemMessage(hDlg,IDC_LIST_VALUE,LB_GETTEXT,(WPARAM)Index,(LPARAM)Name);
	//��ȡ��ֵ
	RegQueryValueEx(hKey,Name,0,&Type,Data,&DataLen);
	SetDlgItemText(hDlg,IDC_EDIT_VALUENAME,Name);
	//�����������Ͷ����ݽ��д���
	switch (Type)
	{
	case REG_EXPAND_SZ:
		{
			//Ϊ��չ�ַ�������ʱֱ�������UI
			SetDlgItemText(hDlg,IDC_COMBO_VLUTYPE,L"REG_EXPAND_SZ");
			SetDlgItemText(hDlg,IDC_EDIT_VALUEDATA,(LPCWSTR)Data);
		}
		break;
	case REG_MULTI_SZ:
		{
			//Ϊ���ַ�������ʱֱ�������UI
			SetDlgItemText(hDlg,IDC_COMBO_VLUTYPE,L"REG_MULTI_SZ");
			SetDlgItemText(hDlg,IDC_EDIT_VALUEDATA,(LPCWSTR)Data);
		}
		break;
	case REG_SZ:
		{
			//Ϊ�ַ�������ʱֱ�������UI
			SetDlgItemText(hDlg,IDC_COMBO_VLUTYPE,L"REG_SZ");
			SetDlgItemText(hDlg,IDC_EDIT_VALUEDATA,(LPCWSTR)Data);
		}
		break;
	case REG_BINARY:
		{
			//Ϊ����������ʱ��������ת��Ϊʮ�����Ƶ��ַ����������UI
			//����
			WCHAR Buff[1024] = {0};
			//ѭ��ת��
			for (int i=0;i<DataLen;i++)
			{
				wsprintf(Buff+i*3,L"%x",*((BYTE*)Data+i));
				wsprintf(Buff+i*3+2,L"%c",' ');
			}
			SetDlgItemText(hDlg,IDC_COMBO_VLUTYPE,L"REG_BINARY");
			SetDlgItemText(hDlg,IDC_EDIT_VALUEDATA,Buff);	
		}
		break;
	case REG_DWORD:
		{
			//ΪDWORD����ʱ����ǿ��ת���������
			SetDlgItemText(hDlg,IDC_COMBO_VLUTYPE,L"REG_DWORD");
			SetDlgItemInt(hDlg,IDC_EDIT_VALUEDATA,*(long*)Data,false);
		}	
		break;
	default:
		break;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//�����ֵ
//////////////////////////////////////////////////////////////////////////
BOOL SaveValue()
{
	//��ֵ���ƻ�����
	WCHAR Name[512] = {0};
	//���ݻ���
	WCHAR Data[1024] = {0};
	//����
	WCHAR Type[32] = {0};
	//���ݻ����С
	DWORD DataLen = 1024;
	//��UI��ȡ��ֵ����
	GetDlgItemText(hDlg,IDC_EDIT_VALUENAME,Name,512);
	//��UI��ȡ��������
	GetDlgItemText(hDlg,IDC_COMBO_VLUTYPE,Type,32);
	//���ݲ�ͬ��������ѡ��ͬ����д��UI��ȡ���ݲ���ע���
	if (wcscmp(Type,L"REG_SZ")==0)
	{
		//��Ϊ�ַ������ͣ�ֱ��д��
		GetDlgItemText(hDlg,IDC_EDIT_VALUEDATA,Data,512);
		RegSetValueEx(hKey,Name,0,REG_SZ,(LPBYTE)Data,wcslen(Data)*2+1);
	}else if (wcscmp(Type,L"REG_EXPAND_SZ")==0)
	{
		//��Ϊ��չ�ַ������ͣ�ֱ��д��
		GetDlgItemText(hDlg,IDC_EDIT_VALUEDATA,Data,512);
		RegSetValueEx(hKey,Name,0,REG_EXPAND_SZ,(LPBYTE)Data,wcslen(Data)*2+1);
	}else if(wcscmp(Type,L"REG_DWORD")==0)
	{
		//��ΪDWORD���ͣ�ǿ��ת��ΪBYTE��д��
		DWORD dwNum;
		dwNum = GetDlgItemInt(hDlg,IDC_EDIT_VALUEDATA,NULL,FALSE);
		RegSetValueEx(hKey,Name,0,REG_DWORD,(LPBYTE)(&dwNum),4);
	}else if (wcscmp(Type,L"REG_BINARY")==0)
	{
		//��Ϊ�������������ͣ�����wStr16ToByte()ת��ΪBYTE
		//BYTE���ͻ�����
		BYTE Buff[1024] = {0};
		GetDlgItemText(hDlg,IDC_EDIT_VALUEDATA,Data,1024);
		wStr16ToByte(Data,Buff);
		RegSetValueEx(hKey,Name,0,REG_BINARY,(LPBYTE)Buff,strlen((char *)Buff));
	}else if (wcscmp(Type,L"REG_MULTI_SZ")==0)
	{
		//��Ϊ���ַ������ͣ�ֱ��д��
		GetDlgItemText(hDlg,IDC_EDIT_VALUEDATA,Data,512);
		RegSetValueEx(hKey,Name,0,REG_MULTI_SZ,(LPBYTE)Data,wcslen(Data)*2+1);
	}
	EnumValue();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//����ʮ�����ƿ��ַ�ת��Ϊһ��BYTEֵ
//////////////////////////////////////////////////////////////////////////
BYTE wChar16Toi(WCHAR *str)
{
	BYTE sum = 0;
	BYTE ret = 0;
	//ѭ������
	for (int i=0;i<2;i++)
	{
		ret = 0;
		//����ʮ�������ַ���ȡ��ʵ�ʴ�С
		if (str[i]-L'0'>=0&&str[i]-L'0'<=9)
		{
			ret = str[i] - L'0';
		}else if(str[i]-L'a'>=0&&str[i]-L'a'<=5)
		{
			ret = str[i] - L'a'+10;
		}else if(str[i]-L'A'>=0&&str[i]-L'A'<=5)
		{
			ret = str[i] - L'A'+10;
		}
		//�ۻ�sum
		sum = sum*16+ret;

	}
	return sum;
}

//////////////////////////////////////////////////////////////////////////
//ʮ�����ƿ��ַ���ת��ΪBYTE����
//////////////////////////////////////////////////////////////////////////
BOOL wStr16ToByte(WCHAR *Data,BYTE *bData)
{
	//ÿ�������ַ�������ʮ�������ַ���һ���ո�ѭ������һ��wChar16Toi()
	for (unsigned int i=0;i<wcslen(Data)/3;i++)
	{
		bData[i] = wChar16Toi(Data+i*3);
	}
	return TRUE;
}