#ifndef MATHLIB_UTILS_HEADER
#define MATHLIB_UTILS_HEADER

#include <math_experimental/forward.h>
#include <stddef.h>

/*#ifdef _MSC_VER  
  #pragma pack(push,1)
#endif*/

namespace math
{

////////////////////////////////////////////////////////////////////////////////////////////
///Преобразования углов (градусы - радианы)
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
const T deg_to_rad (const T& angle);

template <class T>
const T rad_to_deg (const T& angle);

////////////////////////////////////////////////////////////////////////////////////////////
///Преобразования осевой угол - кватернион, углы Эйлера - кватернион
////////////////////////////////////////////////////////////////////////////////////////////
template <class T> const quat<T> axis_angle_to_quat  (const T& angle,const vec<T,3>& axis);
template <class T> const quat<T> axis_angle_to_quat  (const T& angle,const T& axis_x,const T& axis_y,const T& axis_z);
template <class T> void          axis_angle_to_quat  (const T& angle,const vec<T,3>& axis,quat<T>& res);
template <class T> void          axis_angle_to_quat  (const T& angle,const T& axis_x,const T& axis_y,const T& axis_z,quat<T>& res);

template <class T> const quat<T> euler_angle_to_quat (const T& pitch,const T& yaw,const T& roll);
template <class T> void          euler_angle_to_quat (const T& pitch,const T& yaw,const T& roll,quat<T>&q);

////////////////////////////////////////////////////////////////////////////////////////////
///Афинные преобразования
////////////////////////////////////////////////////////////////////////////////////////////

///Параллельный перенос
template <class T> const matrix<T,4> matrix_translate (const vec<T,3>& v);
template <class T> const matrix<T,4> matrix_translate (const T& a,const T& b, const T& c);
template <class T> void              matrix_translate (const vec<T,3>& v,matrix<T,4>& res);
template <class T> void              matrix_translate (const T& a,const T& b, const T& c,matrix<T,4>& res);


///Масштабирование
template <class T> const matrix<T,4> matrix4_scale    (const vec<T,3>& v);
template <class T> const matrix<T,3> matrix3_scale    (const vec<T,3>& v);
template <class T> void              matrix4_scale    (const vec<T,3>& v,matrix<T,4>& res);
template <class T> void              matrix3_scale    (const vec<T,3>& v,matrix<T,3>& res);


///Поворот вокруг оси
template <class T> const matrix<T,4> matrix4_rotate   (const T& angle,const vec<T,3>& axis);
template <class T> const matrix<T,3> matrix3_rotate   (const T& angle,const vec<T,3>& axis);
template <class T> void              matrix4_rotate   (const T& angle,const vec<T,3>& axis,matrix<T,4>& res);
template <class T> void              matrix3_rotate   (const T& angle,const vec<T,3>& axis,matrix<T,3>& res);

#include <math_experimental/impl/utils.inl>

}

#endif

