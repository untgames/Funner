#include "shared.h"

using namespace script;
using namespace math;
using namespace stl;

namespace
{

/*
    Константы (имена библиотек)
*/

const char* MATHLIB_LIBRARY       = "funner.math";
const char* MATHLIB_VEC3F_LIBRARY = "funner.math.vec3f";

/*
    Создание вектора
*/

vec3f create_vec3 (float x, float y, float z)
{
  return vec3f (x, y, z);
}

vec3f create_vec2 (float x, float y)
{
  return vec3f (x, y, 0.0f);
}

vec3f create_vec1 (float x)
{
  return vec3f (x, 0.0f, 0.0f);
}

vec3f create_vec (float scalar)
{
  return vec3f (scalar);
}

/*
    Селекторы компонент вектора
*/

template <size_t I> float get_vec3f_element (vec3f v)
{
  return v [I];
}

template <size_t I, size_t J> vec3f get_vec3f_pair (vec3f v)
{
  return vec3f (v [I], v [J], 0);
}

template <size_t I, size_t J, size_t K> vec3f get_vec3f_tuple (vec3f v)
{
  return vec3f (v [I], v [J], v [K]);
}

/*
    Создание шлюзов на бинарные операции
*/

template <class Ret, class Arg1, class Arg2, template <class, class, class> class Operation>
inline Invoker make_binary_invoker ()
{
  return make_invoker<Ret (Arg1, Arg2)> (Operation<Arg1, Arg2, Ret> ());
}

}

