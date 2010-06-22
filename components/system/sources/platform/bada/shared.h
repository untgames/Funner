#ifndef SYSTEMLIB_BADA_SHARED_HEADER
#define SYSTEMLIB_BADA_SHARED_HEADER

#include <climits>

#include <xtl/common_exceptions.h>
#include <xtl/intrusive_ptr.h>

#include <common/singleton.h>

#include <syslib/window.h>
#include <syslib/platform/bada.h>

#include <platform/platform.h>

#include <FBase.h>
#include <FApp.h>
#include <FUi.h>

namespace syslib
{

namespace bada
{

//генерация системного исключения
void raise (const char* source, result r);

//получение экземпляра текущего приложения
Osp::App::Application& get_application ();

}

}

//получение числа аргументов при вызове OspMain
extern "C" int get_osp_main_argc ();

//получение указателя на список аргументов OspMain
extern "C" const char** get_osp_main_argv ();


#endif
