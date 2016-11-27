#include <stdio.h>
#include <string>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <objbase.h>
#include <atlbase.h>
#include <windows.h>

//#define DEBUG_LOG

/*
    Константы
*/

const int SHELL_PORT = 1663; //порт, на который соединяется launcher


/*++

  Routine Description:

    This routine launches your app using IApplicationActivationManager.

  Arguments:

    strAppUserModelID - AppUserModelID of the app to launch.
    pdwProcessId - Output argument that receives the process id of the launched app.

  Return value:

    HRESULT indicating success/failure

--*/
HRESULT LaunchApp(const std::wstring& strAppUserModelId, PDWORD pdwProcessId)
{
    CComPtr<IApplicationActivationManager> spAppActivationManager;
    HRESULT hrResult = E_INVALIDARG;
    if (!strAppUserModelId.empty())
    {
        // Instantiate IApplicationActivationManager
        hrResult = CoCreateInstance(CLSID_ApplicationActivationManager,
            NULL,
            CLSCTX_LOCAL_SERVER,
            IID_IApplicationActivationManager,
            (LPVOID*)&spAppActivationManager);

        if (SUCCEEDED(hrResult))
        {
            // This call ensures that the app is launched as the foreground window
            hrResult = CoAllowSetForegroundWindow(spAppActivationManager, NULL);
            
            // Launch the app
            if (SUCCEEDED(hrResult))
            {
                hrResult = spAppActivationManager->ActivateApplication(strAppUserModelId.c_str(),
                    NULL,
                    AO_NONE,
                    pdwProcessId);
            }
        }
    }

    return hrResult;
}

//получение строки с сообщением об ошибке
std::string get_error_message (DWORD error_code)
{
  static const size_t MAX_BUFFER_SIZE = 1024;

  void* buffer = malloc (MAX_BUFFER_SIZE);

  if (!buffer)
    return "Win32 error";

  FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                 0, error_code, MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&buffer, 0, 0);                 

    //отсечение завершающих \n и пробелов

  char* iter = (char*)buffer;
  
  iter += strlen (iter);    
  
  if (iter == buffer)
    return "";      
    
  for (bool loop=true; loop;)
    switch (*--iter)
    {
      case '\n':
      case '\r':
      case ' ':
      case '\t':
        break;
      default:
      {
        iter [1] = '\0';
        loop     = false;
        break;
      }
    }

  std::string message = (char*)buffer;

  free (buffer);

  return message;
}

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
  
  char buffer [4096];
  
  vsnprintf (buffer, sizeof (buffer), format, list);
  
  buffer [sizeof (buffer) - 1] = '\0';
  
  size_t length = strlen (buffer);
  
  sock_send (socket, buffer, length);
}

std::string tostring (const wchar_t* string, int length)
{
  if (length == -1)
    length = wcslen (string);

  std::string result;

  result.resize (length * 4);

  int result_size = wcsrtombs (&result [0], &string, length, 0);

  if (result_size < 0)
    return "(common::tostring error)";

  result.resize (result_size);

  return result;
}

std::string tostring (const std::wstring& s)
{
  return tostring (s.c_str (), s.size ());
}

int launch (const wchar_t* app_name)
{
    //активация приложения

  HRESULT hrResult = S_OK;

  if (SUCCEEDED (CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)))
  {
    DWORD dwProcessId = 0;

    hrResult = LaunchApp (app_name, &dwProcessId);

    CoUninitialize ();
  }

  return hrResult;
}

int wmain(int argc, wchar_t* argv[])
{
  if (argc < 1 || (argc > 1 && argc < 3))
  {
    printf ("usage: %S <AppName> [<DllName> <CurDir> <Args>]\n", argv [0]);
    return 1;
  }

  std::wstring app_name = argv [1];

    //активация приложения

  if (argc < 2)
    return launch (app_name.c_str ());

  launch (app_name.c_str ());

    //передача параметров

  std::wstring dll_name = argv [2], cur_dir = argv [3];

  WSADATA wsa_data;

  int result = WSAStartup (MAKEWORD(2, 2), &wsa_data);

  if (result)      
  {
    printf ("Error while calling ::WSAStartup: '%s'", get_error_message (result).c_str ());
    fflush (stdout);
    return 1;
  }

    //инициализация telnet
  
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd < 0) 
  {
    printf ("ERROR opening socket");
    return 1;
  }   

#ifdef DEBUG_LOG
  printf ("Socket has been opened\n");
  fflush (stdout);
#endif

  sockaddr_in serv_addr, cli_addr;

  memset ((char*)&serv_addr, 0, sizeof (serv_addr));  
  
  serv_addr.sin_family      = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port        = htons (SHELL_PORT);
  
  if (bind (sockfd, (struct sockaddr*)&serv_addr, sizeof (serv_addr)) < 0)
  {
    printf ("ERROR on binding");
    fflush (stdout); 
    return 1;
  }

#ifdef DEBUG_LOG
  printf ("Bind successfull\n");
  fflush (stdout);
#endif

  listen (sockfd, 5);

#ifdef DEBUG_LOG
  printf ("Listen successfull\n");
  fflush (stdout);
#endif

//  result = launch (app_name.c_str ());

#ifdef DEBUG_LOG
  printf ("Launch successfull\n");
  fflush (stdout);
#endif

  socklen_t clilen = sizeof (cli_addr);

  memset ((char*)&cli_addr, 0, sizeof (cli_addr));  
  
  int newsockfd = accept (sockfd, (struct sockaddr*)&cli_addr, &clilen);

  if (newsockfd < 0) 
  {
    printf ("ERROR on accept");
    fflush (stdout);
    return 1;
  }  

#ifdef DEBUG_LOG
  printf ("Connection accepted\n");
  fflush (stdout);
#endif

  sock_printf (newsockfd, "%s %s\n", tostring (dll_name).c_str (), tostring (cur_dir).c_str ());

//  for (int i=4; i<=argc; i++)    
//    sock_printf (newsockfd, i == 4 ? "%s" : " %s", argv [i]);

//  sock_printf (newsockfd, "\n");

  int timeout = 1000;

  if (setsockopt (newsockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof timeout))
  {
    printf ("ERROR setsockopt");
    return 1;
  }

  fflush (stdout);

  for (;;)
  {
    char buffer [512];

    int read_count = recv (newsockfd, buffer, sizeof (buffer), 0);

    if (!read_count)
      break;

    if (read_count < 0)
      continue;

    printf ("%s", std::string (buffer, read_count).c_str ());
    fflush (stdout);
  }

  closesocket (newsockfd);  
  closesocket (sockfd);  

  return 0;
}
