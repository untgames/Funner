/*
	Преобразование градусы - радианы
*/

template <class T>
inline const T deg_to_rad (const T& angle)
{
  return angle * T (3.14159265358979323846) / T(180); 
}

template <class T>
inline const T rad_to_deg (const T& angle)
{
  return angle * T (180) /  T(3.14159265358979323846); 
}

/*
	Углы (Эйлера) - кватернион
*/

template <class T> 
inline const quat<T> axis_angle_to_quat  (const T& angle,const vec<T,3>& axis)
{
  quat<T> q;
  vec<T,3> v = normalize(axis);

  T half_angle = angle*T(0.5);
  T sin_a      = sin (half_angle);

  q.x = v.x * sin_a;
  q.y = v.y * sin_a;
  q.z = v.z * sin_a;
  q.w = cos (half_angle);
  return q;
}

template <class T> 
inline const quat<T> axis_angle_to_quat  (const T& angle,const T& axis_x,const T& axis_y,const T& axis_z)
{
  return axis_angle_to_quat(angle,vec<T,3>(axis_x,axis_y,axis_z));
}




template <class T> 
inline void axis_angle_to_quat  (const T& angle,const vec<T,3>& axis,quat<T> q)
{
  vec<T,3> v = normalize(axis);

  T half_angle = angle*T(0.5);
  T sin_a      = sin (half_angle);

  q.x = v.x * sin_a;
  q.y = v.y * sin_a;
  q.z = v.z * sin_a;
  q.w = cos (half_angle);
}

template <class T> 
inline const quat<T> euler_angle_to_quat (const T& pitch,const T& yaw,const T& roll)
{
  quat<T> q;
  T ex = pitch * T (0.5);
  T ey = yaw * T (0.5);
  T ez = roll * T (0.5);

  T cr = cos (ex);
  T cp = cos (ey);
  T cy = cos (ez);

  T sr = sin (ex);
  T sp = sin (ey);
  T sy = sin (ez);

  T cpcy = cp * cy;
  T spsy = sp * sy;

  q.w = cr * cpcy + sr * spsy;

  q.x = sr * cpcy - cr * spsy;
  q.y = cr * sp * cy + sr * cp * sy;
  q.z = cr * cp * sy - sr * sp * cy;


  return normalize (q);
}

template <class T> 
inline void euler_angle_to_quat (const T& pitch,const T& yaw,const T& roll,quat<T>& q)
{
  T ex = pitch * T (0.5);
  T ey = yaw * T (0.5);
  T ez = roll * T (0.5);

  T cr = cos (ex);
  T cp = cos (ey);
  T cy = cos (ez);

  T sr = sin (ex);
  T sp = sin (ey);
  T sy = sin (ez);

  T cpcy = cp * cy;
  T spsy = sp * sy;

  q.w = cr * cpcy + sr * spsy;

  q.x = sr * cpcy - cr * spsy;
  q.y = cr * sp * cy + sr * cp * sy;
  q.z = cr * cp * sy - sr * sp * cy;
  q=normalize(q);
}

/*
	Параллельный перенос
*/

template <class T> 
inline const matrix<T,4> matrix_translate (const vec<T,3>& v)
{
  matrix<T,4> res;
  res [0][3] = v [0];
  res [1][3] = v [1];
  res [2][3] = v [2];
  res [0][0] = res [1][1] = res [2][2] = res [3][3] = 1;
  res [0][1] = res [0][2] = res [1][0] = res [1][2] = 
  res [2][0] = res [2][1] = res [3][0] = res [3][1] =
  res [3][2] = 0;
  return res;
}

template <class T> 
inline const matrix<T,4> matrix_translate (const T& a,const T& b,const T& c)
{
  matrix<T,4> res;
  res [0][3] = a;
  res [1][3] = b;
  res [2][3] = c;
  res [0][0] = res [1][1] = res [2][2] = res [3][3] = 1;
  res [0][1] = res [0][2] = res [1][0] = res [1][2] = 
  res [2][0] = res [2][1] = res [3][0] = res [3][1] =
  res [3][2] = 0;
  return res;
}

