#include <windows.h>
#include <stdio.h>
#include <winsock2.h> 

/*
    Константы
*/

const int SHELL_PORT = 1663; //порт, на который соединяется telnet

/*
    Типы
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
    Функции
*/

void sock_send (int socket, const char* buffer, size_t length)
{
  const char* pos = buffer;
  
  while (length)
  {
    int send_size = send (socket, pos, buffer + length - pos, 0);
    
    if (send_size <= 0)
      break;
     
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

//точка входа
int wmain (int argc, wchar_t* argv [])
{
    //проверка повторного запуска
  printf("launch app\n");fflush(stdout);
  
  HANDLE duplicate_launch_event = CreateEventW (0, FALSE, FALSE, L"console_already_initialized");
  
  if (GetLastError() == ERROR_ALREADY_EXISTS)
  {
    printf ("Launcher has already started\n");
    return -1;
  }    

    //разбор параметров командной строки
    
  if (argc < 4)
  {
    printf ("Usage: launcher <exe_to_start> <log_to_start> <current_dir>\n");
    
    CloseHandle (duplicate_launch_event);
    
    return -1;
  }

  wchar_t* exe_to_start = argv [1];
  wchar_t* log_to_start = argv [2];
  wchar_t* current_dir  = argv [3];

    //инициализация telnet
    
  HINSTANCE core_dll = LoadLibraryW (L"Coredll.dll");
 
  if (!core_dll)
  {
    printf ("Coredll not found\n");
    
    CloseHandle (duplicate_launch_event);    
        return -1;
  }
  
  SetStdioPathWFunc  SetStdioPathW        = (SetStdioPathWFunc)GetProcAddress (core_dll, L"SetStdioPathW");    
  SetCurrentDirWFunc SetCurrentDirectoryW = (SetCurrentDirWFunc)GetProcAddress (core_dll, L"SetCurrentDirectoryW");

  if (!SetStdioPathW)
  {
    printf ("No 'SetStdioPathW' entry in 'coredll.dll' found\n");

    CloseHandle (core_dll);
    CloseHandle (duplicate_launch_event);

    return -1;
  }

  if (!SetCurrentDirectoryW)
  {
    printf ("No 'SetCurrentDirW' entry in 'coredll.dll' found\n");

    CloseHandle (core_dll);
    CloseHandle (duplicate_launch_event);

    return -1;
  }
  
    //установка текущей директории
  if (!SetCurrentDirectoryW (current_dir))
  {
    printf ("Can't set current dirrectory to '%S'\n", current_dir);
    
    CloseHandle (duplicate_launch_event);
    
    return -1;
  }

    //перенаправление ввода / вывода

  DeleteFileW (log_to_start);
  
  if (!SetStdioPathW (1, log_to_start) || !SetStdioPathW (2, log_to_start))
  {
    CloseHandle (core_dll);
    CloseHandle (duplicate_launch_event);    
    
    printf ("SetStdioPathW failed\n");

    return -1;
  }
  
    //инициализация сокетов

  WSADATA wsa_data;
  
  int result = WSAStartup (MAKEWORD(2, 0), &wsa_data);

  if (result)
  {
    printf ("WSAStartup failed with code=%d", result);
    
    CloseHandle (core_dll);
    CloseHandle (duplicate_launch_event);    
    
    return -1;
  }      
    
    //ожидание соединения    

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
  
    //настройка тайм-аута
    
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
  
    //пропуск настроек (сделать поддержку сессии)
  
  char skip_buffer [100];

  for (;;)
  {
    int read_count = recv (sendrecv_socket, skip_buffer, 100, 0);
    
//    if (read_count <= 0)
//      break;

    if (read_count < 100)
      break;
  }  
  
    //запуск процесса
  
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
  
    //ожидание завершения процесса - перенаправление вывода в сокет
     
  HANDLE file = CreateFileW (log_to_start, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_ALWAYS, 0, 0);
  
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
    
  for (;;)
  {
    static const size_t CLOSE_TIMEOUT = 100;    
    
    DWORD wait_result = WaitForSingleObject ((HANDLE)process_information.dwProcessId, CLOSE_TIMEOUT);
    
    char buffer [100];            
    
    for (;;)
    {
      DWORD bytes_read = 0;

      ReadFile (file, buffer, sizeof (buffer), &bytes_read, 0);

      if (!bytes_read)
        break;

      sock_send (sendrecv_socket, buffer, bytes_read);
    }

    if (wait_result != WAIT_TIMEOUT)
      break;
  }

    //освобождение ресурсов
       
  CloseHandle (file);
  closesocket (sendrecv_socket);
  closesocket (listen_socket);
  WSACleanup  ();    
  CloseHandle (core_dll);
  CloseHandle (duplicate_launch_event);
  
  return 0;
}
