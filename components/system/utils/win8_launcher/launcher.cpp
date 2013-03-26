#include <stdio.h>
#include <stdarg.h>
#include <wchar.h>
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
  std::string   app_name;
  volatile bool launching;
  int           argc;
  char**        argv;
  char**        env;
  int           result;
};

struct ArgReader
{
  Log&        log;
  DataReader^ reader;
  char        buffer [512];
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
          log.Printf ("try load\n");
        int max_size = buffer + sizeof (buffer) - write_pos;
 
        task<unsigned int> load_task (reader->LoadAsync (max_size));

        load_task.wait ();

         int read_count = reader->UnconsumedBufferLength;

          log.Printf ("read_count=%d\n", read_count);

        if (read_count > max_size)
          read_count = max_size;

          log.Printf ("read_count=%d corrected\n", read_count);

        if (read_count > 0)
        {
          Platform::Array<unsigned char>^ bytes = ref new Platform::Array<unsigned char> (read_count);

          reader->ReadBytes (bytes);

          memcpy (write_pos, &bytes [0], read_count);

          log.Printf ("test='%s'\n", std::string (write_pos, read_count).c_str ());

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

          log.Printf ("teststr='%s'\n", result.c_str ());
        
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
   
      //чтение параметров запуска
      
    ArgReader arg_reader (log, socket);
      
    std::string app_name = arg_reader.Read (),
                cur_dir  = arg_reader.Read ();
                
    if (app_name.size () >= 2 && app_name [0] == '/' && app_name [1] == '/')
      app_name = app_name.substr (1);

    if (cur_dir.size () >= 2 && cur_dir [0] == '/' && cur_dir [1] == '/')
      cur_dir = cur_dir.substr (1);

  //  chdir (cur_dir.c_str ()); 

    log.Printf ("app_name='%s' cur_dir='%s'\n", app_name.c_str (), cur_dir.c_str ());
    log.Printf ("%S\n", std::wstring (Windows::Storage::ApplicationData::Current->LocalFolder->Path->Data ()).c_str ());
    log.Printf ("%S\n", std::wstring (Windows::Storage::ApplicationData::Current->RoamingFolder->Path->Data ()).c_str ());
    log.Printf ("%S\n", std::wstring (Windows::Storage::ApplicationData::Current->TemporaryFolder->Path->Data ()).c_str ());
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
