#include "shared.h"

using namespace script;
using namespace math;

namespace
{

/*
    Константы (имена библиотек)
*/

const char* MATHLIB_LINEAR_SPLINE_FLOAT_LIBRARY = "Math.LinearSplineFloat";
const char* MATHLIB_LINEAR_SPLINE_VEC2_LIBRARY  = "Math.LinearSplineVec2";
const char* MATHLIB_LINEAR_SPLINE_VEC3_LIBRARY  = "Math.LinearSplineVec3";
const char* MATHLIB_LINEAR_SPLINE_VEC4_LIBRARY  = "Math.LinearSplineVec4";
const char* MATHLIB_SPLINE_WRAP_LIBRARY         = "Math.SplineWrap";

/*
    Создание сплайна
*/

linear_splinef create_linear_splinef ()
{
  return linear_splinef ();
}

linear_spline2f create_linear_spline2f ()
{
  return linear_spline2f ();
}

linear_spline3f create_linear_spline3f ()
{
  return linear_spline3f ();
}

linear_spline4f create_linear_spline4f ()
{
  return linear_spline4f ();
}

/*
   Получение ключей
*/

template <class T>
typename T::time_type get_key_time (const T& spline, size_t index)
{
  return spline.get_key (index).time;
}

template <class T>
typename T::value_type get_key_value (const T& spline, size_t index)
{
  return spline.get_key (index).value;
}

template <class T>
void set_key (T& spline, size_t index, typename T::time_type time, typename T::value_type value)
{
  spline.set_key (index, typename T::key_type (time, value));
}

template <class T>
void add_key (T& spline, typename T::time_type time, typename T::value_type value)
{
  spline.add_key (typename T::key_type (time, value));
}

template <class T>
typename T::value_type eval (T& spline, typename T::time_type time)
{
  return spline.eval (time);
}

/*
    Регистрация библиотеки работы с сплайнами
*/

template <class T>
void bind_spline_library (InvokerRegistry& lib)
{
    //регистрация функций

  lib.Register ("get_BeginWrap",  make_invoker (&T::begin_wrap));
  lib.Register ("get_EndWrap",    make_invoker (&T::end_wrap));
  lib.Register ("get_KeysCount",  make_invoker (&T::keys_count));
  lib.Register ("get_Empty",      make_invoker (&T::empty));
  lib.Register ("get_Capacity",   make_invoker (&T::capacity));
  lib.Register ("get_Closed",     make_invoker (&T::closed));
  lib.Register ("get_ClosedEps",  make_invoker (&T::closed_eps));
  lib.Register ("get_MinTime",    make_invoker (&T::min_time));
  lib.Register ("get_MaxTime",    make_invoker (&T::max_time));
  lib.Register ("set_ClosedEps",  make_invoker (&T::set_closed_eps));
  lib.Register ("Clone",          make_invoker (&T::clone));
  lib.Register ("Wrap",           make_invoker (&T::wrap));
  lib.Register ("Reserve",        make_invoker (&T::reserve));
  lib.Register ("GetKeyTime",     make_invoker (&get_key_time<T>));
  lib.Register ("GetKeyValue",    make_invoker (&get_key_value<T>));
  lib.Register ("SetKey",         make_invoker (&set_key<T>));
  lib.Register ("AddKey",         make_invoker (&add_key<T>));
  lib.Register ("RemoveKey",      make_invoker (&T::remove_key));
  lib.Register ("Clear",          make_invoker (&T::clear));
  lib.Register ("Sort",           make_invoker (&T::sort));
  lib.Register ("Eval",           make_invoker (&eval<T>));
}

}

namespace engine
{

/*
    Регистрация библиотек работы со сплайнами
*/

void bind_math_splines_library (script::Environment& environment)
{
    //создание библиотек
    
  InvokerRegistry linear_splinef_lib  = environment.CreateLibrary (MATHLIB_LINEAR_SPLINE_FLOAT_LIBRARY);
  InvokerRegistry linear_spline2f_lib = environment.CreateLibrary (MATHLIB_LINEAR_SPLINE_VEC2_LIBRARY);
  InvokerRegistry linear_spline3f_lib = environment.CreateLibrary (MATHLIB_LINEAR_SPLINE_VEC3_LIBRARY);
  InvokerRegistry linear_spline4f_lib = environment.CreateLibrary (MATHLIB_LINEAR_SPLINE_VEC4_LIBRARY);
  InvokerRegistry spline_wrap_lib     = environment.CreateLibrary (MATHLIB_SPLINE_WRAP_LIBRARY);

  spline_wrap_lib.Register ("get_Clamp",  make_const (spline_wrap_clamp));
  spline_wrap_lib.Register ("get_Repeat", make_const (spline_wrap_repeat));
  spline_wrap_lib.Register ("get_Mirror", make_const (spline_wrap_mirror));
  
    //регистрация библиотек
  
  bind_spline_library<linear_splinef>  (linear_splinef_lib);
  bind_spline_library<linear_spline2f> (linear_spline2f_lib);
  bind_spline_library<linear_spline3f> (linear_spline3f_lib);
  bind_spline_library<linear_spline4f> (linear_spline4f_lib);

    //регистрация функций создания
  
  linear_splinef_lib.Register  ("Create", make_invoker (&create_linear_splinef));
  linear_spline2f_lib.Register ("Create", make_invoker (&create_linear_spline2f));
  linear_spline3f_lib.Register ("Create", make_invoker (&create_linear_spline3f));
  linear_spline4f_lib.Register ("Create", make_invoker (&create_linear_spline4f));

    //регистрация типов данных

  environment.RegisterType<linear_splinef>  (MATHLIB_LINEAR_SPLINE_FLOAT_LIBRARY);
  environment.RegisterType<linear_spline2f> (MATHLIB_LINEAR_SPLINE_VEC2_LIBRARY);
  environment.RegisterType<linear_spline3f> (MATHLIB_LINEAR_SPLINE_VEC3_LIBRARY);
  environment.RegisterType<linear_spline4f> (MATHLIB_LINEAR_SPLINE_VEC4_LIBRARY);
  environment.RegisterType<spline_wrap>     (MATHLIB_SPLINE_WRAP_LIBRARY);
}

}
