#include <stdio.h>
#include <stdarg.h>
#include <wchar.h>
#include <time.h>

#include <memory>
#include <string>

#include <windows.h>
#include <Subauth.h>

#include <wrl/client.h>
#include <ppltasks.h>

using namespace Platform;
using namespace Windows::Storage::Streams;
using namespace Windows::Networking;
using namespace Windows::Networking::Sockets;
using namespace Windows::System::Threading;
using namespace Windows::Foundation;
using namespace concurrency;

#pragma comment (linker, "/defaultlib:ntdll.lib")

/*
    Константы
*/

const wchar_t* SHELL_HOST = L"127.0.0.1";   //хост, на который соединяется launcher 
const wchar_t* SHELL_PORT = L"1663";        //порт, на который соединяется launcher

/*
    Типы
*/

class Log
{
  public:
    Log ()
    {
      std::wstring log_path = std::wstring (Windows::Storage::ApplicationData::Current->LocalFolder->Path->Data ()) + L"\\launch.log";

      file = _wfopen (log_path.c_str (), L"wt");

      if (!file)
        throw std::runtime_error ("Log file not found");
    }

    ~Log ()
    {
      fclose (file);
    }

    void Printf (const char* format, ...)
    {
      va_list args;

      va_start (args, format);

      vfprintf (file, format, args);
      fflush (file);
    }

  private:
    FILE* file;
};

struct LaunchInfo
{
  Log&          log;
  DataWriter^   stdout_writer;
  HANDLE        stdout_handle;
  std::string   app_name;
  std::string   cur_dir;
  volatile bool launching;
  int           result;

  LaunchInfo (Log& in_log) : log (in_log), stdout_writer (), stdout_handle (), launching (true), result () {}
};

struct ArgReader
{
  Log&        log;
  DataReader^ reader;
  char        buffer [4096];
  char*       read_pos;
  char*       write_pos;
  
  ArgReader (Log& in_log, StreamSocket^ socket)
    : log (in_log)
    , reader (ref new DataReader (socket->InputStream))
    , read_pos (buffer)
    , write_pos (buffer)
  {
    reader->InputStreamOptions = InputStreamOptions::Partial;
  }

  std::string Read ()
  {
    std::string result;
    
    for (;;)
    {
      if (write_pos == buffer + sizeof (buffer))
      {
        size_t shift_offset = read_pos - buffer;
        
        if (!shift_offset)
          throw std::runtime_error ("Command not found\n");
        
        memmove (buffer, read_pos, write_pos - read_pos);
        
        write_pos -= shift_offset;
        read_pos  -= shift_offset;
      }
      
      if (read_pos == write_pos)
      {
        int max_size = buffer + sizeof (buffer) - write_pos;
 
        task<unsigned int> load_task (reader->LoadAsync (max_size));

        load_task.wait ();

        int read_count = reader->UnconsumedBufferLength;

        if (read_count > max_size)
          read_count = max_size;

        if (read_count > 0)
        {
          Platform::Array<unsigned char>^ bytes = ref new Platform::Array<unsigned char> (read_count);

          reader->ReadBytes (bytes);

          memcpy (write_pos, &bytes [0], read_count);

          delete bytes;

          write_pos += read_count;
        }
      }

      for (char* p=read_pos; p!=write_pos; p++)
        if (*p == ' ' || *p == '\n')
        {
          result.assign (read_pos, p);

          read_pos = p + 1;

          if (read_pos > buffer + sizeof (buffer))
            read_pos = buffer + sizeof (buffer);
        
          return result;
        }
    }
  }
};

/*
    Функции
*/

/*void sock_printf (DataWriter^ stream, const char* format, ...)
{
  va_list list;
  
  va_start (list, format);
  
  char buffer [256];
  
  vsnprintf (buffer, sizeof (buffer), format, list);
  
  buffer [255] = '\0';
  
  size_t length = strlen (buffer);

  stream->WriteBytes (ref new array<unsigned char> (length, buffer));
} */