template <class T> 
inline void matrix_translate (const vec<T,3>& v,matrix<T,4>& res)
{
  res [0][3] = v [0];
  res [1][3] = v [1];
  res [2][3] = v [2];
  res [0][0] = res [1][1] = res [2][2] = res [3][3] = 1;
  res [0][1] = res [0][2] = res [1][0] = res [1][2] = 
  res [2][0] = res [2][1] = res [3][0] = res [3][1] =
  res [3][2] = 0;
}

template <class T> 
inline void matrix_translate (const T& a,const T& b,const T& c,matrix<T,4>& res)
{
  res [0][3] = a;
  res [1][3] = b;
  res [2][3] = c;
  res [0][0] = res [1][1] = res [2][2] = res [3][3] = 1;
  res [0][1] = res [0][2] = res [1][0] = res [1][2] = 
  res [2][0] = res [2][1] = res [3][0] = res [3][1] =
  res [3][2] = 0;
}

/*
	Масштабирование
*/

template <class T> 
inline const matrix<T,4> matrix4_scale (const vec<T,3>& v)
{
  matrix<T,4> res;
  res [0][0] = v [0];
  res [1][1] = v [1];
  res [2][2] = v [2];
  res [3][3] = 1;
  res [0][1] = res [0][2] = res [0][3] = 
  res [1][0] = res [1][2] = res [1][3] =
  res [2][0] = res [2][1] = res [2][3] = 
  res [3][0] = res [3][1] = res [3][2] = 0;
  return res;
}

template <class T> 
inline const matrix<T,3> matrix3_scale (const vec<T,3>& v)
{
  matrix<T,3> res;
  res [0][0] = v [0];
  res [1][1] = v [1];
  res [2][2] = v [2];
  res [0][1] = res [0][2] = 
  res [1][0] = res [1][2] = 
  res [2][0] = res [2][1] = 0;
  return res;
}

template <class T> 
inline void matrix3_scale (const vec<T,3>& v,matrix<T,3>& res)
{

  res [0][0] = v [0];
  res [1][1] = v [1];
  res [2][2] = v [2];
  res [0][1] = res [0][2] = 
  res [1][0] = res [1][2] = 
  res [2][0] = res [2][1] = 0;
}

template <class T> 
inline void matrix4_scale (const vec<T,3>& v,matrix<T,4>& res)
{
  res [0][0] = v [0];
  res [1][1] = v [1];
  res [2][2] = v [2];
  res [3][3] = 1;
  res [0][1] = res [0][2] = res [0][3] = 
  res [1][0] = res [1][2] = res [1][3] =
  res [2][0] = res [2][1] = res [2][3] = 
  res [3][0] = res [3][1] = res [3][2] = 0;

}

/*
	Поворот вокруг оси
*/

template <class T> 
inline const matrix<T,4> matrix4_rotate (const T& angle,const vec<T,3>& axis)
{
  T cosine = cos (angle);
  T sine   = sin (angle);
  matrix<T,4> res;
  res [0][0] = axis [0]*axis [0]+(T(1)-axis [0]*axis [0])*cosine;
  res [1][0] = axis [0]*axis [1]*(T(1)-cosine)+axis [2]*sine;
  res [2][0] = axis [0]*axis [2]*(T(1)-cosine)-axis [1]*sine;
  res [3][0] = 0;

  res [0][1] = axis [0]*axis [1]*(T(1)-cosine)-axis [2]*sine;
  res [1][1] = axis [1]*axis [1]+(T(1)-axis [1]*axis [1])*cosine;
  res [2][1] = axis [1]*axis [2]*(T(1)-cosine)+axis [0]*sine;
  res [3][1] = 0;

  res [0][2] = axis [0]*axis [2]*(T(1)-cosine)+axis [1]*sine;
  res [1][2] = axis [1]*axis [2]*(T(1)-cosine)-axis [0]*sine;
  res [2][2] = axis [2]*axis [2]+(T(1)-axis [2]*axis [2])*cosine;
  res [3][2] = 0;

  res [0][3] = 0;
  res [1][3] = 0;
  res [2][3] = 0;
  res [3][3] = 1;
  return res;
}



