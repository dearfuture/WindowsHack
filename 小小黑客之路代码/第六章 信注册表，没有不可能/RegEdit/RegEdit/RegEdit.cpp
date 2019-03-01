

#include "Header.h"
#include "resource.h"



HKEY hKey ;
HKEY hBaseKey;

//////////////////////////////////////////////////////////////////////////
//枚举子键
//////////////////////////////////////////////////////////////////////////
BOOL EnumKey()
{
	int Index = 0;
	DWORD num = 512;
	//缓冲
	WCHAR Buff[512] = {0};
	SendMessage(GetDlgItem(hDlg,IDC_LIST_KEY),LB_RESETCONTENT,NULL,NULL);
	//循环枚举子键直到RegEnumKeyEx()返回ERROR_NO_MORE_ITEMS
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
//枚举键值
//////////////////////////////////////////////////////////////////////////
BOOL EnumValue()
{
	int Index = 0;
	DWORD num = 512;
	//缓冲
	WCHAR Buff[512] = {0};
	SendMessage(GetDlgItem(hDlg,IDC_LIST_VALUE),LB_RESETCONTENT,NULL,NULL);
	//循环枚举子键直到RegEnumValue()返回ERROR_NO_MORE_ITEMS
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
//打开子键
//////////////////////////////////////////////////////////////////////////
BOOL OpenKey()
{
	//缓冲
	WCHAR Buff[1024] = {0};
	//根键名称缓冲
	WCHAR BaseKey[32];
	//从UI获得根键名称
	GetDlgItemText(hDlg,IDC_COMBO_RKEY,BaseKey,32);
	//根据根键名称为hBaseKey（全局HKEY变量）赋值
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
	//获取键路径
	GetDlgItemText(hDlg,IDC_EDIT_PATH,Buff,1024);
	//打开该键
	if (RegOpenKeyEx(hBaseKey,Buff,NULL,KEY_ALL_ACCESS,&hKey))
	{
		//打开失败则对IDC_EDIT_PATH做处理（删除最后一级路径）
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
		MessageBoxA(NULL,"打开子键失败",NULL,NULL);
		return FALSE;
	}
	//枚举子键及键值
	EnumKey();
	EnumValue();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//添加子键
//////////////////////////////////////////////////////////////////////////
BOOL AddKey()
{
	//子键名称缓冲
	WCHAR Name [512];
	//输出结果
	DWORD Disposition;
	DWORD ret;
	//新子键句柄
	HKEY hNewKey;
	GetDlgItemText(hDlg,IDC_EDIT_ADD,Name,512);
	ret = RegCreateKeyEx(hKey,Name,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hNewKey,&Disposition);
	if (ret==ERROR_SUCCESS&&Disposition==REG_CREATED_NEW_KEY)
	{
		EnumKey();
		MessageBoxA(NULL,"创建成功",NULL,NULL);
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//删除子键
//////////////////////////////////////////////////////////////////////////
BOOL DeletKey()
{
	//子键名称缓冲
	WCHAR Name [512] = {0};
	//在LIST控件里的索引号
	int Index = 0;
	//获取当前LIST中选中项的索引号
	Index = SendDlgItemMessage(hDlg,IDC_LIST_KEY,LB_GETCURSEL,0,0);
	//获取键值名称
	SendDlgItemMessage(hDlg,IDC_LIST_KEY,LB_GETTEXT,(WPARAM)Index,(LPARAM)Name);
	if(RegDeleteKey(hKey,Name)==ERROR_SUCCESS)
	{
		EnumKey();
		MessageBoxA(NULL,"删除成功",NULL,NULL);
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//删除键值
//////////////////////////////////////////////////////////////////////////
BOOL DeletValue()
{
	//键值名称缓冲
	WCHAR Name[512];
	//获取键值名称
	GetDlgItemText(hDlg,IDC_EDIT_VALUENAME,Name,512);
	if(RegDeleteValue(hKey,Name)==ERROR_SUCCESS)
	{
		MessageBoxA(NULL,"删除成功",NULL,NULL);
		SetDlgItemText(hDlg,IDC_EDIT_VALUENAME,L"");
		SetDlgItemText(hDlg,IDC_EDIT_VALUEDATA,L"");
		SetDlgItemText(hDlg,IDC_COMBO_VLUTYPE,L"");
		EnumValue();
	}
	else
	{
		MessageBoxA(NULL,"删除失败",NULL,NULL);
		return FALSE;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//读取键值
//////////////////////////////////////////////////////////////////////////
BOOL ReadValue()
{
	//键值在LIST中的索引号
	int Index;
	//键值名缓冲
	WCHAR Name[512];
	//数据缓冲
	BYTE Data[1024] = {0};
	//数据类型
	DWORD Type = REG_SZ;
	//缓冲区的大小
	DWORD DataLen = 1024;
	//获取索引号
	Index = SendDlgItemMessage(hDlg,IDC_LIST_VALUE,LB_GETCURSEL,0,0);
	//获取键值名称
	SendDlgItemMessage(hDlg,IDC_LIST_VALUE,LB_GETTEXT,(WPARAM)Index,(LPARAM)Name);
	//读取键值
	RegQueryValueEx(hKey,Name,0,&Type,Data,&DataLen);
	SetDlgItemText(hDlg,IDC_EDIT_VALUENAME,Name);
	//根据数据类型对数据进行处理
	switch (Type)
	{
	case REG_EXPAND_SZ:
		{
			//为扩展字符串类型时直接输出到UI
			SetDlgItemText(hDlg,IDC_COMBO_VLUTYPE,L"REG_EXPAND_SZ");
			SetDlgItemText(hDlg,IDC_EDIT_VALUEDATA,(LPCWSTR)Data);
		}
		break;
	case REG_MULTI_SZ:
		{
			//为多字符串类型时直接输出到UI
			SetDlgItemText(hDlg,IDC_COMBO_VLUTYPE,L"REG_MULTI_SZ");
			SetDlgItemText(hDlg,IDC_EDIT_VALUEDATA,(LPCWSTR)Data);
		}
		break;
	case REG_SZ:
		{
			//为字符串类型时直接输出到UI
			SetDlgItemText(hDlg,IDC_COMBO_VLUTYPE,L"REG_SZ");
			SetDlgItemText(hDlg,IDC_EDIT_VALUEDATA,(LPCWSTR)Data);
		}
		break;
	case REG_BINARY:
		{
			//为二进制数据时，将数据转换为十六进制的字符串再输出到UI
			//缓冲
			WCHAR Buff[1024] = {0};
			//循环转换
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
			//为DWORD类型时将其强制转换后在输出
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
//保存键值
//////////////////////////////////////////////////////////////////////////
BOOL SaveValue()
{
	//键值名称缓冲区
	WCHAR Name[512] = {0};
	//数据缓冲
	WCHAR Data[1024] = {0};
	//类型
	WCHAR Type[32] = {0};
	//数据缓冲大小
	DWORD DataLen = 1024;
	//从UI获取键值名称
	GetDlgItemText(hDlg,IDC_EDIT_VALUENAME,Name,512);
	//从UI获取数据类型
	GetDlgItemText(hDlg,IDC_COMBO_VLUTYPE,Type,32);
	//根据不同数据类型选择不同方法写在UI获取数据并入注册表
	if (wcscmp(Type,L"REG_SZ")==0)
	{
		//若为字符串类型，直接写入
		GetDlgItemText(hDlg,IDC_EDIT_VALUEDATA,Data,512);
		RegSetValueEx(hKey,Name,0,REG_SZ,(LPBYTE)Data,wcslen(Data)*2+1);
	}else if (wcscmp(Type,L"REG_EXPAND_SZ")==0)
	{
		//若为扩展字符串类型，直接写入
		GetDlgItemText(hDlg,IDC_EDIT_VALUEDATA,Data,512);
		RegSetValueEx(hKey,Name,0,REG_EXPAND_SZ,(LPBYTE)Data,wcslen(Data)*2+1);
	}else if(wcscmp(Type,L"REG_DWORD")==0)
	{
		//若为DWORD类型，强制转换为BYTE后写入
		DWORD dwNum;
		dwNum = GetDlgItemInt(hDlg,IDC_EDIT_VALUEDATA,NULL,FALSE);
		RegSetValueEx(hKey,Name,0,REG_DWORD,(LPBYTE)(&dwNum),4);
	}else if (wcscmp(Type,L"REG_BINARY")==0)
	{
		//若为二进制数据类型，调用wStr16ToByte()转换为BYTE
		//BYTE类型缓冲区
		BYTE Buff[1024] = {0};
		GetDlgItemText(hDlg,IDC_EDIT_VALUEDATA,Data,1024);
		wStr16ToByte(Data,Buff);
		RegSetValueEx(hKey,Name,0,REG_BINARY,(LPBYTE)Buff,strlen((char *)Buff));
	}else if (wcscmp(Type,L"REG_MULTI_SZ")==0)
	{
		//若为多字符串类型，直接写入
		GetDlgItemText(hDlg,IDC_EDIT_VALUEDATA,Data,512);
		RegSetValueEx(hKey,Name,0,REG_MULTI_SZ,(LPBYTE)Data,wcslen(Data)*2+1);
	}
	EnumValue();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//两个十六进制宽字符转换为一个BYTE值
//////////////////////////////////////////////////////////////////////////
BYTE wChar16Toi(WCHAR *str)
{
	BYTE sum = 0;
	BYTE ret = 0;
	//循环两次
	for (int i=0;i<2;i++)
	{
		ret = 0;
		//根据十六进制字符获取其实际大小
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
		//累积sum
		sum = sum*16+ret;

	}
	return sum;
}

//////////////////////////////////////////////////////////////////////////
//十六进制宽字符串转换为BYTE数组
//////////////////////////////////////////////////////////////////////////
BOOL wStr16ToByte(WCHAR *Data,BYTE *bData)
{
	//每三个宽字符（两个十六进制字符，一个空格）循环调用一次wChar16Toi()
	for (unsigned int i=0;i<wcslen(Data)/3;i++)
	{
		bData[i] = wChar16Toi(Data+i*3);
	}
	return TRUE;
}