namespace script
{

/*
    Регистрация математической библиотеки
*/

void bind_math_library (Environment& environment)
{
    //создание математической библиотеки
    
  InvokerRegistry& vec3f_lib = environment.CreateLibrary (MATHLIB_VEC3F_LIBRARY);
  
       //уменьшить количество инстансов!!!
  
    //регистрация селекторов
    
  vec3f_lib.Register ("x",   make_invoker (&get_vec3f_element<0>));
  vec3f_lib.Register ("y",   make_invoker (&get_vec3f_element<1>));
  vec3f_lib.Register ("z",   make_invoker (&get_vec3f_element<2>));

    //регистрация парных селекторов

  vec3f_lib.Register ("xx",  make_invoker (&get_vec3f_pair<0, 0>));
  vec3f_lib.Register ("xy",  make_invoker (&get_vec3f_pair<0, 1>));
  vec3f_lib.Register ("xz",  make_invoker (&get_vec3f_pair<0, 2>));
  vec3f_lib.Register ("yx",  make_invoker (&get_vec3f_pair<1, 0>));  
  vec3f_lib.Register ("yy",  make_invoker (&get_vec3f_pair<1, 1>));
  vec3f_lib.Register ("yz",  make_invoker (&get_vec3f_pair<1, 2>));
  vec3f_lib.Register ("zx",  make_invoker (&get_vec3f_pair<2, 0>));
  vec3f_lib.Register ("zy",  make_invoker (&get_vec3f_pair<2, 1>));
  vec3f_lib.Register ("zz",  make_invoker (&get_vec3f_pair<2, 2>));
  
    //регистрация тройных селекторов

  vec3f_lib.Register ("xxx",  make_invoker (&get_vec3f_tuple<0, 0, 0>));
  vec3f_lib.Register ("xxy",  make_invoker (&get_vec3f_tuple<0, 0, 1>));
  vec3f_lib.Register ("xxz",  make_invoker (&get_vec3f_tuple<0, 0, 2>));
  vec3f_lib.Register ("xyx",  make_invoker (&get_vec3f_tuple<0, 1, 0>));  
  vec3f_lib.Register ("xyy",  make_invoker (&get_vec3f_tuple<0, 1, 1>));
  vec3f_lib.Register ("xyz",  make_invoker (&get_vec3f_tuple<0, 1, 2>));
  vec3f_lib.Register ("xzx",  make_invoker (&get_vec3f_tuple<0, 2, 0>));
  vec3f_lib.Register ("xzy",  make_invoker (&get_vec3f_tuple<0, 2, 1>));
  vec3f_lib.Register ("xzz",  make_invoker (&get_vec3f_tuple<0, 2, 2>));
  vec3f_lib.Register ("yxx",  make_invoker (&get_vec3f_tuple<1, 0, 0>));
  vec3f_lib.Register ("yxy",  make_invoker (&get_vec3f_tuple<1, 0, 1>));
  vec3f_lib.Register ("yxz",  make_invoker (&get_vec3f_tuple<1, 0, 2>));
  vec3f_lib.Register ("yyx",  make_invoker (&get_vec3f_tuple<1, 1, 0>));  
  vec3f_lib.Register ("yyy",  make_invoker (&get_vec3f_tuple<1, 1, 1>));
  vec3f_lib.Register ("yyz",  make_invoker (&get_vec3f_tuple<1, 1, 2>));
  vec3f_lib.Register ("yzx",  make_invoker (&get_vec3f_tuple<1, 2, 0>));
  vec3f_lib.Register ("yzy",  make_invoker (&get_vec3f_tuple<1, 2, 1>));
  vec3f_lib.Register ("yzz",  make_invoker (&get_vec3f_tuple<1, 2, 2>));  
  vec3f_lib.Register ("zxx",  make_invoker (&get_vec3f_tuple<2, 0, 0>));
  vec3f_lib.Register ("zxy",  make_invoker (&get_vec3f_tuple<2, 0, 1>));
  vec3f_lib.Register ("zxz",  make_invoker (&get_vec3f_tuple<2, 0, 2>));
  vec3f_lib.Register ("zyx",  make_invoker (&get_vec3f_tuple<2, 1, 0>));  
  vec3f_lib.Register ("zyy",  make_invoker (&get_vec3f_tuple<2, 1, 1>));
  vec3f_lib.Register ("zyz",  make_invoker (&get_vec3f_tuple<2, 1, 2>));
  vec3f_lib.Register ("zzx",  make_invoker (&get_vec3f_tuple<2, 2, 0>));
  vec3f_lib.Register ("zzy",  make_invoker (&get_vec3f_tuple<2, 2, 1>));
  vec3f_lib.Register ("zzz",  make_invoker (&get_vec3f_tuple<2, 2, 2>));

    //регистрация операций над векторами

//  vec3f_lib.Register ("__unm", make_invoker<vec3f (vec3f)> (&vec_neg<float, 3>));

  vec3f_lib.Register ("__add", make_binary_invoker<vec3f, vec3f, vec3f, plus> ());
  vec3f_lib.Register ("__sub", make_binary_invoker<vec3f, vec3f, vec3f, minus> ());
  vec3f_lib.Register ("__mul", make_binary_invoker<vec3f, vec3f, vec3f, multiplies> ()); //??умножение на скаляр
  vec3f_lib.Register ("__div", make_binary_invoker<vec3f, vec3f, vec3f, divides> ()); //??деление на скаляр

    //регистрация функций
    
  InvokerRegistry& math_lib = environment.CreateLibrary (MATHLIB_LIBRARY);
  
    //регистрация функций создания вектора
    
  math_lib.Register ("vec3", make_invoker (&create_vec3));
  math_lib.Register ("vec2", make_invoker (&create_vec2));
  math_lib.Register ("vec1", make_invoker (&create_vec1));
  math_lib.Register ("vec",  make_invoker (&create_vec));

    //регистрация типов данных

  environment.RegisterType (typeid (vec3f), MATHLIB_VEC3F_LIBRARY);
  
  
    
/*  InvokerRegistry* registry = environment.FindRegistry ("global");
  
  if (!registry)
    return;
    
  registry->Register ("vecLength",    make_invoker<float (vec3f)> (&math::length<float, 3>));
  registry->Register ("vecQLength",   make_invoker<float (vec3f)> (&math::qlen<float, 3>));
////  registry.Register ("vecNormalize", make_invoker<vec3f (vec3f)> (&math::normalize<float, 3>)); //сделать адаптер вызова
  registry->Register ("vecDot",       make_invoker<float (vec3f, vec3f)> (&math::dot<float, 3>));
////  registry.Register ("vecCross",     make_invoker<vec3f (vec3f, vec3f)> (&math::cross<float, 3>));//сделать адаптер вызова
////  registry.Register ("vecAngle",     make_invoker<float (vec3f)> (&math::angle<float, 3>));
  registry->Register ("vecAbs",       make_invoker<vec3f (vec3f)> (&math::abs<float, 3>));
  registry->Register ("vecMin",       make_invoker<vec3f (vec3f, vec3f)> (&math::min<float, 3>));
  registry->Register ("vecMax",       make_invoker<vec3f (vec3f, vec3f)> (&math::max<float, 3>));*/
}

}