template <class T> 
inline const matrix<T,3> matrix3_rotate (const T& angle,const vec<T,3>& axis)
{
  matrix<T,3> res;
  T cosine = cos (angle);
  T sine   = sin (angle);

  res [0][0] = axis [0]*axis [0]+(T(1)-axis [0]*axis [0])*cosine;
  res [1][0] = axis [0]*axis [1]*(T(1)-cosine)+axis [2]*sine;
  res [2][0] = axis [0]*axis [2]*(T(1)-cosine)-axis [1]*sine;

  res [0][1] = axis [0]*axis [1]*(T(1)-cosine)-axis [2]*sine;
  res [1][1] = axis [1]*axis [1]+(T(1)-axis [1]*axis [1])*cosine;
  res [2][1] = axis [1]*axis [2]*(T(1)-cosine)+axis [0]*sine;

  res [0][2] = axis [0]*axis [2]*(T(1)-cosine)+axis [1]*sine;
  res [1][2] = axis [1]*axis [2]*(T(1)-cosine)-axis [0]*sine;
  res [2][2] = axis [2]*axis [2]+(T(1)-axis [2]*axis [2])*cosine;
  return res;
}

template <class T> 
inline void matrix4_rotate (const T& angle,const vec<T,3>& axis,matrix<T,3>& res)
{
  T cosine = cos (angle);
  T sine   = sin (angle);

  res [0][0] = axis [0]*axis [0]+(T(1)-axis [0]*axis [0])*cosine;
  res [1][0] = axis [0]*axis [1]*(T(1)-cosine)+axis [2]*sine;
  res [2][0] = axis [0]*axis [2]*(T(1)-cosine)-axis [1]*sine;

  res [0][1] = axis [0]*axis [1]*(T(1)-cosine)-axis [2]*sine;
  res [1][1] = axis [1]*axis [1]+(T(1)-axis [1]*axis [1])*cosine;
  res [2][1] = axis [1]*axis [2]*(T(1)-cosine)+axis [0]*sine;

  res [0][2] = axis [0]*axis [2]*(T(1)-cosine)+axis [1]*sine;
  res [1][2] = axis [1]*axis [2]*(T(1)-cosine)-axis [0]*sine;
  res [2][2] = axis [2]*axis [2]+(T(1)-axis [2]*axis [2])*cosine;
}

template <class T> 
inline void matrix3_rotate (const T& angle,const vec<T,3>& axis,matrix<T,4>& res)
{
  T cosine = cos (angle);
  T sine   = sin (angle);

  res [0][0] = axis [0]*axis [0]+(T(1)-axis [0]*axis [0])*cosine;
  res [1][0] = axis [0]*axis [1]*(T(1)-cosine)+axis [2]*sine;
  res [2][0] = axis [0]*axis [2]*(T(1)-cosine)-axis [1]*sine;
  res [3][0] = 0;

  res [0][1] = axis [0]*axis [1]*(T(1)-cosine)-axis [2]*sine;
  res [1][1] = axis [1]*axis [1]+(T(1)-axis [1]*axis [1])*cosine;
  res [2][1] = axis [1]*axis [2]*(T(1)-cosine)+axis [0]*sine;
  res [3][1] = 0;

  res [0][2] = axis [0]*axis [2]*(T(1)-cosine)+axis [1]*sine;
  res [1][2] = axis [1]*axis [2]*(T(1)-cosine)-axis [0]*sine;
  res [2][2] = axis [2]*axis [2]+(T(1)-axis [2]*axis [2])*cosine;
  res [3][2] = 0;

  res [0][3] = 0;
  res [1][3] = 0;
  res [2][3] = 0;
  res [3][3] = 1;
}
