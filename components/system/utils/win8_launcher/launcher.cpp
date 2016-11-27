п»ї#include <stdio.h>
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
    РљРѕРЅСЃС‚Р°РЅС‚С‹
*/

const wchar_t* SHELL_HOST = L"127.0.0.1";   //С…РѕСЃС‚, РЅР° РєРѕС‚РѕСЂС‹Р№ СЃРѕРµРґРёРЅСЏРµС‚СЃСЏ launcher 
const wchar_t* SHELL_PORT = L"1663";        //РїРѕСЂС‚, РЅР° РєРѕС‚РѕСЂС‹Р№ СЃРѕРµРґРёРЅСЏРµС‚СЃСЏ launcher
const char*    ENTRY_NAME = "win8_startup"; //С‚РѕС‡РєР° РІС…РѕРґР° РІ С‚РµСЃС‚

/*
    РўРёРїС‹
*/

typedef STRING ANSI_STRING, *PANSI_STRING;

typedef void (*StdoutFn)(const char* buffer, size_t size, void* user_data);

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
  DataWriter^ stdout_writer;
  Log*        log;
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
    reader->InputStreamOptions |= InputStreamOptions::Partial;
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
    Р¤СѓРЅРєС†РёРё
*/

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

std::wstring towstring (const char* string, int length)
{
  if (!string)
    return L"";

  if (length == -1)
    length = strlen (string);

  std::wstring result;

  result.resize (length);

  int result_size = mbsrtowcs (&result [0], &string, length, 0);

  if (result_size < 0)
    return L"(common::towstring error)";

  result.resize (result_size);

  return result;
}

std::wstring towstring (const std::string& s)
{
  return towstring (s.c_str (), s.size ());
}

FORCEINLINE VOID pRtlInitUnicodeString(PUNICODE_STRING destString, PWSTR sourceString)
{
  destString->Buffer                             = sourceString;
  destString->MaximumLength = destString->Length = wcslen(sourceString) * sizeof(WCHAR);
};

FORCEINLINE VOID pRtlInitString(PANSI_STRING destString, PSTR sourceString)
{
  destString->Buffer        = sourceString;
  destString->MaximumLength = destString->Length = strlen(sourceString) * sizeof(CHAR);
}

void stdout_redirect (const char* data, size_t size, void* user_data)
{
  LaunchInfo* info = (LaunchInfo*)user_data;

  if (!info || !data)
    return;

  info->log->Printf (std::string (data, size).c_str ());

  Platform::Array<unsigned char>^ bytes = ref new Platform::Array<unsigned char> (size);

  memcpy (&bytes [0], data, size);

  info->stdout_writer->WriteBytes (bytes);

  task<unsigned int> store_task (info->stdout_writer->StoreAsync ());

  store_task.wait ();
}

extern "C"
{

NTSYSAPI NTSTATUS NTAPI LdrLoadDll (IN PWCHAR PathToFile OPTIONAL, IN ULONG Flags OPTIONAL, IN PUNICODE_STRING ModuleFileName, OUT PHANDLE ModuleHandle);
NTSYSAPI NTSTATUS NTAPI LdrGetProcedureAddress (IN HMODULE ModuleHandle, IN PANSI_STRING FunctionName OPTIONAL, IN WORD Oridinal OPTIONAL, OUT PVOID *FunctionAddress);

}

[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)	
{
  Log log;

  try
  {
    log.Printf ("Creating socket...\n");

    StreamSocket^ socket = ref new StreamSocket ();

    socket->Control->NoDelay = true;

    task<void> connectionTask (socket->ConnectAsync (ref new HostName (ref new String (SHELL_HOST)), ref new String (SHELL_PORT), SocketProtectionLevel::PlainSocket));

    connectionTask.wait ();

    log.Printf ("Sucessfully connected\n");
 
      //С‡С‚РµРЅРёРµ РїР°СЂР°РјРµС‚СЂРѕРІ Р·Р°РїСѓСЃРєР°
      
    ArgReader arg_reader (log, socket);
      
    std::string app_name = arg_reader.Read (),
                cur_dir  = arg_reader.Read ();
                
    if (app_name.size () >= 2 && app_name [0] == '/' && app_name [1] == '/')
      app_name = app_name.substr (1);

    for (std::string::iterator iter=app_name.begin (), end=app_name.end (); iter!=end; ++iter)
      if (*iter == '/')
        *iter = '\\';

    if (cur_dir.size () >= 2 && cur_dir [0] == '/' && cur_dir [1] == '/')
      cur_dir = cur_dir.substr (1);

    log.Printf ("Launching application '%s' with current dir '%s'\n", app_name.c_str (), cur_dir.c_str ());

    typedef int (*EntryFn)(const char* cur_dir, StdoutFn stdout_handler, void* user_data);

    std::wstring app_name_wstr = towstring (app_name);

    UNICODE_STRING app_name_unicode;

    pRtlInitUnicodeString (&app_name_unicode, &app_name_wstr [0]);

    HANDLE handle = 0;

    NTSTATUS ntstatus = LdrLoadDll (0, 0, &app_name_unicode, &handle);

    if (!handle)
      throw std::runtime_error ("::LdrLoadDll failed");

    log.Printf ("Dll has been loaded\n");
     
    HMODULE module = (HMODULE)handle;

    ANSI_STRING entry_point_name;
     
    pRtlInitString (&entry_point_name, (PSTR)ENTRY_NAME);

    EntryFn entry_fn = 0;
     
    ntstatus = LdrGetProcedureAddress (module, &entry_point_name, 0, (PVOID*)&entry_fn);
     
    if (!entry_fn)
      throw std::runtime_error ("::LdrGetProcedureAddress failed");

    LaunchInfo launch_info;

    launch_info.stdout_writer = ref new DataWriter (socket->OutputStream);
    launch_info.log           = &log;

    int result = entry_fn (cur_dir.c_str (), stdout_redirect, &launch_info);

    log.Printf ("Program was exited (exit code is %d)\n", result);

    return result;
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

  log.Printf ("Exit");

  return 0;
}
