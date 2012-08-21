#include "shared.h"

#include <common/file.h>
#include <common/strlib.h>

using namespace script;
using namespace common;
using namespace xtl;

namespace
{

/*
     онстанты (имена библиотек)
*/

const char* COMMON_BACKGROUND_COPY_STATE_LIBRARY               = "Common.BackgroundCopyState";
const char* COMMON_STATIC_BACKGROUND_COPY_STATE_STATUS_LIBRARY = "Common.BackgroundCopyStateStatus";
const char* COMMON_FILE_LIBRARY                                = "Common.File";

/*
    –егистраци€ библиотек
*/

//добавление/удаление пути поиска
void add_search_path (const char* path)
{
  FileSystem::AddSearchPath (path);
}

void remove_search_path (const char* path)
{
  FileSystem::RemoveSearchPath (path);
}

//загрузка строки с фильтрацией определЄнных символов
stl::string load_string_filter_out (const char* file_name, const char* out_filter_string)
{
  stl::string s, filter = common::compress (out_filter_string);

  FileSystem::LoadTextFile (file_name, s);

  stl::string::size_type pos = 0;

  while ((pos = s.find_first_of (filter, pos)) != stl::string::npos)
    s.erase (pos, 1);

  return s;
}

//посылка строки (обычно используетс€ дл€ HTTP запросов)
stl::string post_string (const char* file_name, const char* string)
{
  try
  {
    if (!file_name)
      throw xtl::make_null_argument_exception ("", "file_name");
      
    if (!string)
      throw xtl::make_null_argument_exception ("", "string");
    
    common::StdFile output_file (file_name, common::FileMode_ReadWrite);

    size_t src_size = strlen (string);
    
    size_t result_size = output_file.Write (string, src_size);
    
    if (result_size != src_size)
      throw xtl::format_operation_exception ("", "Can't post %u bytes. Posted only %u bytes (source='%s')", src_size, result_size, file_name);
      
    static const size_t RESERVED_RESULT_SIZE = 16384;
      
    stl::string result;
    
    result.reserve (RESERVED_RESULT_SIZE);
    
    xtl::uninitialized_storage<char> buffer (RESERVED_RESULT_SIZE);

    while (!output_file.Eof ())
    {
      size_t read_size = output_file.Read (buffer.data (), buffer.size ());

      if (!read_size)
        break;
        
      result.append (buffer.data (), buffer.data () + read_size);
    }
    
    return result;
  }
  catch (xtl::exception& e)
  {
    e.touch ("engine::script_binds::post_string");
    throw;
  }  
}

void async_load_callback (const stl::string& result, const xtl::function<void (const stl::string&)>& callback)
{
  ActionQueue::PushAction (xtl::bind (callback, result), ActionThread_Main);
}

struct AsyncLoadCallback
{
  typedef xtl::function<void (const char*)> CallbackHandler;
  
  CallbackHandler callback_handler;
  
  AsyncLoadCallback (const CallbackHandler& in_callback_handler) : callback_handler (in_callback_handler) {}

  void operator () (AsyncResult& result)
  {
    callback_handler (result.Result<stl::string> ().c_str ());
  }
};

stl::string post_string_wrapper (const stl::string& file_name, const stl::string& string)
{
  return post_string (file_name.c_str (), string.c_str ());
}

AsyncResult async_post_string (const char* file_name, const char* string, xtl::function<void (const char*)>& callback)
{
  if (!file_name)
    throw xtl::make_null_argument_exception ("engine::script_binds::async_post_string", "file_name");
    
  if (!string)
    throw xtl::make_null_argument_exception ("engine::script_binds::async_post_string", "string");    

  return async_invoke<stl::string> (xtl::bind (&post_string_wrapper, stl::string (file_name), stl::string (string)), ActionThread_Current, AsyncLoadCallback (callback));
}

stl::string load_file (const stl::string& name)
{
  return FileSystem::LoadTextFile (name.c_str ());
}

AsyncResult async_load_string (const char* file_name, xtl::function<void (const char*)>& callback)
{
  if (!file_name)
    throw xtl::make_null_argument_exception ("engine::script_binds::async_load_string", "file_name");
    
  return async_invoke<stl::string> (xtl::bind (&load_file, stl::string (file_name)), ActionThread_Current, AsyncLoadCallback (callback));
}

stl::string file_hash_string (const FileHash& file_hash)
{
  stl::string return_value;

  return_value.reserve (sizeof (file_hash.md5) * 2);

  for (size_t i = 0; i < sizeof (file_hash.md5); i++)
    return_value.append (common::format ("%02x", file_hash.md5 [i]));

  return return_value;
}

stl::string file_hash (const char* file_name, size_t max_hash_size)
{
  FileHash file_hash;

  FileSystem::GetFileHash (file_name, max_hash_size, file_hash);

  return file_hash_string (file_hash);
}

stl::string file_hash (const char* file_name)
{
  FileHash file_hash;

  FileSystem::GetFileHash (file_name, file_hash);

  return file_hash_string (file_hash);
}

}

