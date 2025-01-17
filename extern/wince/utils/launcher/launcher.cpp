#include <windows.h>
#include <stdio.h>
#include <winsock2.h> 

/*
    Constants
*/

const int SHELL_PORT = 1663; //port, to connect with telnet

/*
    Types
*/

enum ProtocolMsgId
{
  ProtocolMsgId_IAC  = 255,
  ProtocolMsgId_Will = 251,
  ProtocolMsgId_Wont = 252,
  ProtocolMsgId_Do   = 253,
  ProtocolMsgId_Dont = 254,
};

typedef BOOL (WINAPI *SetStdioPathWFunc)(DWORD id,PWSTR pwszPath);
typedef BOOL (WINAPI *SetCurrentDirWFunc)(PWSTR pwszPath);

/*
    Functions
*/

void sock_send (int socket, const char* buffer, size_t length)
{
  const char* pos = buffer;
  
  while (length)
  {
    int send_size = send (socket, pos, buffer + length - pos, 0);
    
    if (send_size == SOCKET_ERROR)
    {
      printf ("sock_send error\n");
      break;
    }
     
    pos    += send_size;
    length -= send_size;
  }
}

void sock_printf (int socket, const char* format, ...)
{
  va_list list;
  
  va_start (list, format);
  
  char buffer [256];
  
  _vsnprintf (buffer, sizeof (buffer), format, list);
  
  buffer [255] = '\0';
  
  size_t length = strlen (buffer);
  
  sock_send (socket, buffer, length);
}

