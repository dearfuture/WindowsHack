// CryptoApi_Rsa_Test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
#include <WinError.h>


void ShowError(char *pszText)
{
	char szErr[MAX_PATH] = { 0 };
	::wsprintf(szErr, "%s Error[0x%x]\n", pszText, ::GetLastError());
#ifdef _DEBUG
	::MessageBox(NULL, szErr, "ERROR", MB_OK | MB_ICONERROR);
#endif
}


// 保存数据到本地文件
BOOL SaveDataToFile(char *pszFileName, PVOID pData, DWORD dwDataLength)
{
	BOOL bRet = FALSE;
	HANDLE hFile = NULL;
	DWORD dwFileSize = 0;
	DWORD dwRet = 0;

	do
	{
		hFile = ::CreateFile(pszFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (INVALID_HANDLE_VALUE == hFile)
		{
			break;
		}

		::WriteFile(hFile, pData, dwDataLength, &dwRet, NULL);
		if (dwRet != dwDataLength)
		{
			break;
		}

		bRet = TRUE;

	} while (FALSE);

	if (NULL != hFile)
	{
		::CloseHandle(hFile);
		hFile = NULL;
	}

	return bRet;
}


// 读取文件数据
BOOL ReadDataFromFile(char *pszFileName, BYTE **ppData, DWORD *pdwDataLength)
{
	BOOL bRet = FALSE;
	HANDLE hFile = NULL;
	DWORD dwFileSize = 0;
	DWORD dwRet = 0;
	BYTE *pData = NULL;
	DWORD dwDataLength = 0;

	do
	{
		hFile = ::CreateFile(pszFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (INVALID_HANDLE_VALUE == hFile)
		{
			break;
		}

		dwDataLength = ::GetFileSize(hFile, NULL);

		pData = new BYTE[dwDataLength];
		if (NULL == pData)
		{
			break;
		}
		::RtlZeroMemory(pData, dwDataLength);

		::ReadFile(hFile, pData, dwDataLength, &dwRet, NULL);
		if (dwRet != dwDataLength)
		{
			break;
		}

		// 返回数据
		*ppData = pData;
		*pdwDataLength = dwDataLength;

		bRet = TRUE;

	} while (FALSE);

	if (FALSE == bRet)
	{
		if (NULL != pData)
		{
			delete []pData;
			pData = NULL;
		}
	}

	if (NULL != hFile)
	{
		::CloseHandle(hFile);
		hFile = NULL;
	}

	return bRet;
}



// 生成公钥和私钥
BOOL GenerateKey(BYTE **ppPublicKey, DWORD *pdwPublicKeyLength, BYTE **ppPrivateKey, DWORD *pdwPrivateKeyLength)
{
	BOOL bRet = FALSE;
	HCRYPTPROV hCryptProv = NULL;
	HCRYPTKEY hCryptKey = NULL;
	BYTE *pPublicKey = NULL;
	DWORD dwPublicKeyLength = 0;
	BYTE *pPrivateKey = NULL;
	DWORD dwPrivateKeyLength = 0;

	do
	{
		// 获取CSP句柄
		bRet = ::CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, 0);
		if (NTE_BAD_KEYSET == ::GetLastError())
		{
			bRet = ::CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET);
			if (FALSE == bRet)
			{
				ShowError("CryptAcquireContext");
				break;
			}
		}

		// 生成公私密钥对
		bRet = ::CryptGenKey(hCryptProv, AT_KEYEXCHANGE, CRYPT_EXPORTABLE, &hCryptKey);
		if (FALSE == bRet)
		{
			ShowError("CryptGenKey");
			break;
		}

		// 获取公钥密钥的长度和内容
		bRet = ::CryptExportKey(hCryptKey, NULL, PUBLICKEYBLOB, 0, NULL, &dwPublicKeyLength);
		if (FALSE == bRet)
		{
			ShowError("CryptExportKey");
			break;
		}
		pPublicKey = new BYTE[dwPublicKeyLength];
		::RtlZeroMemory(pPublicKey, dwPublicKeyLength);
		bRet = ::CryptExportKey(hCryptKey, NULL, PUBLICKEYBLOB, 0, pPublicKey, &dwPublicKeyLength);
		if (FALSE == bRet)
		{
			ShowError("CryptExportKey");
			break;
		}

		// 获取私钥密钥的长度和内容
		bRet = ::CryptExportKey(hCryptKey, NULL, PRIVATEKEYBLOB, 0, NULL, &dwPrivateKeyLength);
		if (FALSE == bRet)
		{
			ShowError("CryptExportKey");
			break;
		}
		pPrivateKey = new BYTE[dwPrivateKeyLength];
		::RtlZeroMemory(pPrivateKey, dwPrivateKeyLength);
		bRet = ::CryptExportKey(hCryptKey, NULL, PRIVATEKEYBLOB, 0, pPrivateKey, &dwPrivateKeyLength);
		if (FALSE == bRet)
		{
			ShowError("CryptExportKey");
			break;
		}

		// 返回数据
		*ppPublicKey = pPublicKey;
		*pdwPublicKeyLength = dwPublicKeyLength;
		*ppPrivateKey = pPrivateKey;
		*pdwPrivateKeyLength = dwPrivateKeyLength;

	} while (FALSE);

	// 释放关闭
	if (hCryptKey)
	{
		::CryptDestroyKey(hCryptKey);
	}
	if (hCryptProv)
	{
		::CryptReleaseContext(hCryptProv, 0);
	}

	return bRet;
}


