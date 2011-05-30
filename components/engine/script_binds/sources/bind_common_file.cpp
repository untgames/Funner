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

const char* COMMON_FILE_LIBRARY = "Common.File";

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

}

namespace engine
{

void bind_common_file_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (COMMON_FILE_LIBRARY);

  lib.Register ("AddSearchPath",       make_invoker (&add_search_path));
  lib.Register ("LoadString",          make_invoker (implicit_cast<stl::string (*)(const char*)> (&FileSystem::LoadTextFile)));
  lib.Register ("LoadStringFilterOut", make_invoker (&load_string_filter_out));
  lib.Register ("PostString",          make_invoker (&post_string));
  lib.Register ("IsFileExist",         make_invoker (&FileSystem::IsFileExist));
  lib.Register ("RemoveSearchPath",    make_invoker (&remove_search_path));
}

}
