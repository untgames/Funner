template <class T>
void trick_matrix4_translate3 (matrix<T,4>& m,const vec<T,3>& v)
{
  matrix_translate (m,v);
}

template <class T> 
const matrix<T,4> translate (const vec<T,3>& v)
{
  return matrix<T,4> (v,trick_matrix4_translate3<T>);
}

template <class T> 
const matrix<T,4> translate (const T& x,const T& y,const T& z)
{
  return translate (vec<T,3>(x,y,z));
}

template <class T>
void trick_matrix4_scale3 (matrix<T,4>& m,const vec<T,3>& v)
{
  matrix_scale (m,v);
}

template <class T> 
const matrix<T,4> scale (const vec<T,3>& v)
{
  return matrix<T,4> (v,trick_matrix4_scale3<T>);
}

template <class T> 
const matrix<T,4> scale (const T& x,const T& y,const T& z)
{
  return scale (vec<T,3>(x,y,z));  
}

template <class T>
void trick_matrix4_rotate3 (matrix<T,4>& m,const T& angle,const vec<T,3>& v)
{
  matrix_rotate (m,angle,v);
}

template <class T> 
const matrix<T,4> rotate (const T& angle,const vec<T,3>& axis)
{
  return matrix<T,4> (angle,axis,trick_matrix4_rotate3<T>);
}

template <class T> 
const matrix<T,4> rotate (const T& angle,const T& x,const T& y,const T& z)
{
  return rotate (angle,vec<T,3>(x,y,z));
}

inline const matrix<float,4>  translatef (float x,float y,float z)
{
  return translate (x,y,z);
}

inline const matrix<double,4> translated (double x,double y,double z)
{
  return translate (x,y,z);
}

inline const matrix<float,4> scalef (float x,float y,float z)
{
  return scale (x,y,z);
}

inline const matrix<double,4> scaled (double x,double y,double z)
{
  return scale (x,y,z);
}

inline const matrix<float,4> rotatef (float angle,const vec<float,3>& axis)
{
  return rotate (angle,axis);
}

inline const matrix<float,4> rotatef (float angle,float x,float y,float z)
{
  return rotate (angle,vec<float,3>(x,y,z));
}

inline const matrix<double,4> rotated (double angle,const vec<double,3>& axis)
{
  return rotate (angle,axis);
}

inline const matrix<double,4> rotated (double angle,double x,double y,double z)
{
  return rotate (angle,vec<double,3>(x,y,z));
}
