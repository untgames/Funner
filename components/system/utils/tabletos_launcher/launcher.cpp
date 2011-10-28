#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#include <string>
#include <memory>

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

struct LaunchInfo
{
  std::string   app_name;
  volatile bool launching;
  int           stdout_file;
  int           newsockfd;
};

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
  
  vsnprintf (buffer, sizeof (buffer), format, list);
  
  buffer [255] = '\0';
  
  size_t length = strlen (buffer);
  
  sock_send (socket, buffer, length);
}

int redirectStdOut(void)
{
  int pipePair [2];

  if (pipe (pipePair) != 0)
  {
    perror ("ERROR on pipe");
    return -1;
  }

  dup2 (pipePair [1], STDOUT_FILENO);  

  return pipePair [0];
}

void* launch (void* data)
{
  LaunchInfo* info = (LaunchInfo*)data;
  
  if (!info)
  {
    printf ("BAD LaunchInfo\n");
    return 0;
  }
  
  system (info->app_name.c_str ());
  
  return 0;
}

void* dump (void* data)
{
  LaunchInfo* info = (LaunchInfo*)data;
  
  if (!info)
  {
    printf ("BAD LaunchInfo\n");
    return 0;
  }

  while (info->launching)
  {
    static const size_t CLOSE_TIMEOUT = 100;
    
    char buffer [100];
    
    while (info->launching)
    {      
      int bytes_read = read (info->stdout_file, buffer, sizeof (buffer));

      if (!bytes_read)
        break;

      sock_send (info->newsockfd, buffer, bytes_read);
    }
  }
  
  return 0;
}

int main ()
{  
    //инициализация telnet
  
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd < 0) 
  {
    perror ("ERROR opening socket");
    return 1;
  }   

  sockaddr_in serv_addr, cli_addr;

  bzero ((char*)&serv_addr, sizeof (serv_addr));  
  
  serv_addr.sin_family      = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port        = htons (SHELL_PORT);
  
  if (bind (sockfd, (struct sockaddr*)&serv_addr, sizeof (serv_addr)) < 0)
  {
    perror ("ERROR on binding");
    return 1;
  }

  listen (sockfd, 5);

  socklen_t clilen = sizeof (cli_addr);
  
  int newsockfd = accept (sockfd, (struct sockaddr*)&cli_addr, &clilen);

  if (newsockfd < 0) 
  {
    perror("ERROR on accept");
    return 1;
  }  
  
    //пропуск настроек (сделать поддержку сессии)
  
  char skip_buffer [100];

  for (;;)
  {
    int read_count = recv (newsockfd, skip_buffer, 100, 0);
    
//    if (read_count <= 0)
//      break;

    if (read_count < 100)
      break;
  }    
  
    //перенаправление стандартного вывода

  int stdout_file = redirectStdOut ();
  
  if (stdout_file < 0)  
    return 1;
    
  fcntl (stdout_file, F_SETFL, O_NONBLOCK);

    //запуск  
    
  std::auto_ptr<LaunchInfo> info (new LaunchInfo);
  
  info->app_name    = "/accounts/devuser/funner/tmp/tabletos-x86/XTL.STL.TESTS/tests/stl/accum1";
  info->launching   = 1;
  info->newsockfd   = newsockfd;
  info->stdout_file = stdout_file;
  
  pthread_t launch_thread, dump_thread;
  
  pthread_create (&launch_thread, 0, &launch, &*info);
  pthread_create (&dump_thread, 0, &dump, &*info);

  pthread_join (launch_thread, 0);

  info->launching = 0;

  pthread_join (dump_thread, 0);

  close (newsockfd);

  return 0;
}

