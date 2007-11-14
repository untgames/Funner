#ifndef SCRIPTBINDS_TESTS_SHARED_HEADER
#define SCRIPTBINDS_TESTS_SHARED_HEADER

#include <stdio.h>
#include <string.h>
#include <time.h>

#include <xtl/shared_ptr.h>
#include <xtl/bind.h>

#include <script/bind_libraries.h>
#include <script/environment.h>
#include <script/bind.h>

#include <common/file.h>
#include <common/strlib.h>

#include <mathlib.h>

using namespace script;
using namespace math;

/*
    Сериализация математических типов в строку
*/

namespace math
{

void to_string (stl::string& buffer, const math::vec3f& v)
{
  buffer = common::format ("[%g %g %g]", v.x, v.y, v.z);
}

}

/*
    Отладочное протоколирование
*/

inline void log_print (const char* message)
{
  printf ("%s\n", message);
}

inline void print (float x)
{
  printf ("%g", x);
}

inline void print (const vec3f& v)
{
  printf ("[%g %g %g]", v.x, v.y, v.z);
}

template <class T>
inline void print (const char* tag, const T& value)
{
  printf ("%s: ", tag);
  print  (value);
  printf ("\n");
}

/*
    Выполнение скрипта    
*/

void load_script (IInterpreter& interpreter, const char* file_name)
{
  common::InputFile file (file_name);

  stl::string buffer (file.Size (), ' ');

  file.Read (&buffer [0], buffer.size ());        

  interpreter.DoCommands (file_name, buffer.c_str (), buffer.size (), &log_print);    
}

#endif