// 公钥加密数据
BOOL RsaEncrypt(BYTE *pPublicKey, DWORD dwPublicKeyLength, BYTE *pData, DWORD &dwDataLength, DWORD dwBufferLength)
{
	BOOL bRet = FALSE;
	HCRYPTPROV hCryptProv = NULL;
	HCRYPTKEY hCryptKey = NULL;

	do
	{
		// 获取CSP句柄
		bRet = ::CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, 0);
		if (NTE_BAD_KEYSET == ::GetLastError())
		{
			bRet = ::CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET);
			if (FALSE == bRet)
			{
				ShowError("CryptAcquireContext");
				break;
			}
		}

		// 导入公钥
		bRet = ::CryptImportKey(hCryptProv, pPublicKey, dwPublicKeyLength, NULL, 0, &hCryptKey);
		if (FALSE == bRet)
		{
			ShowError("CryptImportKey");
			break;
		}

		// 加密数据
		bRet = ::CryptEncrypt(hCryptKey, NULL, TRUE, 0, pData, &dwDataLength, dwBufferLength);
		if (FALSE == bRet)
		{
			ShowError("CryptImportKey");
			break;
		}

	} while (FALSE);

	// 释放并关闭
	if (hCryptKey)
	{
		::CryptDestroyKey(hCryptKey);
	}
	if (hCryptProv)
	{
		::CryptReleaseContext(hCryptProv, 0);
	}

	return bRet;
}


// 私钥解密数据
BOOL RsaDecrypt(BYTE *pPrivateKey, DWORD dwProvateKeyLength, BYTE *pData, DWORD &dwDataLength)
{
	BOOL bRet = FALSE;
	HCRYPTPROV hCryptProv = NULL;
	HCRYPTKEY hCryptKey = NULL;

	do
	{
		// 获取CSP句柄
		bRet = ::CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, 0);
		if (NTE_BAD_KEYSET == ::GetLastError())
		{
			bRet = ::CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET);
			if (FALSE == bRet)
			{
				ShowError("CryptAcquireContext");
				break;
			}
		}

		// 导入私钥
		bRet = ::CryptImportKey(hCryptProv, pPrivateKey, dwProvateKeyLength, NULL, 0, &hCryptKey);
		if (FALSE == bRet)
		{
			ShowError("CryptImportKey");
			break;
		}

		// 解密数据
		bRet = ::CryptDecrypt(hCryptKey, NULL, TRUE, 0, pData, &dwDataLength);
		if (FALSE == bRet)
		{
			ShowError("CryptDecrypt");
			break;
		}

	} while (FALSE);

	// 释放并关闭
	if (hCryptKey)
	{
		::CryptDestroyKey(hCryptKey);
	}
	if (hCryptProv)
	{
		::CryptReleaseContext(hCryptProv, 0);
	}

	return bRet;
}



