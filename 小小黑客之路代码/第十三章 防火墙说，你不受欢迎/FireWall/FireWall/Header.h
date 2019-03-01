#include "resource.h"


#include <WS2spi.h>
#include <SpOrder.h>
#include <vector>
#include <cstring>
#include <shlobj.h>
using namespace std;

#include <CommCtrl.h>
#include <tchar.h>

#pragma comment(lib,"FireWallDll.lib")
#pragma comment(lib,"comctl32.lib")
#pragma comment(lib, "Ws2_32.lib")




typedef struct SockListItem
{
	SOCKET s;
	IN_ADDR RemoteIp;
	USHORT Port;
	WCHAR sPath[MAX_PATH];

}SOCKLISTITEM;

typedef struct IpListItem 
{
	IN_ADDR Ip;
	BOOL Pass;
}IPLISTITEM;

typedef struct PortListItem
{
	USHORT Port;
	BOOL Pass;
}PORTLISTITEM;

typedef struct PathListItem
{
	WCHAR Path[MAX_PATH];
	BOOL Pass;
}PATHLISTITEM;


extern HWND hDlg;
extern HWND hPortDlg;
extern HWND hIpDlg;
extern HWND hPathDlg;


void RemoveProviders();

void InstallProviders();

BOOL AddPort();

BOOL ShowPortList();

BOOL DeletPort();

BOOL AddIp();

BOOL ShowIpList();

BOOL DeletIp();

BOOL AddPath();

BOOL ShowPathList();

BOOL DeletPath();

extern "C" __declspec(dllimport) BOOL InitRWSCriticalSection();

extern "C" __declspec(dllimport) BOOL SetUDPPass(BOOL bPass);

extern "C" __declspec(dllimport) BOOL SetTCPPass(BOOL bPass);

extern "C" __declspec(dllimport) BOOL GetUDPStation();

extern "C" __declspec(dllimport) BOOL GetTCPStation();

extern "C" __declspec(dllimport) BOOL DeletPathListItem(int i);

extern "C" __declspec(dllimport) BOOL AddPathListItem(PATHLISTITEM Item);

extern "C" __declspec(dllimport) BOOL GetPathListItem(int i,PATHLISTITEM *Item);

extern "C" __declspec(dllimport) BOOL DeletIpListItem(int i);

extern "C" __declspec(dllimport) BOOL AddIpListItem(IPLISTITEM Item);

extern "C" __declspec(dllimport) BOOL GetIpListItem(int i,IPLISTITEM *Item);

extern "C" __declspec(dllimport) BOOL DeletPortListItem(int i);

extern "C" __declspec(dllimport) BOOL AddPortListItem(PORTLISTITEM Item);

extern "C" __declspec(dllimport) BOOL GetPortListItem(int i,PORTLISTITEM *Item);