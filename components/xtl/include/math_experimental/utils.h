#ifndef MATHLIB_UTILS_HEADER
#define MATHLIB_UTILS_HEADER

namespace math
{

////////////////////////////////////////////////////////////////////////////////////////////
///Преобразования углов (градусы - радианы)
////////////////////////////////////////////////////////////////////////////////////////////
long double deg2rad (long double angle);
long double rad2deg (long double angle);
double      deg2rad (double angle);
double      rad2deg (double angle);
float       deg2rad (float angle);
float       rad2deg (float angle);

////////////////////////////////////////////////////////////////////////////////////////////
///Преобразования: кватернион - матрица, осевой угол, углы Эйлера
////////////////////////////////////////////////////////////////////////////////////////////
template <class T> void quat2matrix     (const quat<T>& q,matrix<T,4>& res);
template <class T> void quat2matrix     (const quat<T>& q,matrix<T,3>& res);
template <class T> void quat2AxisAngle  (const quat<T>& q,T& angle,vec<T,3>& axis,const T& eps = 1e-03f);
template <class T> void quat2EulerAngle (const quat<T>& q,T& pitch,T& yaw,T& roll);

////////////////////////////////////////////////////////////////////////////////////////////
///Преобразования осевой угол - кватернион, углы Эйлера - кватернион
////////////////////////////////////////////////////////////////////////////////////////////
template <class T> void AxisAngle2quat  (const T& angle,const vec<T,3>& axis,quat<T>& q);
template <class T> void EulerAngle2quat (const T& pitch,const T& yaw,const T& roll,quat<T>& q);

template <class T> const quat<T> fromEulerAngle (const T& pitch,const T& yaw,const T& roll);
template <class T> const quat<T> fromAxisAngle  (const T& angle,const vec<T,3>& axis);

const quat<float>  fromEulerAnglef (float pitch,float yaw,float roll);
const quat<double> fromEulerAngled (double pitch,double yaw,double roll);
const quat<float>  fromAxisAnglef  (float angle,float axis_x,float axis_y,float axis_z);
const quat<double> fromAxisAngled  (double angle,double axis_x,double axis_y,double axis_z);

////////////////////////////////////////////////////////////////////////////////////////////
///Преобразования: матрица - кватернион
////////////////////////////////////////////////////////////////////////////////////////////
template <class T> void matrix2quat (const matrix<T,4>& m,quat<T>& res);
template <class T> void matrix2quat (const matrix<T,3>& m,quat<T>& res);

////////////////////////////////////////////////////////////////////////////////////////////
///Афинные преобразования
////////////////////////////////////////////////////////////////////////////////////////////

///Параллельный перенос
template <class T> void matrix_translate (matrix<T,4>& res,const vec<T,3>&);

///Масштабирование
template <class T> void matrix_scale (matrix<T,4>& res,const vec<T,3>&);
template <class T> void matrix_scale (matrix<T,3>& res,const vec<T,3>&);

///Поворот вокруг оси
template <class T> void matrix_rotate (matrix<T,4>& res,const T& angle,const vec<T,3>& axis);
template <class T> void matrix_rotate (matrix<T,3>& res,const T& angle,const vec<T,3>& axis);

template <class T> const matrix<T,4> translate (const vec<T,3>&);
template <class T> const matrix<T,4> translate (const T& x,const T& y,const T& z);
template <class T> const matrix<T,4> scale     (const vec<T,3>&);
template <class T> const matrix<T,4> scale     (const T& x,const T& y,const T& z);
template <class T> const matrix<T,4> rotate    (const T& angle,const vec<T,3>& axis);
template <class T> const matrix<T,4> rotate    (const T& angle,const T& x,const T& y,const T& z);

const matrix<float,4>  translatef (float x,float y,float z);
const matrix<double,4> translated (double x,double y,double z);
const matrix<float,4>  scalef     (float x,float y,float z);
const matrix<double,4> scaled     (double x,double y,double z);
const matrix<float,4>  rotatef    (float angle,const vec<float,3>& axis);
const matrix<float,4>  rotatef    (float angle,float x,float y,float z);
const matrix<double,4> rotated    (double angle,const vec<double,3>& axis);
const matrix<double,4> rotated    (double angle,double x,double y,double z);

const matrix<float,4> lookatf (const vec<float,3>& pos,const vec<float,3>& center,const vec<float,3>& up);

}

#endif
