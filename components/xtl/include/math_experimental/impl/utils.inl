inline float deg2rad (float angle) 
{ 
  return angle * float (M_PI) / 180.0f; 
}

inline float rad2deg (float angle) 
{ 
  return angle * 180.0f / float (M_PI); 
}

inline double deg2rad (double angle) 
{ 
  return angle * double (M_PI) / 180.0; 
}

inline double rad2deg (double angle) 
{ 
  return angle * 180.0 / double (M_PI); 
}

inline long double deg2rad (long double angle) 
{ 
  return angle * (long double)(M_PI) / 180.0L; 
}

inline long double rad2deg (long double angle) 
{ 
  return angle * 180.0L / (long double)(M_PI); 
}

template <class T> 
void quat2matrix (const quat<T>& q,matrix<T,4>& m)
{
  T wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
  T s = T(2) / norm (q);
 
  x2 = q.x * s;    y2 = q.y * s;    z2 = q.z * s;
  xx = q.x * x2;   xy = q.x * y2;   xz = q.x * z2;
  yy = q.y * y2;   yz = q.y * z2;   zz = q.z * z2;
  wx = q.w * x2;   wy = q.w * y2;   wz = q.w * z2;

  m [0][0] = T(1) - (yy + zz);
  m [1][0] = xy - wz;
  m [2][0] = xz + wy;

  m [0][1] = xy + wz;
  m [1][1] = T(1) - (xx + zz);
  m [2][1] = yz - wx;

  m [0][2] = xz - wy;
  m [1][2] = yz + wx;
  m [2][2] = T(1) - (xx + yy);

  m [0][3] = m [1][3] = m [2][3] = 0;
  m [3][0] = m [3][1] = m [3][2] = 0;
  m [3][3] = 1;  
}

template <class T> 
void quat2matrix (const quat<T>& q,matrix<T,3>& res)
{
  T wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
  T s = T(2) / norm (q);
 
  x2 = q.x * s;    y2 = q.y * s;    z2 = q.z * s;
  xx = q.x * x2;   xy = q.x * y2;   xz = q.x * z2;
  yy = q.y * y2;   yz = q.y * z2;   zz = q.z * z2;
  wx = q.w * x2;   wy = q.w * y2;   wz = q.w * z2;

  res [0][0] = T(1) - (yy + zz);
  res [1][0] = xy - wz;
  res [2][0] = xz + wy;

  res [0][1] = xy + wz;
  res [1][1] = T(1) - (xx + zz);
  res [2][1] = yz - wx;

  res [0][2] = xz - wy;
  res [1][2] = yz + wx;
  res [2][2] = T(1) - (xx + yy);
}

template <class T> 
void quat2AxisAngle (const quat<T>& q,T& angle,vec<T,4>& axis,const T& eps)
{
  T vl = sqrt (q.x*q.x + q.y*q.y + q.z*q.z);

  if (vl > eps)
  {
    T ivl = 1.0f/vl;
    
    axis [0] = q.x*ivl;
    axis [1] = q.y*ivl;
    axis [2] = q.z*ivl;
    axis [3] = 0;

    if (q.w < 0 ) angle = T(2) * atan2 (-vl,-q.w); //-PI,0 
    else          angle = T(2) * atan2 ( vl, q.w); //0,PI 
  }
  else
  {
    axis  = 0;
    angle = 0;
  }
}

template <class T> 
void quat2AxisAngle (const quat<T>& q,T& angle,vec<T,3>& axis,const T& eps)
{
  T vl = sqrt (q.x*q.x + q.y*q.y + q.z*q.z);

  if (vl > eps)
  {
    T ivl = 1.0f/vl;
    
    axis [0] = q.x*ivl;
    axis [1] = q.y*ivl;
    axis [2] = q.z*ivl;

    if (q.w < 0 ) angle = T(2) * atan2 (-vl,-q.w); //-PI,0 
    else          angle = T(2) * atan2 ( vl, q.w); //0,PI 
  }
  else
  {
    axis  = 0;
    angle = 0;
  }
}

template <class T> 
void quat2EulerAngle (const quat<T>& q,T& pitch,T& yaw,T& roll)
{
  T w2 = q.w * q.w,
    x2 = q.x * q.x,
    y2 = q.y * q.y,
    z2 = q.z * q.z;

  pitch = atan (T(2) * (q.y*q.z + q.x*q.w)/(-x2 - y2 + z2 + w2));
  yaw   = asin (T(-2) * (q.x*q.z - q.y*q.w));
  roll  = atan (T(2) * (q.x*q.y + q.z*q.w)/(x2 - y2 - z2 + w2)); 
}

