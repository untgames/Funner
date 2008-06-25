#ifndef __MATHLIB_BASE__
#define __MATHLIB_BASE__


namespace math
{

template <class T,size_t size> class vec;
template <class type,size_t sizeX,size_t sizeY> class matrix;
template <class T>             class quat;

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