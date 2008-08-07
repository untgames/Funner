#include "shared.h"

using namespace script;
using namespace common;
using namespace xtl;

namespace
{

/*
     онстанты (имена библиотек)
*/

const char* COMMON_FILE_LIBRARY = "static.File";

/*
    –егистраци€ библиотек
*/

void bind_file_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (COMMON_FILE_LIBRARY);

  lib.Register ("LoadString", make_invoker (implicit_cast<stl::string (*)(const char*)> (&FileSystem::LoadTextFile)));
}

}

namespace script
{

namespace binds
{

/*
    –егистраци€ библиотеки общего назначени€
*/

void bind_common_library (Environment& environment)
{
    //регистраци€ библиотек

  bind_file_library (environment);
}

}

}
