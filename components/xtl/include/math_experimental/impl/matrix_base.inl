template <class T,size_t size> 
inline void matrix_add (matrix<T,size>& res,const matrix<T,size>& a,const matrix<T,size>& b)
{
  for (size_t i=0;i<size;i++) res [i] = a [i] + b [i];  
}

template <class T,size_t size> 
inline void matrix_sub (matrix<T,size>& res,const matrix<T,size>& a,const matrix<T,size>& b)
{
  for (size_t i=0;i<size;i++) res [i] = a [i] - b [i];  
}

template <class T,size_t size> 
inline void matrix_mul_rr (matrix<T,size>& res,const matrix<T,size>& a,const matrix<T,size>& b)
{
  for (size_t i=0;i<size;i++)
    for (size_t j=0;j<size;j++)
      res [i][j] = dot (a.row (i),b.row (j));
}

template <class T,size_t size> 
inline void matrix_mul_cc (matrix<T,size>& res,const matrix<T,size>& a,const matrix<T,size>& b)
{
  for (size_t i=0;i<size;i++)
    for (size_t j=0;j<size;j++)
    {
      T r = 0;

      for (size_t k=0;k<size;k++)
        r += a [k][i]*b [k][j];

      res [i][j] = r;
    }  
}

template <class T,size_t size> 
inline void matrix_mul_rc (matrix<T,size>& res,const matrix<T,size>& a,const matrix<T,size>& b)
{
  for (size_t i=0;i<size;i++)
    for (size_t j=0;j<size;j++)
    {
      T r = 0;

      for (size_t k=0;k<size;k++)
        r += a [i][k] * b [k][j];

      res [i][j] = r;
    }  
}

template <class T,size_t size> 
inline void matrix_mul_cr (matrix<T,size>& res,const matrix<T,size>& a,const matrix<T,size>& b)
{
  for (size_t i=0;i<size;i++)
    for (size_t j=0;j<size;j++)
    {
      T r = 0;

      for (size_t k=0;k<size;k++)
        r += a [k][i] * b [k][j];

      res [i][j] = r;
    }  
}

template <class T,size_t size> 
inline void matrix_mul (matrix<T,size>& res,const matrix<T,size>& a,const matrix<T,size>& b)
{
  matrix_mul_rc (res,a,b);
}

template <class T,size_t size> 
inline void matrix_mul_rr (matrix<T,size>& a,const matrix<T,size>& b)
{
  for (size_t i=0;i<size;i++)
  {
    typename matrix<T,size>::vector tmp = a [i];

    for (size_t j=0;j<size;j++)
      a [i][j] = dot (tmp,b.row (j));
  }    
}

template <class T,size_t size> 
inline void matrix_mul_rc (matrix<T,size>& a,const matrix<T,size>& b)
{
  for (size_t i=0;i<size;i++)
  {
    typename matrix<T,size>::vector tmp = a [i];

    for (size_t j=0;j<size;j++)
    {
      T r = 0;

      for (size_t k=0;k<size;k++)
        r += tmp [k] * b [k][j];

      a [i][j] = r;
    }
  }    
}

template <class T,size_t size> 
inline void matrix_mul_cr (matrix<T,size>& a,const matrix<T,size>& b)
{
  matrix<T,size> tmp = transpose (a);

  for (size_t i=0;i<size;i++)
    for (size_t j=0;j<size;j++)
      a [i][j] = dot (tmp.row (i),b.row (j));
}

template <class T,size_t size> 
inline void matrix_mul_cc (matrix<T,size>& a,const matrix<T,size>& b)
{
  matrix<T,size> tmp = a;

  for (size_t i=0;i<size;i++)
  {
    for (size_t j=0;j<size;j++)
    {
      T r = 0;

      for (size_t k=0;k<size;k++)
        r += tmp [k][i] * b [k][j];

      a [i][j] = r;
    }
  }    
}

template <class T,size_t size> 
inline void matrix_mul (matrix<T,size>& a,const matrix<T,size>& b)
{
  matrix_mul_rc (a,b);  
}

template <class T,size_t size> 
inline void matrix_mul_scalar (matrix<T,size>& res,const matrix<T,size>& a,const T& b)
{
  for (size_t i=0;i<size;i++) res [i] = a [i] * b;  
}

