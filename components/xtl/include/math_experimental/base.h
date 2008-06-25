#ifndef __MATHLIB_BASE__
#define __MATHLIB_BASE__

namespace math
{
template <class T,size_t size> class vec;
template <class T,size_t size> class matrix;
template <class T>             class quat;

/*
        Векторы
*/



/*
	Шаблонный класс операций сравнения
*/




///Скалярное произведение
//template <class T,size_t size> T vec_dot_product (const vec<T,size>& a,const vec<T,size>& b);

//Векторное произведение
/*template <class T>  void  vec_cross_product (vec<T,3>& res,const vec<T,3>& a,const vec<T,3>& b);
template <class T>  void  vec_cross_product (vec<T,4>& res,const vec<T,4>& a,const vec<T,4>& b); //???*/

/*
        Матрицы
*/

///Основные операции
template <class T,size_t size> void matrix_add (matrix<T,size,size>& res,const matrix<T,size,size>& a,const matrix<T,size,size>& b);
template <class T,size_t size> void matrix_sub (matrix<T,size,size>& res,const matrix<T,size,size>& a,const matrix<T,size,size>& b);

/*
  Умножение матрицы на матрицу
    rr - строка на строку
    cc - столбец на столбец
    rc - строка на столбец 
    cr - столбец на строку
*/
template <class T,size_t size> void matrix_mul_rr (matrix<T,size,size>& res,const matrix<T,size,size>& a,const matrix<T,size,size>& b);
template <class T,size_t size> void matrix_mul_cc (matrix<T,size,size>& res,const matrix<T,size,size>& a,const matrix<T,size,size>& b);
template <class T,size_t size> void matrix_mul_rc (matrix<T,size,size>& res,const matrix<T,size,size>& a,const matrix<T,size,size>& b);
template <class T,size_t size> void matrix_mul_cr (matrix<T,size,size>& res,const matrix<T,size,size>& a,const matrix<T,size,size>& b);

template <class T,size_t size> void matrix_mul_rr (matrix<T,size,size>& a,const matrix<T,size,size>& b); //*=
template <class T,size_t size> void matrix_mul_cc (matrix<T,size,size>& a,const matrix<T,size,size>& b); //*=
template <class T,size_t size> void matrix_mul_rc (matrix<T,size,size>& a,const matrix<T,size,size>& b); //*=
template <class T,size_t size> void matrix_mul_cr (matrix<T,size,size>& a,const matrix<T,size,size>& b); //*=

//По умолчанию строка на столбец
template <class T,size_t size> void matrix_mul (matrix<T,size,size>& res,const matrix<T,size,size>& a,const matrix<T,size,size>& b);
template <class T,size_t size> void matrix_mul (matrix<T,size,size>& a,const matrix<T,size,size>& b); //*=

template <class T,size_t size> void matrix_mul_scalar (matrix<T,size,size>& res,const matrix<T,size,size>& a,const T& b);
template <class T,size_t size> void matrix_div_scalar (matrix<T,size,size>& res,const matrix<T,size,size>& a,const T& b);

///Присваивание / копирование
template <class T,size_t size> void matrix_copy          (matrix<T,size,size>& res,const matrix<T,size,size>& src);
template <class T,size_t size> void matrix_assign_scalar (matrix<T,size,size>& res,const T& value);

///Сравнение 
template <class T,size_t size> bool matrix_equal  (const matrix<T,size,size>& a,const matrix<T,size,size>& b);
template <class T,size_t size> bool matrix_equal  (const matrix<T,size,size>& a,const matrix<T,size,size>& b,const T& eps);
template <class T,size_t size> bool matrix_nequal (const matrix<T,size,size>& a,const matrix<T,size,size>& b);

///Изменение знака
template <class T,size_t size> void matrix_neg (matrix<T,size,size>& res,const matrix<T,size,size>& src);

///Утилиты
template <class T,size_t size> T     matrix_det       (const matrix<T,size,size>&);
template <class T,size_t size> T     matrix_minor     (const matrix<T,size,size>&,size_t,size_t);
template <class T,size_t size> void  matrix_transpose (matrix<T,size,size>& res,const matrix<T,size,size>& src);
template <class T,size_t size> void  matrix_transpose (matrix<T,size,size>&);
template <class T,size_t size> void  matrix_invert    (matrix<T,size,size>& res,const matrix<T,size,size>& src);
template <class T,size_t size> void  matrix_invert    (matrix<T,size,size>&);
template <class T,size_t size> void  matrix_normalize (matrix<T,size,size>& res,const matrix<T,size,size>& src);
template <class T,size_t size> void  matrix_normalize (matrix<T,size,size>&);

/*
   Умножение матрицы на вектор     
     1. Матрица строка на вектор столбец
     2. Матрица столбец на вектор строку
*/
template <class T,size_t size> void  matrix_mul_vec (vec<T,size>&,const matrix<T,size,size>&,const vec<T,size>&);
template <class T,size_t size> void  vec_mul_matrix (vec<T,size>&,const vec<T,size>&,const matrix<T,size,size>&);

//Без учета смещения
template <class T,size_t size> void  matrix_mul_vec (vec<T,size>&,const matrix<T,size+1>&,const vec<T,size>&);
template <class T,size_t size> void  vec_mul_matrix (vec<T,size>&,const vec<T,size>&,const matrix<T,size+1>&);

/*
        Кватернионы
*/

///Основные операции
template <class T> void quat_add (quat<T>& res,const quat<T>& a,const quat<T>& b);
template <class T> void quat_sub (quat<T>& res,const quat<T>& a,const quat<T>& b);
template <class T> void quat_mul (quat<T>& res,const quat<T>& a,const quat<T>& b);
template <class T> void quat_mul (quat<T>& res,const quat<T>& q);

template <class T> void quat_mul_scalar (quat<T>& res,const quat<T>& a,const T& b);
template <class T> void quat_div_scalar (quat<T>& res,const quat<T>& a,const T& b);

///Присваивание / копирование
template <class T> void quat_copy          (quat<T>& res,const quat<T>& src);
template <class T> void quat_assign_scalar (quat<T>& res,const T& value);

///Сравнение 
template <class T> bool quat_equal  (const quat<T>& a,const quat<T>& b);
template <class T> bool quat_equal  (const quat<T>& a,const quat<T>& b,const T& eps);
template <class T> bool quat_nequal (const quat<T>& a,const quat<T>& b);

///Изменение знака
template <class T> void quat_neg (quat<T>& res,const quat<T>& src);

///Утилиты
template <class T> T     quat_length    (const quat<T>&);
template <class T> T     quat_norm      (const quat<T>&);
template <class T> void  quat_normalize (quat<T>& res,const quat<T>& src);

///Определение кватерниона обратного данному
template <class T> void quat_invert (quat<T>& res,const quat<T>& src);

///Скалярное произведение
template <class T> T quat_inner (const quat<T>& a,const quat<T>& b);

///Умножение кватерниона на вектор (поворот)
template <class T> void  quat_mul_vec (vec<T,3>& res,const quat<T>& q,const vec<T,3>& v);
template <class T> void  quat_mul_vec (vec<T,4>& res,const quat<T>& q,const vec<T,4>& v);

}

#endif
