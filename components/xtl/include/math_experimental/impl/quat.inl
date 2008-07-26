namespace detail
{
  template <class T> const T& get_component(const quat<T>& q,size_t i)
  {
    return q[i];
  }
}

/*
	Конструкторы
*/

template <class T>
inline quat<T>::quat ()
{
  x = T (0);
  y = T (0);
  z = T (0);
  w = T (1);
}

template <class T> template <class T1,class Fn>
inline quat<T>::quat (const T1& a,Fn fn)
{
  fn (*this,a);
}

template <class T> template <class T1,class T2,class Fn> 
inline quat<T>::quat (const T1& a,const T2& b,Fn fn)
{
  fn (*this,a,b);
}



template <class T> template <class T1>
inline quat<T>::quat (const T1& a,void (*eval)(quat&,const T1&))
{
  eval (*this,a);
}

template <class T> template <class T1,class T2> 
inline quat<T>::quat (const T1& a,const T2& b,void (*eval)(quat&,const T1&,const T2&))
{
  eval (*this,a,b);
}

template <class T> template <class T1,class T2,class T3> 
inline quat<T>::quat (const T1& a,const T2& b,const T3& c,void (*eval)(quat&,const T1&,const T2&,const T3&))
{
  eval (*this,a,b,c);
}

template <class T> 
inline quat<T>::quat (const T& x_,const T& y_,const T& z_,const T& w_)
{  
  x = x_;
  y = y_;
  z = z_;
  w = w_;
}

template <class T>
inline quat<T>::quat (const T& a)
{
  make_unary_operation<quat<T> > (a,assign<T> (),*this);
}

/*
	Присваивание
*/

template <class T>
inline quat<T>& quat<T>::operator = (const quat_base<T>& a)
{
  quat_base<T>::operator = (a);
  return *this;
}

template <class T>
inline quat<T>& quat<T>::operator = (const T& a)
{
  make_unary_operation<quat<T> > (a,assign<T>(),*this);
  return *this;
}

/*
	Унарные операции, длина
*/

template <class T>
inline T quat<T>::squared_length () const
{
  return binary_accumulation_function<T> () (*this,*this,T(0),plus<T> (),multiplies<T> ());
}

template <class T>
inline T quat<T>::length () const
{
  return sqrt (squared_length());
}

template <class T>
inline const quat<T> quat<T>::operator - () const
{
  return make_unary_operation(*this,negate<T> ());
}

/*
	Бинарные операции
*/

template <class T>
inline quat<T>& quat<T>::operator += (const quat<T>& q)
{
  make_binary_operation<quat<T> > (*this,q,plus<T>(),*this);
  return *this;
}

template <class T>
inline quat<T>& quat<T>::operator -= (const quat<T>& q)
{
  make_binary_operation<quat<T> > (*this,q,minus<T>(),*this);
  return *this;
}

template <class T>
inline quat<T>& quat<T>::operator *= (const T& a)
{
  make_binary_operation<quat<T> > (*this,a,multiplies<T>(),*this);
  return *this;
}

template <class T>
inline quat<T>& quat<T>::operator /= (const T& a)
{
  make_binary_operation<quat<T> > (*this,a,divides<T>(),*this);
  return *this;
}

template <class T>  
inline quat<T>& quat<T>::operator *= (const quat<T>& q)
{  
  quat<T> res;
  res.x = (*this).w*q.x + (*this).x*q.w + (*this).y*q.z - (*this).z*q.y;
  res.y = (*this).w*q.y + (*this).y*q.w + (*this).z*q.x - (*this).x*q.z;
  res.z = (*this).w*q.z + (*this).z*q.w + (*this).x*q.y - (*this).y*q.x;
  res.w = (*this).w*q.w - (*this).x*q.x - (*this).y*q.y - (*this).z*q.z;
  *this=res;
  return *this;
}

template <class T>
inline const quat<T> quat<T>::operator + (const quat<T>& q) const
{
  return make_binary_operation<quat<T> > (*this,q,plus<T>());
}

template <class T>
inline const quat<T> quat<T>::operator - (const quat<T>& q) const
{
  return make_binary_operation<quat<T> > (*this,q,minus<T>());
}

template <class T>
inline const quat<T> quat<T>::operator * (const quat<T>& q) const
{
  return quat<T>(*this)*=q;
}

template <class T> 
inline const quat<T> quat<T>::operator * (const T& a) const
{
  return make_binary_operation<quat<T> > (*this,a,multiplies<T>());
}

