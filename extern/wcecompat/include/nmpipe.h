#ifndef _NMPIPE_H_
#define _NMPIPE_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

// Define the NamedPipe definitions
#define FILE_FLAG_FIRST_PIPE_INSTANCE   0x00080000

#define NMPWAIT_WAIT_FOREVER            0xffffffff
#define NMPWAIT_NOWAIT                  0x00000001
#define NMPWAIT_USE_DEFAULT_WAIT        0x00000000
//
// Define the dwOpenMode values for CreateNamedPipe
//

#define PIPE_ACCESS_INBOUND         0x00000001
#define PIPE_ACCESS_OUTBOUND        0x00000002
#define PIPE_ACCESS_DUPLEX          0x00000003

//
// Define the Named Pipe End flags for GetNamedPipeInfo
//

#define PIPE_CLIENT_END             0x00000000
#define PIPE_SERVER_END             0x00000001

//
// Define the dwPipeMode values for CreateNamedPipe
//

#define PIPE_WAIT                   0x00000000
#define PIPE_NOWAIT                 0x00000001
#define PIPE_READMODE_BYTE          0x00000000
#define PIPE_READMODE_MESSAGE       0x00000002
#define PIPE_TYPE_BYTE              0x00000000
#define PIPE_TYPE_MESSAGE           0x00000004

//Special flags for Windows CE
#define PIPE_TYPE_SERVER			0x00000010
#define PIPE_TYPE_CLIENT			0x00000020

//
// Define the well known values for CreateNamedPipe nMaxInstances
//

#define PIPE_UNLIMITED_INSTANCES    255

BOOL
WINAPI
CreatePipe(
    OUT PHANDLE phReadPipe,
    OUT PHANDLE phWritePipe,
    IN LPSECURITY_ATTRIBUTES lpPipeAttributes,
    IN DWORD nSize
    );


BOOL
WINAPI
ConnectNamedPipe(
    IN HANDLE hNamedPipe,
    IN LPOVERLAPPED lpOverlapped
    );


BOOL
WINAPI
DisconnectNamedPipe(
    IN HANDLE hNamedPipe
    );


BOOL
WINAPI
SetNamedPipeHandleState(
    IN HANDLE hNamedPipe,
    IN LPDWORD lpMode,
    IN LPDWORD lpMaxCollectionCount,
    IN LPDWORD lpCollectDataTimeout
    );


BOOL
WINAPI
GetNamedPipeInfo(
    IN HANDLE hNamedPipe,
    IN LPDWORD lpFlags,
    OUT LPDWORD lpOutBufferSize,
    OUT LPDWORD lpInBufferSize,
    OUT LPDWORD lpMaxInstances
    );


BOOL
WINAPI
PeekNamedPipe(
    IN HANDLE hNamedPipe,
    OUT LPVOID lpBuffer,
    IN DWORD nBufferSize,
    OUT LPDWORD lpBytesRead,
    OUT LPDWORD lpTotalBytesAvail,
    OUT LPDWORD lpBytesLeftThisMessage
    );


BOOL
WINAPI
TransactNamedPipe(
    IN HANDLE hNamedPipe,
    IN LPVOID lpInBuffer,
    IN DWORD nInBufferSize,
    OUT LPVOID lpOutBuffer,
    IN DWORD nOutBufferSize,
    OUT LPDWORD lpBytesRead,
    IN LPOVERLAPPED lpOverlapped
    );


HANDLE
WINAPI
CreateNamedPipeW(
    IN LPCWSTR lpName,
    IN DWORD dwOpenMode,
    IN DWORD dwPipeMode,
    IN DWORD nMaxInstances,
    IN DWORD nOutBufferSize,
    IN DWORD nInBufferSize,
    IN DWORD nDefaultTimeOut,
    IN LPSECURITY_ATTRIBUTES lpSecurityAttributes
    );
#ifdef UNICODE
#define CreateNamedPipe  CreateNamedPipeW
#else
#define CreateNamedPipe  CreateNamedPipeA
#endif // !UNICODE


BOOL
WINAPI
GetNamedPipeHandleStateW(
    IN HANDLE hNamedPipe,
    OUT LPDWORD lpState,
    OUT LPDWORD lpCurInstances,
    OUT LPDWORD lpMaxCollectionCount,
    OUT LPDWORD lpCollectDataTimeout,
    OUT LPWSTR lpUserName,
    IN DWORD nMaxUserNameSize
    );
#ifdef UNICODE
#define GetNamedPipeHandleState  GetNamedPipeHandleStateW
#else
#define GetNamedPipeHandleState  GetNamedPipeHandleStateA
#endif // !UNICODE


BOOL
WINAPI
CallNamedPipeW(
    IN LPCWSTR lpPipeName,
    IN LPVOID lpInBuffer,
    IN DWORD nInBufferSize,
    OUT LPVOID lpOutBuffer,
    IN DWORD nOutBufferSize,
    OUT LPDWORD lpBytesRead,
    IN DWORD nTimeOut
    );
#ifdef UNICODE
#define CallNamedPipe  CallNamedPipeW
#else
#define CallNamedPipe  CallNamedPipeA
#endif // !UNICODE


BOOL
WINAPI
WaitNamedPipeW(
    IN LPCWSTR lpNamedPipeName,
    IN DWORD nTimeOut
    );
#ifdef UNICODE
#define WaitNamedPipe  WaitNamedPipeW
#else
#define WaitNamedPipe  WaitNamedPipeA
#endif // !UNICODE



//Since named pipes arent tied in to the file apis yet, these functions will be used instead of the ReadFile/WriteFile/CloseHandle apis
//The signature of these functions is the same as the corresponding file apis

BOOL
WINAPI
ReadNamedPipe(
  IN HANDLE hNamedPipe,               
  IN LPVOID lpBuffer,            
  IN DWORD nNumberOfBytesToRead, 
  OUT LPDWORD lpNumberOfBytesRead,
  OUT LPOVERLAPPED lpOverlapped 
);


BOOL
WINAPI
WriteNamedPipe(
  IN HANDLE hNamedPipe,                    
  IN LPCVOID lpBuffer,                
  IN DWORD nNumberOfBytesToWrite,     
  OUT LPDWORD lpNumberOfBytesWritten,  
  OUT LPOVERLAPPED lpOverlapped        
);


BOOL
WINAPI
CloseNamedPipe(
  IN HANDLE hPipe
);

BOOL
WINAPI
QuerySessionKeyOfBinding(
  IN LPWSTR  lpPipeName,
  OUT LPVOID lpSessionKey,
  IN DWORD   nSessionKeySize
);

#ifdef __cplusplus
}
#endif

#endif
