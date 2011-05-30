////////////////////////////////////////////////////////////////////////////////////
// 
//  How to Add this console to your App
//
//  1. Add this file to your project
//  2. Define int InitConsole(); in your.cpp
//  3. Call InitConsole() from main() or other func
//  4. Run your.cpp and try telnet you_app_ip SHELL_PORT
//  5. Use u/p: Admin:qwerty

////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <winbase.h>

#define  WIN32_LEAN_AND_MEAN

#include  <stdio.h>
#include  <windows.h>
#include  <winsock2.h> 

#define IAC 255
#define WILL 251
#define WONT 252
#define DO 253
#define DONT 254

extern _TCHAR *exeToStart;
extern _TCHAR *logToStart;

int  InitConsole();
DWORD WINAPI Session(LPVOID sr);
DWORD WINAPI Service(LPVOID lpParameter);
DWORD WINAPI EndSess(LPVOID sr);


#define SHELL_PORT  1663
#define SHELL_NAME  L"cmd\0"

typedef BOOL (WINAPI *SetStdioPathWFunc)(DWORD id,PWSTR pwszPath);
SetStdioPathWFunc SetStdioPathW;

DWORD WINAPI EndSess(LPVOID sr)
{    
    SOCKET sendrecv = *((SOCKET*)sr);
    Sleep( 7500 ); 
    closesocket(sendrecv);
    return 0 ;
}

DWORD WINAPI Session(LPVOID sr)
{    
	int err;
    SOCKET sendrecv = *((SOCKET*)sr);

//    if (login(sendrecv))
    { 
        STARTUPINFO si;
        SECURITY_ATTRIBUTES sa;  
        PROCESS_INFORMATION pi;


        memset(( void *) &si, 0, sizeof (si));
        memset(( void *) &pi, 0, sizeof (pi));

		sa.nLength              = sizeof (SECURITY_ATTRIBUTES);
        sa.bInheritHandle       = TRUE;
        sa.lpSecurityDescriptor = NULL;

        si.cb = sizeof (si);
//        si.dwFlags = STARTF_USESTDHANDLES + STARTF_USESHOWWINDOW;

        si.wShowWindow = SW_HIDE;
        si.hStdInput   = ( void  *)sendrecv;
        si.hStdOutput  = ( void  *)sendrecv;
        si.hStdError   = ( void  *)sendrecv;

	 	LPTSTR szCmdline = _tcsdup(TEXT("C:\\Windows\\System32\\cmd.exe"));

		//Пропустить настройки
		unsigned char c[100];
		int count=99;
		while(count>1)
		{
			count=recv(sendrecv,(char*)(&c), 100,0 );
			if(count<100)
				count=0;
		}

//		send(sendrecv,"-------Hello world---------",15,NULL);

		DeleteFile(logToStart);
	    SetStdioPathW(1, logToStart);
		SetStdioPathW(2, logToStart);

		PROCESS_INFORMATION processInformation;
		STARTUPINFO startupInfo;
		memset(&processInformation, 0, sizeof(processInformation));
		memset(&startupInfo, 0, sizeof(startupInfo));
		startupInfo.cb = sizeof(startupInfo);

		BOOL res=CreateProcessW(exeToStart,exeToStart,    
			NULL,                 // Дескриптор процесса не наследуемый.
			NULL,                 // Дескриптор потока не наследуемый.
			FALSE,                // Установим наследование дескриптора в FALSE.
			0,                    // Флажков создания нет.
			NULL,                 // Используйте блок конфигурации родителя.
			NULL,                 // Используйте стартовый каталог родителя.
			&startupInfo,                  // Указатель на структуру STARTUPINFO.
			&processInformation ) ;               // Указатель на структуру PROCESS_INFORMATION.

		HANDLE file=CreateFile(logToStart,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_ALWAYS,NULL,NULL);
		while(1)
		{
			char buf[100];
			DWORD bytesRead;

			DWORD res=WaitForSingleObject((HANDLE)processInformation.dwProcessId,100);
			ReadFile(file,buf,sizeof(buf),&bytesRead,NULL);
			while(bytesRead!=0)
			{
				send(sendrecv,buf,bytesRead,NULL);
				ReadFile(file,buf,sizeof(buf),&bytesRead,NULL);
			}
			if(res!=WAIT_TIMEOUT)
				exit(0);
		}
    }   
    return 0 ;
};


DWORD WINAPI Service(LPVOID lpParameter)
{
    HANDLE evnt = *((HANDLE*)lpParameter);

    SOCKET sock;
    struct  sockaddr_in sock_addr,sendrecv_addr;
    WSADATA data;
    WORD p;  
    
    p = MAKEWORD( 2,0 );
    WSAStartup(p,&data);

    sock = WSASocket (AF_INET, SOCK_STREAM, 0, 0, 0, 0 );

    sock_addr.sin_family      = PF_INET;
    sock_addr.sin_port        = htons(SHELL_PORT);  
    sock_addr.sin_addr.s_addr = INADDR_ANY;

    bind(sock,( struct sockaddr*)&sock_addr,sizeof(struct  sockaddr_in));

    listen(sock, 10 );

    int lun = sizeof (struct  sockaddr);
    

    while(true )
    {        
        SOCKET sendrecv = accept(sock,( struct  sockaddr*) &sendrecv_addr, &lun);
         
        DWORD dwThread; 
        CreateThread(NULL, 0, Session, &sendrecv, 0 , &dwThread); 
       // CreateThread(NULL, 0, EndSess, &sendrecv, 0 , &dwThread); 
    }

    closesocket(sock);
    WSACleanup(); 

    return 0 ;
}

int  InitConsole()
{
    HANDLE hevt = CreateEvent(NULL, FALSE, FALSE, TEXT( "console_already_inited" ));
    DWORD dwThread;
	int err;
    if  (GetLastError() != ERROR_ALREADY_EXISTS) 
    {
	    HINSTANCE dllInst=LoadLibraryW(L"Coredll.dll");
		SetStdioPathW=(SetStdioPathWFunc)GetProcAddress(dllInst,L"SetStdioPathW");
        CreateThread(NULL, 0,Service,&hevt,0 ,&dwThread); 
		err=GetLastError();
    } 
    else return -1 ;     
    return 0 ;
}
