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
const char* MATHLIB_MAT4F_LIBRARY = "funner.math.mat4f";

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
    Утилиты
*/

vec3f normalize_vec3f (const vec3f& v)
{
  return normalize (v);
}

vec3f cross_vec3f (const vec3f& v1, const vec3f& v2)
{
  return cross (v1, v2);
}

/*
    Создание шлюзов на бинарные операции
*/

template <class Ret, class Arg1, class Arg2, template <class, class, class> class Operation>
inline Invoker make_binary_invoker ()
{
  return make_invoker<Ret (Arg1, Arg2)> (Operation<Arg1, Arg2, Ret> ());
}

/*
    Создание матрицы
*/

mat4f create_mat4a (float a)
{
  return mat4f (a);
}

mat4f create_mat4m (mat4f m)
{
  return mat4f (m);
}

/*
    Селекторы компонент матрицы
*/

vec3f get_mat4f_row (mat4f m, size_t i)
{
  return vec3f(m.row (i));
}

vec3f get_mat4f_column (mat4f m, size_t i)
{
  return m.column (i);
}

float get_mat4f_element (mat4f m, size_t i, size_t j)
{
  return m[i][j];
}

/*
   Операции над матрицами
*/

void transpose (mat4f m)
{
  m.transpose ();
}

void invert (mat4f m)
{
  m.invert ();
}

/*void normalize (mat4f m)
{
  m.normalize ();
} */

mat4f set_row (mat4f m, size_t i, vec3f v)
{
  m[i] = v;
  return m;
}

mat4f set_column (mat4f m, size_t i, vec3f v)
{
  m[0][i] = v.x;
  m[1][i] = v.y;
  m[2][i] = v.z;
  return m;
}

mat4f set_element (mat4f m, size_t i, size_t j, float value)
{
  m[i][j] = value;
  return m;
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
  
    //регистрация функций унарных функций над вектором
    
  math_lib.Register ("length",    make_invoker<float (vec3f)> (&math::length<float, 3>));
  math_lib.Register ("qlength",   make_invoker<float (vec3f)> (&math::qlen<float, 3>));
  math_lib.Register ("normalize", make_invoker<vec3f (vec3f)> (&normalize_vec3f));
  math_lib.Register ("dot",       make_invoker<float (vec3f, vec3f)> (&math::dot<float, 3>));
  math_lib.Register ("cross",     make_invoker<vec3f (vec3f, vec3f)> (&cross_vec3f));
  math_lib.Register ("abs",       make_invoker<vec3f (vec3f)> (&math::abs<float, 3>));
  math_lib.Register ("min",       make_invoker<vec3f (vec3f, vec3f)> (&math::min<float, 3>));
  math_lib.Register ("max",       make_invoker<vec3f (vec3f, vec3f)> (&math::max<float, 3>));

    //регистрация типов данных

  environment.RegisterType (typeid (vec3f), MATHLIB_VEC3F_LIBRARY);
  
    //создание математической библиотеки
    
  InvokerRegistry& mat4f_lib = environment.CreateLibrary (MATHLIB_MAT4F_LIBRARY);
  
       //уменьшить количество инстансов!!!
  
    //регистрация селекторов
    
  mat4f_lib.Register ("row",    make_invoker (&get_mat4f_row));
  mat4f_lib.Register ("column", make_invoker (&get_mat4f_column));
  mat4f_lib.Register ("get",    make_invoker (&get_mat4f_element));

  mat4f_lib.Register ("set_row",     make_invoker (&set_row));
  mat4f_lib.Register ("set_column",  make_invoker (&set_column));
  mat4f_lib.Register ("set_element", make_invoker (&set_element));
  
    //регистрация функций над матрицами
    
  mat4f_lib.Register ("transpose", make_invoker (&transpose));
  mat4f_lib.Register ("invert",    make_invoker (&invert));
//  mat4f_lib.Register ("normalize", make_invoker (&normalize));
  mat4f_lib.Register ("det",       make_invoker (&math::det<float, 4>));
  mat4f_lib.Register ("minor",     make_invoker (&math::minor<float, 4>));
  
  math_lib.Register ("multiply_mat_vec", make_binary_invoker<vec3f, mat4f, vec3f, multiplies> ());  

    //регистрация операций над векторами

  mat4f_lib.Register ("__add", make_binary_invoker<mat4f, mat4f, mat4f, plus> ());
  mat4f_lib.Register ("__sub", make_binary_invoker<mat4f, mat4f, mat4f, minus> ());
  mat4f_lib.Register ("__mul", make_binary_invoker<mat4f, mat4f, mat4f, multiplies> ());
  mat4f_lib.Register ("__div", make_binary_invoker<mat4f, mat4f, float, divides> ());

    //регистрация функций создания матрицы
    
  math_lib.Register ("mat4", make_invoker (&create_mat4a));

    //регистрация типов данных

  environment.RegisterType (typeid (mat4f), MATHLIB_MAT4F_LIBRARY);
    
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
