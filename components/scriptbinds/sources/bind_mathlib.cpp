#include "shared.h"

using namespace script;
using namespace math;
using namespace stl;
using namespace common;

namespace
{

/*
    Константы (имена библиотек)
*/

const char* MATHLIB_LIBRARY      = "Math";
const char* MATHLIB_VEC2_LIBRARY = "Math.Vector2";
const char* MATHLIB_VEC3_LIBRARY = "Math.Vector3";
const char* MATHLIB_VEC4_LIBRARY = "Math.Vector4";
const char* MATHLIB_MAT4_LIBRARY = "Math.Matrix4";
const char* MATHLIB_QUAT_LIBRARY = "Math.Quaternion";

/*
    Создание шлюзов унарных и бинарных операций
*/

template <class Ret, class Arg1, class Arg2, template <class, class, class> class Operation>
inline Invoker make_binary_invoker ()
{
  return make_invoker<Ret (Arg1, Arg2)> (Operation<Arg1, Arg2, Ret> ());
}

template <class Ret, class Arg, template <class, class> class Operation>
inline Invoker make_unary_invoker ()
{
  return make_invoker<Ret (Arg)> (Operation<Arg, Ret> ());
}

/*
    Селекторы компонент вектора
*/

template <class T> struct vec_get_element
{
  vec_get_element (size_t in_index) : index (in_index) {}
  
  typename T::value_type operator () (const T& v) const { return v [index]; }

  size_t index;
};

template <class T> struct vec_set_element
{
  typedef typename T::value_type value_type;

  vec_set_element (size_t in_index) : index (in_index) {}
  
  value_type operator () (T& v, const value_type& value) const { return v [index] = value; }
  
  size_t index;
};

/*
    Селекторы кортежей
*/

template <class T, size_t Size, size_t TupleSize> class vec_get_tuple
{
  public:
    vec_get_tuple (size_t i1, size_t i2=0, size_t i3=0, size_t i4=0)
    {
      index [0] = i1;
      index [1] = i2;
      index [2] = i3;
      index [3] = i4;
    }  
    
    vec<T, TupleSize> operator () (const vec<T, Size>& v) const
    {    
      return vec<T, TupleSize> (select (v, 0), select (v, 1), select (v, 2), select (v, 3));
    }
  
  private:
    T select (const vec<T, Size>& v, size_t base_index) const
    {
      size_t new_index = index [base_index];

      return new_index < Size ? v [new_index] : T (0);
    }

    size_t index [4];
};

template <class T, size_t Size, size_t TupleSize> class vec_set_tuple
{
  public:
    vec_set_tuple (size_t i1, size_t i2=0, size_t i3=0, size_t i4=0)
    {
      index [0] = i1;
      index [1] = i2;
      index [2] = i3;
      index [3] = i4;
    }

    void operator () (vec<T, Size>& res, const vec<T, TupleSize>& src) const
    {
      for (size_t i=0; i<TupleSize; i++)
        select (res, i) = src [i];
    }
    
  private:
    T& select (vec<T, Size>& v, size_t base_index) const
    {
      static T dummy;
      
      size_t new_index = index [base_index];

      return new_index < Size ? v [new_index] : dummy;
    }

  private:
    size_t index [4];
};

template <class Vector, size_t TupleSize>
inline Invoker make_vec_get_tuple_invoker (size_t i1, size_t i2=0, size_t i3=0, size_t i4=0)
{
  typedef typename Vector::value_type type;

  enum { size = Vector::_size };  

  return make_invoker<vec<type, TupleSize> (const Vector&)> (vec_get_tuple<type, size, TupleSize> (i1, i2, i3, i4));
}

template <class Vector, size_t TupleSize>
inline Invoker make_vec_set_tuple_invoker (size_t i1, size_t i2=0, size_t i3=0, size_t i4=0)
{
  typedef typename Vector::value_type type;

  enum { size = Vector::_size };  

  return make_invoker<void (Vector&, const vec<type, TupleSize>&)> (vec_set_tuple<type, size, TupleSize> (i1, i2, i3, i4));
}

/*
    Утилиты
*/

template <class T, size_t Size>
vec<T, Size> vec_normalize (const vec<T, Size>& v)
{
  return normalize (v);
}

template <class T, size_t Size>
vec<T, Size> vec_cross (const vec<T, Size>& v1, const vec<T, Size>& v2)
{
  return cross (v1, v2);
}

/*
    Селекторы компонент матрицы
*/

vec3f get_mat4f_row (const mat4f& m, size_t i)
{
  return vec3f (m.row (i));
}

vec3f get_mat4f_column (mat4f m, size_t i)
{
  return m.column (i);
}

float get_mat4f_element (mat4f m, size_t i, size_t j)
{
  return m [i][j];
}

/*
    Селекторы элементов матрицы
*/

template <class T, size_t Size>
T matrix_get_element (const matrix<T, Size>& m, size_t row, size_t column)
{
  if (row >= Size)
    RaiseOutOfRange ("script::matrix_get_element", "row", row, Size);
    
  if (column >= Size)
    RaiseOutOfRange ("script::matrix_get_element", "column", column, Size);

  return m [row][column];
}

template <class T, size_t Size>
void matrix_set_element (matrix<T, Size>& m, size_t row, size_t column, T value)
{
  if (row >= Size)
    RaiseOutOfRange ("script::matrix_set_element", "row", row, Size);
    
  if (column >= Size)
    RaiseOutOfRange ("script::matrix_set_element", "column", column, Size);

  m [row][column] = value;
}

/*
    Селекторы строк матрицы
*/

template <class T, size_t Size> struct matrix_get_row
{
  matrix_get_row (size_t in_index) : index (in_index) {}
  
  xtl::reference_wrapper<vec<T, Size> > operator () (matrix<T, Size>& m) const
  {
    return xtl::ref (m [index]);
  }

  size_t index;
};

template <class T, size_t Size> struct matrix_set_row
{
  matrix_set_row (size_t in_index) : index (in_index) {}
  
  void operator () (matrix<T, Size>& m, const vec<T, Size>& value) const
  {
    m [index] = value;
  }
  
  size_t index;
};

/*
    Создание вектора
*/

vec2f create_vec2 (float x, float y)
{
  return vec2f (x, y);
}

vec3f create_vec3 (float x, float y, float z)
{
  return vec3f (x, y, z);
}

vec4f create_vec4 (float x, float y, float z, float w)
{
  return vec4f (x, y, z, w);
}

vec2f create_scalar2 (float scalar)
{
  return vec2f (scalar);
}

vec3f create_scalar3 (float scalar)
{
  return vec3f (scalar);
}

vec4f create_scalar4 (float scalar)
{
  return vec4f (scalar);
}

/*
    Создание матрицы
*/

mat4f create_mat4 (float a)
{
  return mat4f (a);
}

/*
    Селекторы компонент кватерниона
*/

template <class T>
T quat_get_element (const quat<T>& q, size_t index)
{
  if (index >= 4)
    RaiseOutOfRange ("script::quat_get_element", "index", index, 4);
    
  return q [index];
}

template <class T>
void quat_set_element (quat<T>& q, size_t index, T value)
{
  if (index >= 4)
    RaiseOutOfRange ("script::quat_set_element", "index", index, 4);
  
  q [index]=value;
}

/*
    Создание кватерниона
*/

template <class T>
quat<T> create_quat1 (T a)
{
  return quat<T> (a);
}

template <class T>
quat<T> create_quat (T x, T y, T z, T w)
{
  return quat<T> (x, y, z, w);
}

/*
    Утилиты кватерниона
*/

template <class T>
quat<T> quat_normalize (const quat<T>& q)
{
  return normalize (q);
}

/*
    Регистрация библиотеки работы с векторами
*/

template <class T, size_t Size>
void bind_vec_library (InvokerRegistry& vec_lib)
{
  typedef vec<T, Size> vec_type;

    //регистрация селекторов

  vec_lib.Register ("get_x", make_invoker<T (const vec_type&)> (vec_get_element<vec_type> (0)));
  vec_lib.Register ("get_y", make_invoker<T (const vec_type&)> (vec_get_element<vec_type> (1)));
  vec_lib.Register ("get_z", make_invoker<T (const vec_type&)> (vec_get_element<vec_type> (2)));
  vec_lib.Register ("get_w", make_invoker<T (const vec_type&)> (vec_get_element<vec_type> (3)));
  vec_lib.Register ("set_x", make_invoker<T (vec_type&, T)> (vec_set_element<vec_type> (0)));
  vec_lib.Register ("set_y", make_invoker<T (vec_type&, T)> (vec_set_element<vec_type> (1)));
  vec_lib.Register ("set_z", make_invoker<T (vec_type&, T)> (vec_set_element<vec_type> (2)));
  vec_lib.Register ("set_w", make_invoker<T (vec_type&, T)> (vec_set_element<vec_type> (3)));
  vec_lib.Register ("get_0", "get_x");
  vec_lib.Register ("get_1", "get_y");
  vec_lib.Register ("get_2", "get_z");
  vec_lib.Register ("get_3", "get_w");  
  vec_lib.Register ("set_0", "set_x");
  vec_lib.Register ("set_1", "set_y");
  vec_lib.Register ("set_2", "set_z");
  vec_lib.Register ("set_3", "set_w");
  
    //занести в массивы!!!

    //регистрация селекторов пар

  vec_lib.Register ("get_xx", make_vec_get_tuple_invoker<vec_type, 2> (0, 0));
  vec_lib.Register ("get_xy", make_vec_get_tuple_invoker<vec_type, 2> (0, 1));
  vec_lib.Register ("get_xz", make_vec_get_tuple_invoker<vec_type, 2> (0, 2));
  vec_lib.Register ("get_yx", make_vec_get_tuple_invoker<vec_type, 2> (1, 0));
  vec_lib.Register ("get_yy", make_vec_get_tuple_invoker<vec_type, 2> (1, 1));
  vec_lib.Register ("get_yz", make_vec_get_tuple_invoker<vec_type, 2> (1, 2));
  vec_lib.Register ("get_zx", make_vec_get_tuple_invoker<vec_type, 2> (2, 0));
  vec_lib.Register ("get_zy", make_vec_get_tuple_invoker<vec_type, 2> (2, 1));
  vec_lib.Register ("get_zz", make_vec_get_tuple_invoker<vec_type, 2> (2, 2));
  
  vec_lib.Register ("set_xx", make_vec_set_tuple_invoker<vec_type, 2> (0, 0));
  vec_lib.Register ("set_xy", make_vec_set_tuple_invoker<vec_type, 2> (0, 1));
  vec_lib.Register ("set_xz", make_vec_set_tuple_invoker<vec_type, 2> (0, 2));
  vec_lib.Register ("set_yx", make_vec_set_tuple_invoker<vec_type, 2> (1, 0));
  vec_lib.Register ("set_yy", make_vec_set_tuple_invoker<vec_type, 2> (1, 1));
  vec_lib.Register ("set_yz", make_vec_set_tuple_invoker<vec_type, 2> (1, 2));
  vec_lib.Register ("set_zx", make_vec_set_tuple_invoker<vec_type, 2> (2, 0));
  vec_lib.Register ("set_zy", make_vec_set_tuple_invoker<vec_type, 2> (2, 1));
  vec_lib.Register ("set_zz", make_vec_set_tuple_invoker<vec_type, 2> (2, 2));

    //регистрация селекторов триад

  vec_lib.Register ("get_xxx", make_vec_get_tuple_invoker<vec_type, 3> (0, 0, 0));
  vec_lib.Register ("get_xxy", make_vec_get_tuple_invoker<vec_type, 3> (0, 0, 1));
  vec_lib.Register ("get_xxz", make_vec_get_tuple_invoker<vec_type, 3> (0, 0, 2));
  vec_lib.Register ("get_xyx", make_vec_get_tuple_invoker<vec_type, 3> (0, 1, 0));
  vec_lib.Register ("get_xyy", make_vec_get_tuple_invoker<vec_type, 3> (0, 1, 1));
  vec_lib.Register ("get_xyz", make_vec_get_tuple_invoker<vec_type, 3> (0, 1, 2));
  vec_lib.Register ("get_xzx", make_vec_get_tuple_invoker<vec_type, 3> (0, 2, 0));
  vec_lib.Register ("get_xzy", make_vec_get_tuple_invoker<vec_type, 3> (0, 2, 1));
  vec_lib.Register ("get_xzz", make_vec_get_tuple_invoker<vec_type, 3> (0, 2, 2));
  vec_lib.Register ("get_yxx", make_vec_get_tuple_invoker<vec_type, 3> (1, 0, 0));
  vec_lib.Register ("get_yxy", make_vec_get_tuple_invoker<vec_type, 3> (1, 0, 1));
  vec_lib.Register ("get_yxz", make_vec_get_tuple_invoker<vec_type, 3> (1, 0, 2));
  vec_lib.Register ("get_yyx", make_vec_get_tuple_invoker<vec_type, 3> (1, 1, 0));
  vec_lib.Register ("get_yyy", make_vec_get_tuple_invoker<vec_type, 3> (1, 1, 1));
  vec_lib.Register ("get_yyz", make_vec_get_tuple_invoker<vec_type, 3> (1, 1, 2));
  vec_lib.Register ("get_yzx", make_vec_get_tuple_invoker<vec_type, 3> (1, 2, 0));
  vec_lib.Register ("get_yzy", make_vec_get_tuple_invoker<vec_type, 3> (1, 2, 1));
  vec_lib.Register ("get_yzz", make_vec_get_tuple_invoker<vec_type, 3> (1, 2, 2));
  vec_lib.Register ("get_zxx", make_vec_get_tuple_invoker<vec_type, 3> (2, 0, 0));
  vec_lib.Register ("get_zxy", make_vec_get_tuple_invoker<vec_type, 3> (2, 0, 1));
  vec_lib.Register ("get_zxz", make_vec_get_tuple_invoker<vec_type, 3> (2, 0, 2));
  vec_lib.Register ("get_zyx", make_vec_get_tuple_invoker<vec_type, 3> (2, 1, 0));
  vec_lib.Register ("get_zyy", make_vec_get_tuple_invoker<vec_type, 3> (2, 1, 1));
  vec_lib.Register ("get_zyz", make_vec_get_tuple_invoker<vec_type, 3> (2, 1, 2));
  vec_lib.Register ("get_zzx", make_vec_get_tuple_invoker<vec_type, 3> (2, 2, 0));
  vec_lib.Register ("get_zzy", make_vec_get_tuple_invoker<vec_type, 3> (2, 2, 1));
  vec_lib.Register ("get_zzz", make_vec_get_tuple_invoker<vec_type, 3> (2, 2, 2));
  
  vec_lib.Register ("set_xxx", make_vec_set_tuple_invoker<vec_type, 3> (0, 0, 0));
  vec_lib.Register ("set_xxy", make_vec_set_tuple_invoker<vec_type, 3> (0, 0, 1));
  vec_lib.Register ("set_xxz", make_vec_set_tuple_invoker<vec_type, 3> (0, 0, 2));
  vec_lib.Register ("set_xyx", make_vec_set_tuple_invoker<vec_type, 3> (0, 1, 0));
  vec_lib.Register ("set_xyy", make_vec_set_tuple_invoker<vec_type, 3> (0, 1, 1));
  vec_lib.Register ("set_xyz", make_vec_set_tuple_invoker<vec_type, 3> (0, 1, 2));
  vec_lib.Register ("set_xzx", make_vec_set_tuple_invoker<vec_type, 3> (0, 2, 0));
  vec_lib.Register ("set_xzy", make_vec_set_tuple_invoker<vec_type, 3> (0, 2, 1));
  vec_lib.Register ("set_xzz", make_vec_set_tuple_invoker<vec_type, 3> (0, 2, 2));
  vec_lib.Register ("set_yxx", make_vec_set_tuple_invoker<vec_type, 3> (1, 0, 0));
  vec_lib.Register ("set_yxy", make_vec_set_tuple_invoker<vec_type, 3> (1, 0, 1));
  vec_lib.Register ("set_yxz", make_vec_set_tuple_invoker<vec_type, 3> (1, 0, 2));
  vec_lib.Register ("set_yyx", make_vec_set_tuple_invoker<vec_type, 3> (1, 1, 0));
  vec_lib.Register ("set_yyy", make_vec_set_tuple_invoker<vec_type, 3> (1, 1, 1));
  vec_lib.Register ("set_yyz", make_vec_set_tuple_invoker<vec_type, 3> (1, 1, 2));
  vec_lib.Register ("set_yzx", make_vec_set_tuple_invoker<vec_type, 3> (1, 2, 0));
  vec_lib.Register ("set_yzy", make_vec_set_tuple_invoker<vec_type, 3> (1, 2, 1));
  vec_lib.Register ("set_yzz", make_vec_set_tuple_invoker<vec_type, 3> (1, 2, 2));
  vec_lib.Register ("set_zxx", make_vec_set_tuple_invoker<vec_type, 3> (2, 0, 0));
  vec_lib.Register ("set_zxy", make_vec_set_tuple_invoker<vec_type, 3> (2, 0, 1));
  vec_lib.Register ("set_zxz", make_vec_set_tuple_invoker<vec_type, 3> (2, 0, 2));
  vec_lib.Register ("set_zyx", make_vec_set_tuple_invoker<vec_type, 3> (2, 1, 0));
  vec_lib.Register ("set_zyy", make_vec_set_tuple_invoker<vec_type, 3> (2, 1, 1));
  vec_lib.Register ("set_zyz", make_vec_set_tuple_invoker<vec_type, 3> (2, 1, 2));
  vec_lib.Register ("set_zzx", make_vec_set_tuple_invoker<vec_type, 3> (2, 2, 0));
  vec_lib.Register ("set_zzy", make_vec_set_tuple_invoker<vec_type, 3> (2, 2, 1));
  vec_lib.Register ("set_zzz", make_vec_set_tuple_invoker<vec_type, 3> (2, 2, 2));    
  
    //регистрация операций
  
  vec_lib.Register ("__unm", make_unary_invoker<vec_type, vec_type, negate> ());
  vec_lib.Register ("__add", make_binary_invoker<vec_type, vec_type, vec_type, plus> ());
  vec_lib.Register ("__sub", make_binary_invoker<vec_type, vec_type, vec_type, minus> ());
  vec_lib.Register ("__mul", make_binary_invoker<vec_type, vec_type, vec_type, multiplies> ()); //??умножение на скаляр
  vec_lib.Register ("__div", make_binary_invoker<vec_type, vec_type, vec_type, divides> ()); //??деление на скаляр  
  
    //регистрация функций
    
  vec_lib.Register ("get_length",  make_invoker<T (vec_type)> (&math::length<T, Size>));
  vec_lib.Register ("get_qlength", make_invoker<T (vec_type)> (&math::qlen<T, Size>));
  vec_lib.Register ("normalize",   make_invoker<vec_type (vec_type)> (&vec_normalize<T, Size>));
  vec_lib.Register ("dot",         make_invoker<T (vec_type, vec_type)> (&math::dot<T, Size>));
  vec_lib.Register ("abs",         make_invoker<vec_type (vec_type)> (&math::abs<T, Size>));
  vec_lib.Register ("min",         make_invoker<vec_type (vec_type, vec_type)> (&math::min<T, Size>));
  vec_lib.Register ("max",         make_invoker<vec_type (vec_type, vec_type)> (&math::max<T, Size>));
}

template <class T, size_t Size>
void bind_matrix_library (InvokerRegistry& mat_lib)
{
  typedef matrix<T, Size> matrix_type;
  typedef vec<T, Size>    vec_type;

    //регистрация селекторов

  mat_lib.Register ("get", make_invoker (&matrix_get_element<T, Size>));
  mat_lib.Register ("set", make_invoker (&matrix_set_element<T, Size>));
  
  mat_lib.Register ("get_0", make_invoker<xtl::reference_wrapper<vec_type> (matrix_type&)> (matrix_get_row<T, Size> (0)));
  mat_lib.Register ("get_1", make_invoker<xtl::reference_wrapper<vec_type> (matrix_type&)> (matrix_get_row<T, Size> (1)));
  mat_lib.Register ("get_2", make_invoker<xtl::reference_wrapper<vec_type> (matrix_type&)> (matrix_get_row<T, Size> (2)));
  mat_lib.Register ("get_3", make_invoker<xtl::reference_wrapper<vec_type> (matrix_type&)> (matrix_get_row<T, Size> (3)));
  mat_lib.Register ("set_0", make_invoker<void (matrix_type&, vec_type)> (matrix_set_row<T, Size> (0)));
  mat_lib.Register ("set_1", make_invoker<void (matrix_type&, vec_type)> (matrix_set_row<T, Size> (1)));
  mat_lib.Register ("set_2", make_invoker<void (matrix_type&, vec_type)> (matrix_set_row<T, Size> (2)));
  mat_lib.Register ("set_3", make_invoker<void (matrix_type&, vec_type)> (matrix_set_row<T, Size> (3)));  
  
    //регистрация операций над матрицами

  mat_lib.Register ("__add", make_binary_invoker<matrix_type, matrix_type, matrix_type, plus> ());
  mat_lib.Register ("__sub", make_binary_invoker<matrix_type, matrix_type, matrix_type, minus> ());
  mat_lib.Register ("__mul", make_binary_invoker<matrix_type, matrix_type, matrix_type, multiplies> ());
  mat_lib.Register ("__div", make_binary_invoker<matrix_type, matrix_type, T, divides> ());
  
    //регистрация функций над матрицами  
    
  mat_lib.Register ("transpose", make_invoker (&math::transpose<T, Size>));
  mat_lib.Register ("inverse", make_invoker (&math::invert<T, Size>));
  mat_lib.Register ("det", make_invoker (&math::det<T, Size>));
}

template <class T>
void bind_quat_library (InvokerRegistry& quat_lib)
{
//  typedef matrix<T, Size> matrix_type;
//  typedef vec<T, Size>    vec_type;
  typedef quat<T>         quat_type;  

    //регистрация селекторов

  quat_lib.Register ("get", make_invoker (&quat_get_element<T>));
  quat_lib.Register ("set", make_invoker (&quat_set_element<T>));
  quat_lib.Register ("get_x", make_invoker<T (const quat_type&)> (vec_get_element<quat_type> (0)));
  quat_lib.Register ("get_y", make_invoker<T (const quat_type&)> (vec_get_element<quat_type> (1)));
  quat_lib.Register ("get_z", make_invoker<T (const quat_type&)> (vec_get_element<quat_type> (2)));
  quat_lib.Register ("get_w", make_invoker<T (const quat_type&)> (vec_get_element<quat_type> (3)));
  quat_lib.Register ("set_x", make_invoker<T (quat_type&, T)> (vec_set_element<quat_type> (0)));
  quat_lib.Register ("set_y", make_invoker<T (quat_type&, T)> (vec_set_element<quat_type> (1)));
  quat_lib.Register ("set_z", make_invoker<T (quat_type&, T)> (vec_set_element<quat_type> (2)));
  quat_lib.Register ("set_w", make_invoker<T (quat_type&, T)> (vec_set_element<quat_type> (3)));  
  quat_lib.Register ("get_0", "get_x");
  quat_lib.Register ("get_1", "get_y");
  quat_lib.Register ("get_2", "get_z");
  quat_lib.Register ("get_3", "get_w");
  quat_lib.Register ("set_0", "set_x");
  quat_lib.Register ("set_1", "set_y");
  quat_lib.Register ("set_2", "set_z");
  quat_lib.Register ("set_3", "set_w");  
  
    //регистрация операций  
    
  quat_lib.Register ("__unm", make_unary_invoker<quat_type, quat_type, negate> ());
  quat_lib.Register ("__add", make_binary_invoker<quat_type, quat_type, quat_type, plus> ());
  quat_lib.Register ("__sub", make_binary_invoker<quat_type, quat_type, quat_type, minus> ());
  quat_lib.Register ("__mul", make_binary_invoker<quat_type, quat_type, quat_type, multiplies> ()); //??умножение на скаляр
  
  
    //регистрация функций
    
// vec_lib.Register ("get_length",  make_invoker<T (quat_type)> (&math::length<T, Size>));
//  vec_lib.Register ("get_qlength", make_invoker<T (quat_type)> (&math::qlen<T, Size>));
  quat_lib.Register ("normalize",   make_invoker<quat_type (quat_type)> (&quat_normalize<T>));
  quat_lib.Register ("inner",       make_invoker<T (quat_type, quat_type)> (&math::inner<T>));
}

}