template <class T> 
void matrix2quat (const matrix<T,4>& m,quat<T>& q)
{
  T tr = m [0][0] + m [1][1] + m [2][2];

  if (tr > 0)
  {
    T  s = sqrt (tr + T (1));
    q.w  = s * T (0.5);
    s    = T (0.5) / s;
    q.x  = (m [2][1] - m [1][2]) * s;
    q.y  = (m [0][2] - m [2][0]) * s;
    q.z  = (m [1][0] - m [0][1]) * s;
  }
  else
  {
    const size_t nxt [3] = {1, 2, 0};

    size_t i = 0;

    if (m [1][1] > m[0][0]) i = 1;
    if (m [2][2] > m[i][i]) i = 2;

    size_t j = nxt[i];
    size_t k = nxt[j];

    T s  = sqrt ((m [i][i] - (m [j][j] + m [k][k])) + T (1));

    q [i] = s * T (0.5);

    if (s != T (0)) s = T (0.5) / s;

    q [3] = (m [k][j] - m [j][k]) * s;
    q [j] = (m [j][i] + m [i][j]) * s;
    q [k] = (m [k][i] + m [i][k]) * s;
  }
}

template <class T> 
void matrix2quat (const matrix<T,3>& m,quat<T>& res)
{
  T tr = m [0][0] + m [1][1] + m [2][2];

  if (tr > 0)
  {
    T  s   = sqrt (tr + T (1));
    res.w  = s * T (0.5);
    s      = T (0.5) / s;
    res.x  = (m [2][1] - m [1][2]) * s;
    res.y  = (m [0][2] - m [2][0]) * s;
    res.z  = (m [1][0] - m [0][1]) * s;
  }
  else
  {
    const size_t nxt [3] = {1, 2, 0};

    size_t i = 0;

    if (m [1][1] > m[0][0]) i = 1;
    if (m [2][2] > m[i][i]) i = 2;

    size_t j = nxt[i];
    size_t k = nxt[j];

    T s  = sqrt ((m [i][i] - (m [j][j] + m [k][k])) + T (1));

    res [i] = s * T (0.5);

    if (s != T (0)) s = T (0.5) / s;

    res [3] = (m [k][j] - m [j][k]) * s;
    res [j] = (m [j][i] + m [i][j]) * s;
    res [k] = (m [k][i] + m [i][k]) * s;
  }
}

template <class T> 
void matrix_translate (matrix<T,4>& res,const vec<T,3>& v)
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
void matrix_scale (matrix<T,4>& res,const vec<T,3>& v)
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

template <class T> 
void matrix_scale (matrix<T,3>& res,const vec<T,3>& v)
{
  res [0][0] = v [0];
  res [1][1] = v [1];
  res [2][2] = v [2];
  res [0][1] = res [0][2] = 
  res [1][0] = res [1][2] = 
  res [2][0] = res [2][1] = 0;
}

template <class T> 
void matrix_rotate (matrix<T,4>& res,const T& angle,const vec<T,3>& axis)
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

template <class T> 
void matrix_rotate (matrix<T,3>& res,const T& angle,const vec<T,3>& axis)
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
void AxisAngle2quat  (const T& angle,const vec<T,3>& axis,quat<T>& q)
{
  vec<T,3> v = axis.normalize();

  T half_angle = angle*T(0.5);
  T sin_a      = sin (half_angle);

  q.x = v.x * sin_a;
  q.y = v.y * sin_a;
  q.z = v.z * sin_a;
  q.w = cos (half_angle);
}

template <class T> 
void EulerAngle2quat (const T& pitch,const T& yaw,const T& roll,quat<T>& q)
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

  quat_normalize (q,q);
}

template <class T>
inline void trick_EulerAngle2quat (quat<T>& q,const T& pitch,const T& yaw,const T& roll)
{
  EulerAngle2quat (pitch,yaw,roll,q);
}

template <class T>
inline void trick_AxisAngle2quat (quat<T>& q,const T& angle,const vec<T,3>& axis)
{
  AxisAngle2quat (angle,axis,q);
}

template <class T> 
inline const quat<T> fromEulerAngle (const T& pitch,const T& yaw,const T& roll)
{
  return quat<T> (pitch,yaw,roll,trick_EulerAngle2quat<T>);
}

template <class T> 
inline const quat<T> fromAxisAngle (const T& angle,const vec<T,3>& axis)
{
  return quat<T> (angle,axis,trick_AxisAngle2quat<T>);
}

inline const quat<float> fromEulerAnglef (float pitch,float yaw,float roll)
{
  return fromEulerAngle (pitch,yaw,roll);
}

inline const quat<double> fromEulerAngled (double pitch,double yaw,double roll)
{
  return fromEulerAngle (pitch,yaw,roll);
}

inline const quat<float> fromAxisAnglef  (float angle,float x,float y,float z)
{
  return fromAxisAngle (angle,vec<float,3>(x,y,z));
}

inline const quat<double>  fromAxisAngled  (double angle,double x,double y,double z)
{
  return fromAxisAngle (angle,vec<double,3>(x,y,z));
}

inline const matrix<float,4> lookatf (const vec<float,3>& pos,const vec<float,3>& center,const vec<float,3>& up)
{ 
  matrix<float,4> view;
     
  vec<float,3> z = vec<float,3> (pos - center).normalize(),
               y = up,
               x = vec<float,3> (y ^ z).normalize();

  y = vec<float,3> (z ^ x).normalize();
  
  view [0] = vec<float,4> (x,0.0f);
  view [1] = vec<float,4> (y,0.0f);
  view [2] = vec<float,4> (z,0.0f);
  view [3] = vec<float,4> (0.0f,0.0f,0.0f,1.0f);

  return invert (view * translate (-pos));
}