namespace engine
{

void bind_static_background_copy_state_library (Environment& environment)
{
  InvokerRegistry background_copy_state_status_lib = environment.CreateLibrary (COMMON_STATIC_BACKGROUND_COPY_STATE_STATUS_LIBRARY);

  background_copy_state_status_lib.Register ("get_Started",    make_const (BackgroundCopyStateStatus_Started));
  background_copy_state_status_lib.Register ("get_InProgress", make_const (BackgroundCopyStateStatus_InProgress));
  background_copy_state_status_lib.Register ("get_Finished",   make_const (BackgroundCopyStateStatus_Finished));
  background_copy_state_status_lib.Register ("get_Canceled",   make_const (BackgroundCopyStateStatus_Canceled));
  background_copy_state_status_lib.Register ("get_Failed",     make_const (BackgroundCopyStateStatus_Failed));

  environment.RegisterType<BackgroundCopyStateStatus> (COMMON_STATIC_BACKGROUND_COPY_STATE_STATUS_LIBRARY);
}

void bind_common_background_copy_state (script::Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (COMMON_BACKGROUND_COPY_STATE_LIBRARY);

  bind_static_background_copy_state_library (environment);

  lib.Register ("get_BytesCopied", make_invoker (&BackgroundCopyState::BytesCopied));
  lib.Register ("get_FileSize",    make_invoker (&BackgroundCopyState::FileSize));
  lib.Register ("get_Status",      make_invoker (&BackgroundCopyState::Status));
  lib.Register ("get_StatusText",  make_invoker (&BackgroundCopyState::StatusText));

  environment.RegisterType<BackgroundCopyState> (COMMON_BACKGROUND_COPY_STATE_LIBRARY);
}

void bind_common_file_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (COMMON_FILE_LIBRARY);

  lib.Register ("AddSearchPath",       make_invoker (&add_search_path));
  lib.Register ("FileHash",            make_invoker (make_invoker (xtl::implicit_cast<stl::string (*) (const char*, size_t)> (&file_hash)),
                                                     make_invoker (xtl::implicit_cast<stl::string (*) (const char*)> (&file_hash))));
  lib.Register ("LoadString",          make_invoker (implicit_cast<stl::string (*)(const char*)> (&FileSystem::LoadTextFile)));
  lib.Register ("LoadStringFilterOut", make_invoker (&load_string_filter_out));
  lib.Register ("PostString",          make_invoker (&post_string));
  lib.Register ("AsyncLoadString",     make_invoker (&async_load_string));
  lib.Register ("AsyncPostString",     make_invoker (&async_post_string));
  lib.Register ("IsFileExist",         make_invoker (&FileSystem::IsFileExist));
  lib.Register ("RemoveSearchPath",    make_invoker (&remove_search_path));
  lib.Register ("BackgroundCopyFile",  make_invoker (
    make_invoker<Action (const char*, const char*, const FileSystem::BackgroundCopyFileCallback&, size_t)> (xtl::bind (&FileSystem::BackgroundCopyFile, _1, _2, _3, ActionThread_Main, _4)),
    make_invoker<Action (const char*, const char*, const FileSystem::BackgroundCopyFileCallback&)> (xtl::bind (&FileSystem::BackgroundCopyFile, _1, _2, _3, ActionThread_Main, 0))));

  lib.Register ("CreateAsyncLoadHandler",           make_callback_invoker<AsyncLoadCallback::CallbackHandler::signature_type> ());
  lib.Register ("CreateBackgroundCopyFileCallback", make_callback_invoker<FileSystem::BackgroundCopyFileCallback::signature_type> ());
}

}
