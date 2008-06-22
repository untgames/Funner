template <class T> 
void quat_add (quat<T>& res,const quat<T>& a,const quat<T>& b)
{
  for (int i=0;i<4;i++) res [i] = a [i] + b [i];
}

template <class T> 
void quat_sub (quat<T>& res,const quat<T>& a,const quat<T>& b)
{
  for (int i=0;i<4;i++) res [i] = a [i] - b [i];
}

template <class T> 
inline void quat_mul (quat<T>& res,const quat<T>& a,const quat<T>& b)
{
  res.x = a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y;
  res.y = a.w*b.y + a.y*b.w + a.z*b.x - a.x*b.z;
  res.z = a.w*b.z + a.z*b.w + a.x*b.y - a.y*b.x;
  res.w = a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z;
}

template <class T> 
inline void quat_mul (quat<T>& a,const quat<T>& b)
{
  quat_mul (a,quat<T> (a),b);
}

template <class T> 
void quat_mul_scalar (quat<T>& res,const quat<T>& a,const T& b)
{
  for (int i=0;i<4;i++) res [i] = a [i] * b;
}

template <class T> 
inline void quat_div_scalar (quat<T>& res,const quat<T>& a,const T& b)
{
  quat_mul_scalar (res,a,T(1)/b);
}

template <class T> 
inline void quat_copy (quat<T>& res,const quat<T>& src)
{
  for (int i=0;i<4;i++) res [i] = src [i]; 
}

template <class T> 
void quat_assign_scalar (quat<T>& res,const T& value)
{
  for (int i=0;i<4;i++) res [i] = value;
}

template <class T> 
void quat_neg (quat<T>& res,const quat<T>& src)
{
  for (int i=0;i<4;i++) res [i] = -src [i]; 
}

template <class T> 
T quat_inner (const quat<T>& a,const quat<T>& b)
{
  T result = T (0);

  for (int i=0;i<4;i++)
    result += a [i] * b [i];

  return result;  
}

template <class T> 
inline T quat_length (const quat<T>& v)
{
  return sqrt (quat_norm (v));    
}

template <>
inline float quat_length (const quat<float>& v)
{
  return sqrtf (quat_norm (v));
}

#ifdef _MSC_VER

template <>
inline long double quat_length (const quat<long double>& v)
{
  return sqrtl (quat_norm (v));
}

#endif

template <class T> 
inline T quat_norm (const quat<T>& v)
{
  return quat_inner (v,v);
}

template <class T> 
inline void quat_normalize (quat<T>& res,const quat<T>& src)
{
  quat_div_scalar (res,src,quat_length (src));
}

template <class T> 
bool quat_equal (const quat<T>& a,const quat<T>& b)
{
  for (int i=0;i<4;i++)
    if (a [i] != b [i])
      return false;

  return true;
}

template <class T> 
bool quat_nequal (const quat<T>& a,const quat<T>& b)
{
  for (int i=0;i<4;i++)
    if (a [i] != b [i])
      return true;

  return false;
}

template <class T> 
bool quat_equal (const quat<T>& a,const quat<T>& b,const T& eps)
{
  for (int i=0;i<4;i++)
  {
    T t = a [i] - b [i];

    if (t > eps || t < -eps)
      return false;  
  }    

  return true;  
}

template <class T> 
void quat_mul_vec (vec<T,3>& res,const quat<T>& q,const vec<T,3>& v)
{
  T x = v.x * q.w + v.z * q.y - v.y * q.z,
    y = v.y * q.w + v.x * q.z - v.z * q.x,
    z = v.z * q.w + v.y * q.x - v.x * q.y,
    w = v.x * q.x + v.y * q.y + v.z * q.z;

  res [0] = q.w * x + q.x * w + q.y * z - q.z * y;
  res [1] = q.w * y + q.y * w + q.z * x - q.x * z;
  res [2] = q.w * z + q.z * w + q.x * y - q.y * x;

  T _norm=quat_norm (q);
  res=make_binary_operation<vec<T,3> >(res,_norm,divides<T,T,T> ());
}

template <class T>
void  quat_mul_vec (vec<T,4>& res,const quat<T>& q,const vec<T,4>& v)
{
  T x = v.x * q.w + v.z * q.y - v.y * q.z,
    y = v.y * q.w + v.x * q.z - v.z * q.x,
    z = v.z * q.w + v.y * q.x - v.x * q.y,
    w = v.x * q.x + v.y * q.y + v.z * q.z;

  res [0] = q.w * x + q.x * w + q.y * z - q.z * y;
  res [1] = q.w * y + q.y * w + q.z * x - q.x * z;
  res [2] = q.w * z + q.z * w + q.x * y - q.y * x;
  res [3] = 0;

  T _norm=quat_norm (q);
//component_fn<divides<T,T,T> > () (res,res,_norm);
  res=make_binary_operation<vec<T,3> >(res,_norm,divides<T,T,T> ());
}

template <class T>
void quat_invert (quat<T>& res, const quat<T>& src)
{
  T norm = quat_norm (src);
  
  if (norm > T (0))
  {
    T inv_norm = T (1) / norm;
    
    res.w =  src.w * inv_norm;
    res.x = -src.x * inv_norm;
    res.y = -src.y * inv_norm;
    res.z = -src.z * inv_norm;
  }
  else res.x = res.y = res.z = res.w = T (0);
}
