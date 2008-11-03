#include "shared.h"

#include <common/file.h>
#include <common/strlib.h>

using namespace script;
using namespace common;
using namespace xtl;

namespace
{

/*
    Константы (имена библиотек)
*/

const char* COMMON_FILE_LIBRARY = "File";
const char* COMPONENT_NAME      = "script.binds.Common";
const char* BINDER_NAME         = "Common";

/*
    Регистрация библиотек
*/

//загрузка строки с фильтрацией определённых символов
stl::string load_string_filter_out (const char* file_name, const char* out_filter_string)
{
  stl::string s, filter = common::compress (out_filter_string);
  
  FileSystem::LoadTextFile (file_name, s);
  
  stl::string::size_type pos = 0;  
  
  while ((pos = s.find_first_of (filter, pos)) != stl::string::npos)
    s.erase (pos, 1);

  return s;
}

}

namespace engine
{

void bind_file_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (COMMON_FILE_LIBRARY);

  lib.Register ("LoadString",          make_invoker (implicit_cast<stl::string (*)(const char*)> (&FileSystem::LoadTextFile)));
  lib.Register ("LoadStringFilterOut", make_invoker (&load_string_filter_out));
}

}