void dump_thread (LaunchInfo* info)
{
  Log& log = info->log;

  log.Printf ("Dump thread has been started\n");

  char buffer [4096];

  for (;;)
  {
    if (!info->launching)
      return;

    DWORD read_count = 0;

  printf ("!!! %u\n", clock ());
  fflush (stdout);

    log.Printf ("%s(%u)\n", __FUNCTION__, __LINE__);    

//    if (!ReadFile (info->stdout_handle, buffer, sizeof (buffer), &read_count, 0))
    if (!ReadFile (info->stdout_handle, buffer, 3, &read_count, 0))
      continue;

    log.Printf ("%s(%u)\n", __FUNCTION__, __LINE__);    

    log.Printf ("read %u bytes\n", read_count);    

    if (!read_count)
      continue;

    Platform::Array<unsigned char>^ bytes = ref new Platform::Array<unsigned char> (read_count);

    memcpy (&bytes [0], buffer, read_count);
    
    info->stdout_writer->WriteBytes (bytes);

    delete bytes;
  }  
}

[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)	
{
  Log log;

  try
  {
    log.Printf ("Creating socket...\n");

    StreamSocket^ socket = ref new StreamSocket ();

    task<void> connectionTask (socket->ConnectAsync (ref new HostName (ref new String (SHELL_HOST)), ref new String (SHELL_PORT), SocketProtectionLevel::PlainSocket));

    connectionTask.wait ();

    log.Printf ("Sucessfully connected\n");

      //stdout redirect

    HANDLE hRead, hWrite;

//    SECURITY_ATTRIBUTES pipe_attr; 
    
//    pipe_attr.nLength              = sizeof (SECURITY_ATTRIBUTES); 
//    pipe_attr.bInheritHandle       = TRUE; 
//    pipe_attr.lpSecurityDescriptor = NULL; 

    if (!CreatePipe (&hRead, &hWrite, 0, 0))
      throw std::runtime_error ("::CreatePipe has been failed");

    if (!SetStdHandle (STD_OUTPUT_HANDLE, hWrite))
      throw std::runtime_error ("::SetStdHandle failed");
  
    printf("\n->Start of parent execution.\n"); fflush (stdout);

    log.Printf ("StdOut has been redirected\n");
   
      //чтение параметров запуска
      
    ArgReader arg_reader (log, socket);
      
    std::string app_name = arg_reader.Read (),
                cur_dir  = arg_reader.Read ();
                
    if (app_name.size () >= 2 && app_name [0] == '/' && app_name [1] == '/')
      app_name = app_name.substr (1);

    if (cur_dir.size () >= 2 && cur_dir [0] == '/' && cur_dir [1] == '/')
      cur_dir = cur_dir.substr (1);

    log.Printf ("Launching application '%s' with current dir '%s'\n", app_name.c_str (), cur_dir.c_str ());

    LaunchInfo info (log), *info_ptr = &info;

    info.app_name      = app_name;
    info.cur_dir       = cur_dir;
    info.stdout_handle = hRead;
    info.stdout_writer = ref new DataWriter (socket->OutputStream);

    task<void> dump_task (Windows::System::Threading::ThreadPool::RunAsync (ref new WorkItemHandler ([&info_ptr](IAsyncAction^) { dump_thread (info_ptr); })));

    dump_task.wait ();

//    _chdir (cur_dir.c_str ()); 

  }
  catch (std::exception& e)
  {
    log.Printf ("%s\n    at %s\n", e.what (), __FUNCTION__);
  }
  catch (Platform::Exception^ e)
  {
    log.Printf ("%S\n    at %s\n", e->Message->Data (), __FUNCTION__);
  }
  catch (...)
  {
    log.Printf ("unknown exception\n    at %s\n", __FUNCTION__);
  }

  log.Printf ("exit");

  return 0;
}
