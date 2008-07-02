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
  eval (*this,a,b);
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
  *this=make_unary_operation<quat<T> > (a,assign<T> ());
}

template <class T>
inline quat<T>::quat (const matrix<T,3>& m)
{
  matrix2quat (m,*this);
}

template <class T>
inline quat<T>::quat (const matrix<T,4>& m)
{
  matrix2quat (m,*this);
}

template <class T>  
inline quat<T>& quat<T>::operator = (const matrix<T,3>& m)
{
  matrix2quat (m,*this);
  return *this;
}

template <class T>  
inline quat<T>& quat<T>::operator = (const matrix<T,4>& m)
{
  matrix2quat (m,*this);
  return *this;
}

template <class T>
inline quat<T>& quat<T>::operator = (const quat_base<T>& a)
{
  quat_base<T>::operator = (a);
  return *this;
}

template <class T>
inline quat<T>& quat<T>::operator = (const T& a)
{
  quat_assign_scalar (*this,a);
  return *this;
}

template <class T>
inline T quat<T>::operator ~ () const
{
  return quat_length (*this);
}

template <class T>
inline const quat<T> quat<T>::operator - () const
{
  return quat (*this,quat_neg<T>);
}

template <class T>
inline quat<T>& quat<T>::operator += (const quat<T>& q)
{
  quat_add (*this,*this,q);
  return *this;
}

template <class T>
inline quat<T>& quat<T>::operator -= (const quat<T>& q)
{
  quat_sub (*this,*this,q);
  return *this;
}

template <class T>
inline quat<T>& quat<T>::operator *= (const T& a)
{
  quat_mul_scalar (*this,*this,a);
  return *this;
}

template <class T>
inline quat<T>& quat<T>::operator /= (const T& a)
{
  quat_div_scalar (*this,*this,T(1)/a);
  return *this;
}

template <class T>  
inline quat<T>& quat<T>::operator *= (const quat<T>& q)
{  
  quat_mul (*this,q);
  return *this;
}

template <class T>
inline const quat<T> quat<T>::operator + (const quat<T>& q) const
{
  return quat (*this,q,quat_add<T>);
}

template <class T>
inline const quat<T> quat<T>::operator - (const quat<T>& q) const
{
  return quat (*this,q,quat_sub<T>);
}

template <class T>
inline const quat<T> quat<T>::operator * (const quat<T>& q) const
{
  return quat (*this,q,quat_mul<T>);
}

template <class T> 
inline const quat<T> quat<T>::operator * (const T& a) const
{
  return quat (*this,a,quat_mul_scalar<T>);
}

template <class T> 
inline const quat<T> quat<T>::operator / (const T& a) const
{
  return quat (*this,a,quat_div_scalar<T>);
}

template <class T>  
inline bool quat<T>::operator == (const quat<T>& q) const
{
  return quat_equal (*this,q);
}

template <class T>  
inline bool quat<T>::operator != (const quat<T>& q) const
{
  return quat_nequal (*this,q);
}

template <class T>
inline void trick_quat_mul_vec3 (vec<T,3>& res,const quat<T>& q,const vec<T,3>& v)
{
  quat_mul_vec (res,q,v);
}

template <class T>
inline void trick_quat_mul_vec4 (vec<T,4>& res,const quat<T>& q,const vec<T,4>& v)
{
  quat_mul_vec (res,q,v);
}

template <class T>
inline const vec<T,3> quat<T>::operator * (const vec<T,3>& v) const
{
  return vec<T,3> (*this,v,trick_quat_mul_vec3<T>);  
} 

template <class T>
inline const vec<T,4> quat<T>::operator * (const vec<T,4>& v) const
{
  return vec<T,4> (*this,v,trick_quat_mul_vec4<T>);  
}

template <class T>
inline T length (const quat<T>& q)
{
  return quat_length (q);
}

template <class T>
inline T norm (const quat<T>& q)
{
  return quat_norm (q);
}

template <class T>
inline quat<T> normalize (const quat<T>& q)
{
  return quat<T> (q,quat_normalize<T>);
}

template <class T> 
inline bool equal (const quat<T>& a,const quat<T>& b,const T& eps)
{
  return quat_equal (a,b,eps);  
}

template <class T>
inline T inner (const quat<T>& a,const quat<T>& b)
{
  return quat_inner (a,b);
}

template <class T>
inline quat<T> invert (const quat<T>& q)
{
  return quat<T> (q,quat_invert<T>);
}
