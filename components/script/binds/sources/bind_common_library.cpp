#include "shared.h"

using namespace script;
using namespace common;
using namespace xtl;

namespace
{

/*
    Константы (имена библиотек)
*/

const char* COMMON_FILE_LIBRARY = "File";

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

void bind_file_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (COMMON_FILE_LIBRARY);

  lib.Register ("LoadString",          make_invoker (implicit_cast<stl::string (*)(const char*)> (&FileSystem::LoadTextFile)));
  lib.Register ("LoadStringFilterOut", make_invoker (&load_string_filter_out));
}

}

namespace script
{

namespace binds
{

/*
    Регистрация библиотеки общего назначения
*/

void bind_common_library (Environment& environment)
{
    //регистрация библиотек

  bind_file_library (environment);
}

}

}