namespace script
{

/*
    Регистрация математической библиотеки
*/

void bind_math_library (Environment& environment)
{
    //создание библиотек
    
  InvokerRegistry& global_lib = environment.Library ("global");
  InvokerRegistry& math_lib   = environment.CreateLibrary (MATHLIB_LIBRARY);
  InvokerRegistry& vec2_lib   = environment.CreateLibrary (MATHLIB_VEC2_LIBRARY);
  InvokerRegistry& vec3_lib   = environment.CreateLibrary (MATHLIB_VEC3_LIBRARY);
  InvokerRegistry& vec4_lib   = environment.CreateLibrary (MATHLIB_VEC4_LIBRARY);
  InvokerRegistry& mat4_lib   = environment.CreateLibrary (MATHLIB_MAT4_LIBRARY);
  InvokerRegistry& quat_lib   = environment.CreateLibrary (MATHLIB_QUAT_LIBRARY);
  
    //регистрация библиотек
  
  bind_vec_library<float, 2> (vec2_lib);
  bind_vec_library<float, 3> (vec3_lib);
  bind_vec_library<float, 4> (vec4_lib);     
  bind_matrix_library<float, 4> (mat4_lib);
  bind_quat_library<float> (quat_lib);
  
    //регистрация специфических операций над векторами

  vec3_lib.Register ("cross", make_invoker<vec3f (vec3f, vec3f)> (&vec_cross<float, 3>));
//  vec4_lib.Register ("cross", make_invoker<vec4f (vec4f, vec4f)> (&vec_cross<float, 4>));

    //регистрация функций создания векторов и матриц

  math_lib.Register   ("vec2",    make_invoker (&create_vec2));
  math_lib.Register   ("vec3",    make_invoker (&create_vec3));
  math_lib.Register   ("vec4",    make_invoker (&create_vec4));
  math_lib.Register   ("scalar2", make_invoker (&create_scalar2));
  math_lib.Register   ("scalar3", make_invoker (&create_scalar3));
  math_lib.Register   ("scalar4", make_invoker (&create_scalar4));
  global_lib.Register ("vec2", math_lib, "vec2");
  global_lib.Register ("vec3", math_lib, "vec3");
  global_lib.Register ("vec4", math_lib, "vec4");
  global_lib.Register ("scalar2", math_lib, "scalar2");
  global_lib.Register ("scalar3", math_lib, "scalar3");
  global_lib.Register ("scalar4", math_lib, "scalar4");
  
  math_lib.Register   ("mat4", make_invoker (&create_mat4));  
  global_lib.Register ("mat4", math_lib, "mat4");
  
  quat_lib.Register   ("quat", make_invoker (&create_quat<float>));
  quat_lib.Register   ("quat1", make_invoker (&create_quat1<float>));
  global_lib.Register ("quat", quat_lib, "quat");
  global_lib.Register ("quat1", quat_lib, "quat1");

    //регистрация типов данных

  environment.RegisterType<vec2f> (MATHLIB_VEC2_LIBRARY);
  environment.RegisterType<vec3f> (MATHLIB_VEC3_LIBRARY);
  environment.RegisterType<vec4f> (MATHLIB_VEC4_LIBRARY);
  environment.RegisterType<mat4f> (MATHLIB_MAT4_LIBRARY);
  environment.RegisterType<quatf> (MATHLIB_QUAT_LIBRARY);  
}

}
