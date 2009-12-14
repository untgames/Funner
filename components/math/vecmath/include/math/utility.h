#ifndef MATHLIB_VECMATH_UTILITY_HEADER
#define MATHLIB_VECMATH_UTILITY_HEADER

#include <cmath>

#include <math/angle.h>
#include <math/matrix.h>
#include <math/quat.h>

namespace math
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Преобразования: кватернион - матрица, осевой угол, углы Эйлера
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> void         to_matrix (const quat<T>& q, matrix<typename quat<T>::value_type, 4>& res);
template <class T> void         to_matrix (const quat<T>& q, matrix<typename quat<T>::value_type, 3>& res);
template <class T> matrix<T, 4> to_matrix (const quat<T>&);

template <class T> quat<T> to_quat (const matrix<T, 4>&);
template <class T> quat<T> to_quat (const matrix<T, 3>&);
template <class T> quat<T> to_quat (const angle<T>& pitch, const angle<T>& yaw, const angle<T>& roll);
template <class T> quat<T> to_quat (const angle<T>& ang, const vector<T, 3>& axis);

template <class T> void to_axis_angle   (const quat<T>& q, angle<T>& ang, vector<T, 3>& axis, const T& eps = T (0));
template <class T> void to_euler_angles (const quat<T>& q, angle<T>& pitch, angle<T>& yaw, angle<T>& roll);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Афинные преобразования
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> matrix<T, 4> translate (const vector<T, 3>&);
template <class T> matrix<T, 4> scale     (const vector<T, 3>&);
template <class T> matrix<T, 4> rotate    (const angle<T>& ang, const vector<T, 3>& axis);
template <class T> matrix<T, 4> lookat    (const vector<T, 3>& pos, const vector<T, 3>& center, const vector<T, 3>& up);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Угол между векторами
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T, unsigned int Size>
angle<T> find_angle (const vector<T, Size>& a, const vector<T, Size>& b, const T& eps = T (0));

#include <math/detail/utility.inl>

}

#endif