int _tmain(int argc, _TCHAR* argv[])
{
	int n = 0;
	BOOL bRet = FALSE;
	BYTE *pPublicKey = NULL;
	DWORD dwPublicKeyLength = 0;
	BYTE *pPrivateKey = NULL;
	DWORD dwPrivateKeyLength = 0;
	BYTE *pData = NULL;
	DWORD dwTextLength = 0;
	DWORD dwDataLength = 4096;
	DWORD dwBufferLength = 4096;
	DWORD i = 0;

	char szFileName[MAX_PATH] = { 0 };
	pData = new BYTE[dwDataLength];

	::RtlZeroMemory(pData, dwDataLength);
	::RtlZeroMemory(szFileName, MAX_PATH);


	do
	{
		printf("----------------- Win32 Crypto Api Test -----------------\n");
		printf("1. 生成公私密钥对, 并保存到本地文件\n");
		printf("2. 公钥加密数据\n");
		printf("3. 私钥解密数据\n");
		printf("---------------------------------------------------------\n");

		scanf("%d", &n);
		getchar();

		switch (n)
		{
		case 1:
		{
			// 生成公钥和私钥
			bRet = GenerateKey(&pPublicKey, &dwPublicKeyLength, &pPrivateKey, &dwPrivateKeyLength);
			if (FALSE == bRet)
			{
				exit(0);
				break;
			}
			SaveDataToFile("public.enc", pPublicKey, dwPublicKeyLength);
			SaveDataToFile("private.enc", pPrivateKey, dwPrivateKeyLength);

			break;
		}
		case 2:
		{
			::RtlZeroMemory(pData, dwDataLength);
			::RtlZeroMemory(szFileName, MAX_PATH);

			printf("输入要加密的字符串:\n");
			gets((char *)pData);

			printf("输入公钥文件路径:\n");
			gets(szFileName);
			bRet = ReadDataFromFile(szFileName, &pPublicKey, &dwPublicKeyLength);
			if (FALSE == bRet)
			{
				exit(0);
				break;
			}

			// 加密
			dwTextLength = 1 + ::lstrlen((char *)pData);
			bRet = RsaEncrypt(pPublicKey, dwPublicKeyLength, pData, dwTextLength, dwBufferLength);
			if (FALSE == bRet)
			{
				exit(0);
				break;
			}

			// 保存密文到文件
			SaveDataToFile("encdata.enc", pData, dwTextLength);

			break;
		}
		case 3:
		{
			::RtlZeroMemory(pData, dwDataLength);
			::RtlZeroMemory(szFileName, MAX_PATH);

			printf("输入密文路径:\n");
			gets(szFileName);
			bRet = ReadDataFromFile(szFileName, &pData, &dwTextLength);
			if (FALSE == bRet)
			{
				exit(0);
				break;
			}

			printf("输入私钥文件路径:\n");
			gets(szFileName);
			bRet = ReadDataFromFile(szFileName, &pPrivateKey, &dwPrivateKeyLength);
			if (FALSE == bRet)
			{
				exit(0);
				break;
			}

			bRet = RsaDecrypt(pPrivateKey, dwPrivateKeyLength, pData, dwTextLength);
			if (FALSE == bRet)
			{
				exit(0);
				break;
			}

			// 显示
			printf("解密结果为:\n");
			printf("%s", (char *)pData);
			printf("\n\n\n\n");

			break;
		}
		default:
		{
			printf("input error!\n");
			break;
		}
		}
		
	} while (0 != n);

	// 释放
	if (pData)
	{
		delete[]pData;
		pData = NULL;
	}
	if (pPrivateKey)
	{
		delete[]pPrivateKey;
		pPrivateKey = NULL;
	}
	if (pPublicKey)
	{
		delete[]pPublicKey;
		pPublicKey = NULL;
	}

	system("pause");
	return 0;
}

