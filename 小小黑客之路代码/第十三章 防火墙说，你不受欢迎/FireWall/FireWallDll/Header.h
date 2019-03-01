#define  UNICODE
#define  _UNICODE

#include <ws2spi.h>
#include <tchar.h>
#include <vector>
using namespace std;

#pragma comment(lib,"WS2_32.lib")







//typedef struct SockListItem
//{
//	SOCKET s;
//	IN_ADDR RemoteIp;
//	USHORT Port;
//	WCHAR sPath[MAX_PATH];
//		
//}SOCKLISTITEM;

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



extern IPLISTITEM IpList[1024];
extern int IpListSize;
extern PORTLISTITEM PortList[1024];
extern int PortListSize;
extern PATHLISTITEM PathList[1024];
extern int PathListSize ;

extern BOOL TCPOpen;
extern BOOL UDPOpen;
extern CRITICAL_SECTION RWSCriticalSection;










extern "C" __declspec(dllexport) BOOL InitRWSCriticalSection();

extern "C" __declspec(dllexport) BOOL SetUDPPass(BOOL bPass);

extern "C" __declspec(dllexport) BOOL SetTCPPass(BOOL bPass);

extern "C" __declspec(dllexport) BOOL GetUDPStation();

extern "C" __declspec(dllexport) BOOL GetTCPStation();

extern "C" __declspec(dllexport) BOOL DeletPathListItem(int i);

extern "C" __declspec(dllexport) BOOL AddPathListItem(PATHLISTITEM Item);

extern "C" __declspec(dllexport) BOOL GetPathListItem(int i,PATHLISTITEM *Item);

extern "C" __declspec(dllexport) BOOL DeletIpListItem(int i);

extern "C" __declspec(dllexport) BOOL AddIpListItem(IPLISTITEM Item);

extern "C" __declspec(dllexport) BOOL GetIpListItem(int i,IPLISTITEM *Item);

extern "C" __declspec(dllexport) BOOL DeletPortListItem(int i);

extern "C" __declspec(dllexport) BOOL AddPortListItem(PORTLISTITEM Item);

extern "C" __declspec(dllexport) BOOL GetPortListItem(int i,PORTLISTITEM *Item);


SOCKET WSPAPI FWWSPSocket(
						  int			af,                               
						  int			type,                             
						  int			protocol,                         
						  LPWSAPROTOCOL_INFOW lpProtocolInfo,   
						  GROUP		g,                              
						  DWORD		dwFlags,                        
						  LPINT		lpErrno
						  );




int WINAPI FWWSPBind(
					 __in          SOCKET s,
					 __in          const struct sockaddr* name,
					 __in          int namelen,
					 __out         LPINT lpErrno
					 );



int WINAPI FWWSPListen(
					   __in          SOCKET s,
					   __in          int backlog,
					   __out         LPINT lpErrno
					   );




int WINAPI FWWSPConnect(
						__in          SOCKET s,
						__in          const struct sockaddr* name,
						__in          int namelen,
						__in          LPWSABUF lpCallerData,
						__out         LPWSABUF lpCalleeData,
						__in          LPQOS lpSQOS,
						__in          LPQOS lpGQOS,
						__out         LPINT lpErrno
						);


int WINAPI FWWSPRecvFrom(
 SOCKET s,
 LPWSABUF lpBuffers,
 DWORD dwBufferCount,
 LPDWORD lpNumberOfBytesRecvd,
 LPDWORD lpFlags,
struct sockaddr* lpFrom,
LPINT lpFromlen,
LPWSAOVERLAPPED lpOverlapped,
LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine,
LPWSATHREADID lpThreadId,
LPINT lpErrno
);




int WINAPI FWWSPSendTo(
					   __in          SOCKET s,
					   __in          LPWSABUF lpBuffers,
					   __in          DWORD dwBufferCount,
					   __out         LPDWORD lpNumberOfBytesSent,
					   __in          DWORD dwFlags,
					   __in          const struct sockaddr* lpTo,
					   __in          int iTolen,
					   __in          LPWSAOVERLAPPED lpOverlapped,
					   __in          LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine,
					   __in          LPWSATHREADID lpThreadId,
					   __out         LPINT lpErrno
					   );



BOOL CheckPort(USHORT Port);

BOOL CheckAppPath(WCHAR * Path);

BOOL CheckIp(IN_ADDR Ip);