template <class T,size_t size> 
inline void matrix_div_scalar (matrix<T,size>& res,const matrix<T,size>& a,const T& b)
{
  matrix_mul_scalar (res,a,T(1)/b);
}

template <class T,size_t size> 
inline void matrix_copy (matrix<T,size>& res,const matrix<T,size>& src)
{
  for (size_t i=0;i<size;i++) res [i] = src [i];
}

template <class T,size_t size> 
inline void matrix_assign_scalar (matrix<T,size>& res,const T& value)
{
  for (size_t i=0;i<size;i++) 
  {
    res [i][i] = value;

    for (size_t j=i+1;j<size;j++)
      res [i][j] = res [j][i] = 0;
  }  
}

template <class T,size_t size> 
bool matrix_equal (const matrix<T,size>& a,const matrix<T,size>& b)
{
  for (size_t i=0;i<size;i++)
    if (a [i] != b [i])
      return false;

  return true;    
}

template <class T,size_t size> 
bool matrix_nequal (const matrix<T,size>& a,const matrix<T,size>& b)
{
  for (size_t i=0;i<size;i++)
    if (a [i] != b [i])
      return true;

  return false;
}

template <class T,size_t size> 
bool matrix_equal (const matrix<T,size>& a,const matrix<T,size>& b,const T& eps)
{
  for (size_t i=0;i<size;i++)
    if (!equal (a [i],b [i],eps))
      return false;

  return true;  
}

template <class T,size_t size> 
inline void matrix_neg (matrix<T,size>& res,const matrix<T,size>& src)
{
  for (size_t i=0;i<size;i++) res [i] = -src [i];
}

template <class T,size_t size> 
T matrix_det (const matrix<T,size>& m)
{
  T res = 0;
  size_t i;

  for (i=0;i<size;i+=2) res += m [0][i] * minor (m,0,i);
  for (i=1;i<size;i+=2)     res -= m [0][i] * minor (m,0,i);

  return res;
}

template <class T>
T matrix_det (const matrix<T,1>& m) 
{ 
  return m [0][0];
}

template <class T>
T matrix_det (const matrix<T,2>& m) 
{ 
  return m [0][0]*m[1][1]-m[0][1]*m[1][0]; 
}

template <class T>
T matrix_det (const matrix<T,3>& m) 
{ 
  return m[0][0]*m[1][1]*m[2][2] + m[0][1]*m[1][2]*m[2][0] + m[0][2]*m[1][0]*m[2][1] -
         m[2][0]*m[1][1]*m[0][2] - m[2][1]*m[1][2]*m[0][0] - m[2][2]*m[1][0]*m[0][1]; 
}

template <class T,size_t size> 
T matrix_minor (const matrix<T,size>& src,size_t m,size_t n)
{
  matrix<T,size-1> tmp;
  size_t i,j;

  for (i=0;i<m;i++)
  {
    for (j=0;j<n;j++)    tmp [i][j]   = src [i][j];
    for (j++;j<size;j++) tmp [i][j-1] = src [i][j];
  }      

  for (i++;i<size;i++)
  {
    for (j=0;j<n;j++)    tmp [i-1][j]   = src [i][j];
    for (j++;j<size;j++) tmp [i-1][j-1] = src [i][j];
  }

  return matrix_det (tmp);
}

template <class T> 
T matrix_minor (const matrix<T,2>& src,size_t m,size_t n) 
{
  return src [(m+1)&1][(n+1)&1];
} 

template <class T> 
T matrix_minor (const matrix<T,3>& src,size_t m,size_t n) 
{
  return (m+n)&1 ? src [(m+2)%3][(n+1)%3]*src [(m+1)%3][(n+2)%3] -
                   src [(m+2)%3][(n+2)%3]*src [(m+1)%3][(n+1)%3]
                 : src [(m+2)%3][(n+2)%3]*src [(m+1)%3][(n+1)%3] -
                   src [(m+2)%3][(n+1)%3]*src [(m+1)%3][(n+2)%3];
} 

template <class T,size_t size> 
inline void matrix_transpose (matrix<T,size>& dst,const matrix<T,size>& src)
{
  for (size_t i=0;i<size;i++)
    for (size_t j=0;j<size;j++)
      dst [i][j] = src [j][i];
}

template <class T,size_t size> 
inline void matrix_transpose (matrix<T,size>& m)
{
  for (size_t i=1;i<size;i++)
    for (size_t j=i;j<size;j++)
    {
      T    t     = m [i-1][j];
      m [i-1][j] = m [j][i-1];
      m [j][i-1] = t;
    }
}