template <class T> 
inline const quat<T> quat<T>::operator / (const T& a) const
{
  return make_binary_operation<quat<T> > (*this,a,divides<T>());
}

template <class T>  
inline bool quat<T>::operator == (const quat<T>& q) const
{
  return compare_function<T,equal_to<T> > () (*this,q,T(0));
}

template <class T>  
inline bool quat<T>::operator != (const quat<T>& q) const
{
  return compare_function<T,not_equal_to<T> > () (*this,q,T(0));
}

template <class T>
inline const vec<T,3> quat<T>::operator * (const vec<T,3>& v) const
{
  T x_ = v.x * (*this).w + v.z * (*this).y - v.y * (*this).z,
    y_ = v.y * (*this).w + v.x * (*this).z - v.z * (*this).x,
    z_ = v.z * (*this).w + v.y * (*this).x - v.x * (*this).y,
    w_ = v.x * (*this).x + v.y * (*this).y + v.z * (*this).z;
  quat<T> res;
  res [0] = (*this).w * x_ + (*this).x * w_ + (*this).y * z_ - (*this).z * y_;
  res [1] = (*this).w * y_ + (*this).y * w_ + (*this).z * x_ - (*this).x * z_;
  res [2] = (*this).w * z_ + (*this).z * w_ + (*this).x * y_ - (*this).y * x_;

  T norm_=squared_length();
  return make_binary_operation<vec<T,3> >(res,norm_,divides<T,T,T> ());
} 

template <class T>
inline const vec<T,4> quat<T>::operator * (const vec<T,4>& v) const
{
  
  T x_ = v.x * (*this).w + v.z * (*this).y - v.y * (*this).z,
    y_ = v.y * (*this).w + v.x * (*this).z - v.z * (*this).x,
    z_ = v.z * (*this).w + v.y * (*this).x - v.x * (*this).y,
    w_ = v.x * (*this).x + v.y * (*this).y + v.z * (*this).z;

  vec<T,4> res;
  res [0] = (*this).w * x_ + (*this).x * w_ + (*this).y * z_ - (*this).z * y_;
  res [1] = (*this).w * y_ + (*this).y * w_ + (*this).z * x_ - (*this).x * z_;
  res [2] = (*this).w * z_ + (*this).z * w_ + (*this).x * y_ - (*this).y * x_;
  res [3] = 0;

  T norm_=squared_length();
  return make_binary_operation<vec<T,4> >(res,norm_,divides<T,T,T> ());
}

template<class T>
inline const T quat<T>::operator& (const quat<T>& b) const
{
  return binary_accumulation_function<T> () (*this,b,T(0),plus<T>(),multiplies<T>());
}

template <class T>
inline quat<T> normalize (const quat<T>& q)
{
  return make_binary_operation<quat<T> > (q,q.length(),divides<T>());
}

/*
	Утилиты
*/

template <class type>
const quat<type> invert (const quat<type>& src)
{
  quat<T> res;
  T norm = normalize (src);
  
  if (norm > T (0))
  {
    T inv_norm = T (1) / norm;
    
    res.w =  src.w * inv_norm;
    res.x = -src.x * inv_norm;
    res.y = -src.y * inv_norm;
    res.z = -src.z * inv_norm;
  }
  else res.x = res.y = res.z = res.w = T (0);
  return res;
}

template <class T> 
inline const matrix<T,4> quat_to_matrix (const quat<T>& q)
{
  matrix<T,4> m;
  T wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
  T s = T(2) / q.squared_length();
 
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
  return m;
}

template <class T> 
inline const matrix<T,3> quat_to_matrix (const quat<T>& q)
{
  matrix<T,3> res;
  T wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
  T s = T(2) / q.squared_length();
 
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
  return res;
}

template <class T> inline void quat_to_axis_angle  (const quat<T>& q,T& angle,vec<T,4>& axis,const T& eps)
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

template <class T> inline void quat_to_axis_angle  (const quat<T>& q,T& angle,vec<T,3>& axis,const T& eps)
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
template <class T> inline void quat_to_euler_angle (const quat<T>& q,T& pitch,T& yaw,T& roll)
{
  T w2 = q.w * q.w,
    x2 = q.x * q.x,
    y2 = q.y * q.y,
    z2 = q.z * q.z;

  pitch = atan (T(2) * (q.y*q.z + q.x*q.w)/(-x2 - y2 + z2 + w2));
  yaw   = asin (T(-2) * (q.x*q.z - q.y*q.w));
  roll  = atan (T(2) * (q.x*q.y + q.z*q.w)/(x2 - y2 - z2 + w2)); 
}