//entry point
int wmain (int argc, wchar_t* argv [])
{
    //Check duplicate launch
  printf("launch app\n");fflush(stdout);
  
  HANDLE duplicate_launch_event = CreateEventW (0, FALSE, FALSE, L"console_already_initialized");
  
  if (GetLastError() == ERROR_ALREADY_EXISTS)
  {
    printf ("Launcher has already started\n");
    return -1;
  }    

    //parse launch arguments
    
  if (argc < 4)
  {
    printf ("Usage: launcher <exe_to_start> <log_to_start> <current_dir>\n");
    
    CloseHandle (duplicate_launch_event);
    
    return -1;
  }

  wchar_t* exe_to_start = argv [1];
  wchar_t* log_to_start = argv [2];
  wchar_t* current_dir  = argv [3];

printf("exe_to_start=%S\n",exe_to_start);
printf("log_to_start=%S\n",log_to_start);
printf(" current_dir=%S\n",current_dir);

  for (unsigned int i=0;i<wcslen(exe_to_start);i++)
    if (exe_to_start[i]=='/')
      exe_to_start[i]='\\';

  for (unsigned int i=0;i<wcslen(log_to_start);i++)
    if (log_to_start[i]=='/')
      log_to_start[i]='\\';

  for (unsigned int i=0;i<wcslen(current_dir);i++)
    if (current_dir[i]=='/')
      current_dir[i]='\\';

  wchar_t filename[MAX_PATH+50];
  wsprintf (filename,L"%s.env",exe_to_start);
printf("currentdir_file=%S\n",filename);
  HANDLE currentdir_file = CreateFileW (filename, GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, 0, CREATE_ALWAYS, 0, 0);
  if( !currentdir_file)
  {
    printf ("Can't create environment variables file\n");
    
    CloseHandle (duplicate_launch_event);    
    return -1;
  }

  DWORD len=0;
  wsprintf (filename,L"CurrentDir='%s'",current_dir);
  BOOL res=WriteFile (currentdir_file,filename,wcslen(filename)*sizeof(wchar_t),&len,0);
  if( !res )
  {
    printf ("Can't write environment variables file\n");
    
    CloseHandle (duplicate_launch_event);    
    CloseHandle (currentdir_file);
    return -1;
  }

//exe_to_start=L"\\funner\\tmp\\winmobile6\\XTL.STL.TESTS\\tests\\stl\\accum1.exe";
//log_to_start=L"\\funner\\tmp\\winmobile6\\XTL.STL.TESTS\\tests\\stl\\accum1.exe.stdout";
//current_dir=L"\\funner\\tests\\stl";
    //telnet initialization
    
  HINSTANCE core_dll = LoadLibraryW (L"Coredll.dll");
 
  if (!core_dll)
  {
    printf ("Coredll not found\n");
    
    CloseHandle (duplicate_launch_event);    
        return -1;
  }
  
  SetStdioPathWFunc  SetStdioPathW        = (SetStdioPathWFunc)GetProcAddress (core_dll, L"SetStdioPathW");    
//  SetCurrentDirWFunc SetCurrentDirectoryW = (SetCurrentDirWFunc)GetProcAddress (core_dll, L"SetCurrentDirectoryW");

  if (!SetStdioPathW)
  {
    printf ("No 'SetStdioPathW' entry in 'coredll.dll' found\n");

    CloseHandle (core_dll);
    CloseHandle (duplicate_launch_event);

    return -1;
  }

/*  if (!SetCurrentDirectoryW)
  {
    printf ("No 'SetCurrentDirW' entry in 'coredll.dll' found\n");

    CloseHandle (core_dll);
    CloseHandle (duplicate_launch_event);

    return -1;
  }
  
    //set current directory
  if (!SetCurrentDirectoryW (current_dir))
  {
    printf ("Can't set current dirrectory to '%S'\n", current_dir);
    
    CloseHandle (duplicate_launch_event);
    
    return -1;
  }
*/
    //redirect input / output

  DeleteFileW (log_to_start);
  
  if (!SetStdioPathW (1, log_to_start) || !SetStdioPathW (2, log_to_start))
  {
    CloseHandle (core_dll);
    CloseHandle (duplicate_launch_event);    
    
    printf ("SetStdioPathW failed\n");

    return -1;
  }
  
    //initialize sockets

  WSADATA wsa_data;
  
  int result = WSAStartup (MAKEWORD(2, 0), &wsa_data);

  if (result)
  {
    printf ("WSAStartup failed with code=%d", result);
    
    CloseHandle (core_dll);
    CloseHandle (duplicate_launch_event);    
    
    return -1;
  }      
    
    //wait for connection

  SOCKET listen_socket = WSASocketW (AF_INET, SOCK_STREAM, 0, 0, 0, 0);
  
  if (listen_socket == INVALID_SOCKET)
  {
    printf ("Socket WSASocketW failed\n");
    
    WSACleanup ();    
    CloseHandle (core_dll);
    CloseHandle (duplicate_launch_event);

    return -1;
  }
  
  sockaddr_in sock_addr;
  
  memset (&sock_addr, 0, sizeof (sock_addr));

  sock_addr.sin_family      = PF_INET;
  sock_addr.sin_port        = htons (SHELL_PORT);  
  sock_addr.sin_addr.s_addr = INADDR_ANY;

  result = bind (listen_socket, (sockaddr*)&sock_addr, sizeof (struct sockaddr_in));
  
  if (result)
  {
    printf ("Socket bind is failed wit code=%d\n", result);

    closesocket (listen_socket);
    WSACleanup ();    
    CloseHandle (core_dll);
    CloseHandle (duplicate_launch_event);

    return -1;
  }
  
  static const size_t MAX_CONNECTIONS_COUNT = 1;

  result = listen (listen_socket, MAX_CONNECTIONS_COUNT);
  
  if (result)
  {
    printf ("Socket listen is failed with code=%d\n", result);
    
    closesocket (listen_socket);
    WSACleanup  ();
    CloseHandle (core_dll);
    CloseHandle (duplicate_launch_event);

    return -1;
  }  

  sockaddr_in sendrecv_addr;
  
  int addr_len = sizeof (sockaddr);    

  SOCKET sendrecv_socket = accept (listen_socket, (sockaddr*)&sendrecv_addr, &addr_len);
  
  if (sendrecv_socket == INVALID_SOCKET)
  {
    printf ("Socket accept failed\n");

    closesocket (listen_socket);
    WSACleanup  ();
    CloseHandle (core_dll);
    CloseHandle (duplicate_launch_event);    

    return -1;
  }
  
    //setup time-out
    
  timeval timeout;

  timeout.tv_sec  = 0;
  timeout.tv_usec = 0;
  
/*  if (int result = setsockopt (sendrecv_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof (timeout)))
  {
    sock_printf (sendrecv_socket, "Socket setsockopt failed with code %d\n", result);
    
    closesocket (sendrecv_socket);
    closesocket (listen_socket);
    WSACleanup  ();    
    CloseHandle (core_dll);
    CloseHandle (duplicate_launch_event);
    
    return -1;
  }*/
  
    //skip settings (todo session support)
  
  char skip_buffer [100];

  for (;;)
  {
    int read_count = recv (sendrecv_socket, skip_buffer, 100, 0);
    
//    if (read_count <= 0)
//      break;

    if (read_count < 100)
      break;
  }  
  
    //launch process
  
  PROCESS_INFORMATION process_information;

  memset (&process_information, 0, sizeof (process_information));

  if (!CreateProcessW (exe_to_start, exe_to_start, 0, 0, FALSE, 0, 0, 0, 0, &process_information))
  {
    sock_printf (sendrecv_socket, "Can't create process for '%S'\n", exe_to_start);

    closesocket (sendrecv_socket);
    closesocket (listen_socket);
    WSACleanup  ();    
    CloseHandle (core_dll);
    CloseHandle (duplicate_launch_event);    

    return -1;
  }  
  
    //wait for process complete - redirect output to socket
     
 /* int err=GetLastError();
  while (file==INVALID_HANDLE_VALUE)
  {
	  printf("-------------@@@@@@@@@@@-------------------");
	  Sleep(3000);
	  file = CreateFileW (log_to_start, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, 0, OPEN_ALWAYS, 0, 0);
  }*/
    
  DWORD file_pos=0;  
  for (;;)
  {
    static const size_t CLOSE_TIMEOUT = 100;    
    
    DWORD wait_result = WaitForSingleObject ((HANDLE)process_information.dwProcessId, CLOSE_TIMEOUT);

    char buffer [100];            
    
    HANDLE file = CreateFileW (log_to_start, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, 0, OPEN_ALWAYS, 0, 0);
    if (!file)
    {
      sock_printf (sendrecv_socket, "Can't open log file '%S'\n", log_to_start);

      closesocket (sendrecv_socket);
      closesocket (listen_socket);
      WSACleanup  ();    
      CloseHandle (core_dll);
      CloseHandle (duplicate_launch_event);    

      return -1;
    }
 
    SetFilePointer (file, file_pos, 0, FILE_BEGIN);
    for (;;)
    {
      DWORD bytes_read = 0;

      ReadFile (file, buffer, sizeof (buffer), &bytes_read, 0);
      if (!bytes_read)
        break;

      file_pos+=bytes_read;
      sock_send (sendrecv_socket, buffer, bytes_read);
    }
    CloseHandle (file);

    if (wait_result == 0)
      break;
  }

    //free resources
       
  closesocket (sendrecv_socket);
  closesocket (listen_socket);
  WSACleanup  ();    
  CloseHandle (core_dll);
  CloseHandle (duplicate_launch_event);
  
  return 0;
}