template <class T,size_t size> 
inline void matrix_invert (matrix<T,size>& res,const matrix<T,size>& src)
{
  size_t i,j;

  for (i=0;i<size;i+=2)
  {
    for (j=0;j<size;j+=2)  res [j][i] = minor (src,i,j);
    for (j=1;j<size;j+=2)  res [j][i] = -minor (src,i,j);
  }  

  for (i=1;i<size;i+=2)
  {
    for (j=0;j<size;j+=2)  res [j][i] = -minor (src,i,j);
    for (j=1;j<size;j+=2)  res [j][i] = minor (src,i,j);
  }  

  res /= det (src);
}

template <class T>
inline void matrix_invert (matrix<T,2>& dst,const matrix<T,2>& src)
{
  T k = T (1) / det (src);

  dst [0][0] = k  * src [1][1];
  dst [0][1] = k  * src [0][1];
  dst [1][0] = -k * src [1][0];
  dst [1][1] = -k * src [0][0];
} 

template <class T>
inline void matrix_invert (matrix<T,3>& dst,const matrix<T,3>& src)
{
  T k = T (1) / det (src);

  dst [0][0] = k * (src [2][2]*src [1][1]-src [2][1]*src [1][2]);
  dst [0][1] = k * (src [0][2]*src [2][1]-src [0][1]*src [2][2]);
  dst [0][2] = k * (src [1][2]*src [0][1]-src [1][1]*src [0][2]);
  dst [1][0] = k * (src [2][0]*src [1][2]-src [2][2]*src [1][0]);
  dst [1][1] = k * (src [0][0]*src [2][2]-src [0][2]*src [2][0]);
  dst [1][2] = k * (src [1][0]*src [0][2]-src [1][2]*src [0][0]);
  dst [2][0] = k * (src [2][1]*src [1][0]-src [2][0]*src [1][1]);
  dst [2][1] = k * (src [0][1]*src [2][0]-src [0][0]*src [2][1]);
  dst [2][2] = k * (src [1][1]*src [0][0]-src [1][0]*src [0][1]);
} 

template <class T,size_t size> 
inline void matrix_invert (matrix<T,size>& m)
{
  matrix<T,size> tmp;
  matrix_invert (m,tmp);
  m = tmp;
}

template <class T,size_t size> 
inline void matrix_normalize (matrix<T,size>& res,const matrix<T,size>& src)
{
  matrix_div_scalar (res,src,sqrt (det (src)));
}

template <class T,size_t size> 
inline void matrix_normalize (matrix<T,size>& m)
{
  matrix_normalize (m,m);
}

template <size_t size>
inline void matrix_normalize (matrix<float,size>& res,const matrix<float,size>& src)
{
  matrix_div_scalar (res,src,sqrtf (det (src)));
}

template <size_t size>
inline void matrix_normalize (matrix<long double,size>& res,const matrix<long double,size>& src)
{
  matrix_div_scalar (res,src,sqrtd (det (src)));
}

template <class T,size_t size>
inline void matrix_mul_vec (vec<T,size>& res,const matrix<T,size>& m,const vec<T,size>& v)
{
  for (size_t i=0;i<size;i++) res [i] = dot (m [i],v);
}

template <class T,size_t size>
inline void matrix_mul_vec (vec<T,size>& res,const matrix<T,size+1>& m,const vec<T,size>& v)
{
  for (size_t i=0;i<size;i++) res [i] = dot ((vec<T,size>&)m [i],v) + m [i][size];
}

template <class T,size_t size>
inline void vec_mul_matrix (vec<T,size>& res,const vec<T,size>& v,const matrix<T,size>& m)
{
  for (size_t i=0;i<size;i++) 
  {
    T r = 0;

    for (size_t j=0;j<size;j++)
      r += m [j][i] * v [j];

    res [i] = r;
  }    
}

template <class T,size_t size>
inline void vec_mul_matrix (vec<T,size>& res,const vec<T,size>& v,const matrix<T,size+1>& m)
{
  for (size_t i=0;i<size;i++) 
  {
    T r = 0;

    for (size_t j=0;j<size;j++)
      r += m [j][i] * v [j]; 

    res [i] = r + m [size][i];
  }    
}
