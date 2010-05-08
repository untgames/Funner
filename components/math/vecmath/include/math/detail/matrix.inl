namespace detail
{

/*
    Бинарные операции
*/

struct matrix_add {
  template <class T, unsigned int Size> 
  void operator () (const matrix<T, Size>& a, const matrix<T, Size>& b, matrix<T, Size>& res) const
  {
    for (unsigned int i=0; i<Size; i++)
      res [i] = a [i] + b [i];
  }
};

struct matrix_sub {
  template <class T, unsigned int Size> 
  void operator () (const matrix<T, Size>& a, const matrix<T, Size>& b, matrix<T, Size>& res) const
  {
    for (unsigned int i=0; i<Size; i++)
      res [i] = a [i] - b [i];
  }
};

struct matrix_mul {
  template <class T, unsigned int Size> 
  void operator () (const matrix<T, Size>& a, const matrix<T, Size>& b, matrix<T, Size>& res) const // res = a * b
  {
    for (unsigned int i=0; i<Size; i++)
      for (unsigned int j=0; j<Size; j++)
      {
        T r = 0;

        for (unsigned int k=0; k<Size; k++)
          r += a [i][k] * b [k][j];

        res [i][j] = r;
      }  
  }
  
  template <class T, unsigned int Size> 
  void operator () (matrix<T, Size>& a, const matrix<T, Size>& b) const // a *= b
  {
    for (unsigned int i=0; i<Size; i++)
    {
      typename matrix<T, Size>::vector tmp = a [i];

      for (unsigned int j=0; j<Size; j++)
      {
        T r = 0;

        for (unsigned int k=0; k<Size; k++)
          r += tmp [k] * b [k][j];

        a [i][j] = r;
      }
    }    
  }
  
#ifdef VECMATH_SSE
  __forceinline void operator () (const matrix<float, 4>& a, const matrix<float, 4>& b, matrix<float, 4>& res) const
  {
    matrix<float, 4> transposed_b = transpose (b);
  
    __m128 r0, r1, r2, r3, r4, r5, r6;

    for (unsigned int i=0; i<4; i++)
    {            
      r0 = _mm_mul_ps     (a [i].data (), transposed_b [0].data ());
      r1 = _mm_mul_ps     (a [i].data (), transposed_b [1].data ());
      r2 = _mm_mul_ps     (a [i].data (), transposed_b [2].data ());
      r3 = _mm_mul_ps     (a [i].data (), transposed_b [3].data ());

      r5 = r0;
      r5 = _mm_movelh_ps  (r5, r1);
      r1 = _mm_movehl_ps  (r1, r0);
      r5 = _mm_add_ps     (r5, r1);

      r4 = r2;
      r2 = _mm_movelh_ps  (r2, r3);
      r3 = _mm_movehl_ps  (r3, r4);
      r3 = _mm_add_ps     (r3, r2);

      r6 = r3;
      r6 = _mm_shuffle_ps (r5, r3, _MM_SHUFFLE (2, 0, 2, 0));
      r5 = _mm_shuffle_ps (r5, r3, _MM_SHUFFLE (3, 1, 3, 1));
      r6 = _mm_add_ps     (r6, r5);

      res [i].store (r6);
    }
  } 

  __forceinline void operator () (matrix<float, 4>& a, const matrix<float, 4>& b) const
  {
    matrix<float, 4> transposed_b = transpose (b);
    
    __m128 r0, r1, r2, r3, r4, r5, r6;

    for (unsigned int i=0; i<4; i++)
    {            
      r0 = _mm_mul_ps     (a [i].data (), transposed_b [0].data ());
      r1 = _mm_mul_ps     (a [i].data (), transposed_b [1].data ());
      r2 = _mm_mul_ps     (a [i].data (), transposed_b [2].data ());
      r3 = _mm_mul_ps     (a [i].data (), transposed_b [3].data ());

      r5 = r0;
      r5 = _mm_movelh_ps  (r5, r1);
      r1 = _mm_movehl_ps  (r1, r0);
      r5 = _mm_add_ps     (r5, r1);

      r4 = r2;
      r2 = _mm_movelh_ps  (r2, r3);
      r3 = _mm_movehl_ps  (r3, r4);
      r3 = _mm_add_ps     (r3, r2);

      r6 = r3;
      r6 = _mm_shuffle_ps (r5, r3, _MM_SHUFFLE (2, 0, 2, 0));
      r5 = _mm_shuffle_ps (r5, r3, _MM_SHUFFLE (3, 1, 3, 1));
      r6 = _mm_add_ps     (r6, r5);

      a [i].store (r6);
    }
  } 
#endif
};

struct matrix_mul_scalar {
  template <class T, unsigned int Size> 
  void operator () (const matrix<T, Size>& a, const T& b, matrix<T, Size>& res) const
  {
    for (unsigned int i=0; i<Size; i++)
      res [i] = a [i] * b;
  }
  
#ifdef VECMATH_SSE
  __forceinline void operator () (const matrix<float, 4>& a, float b, matrix<float, 4>& res) const
  {
    __m128 r = _mm_set_ps1 (b);

    for (unsigned int i=0; i<4; i++)
      res [i].store (_mm_mul_ps (a [i].data (), r));
  }
#endif
};

struct matrix_div_scalar {
  template <class T, unsigned int Size> 
  void operator () (const matrix<T, Size>& a, const T& b, matrix<T, Size>& res) const
  {
    matrix_mul_scalar ()(a, T (1) / b, res);
  }
};

struct matrix_mul_vec {
  template <class T, unsigned int Size>
  void operator () (const matrix<T, Size>& m, const vector<T, Size>& v, vector<T, Size>& res) const
  {
    for (unsigned int i=0; i<Size; i++)
      res [i] = dot (m [i], v);
  }

  template <class T, unsigned int Size>
  void operator () (const matrix<T, Size+1>& m, const vector<T, Size>& v, vector<T, Size>& res) const
  {
    for (unsigned int i=0; i<Size; i++)
      res [i] = dot ((vector<T, Size>&)m [i], v) + m [i][Size];
  }
};

struct vec_mul_matrix {
  template <class T, unsigned int Size>
  void operator () (const vector<T, Size>& v, const matrix<T, Size>& m, vector<T, Size>& res) const
  {
    for (unsigned int i=0; i<Size; i++) 
    {
      T r = 0;

      for (unsigned int j=0; j<Size; j++)
        r += m [j][i] * v [j];

      res [i] = r;
    }    
  }

  template <class T, unsigned int Size>
  void operator () (const vector<T, Size>& v, const matrix<T, Size+1>& m, vector<T, Size>& res) const
  {
    for (unsigned int i=0; i<Size; i++) 
    {
      T r = 0;

      for (unsigned int j=0; j<Size; j++)
        r += m [j][i] * v [j];

      res [i] = r + m [Size][i];
    }    
  }
};

/*
    Унарные операции
*/

struct matrix_copy {
  template <class T, unsigned int Size> 
  void operator () (const matrix<T, Size>& src, matrix<T, Size>& res) const
  {
    for (unsigned int i=0; i<Size; i++)
      res [i] = src [i];
  }
};

struct matrix_copy_column {
  template <class T, unsigned int Size>
  void operator () (const matrix<T, Size>& m, unsigned int column, vector<T, Size>& res) const
  {
    for (unsigned int i=0; i<Size; i++)
      res [i] = m [i][column];
  }
};

struct matrix_assign_scalar {
  template <class T, unsigned int Size> 
  void operator () (const T& value, matrix<T, Size>& res) const
  {
    for (unsigned int i=0; i<Size; i++) 
    {
      res [i][i] = value;

      for (unsigned int j=i+1; j<Size; j++)
        res [i][j] = res [j][i] = 0;
    }  
  }
  
#ifdef VECMATH_SSE
  __forceinline void operator () (float value, matrix<float, 4>& res) const
  {
    __m128 r = _mm_set_ps (0.0f, 0.0f, 0.0f, value);
    
    res [0].store (r);
    res [1].store (_mm_shuffle_ps (r, r, _MM_SHUFFLE (1, 1, 0, 1)));
    res [2].store (_mm_shuffle_ps (r, r, _MM_SHUFFLE (1, 0, 1, 1)));
    res [3].store (_mm_shuffle_ps (r, r, _MM_SHUFFLE (0, 1, 1, 1)));
  }
#endif
};

struct matrix_neg {
  template <class T, unsigned int Size> 
  void operator () (const matrix<T, Size>& src, matrix<T, Size>& res) const
  {
    for (unsigned int i=0; i<Size; i++)
      res [i] = -src [i];
  }
}; 

struct matrix_transpose {
  template <class T, unsigned int Size> 
  void operator () (const matrix<T, Size>& src, matrix<T, Size>& res) const
  {
    for (unsigned int i=0; i<Size; i++)
      for (unsigned int j=0; j<Size; j++)
        res [i][j] = src [j][i];
  }
  
#ifdef VECMATH_SSE
  __forceinline void operator () (const matrix<float, 4>& src, matrix<float, 4>& res) const
  {
    __m128 r3, r2, r1, r0;

    r0 = _mm_shuffle_ps (src [0].data (), src [1].data (), 0x44);
    r2 = _mm_shuffle_ps (src [0].data (), src [1].data (), 0xEE);
    r1 = _mm_shuffle_ps (src [2].data (), src [3].data (), 0x44);
    r3 = _mm_shuffle_ps (src [2].data (), src [3].data (), 0xEE);

    res [0].store (_mm_shuffle_ps (r0, r1, 0x88));
    res [1].store (_mm_shuffle_ps (r0, r1, 0xDD));
    res [2].store (_mm_shuffle_ps (r2, r3, 0x88));
    res [3].store (_mm_shuffle_ps (r2, r3, 0xDD));
  }
#endif
};

struct matrix_inverse {
  template <class T, unsigned int Size> 
  void operator () (const matrix<T, Size>& src, matrix<T, Size>& res) const
  {
    unsigned int i, j;

    for (i=0; i<Size; i+=2)
    {
      for (j=0; j<Size; j+=2) res [j][i] =  minor (src, i, j);
      for (j=1; j<Size; j+=2) res [j][i] = -minor (src, i, j);
    }  

    for (i=1; i<Size; i+=2)
    {
      for (j=0; j<Size; j+=2) res [j][i] = -minor (src, i, j);
      for (j=1; j<Size; j+=2) res [j][i] =  minor (src, i, j);
    }  

    res /= det (src);
  }

  template <class T>
  void operator () (const matrix<T, 2>& src, matrix<T, 2>& res) const
  {
    T k = T (1) / det (src);

    res [0][0] =  k * src [1][1];
    res [0][1] =  k * src [0][1];
    res [1][0] = -k * src [1][0];
    res [1][1] = -k * src [0][0];
  }
  
  template <class T>
  void operator () (const matrix<T, 3>& src, matrix<T, 3>& res) const
  {
    T k = T (1) / det (src);

    res [0][0] = k * (src [2][2]*src [1][1]-src [2][1]*src [1][2]);
    res [0][1] = k * (src [0][2]*src [2][1]-src [0][1]*src [2][2]);
    res [0][2] = k * (src [1][2]*src [0][1]-src [1][1]*src [0][2]);
    res [1][0] = k * (src [2][0]*src [1][2]-src [2][2]*src [1][0]);
    res [1][1] = k * (src [0][0]*src [2][2]-src [0][2]*src [2][0]);
    res [1][2] = k * (src [1][0]*src [0][2]-src [1][2]*src [0][0]);
    res [2][0] = k * (src [2][1]*src [1][0]-src [2][0]*src [1][1]);
    res [2][1] = k * (src [0][1]*src [2][0]-src [0][0]*src [2][1]);
    res [2][2] = k * (src [1][1]*src [0][0]-src [1][0]*src [0][1]);
  } 
  
#ifdef VECMATH_SSE
  __forceinline void swaprow (matrix<float,4>& a, unsigned int i, unsigned int j) const
  {
    __declspec(align(1)) vector<float, 4> tmp = a [i];
    a [i]                = a [j];
    a [j]                = tmp;      
  }

  __forceinline void operator () (const matrix<float, 4>& src, matrix<float, 4>& res) const
  {
    __declspec(align(1)) matrix<float, 4> a = src;
    __declspec(align(1)) vector<float, 4> t;

    res = 1.0f;

    for (unsigned int i=0; i<4; i++)
    {
      if (a [i][i] == 0.0f)
        for (unsigned int k=i+1; k<4; k++)
          if (a [k][i] != 0.0f)
          {
            swaprow (a, i, k);
            swaprow (res, i, k);
            break;
          }

      res [i] /= a[i][i];
      a [i]   /= a[i][i];
      
      unsigned int j;

      for (j=0; j<i; j++)
      {
        t        = a [j][i];
        a   [j] -= a   [i] * t;
        res [j] -= res [i] * t;
      }

      for (j++; j<4; j++)
      {
        t        = a [j][i];
        a   [j] -= a   [i] * t;
        res [j] -= res [i] * t;
      }
    }
  }
#endif
}; 

struct matrix_normalize {
  template <class T, unsigned int Size>
  void operator () (const matrix<T, Size>& src, matrix<T, Size>& res) const
  {
    matrix_div_scalar ()(src, sqrt (det (src)), res);
  }

  template <class T, unsigned int Size> 
  void operator () (matrix<T, Size>& m) const
  {
    (*this)(m, m);
  }

  template <unsigned int size>
  void operator () (const matrix<float, size>& src, matrix<float, size>& res) const
  {
    matrix_div_scalar ()(src, sqrtf (det (src)), res);
  }

  template <unsigned int size>
  void operator () (const matrix<long double, size>& src, matrix<long double, size>& res) const
  {
    matrix_div_scalar ()(src, sqrtd (det (src)), res);
  }
};

}

/*
    Конструкторы
*/

template <class T, unsigned int Size>
matrix<T, Size>::matrix ()
{
  detail::matrix_assign_scalar ()(T (1), *this);
}

template <class T, unsigned int Size>
matrix<T, Size>::matrix (const value_type& a)
{
  detail::matrix_assign_scalar ()(a, *this);
}

template <class T, unsigned int Size>
matrix<T, Size>::matrix (const value_type* a)
{
  for (unsigned int i=0; i<Size; i++)
    for (unsigned int j=0; j<Size; j++)
      (*this)[i][j] = *a++;
}

template <class T, unsigned int Size> template <class T1, class Fn>
matrix<T, Size>::matrix (const T1& a, Fn fn, return_value_tag)
{
  fn (a, *this);
}

template <class T, unsigned int Size> template <class T1, class T2, class Fn>
matrix<T, Size>::matrix (const T1& a, const T2& b, Fn fn, return_value_tag)
{
  fn (a, b, *this);
}

template <class T, unsigned int Size> template <class T1, class T2, class T3, class Fn>
matrix<T, Size>::matrix (const T1& a, const T2& b, const T3& c, Fn fn, return_value_tag)
{
  fn (a, b, c, *this);
}

/*
    Индексирование
*/

template <class T, unsigned int Size>
typename matrix<T, Size>::vector& matrix<T, Size>::operator [] (unsigned int i)
{
  return x [i];
}

template <class T, unsigned int Size>
const typename matrix<T, Size>::vector& matrix<T, Size>::operator [] (unsigned int i) const
{
  return x [i];
}

template <class T, unsigned int Size>
typename matrix<T, Size>::vector& matrix<T, Size>::row (unsigned int i) 
{
  return x [i];
}

template <class T, unsigned int Size>
const typename matrix<T, Size>::vector& matrix<T, Size>::row (unsigned int i) const
{
  return x [i];
}

template <class T, unsigned int Size>
typename matrix<T, Size>::vector matrix<T, Size>::column (unsigned int i) const
{
  return vector (*this, i, detail::matrix_copy_column (), return_value_tag ());
}

template <class T, unsigned int Size> 
matrix<T, Size>& matrix<T, Size>::operator = (const value_type& a)
{
  detail::matrix_assign_scalar ()(a, *this);
  return *this;
}

/*
    Унарные операции
*/

template <class T, unsigned int Size>
const matrix<T, Size>& matrix<T, Size>::operator + () const
{
  return *this;
}

template <class T, unsigned int Size>
matrix<T, Size> matrix<T, Size>::operator - () const
{
  return matrix (*this, detail::matrix_neg (), return_value_tag ());
}

/*
    Бинарные операции
*/

template <class T, unsigned int Size> 
matrix<T, Size>& matrix<T, Size>::operator += (const matrix<T, Size>& a)
{
  detail::matrix_add ()(*this, a, *this);
  return *this;
}

template <class T, unsigned int Size>   
matrix<T, Size>& matrix<T, Size>::operator -= (const matrix<T, Size>& a)
{
  detail::matrix_sub ()(*this, a, *this);
  return *this;
}

template <class T, unsigned int Size>   
matrix<T, Size>& matrix<T, Size>::operator *= (const matrix<T, Size>& a)
{
  detail::matrix_mul ()(*this, a);
  return *this;
}

template <class T, unsigned int Size>
matrix<T, Size>& matrix<T, Size>::operator *= (const value_type& a)
{
  detail::matrix_mul_scalar ()(*this, a, *this);
  return *this;
}

template <class T, unsigned int Size>
matrix<T, Size>& matrix<T, Size>::operator /= (const value_type& a)
{
  detail::matrix_div_scalar ()(*this, a, *this);
  return *this;
}

template <class T, unsigned int Size>   
matrix<T, Size> matrix<T, Size>::operator + (const matrix<T, Size>& m) const
{
  return matrix (*this, m, detail::matrix_add (), return_value_tag ());
}

template <class T, unsigned int Size>   
matrix<T, Size> matrix<T, Size>::operator - (const matrix<T, Size>& m) const
{
  return matrix (*this, m, detail::matrix_sub (), return_value_tag ());
}

template <class T, unsigned int Size>
matrix<T, Size> matrix<T, Size>::operator * (const value_type& a) const
{
  return matrix (*this, a, detail::matrix_mul_scalar (), return_value_tag ());
}

template <class T, unsigned int Size>
matrix<T, Size> operator * (const T& a, const matrix<T, Size>& m)
{
  return matrix<T, Size> (m, a, detail::matrix_mul_scalar (), return_value_tag ());
}

template <class T, unsigned int Size>
matrix<T, Size> matrix<T, Size>::operator / (const value_type& a) const
{
  return matrix (*this, a, detail::matrix_div_scalar (), return_value_tag ());
}

template <class T, unsigned int Size>   
matrix<T, Size> matrix<T, Size>::operator * (const matrix<T, Size>& a) const
{
  return matrix (*this, a, detail::matrix_mul (), return_value_tag ());
} 

template <class T, unsigned int Size> 
typename matrix<T, Size>::vector matrix<T, Size>::operator * (const vector& v) const
{
  return vector (*this, v, detail::matrix_mul_vec (), return_value_tag ());
}

template <class T, unsigned int Size> 
vector<T, Size-1> matrix<T, Size>::operator * (const math::vector<T, Size-1>& v) const
{
  return math::vector<T, Size-1> (*this, v, detail::matrix_mul_vec (), return_value_tag ());
}

template <class T, unsigned int Size>
vector<T, Size> operator * (const vector<T, Size>& v, const matrix<T, Size>& m)
{
  return vector<T, Size> (v, m, detail::vec_mul_matrix (), return_value_tag ());
}

template <class T, unsigned int Size>
vector<T, Size> operator * (const vector<T, Size>& v, const matrix<T, Size+1>& m)
{
  return vector<T, Size> (v, m, detail::vec_mul_matrix (), return_value_tag ());
}

template <class T, unsigned int Size>
vector<T, Size>& operator *= (vector<T, Size>& v, const matrix<T, Size>& m)
{
  return v = v * m;
} 

template <class T, unsigned int Size>
vector<T, Size>& operator *= (vector<T, Size>& v, const matrix<T, Size+1>& m)
{
  return v = v * m;
}

/*
    Сравнение
*/

template <class T, unsigned int Size> 
bool matrix<T, Size>::operator == (const matrix<T, Size>& a) const
{
  for (unsigned int i=0; i<Size; i++)
    if ((*this)[i] != a [i])
      return false;

  return true;
}

template <class T, unsigned int Size> 
bool matrix<T, Size>::operator != (const matrix<T, Size>& a) const
{
  return !(*this == a);
}

template <class T, unsigned int Size> 
bool equal (const matrix<T, Size>& a, const matrix<T, Size>& b, const T& eps)
{
  for (unsigned int i=0; i<Size; i++)
    if (!equal (a [i], b [i], eps))
      return false;

  return true;
} 

/*
    Операции над матрицей
*/

template <class T, unsigned int Size>
matrix<T, Size> transpose (const matrix<T, Size>& m)
{
  return matrix<T, Size> (m, detail::matrix_transpose (), return_value_tag ());
} 

template <class T, unsigned int Size>
matrix<T, Size> inverse (const matrix<T, Size>& m)
{
  return matrix<T, Size> (m, detail::matrix_inverse (), return_value_tag ()); 
}

template <class T, unsigned int Size>
matrix<T, Size> normalize (const matrix<T, Size>& m)
{
  return matrix<T, Size> (m, detail::matrix_normalize (), return_value_tag ());
} 

/*
    Нахождение определителя
*/

template <class T, unsigned int Size> 
typename matrix<T, Size>::value_type det (const matrix<T, Size>& m)
{
  typename matrix<T, Size>::value_type res = 0;

  unsigned int i;

  for (i=0; i<Size; i+=2) res += m [0][i] * minor (m, 0, i);
  for (i=1; i<Size; i+=2) res -= m [0][i] * minor (m, 0, i);

  return res;
}

template <class T>
typename matrix<T, 1>::value_type det (const matrix<T, 1>& m)
{ 
  return m [0][0];
}

template <class T>
typename matrix<T, 2>::value_type det (const matrix<T, 2>& m) 
{ 
  return m [0][0]*m[1][1]-m[0][1]*m[1][0]; 
}

template <class T>
typename matrix<T, 2>::value_type det (const matrix<T, 3>& m) 
{ 
  return m[0][0]*m[1][1]*m[2][2] + m[0][1]*m[1][2]*m[2][0] + m[0][2]*m[1][0]*m[2][1] -
         m[2][0]*m[1][1]*m[0][2] - m[2][1]*m[1][2]*m[0][0] - m[2][2]*m[1][0]*m[0][1]; 
}

#ifdef VECMATH_SSE

inline float det (const matrix<float, 4>& m) 
{ 
  __m128 r1, r2, r3, r4, r5, r6; 
  float res;
  
  r1 = _mm_shuffle_ps (m [1].data (), m [1].data (), _MM_SHUFFLE (0, 3, 2, 1));
  r2 = _mm_shuffle_ps (m [2].data (), m [2].data (), _MM_SHUFFLE (2, 1, 0, 3));
  r3 = _mm_shuffle_ps (m [3].data (), m [3].data (), _MM_SHUFFLE (1, 0, 3, 2));

  r5 = _mm_mul_ps     (r1, r2);  
 
  r1 = _mm_shuffle_ps (r1, r1, _MM_SHUFFLE (1, 0, 3, 2));
  r2 = _mm_shuffle_ps (r2, r2, _MM_SHUFFLE (1, 0, 3, 2));

  r4 = _mm_mul_ps     (r1, r2);
  r5 = _mm_sub_ps     (r5, r4);
  r6 = _mm_mul_ps     (r5, r3);

  r1 = _mm_shuffle_ps (r1, r1, _MM_SHUFFLE (2, 1, 0, 3));
  r3 = _mm_shuffle_ps (r3, r3, _MM_SHUFFLE (0, 3, 2, 1));

  r5 = _mm_mul_ps     (r1, r2);

  r1 = _mm_shuffle_ps (r1, r1, _MM_SHUFFLE (2, 1, 0, 3));
  r2 = _mm_shuffle_ps (r2, r2, _MM_SHUFFLE (0, 3, 2, 1));

  r4 = _mm_mul_ps     (r1, r2);
  r5 = _mm_sub_ps     (r5, r4);
  r5 = _mm_mul_ps     (r5, r3);
  r6 = _mm_add_ps     (r6, r5);

  r1 = _mm_shuffle_ps (r1, r1, _MM_SHUFFLE (1, 0, 3, 2));
  r3 = _mm_shuffle_ps (r3, r3, _MM_SHUFFLE (1, 0, 3, 2));

  r5 = _mm_mul_ps     (r1, r2);

  r1 = _mm_shuffle_ps (r1, r1, _MM_SHUFFLE (2, 1, 0, 3));
  r2 = _mm_shuffle_ps (r2, r2, _MM_SHUFFLE (0, 3, 2, 1));

  r4 = _mm_mul_ps     (r1, r2);
  r5 = _mm_sub_ps     (r5, r4);
  r5 = _mm_mul_ps     (r5, r3);
  r6 = _mm_add_ps     (r6, r5);

  r6 = _mm_mul_ps     (r6, m [0].data ());

  r4 = _mm_shuffle_ps (r6, r6, _MM_SHUFFLE (1, 0, 3, 2));
  r6 = _mm_add_ps     (r6, r4);
  r4 = _mm_shuffle_ps (r6, r6, _MM_SHUFFLE (2, 3, 0, 1));
  r6 = _mm_sub_ps     (r4, r6);

  _mm_store_ss        (&res, r6);

  return res;
}

#endif

template <class T, unsigned int Size> 
typename matrix<T, Size>::value_type minor (const matrix<T, Size>& src, unsigned int m, unsigned int n)
{
  matrix<T, Size-1> tmp;
  
  unsigned int i, j;

  for (i=0; i<m; i++)
  {
    for (j=0; j<n;    j++) tmp [i][j]   = src [i][j];
    for (j++; j<Size; j++) tmp [i][j-1] = src [i][j];
  }      

  for (i++; i<Size; i++)
  {
    for (j=0; j<n;    j++) tmp [i-1][j]   = src [i][j];
    for (j++; j<Size; j++) tmp [i-1][j-1] = src [i][j];
  }

  return det (tmp);
}

template <class T> 
typename matrix<T, 2>::value_type minor (const matrix<T, 2>& src, unsigned int m, unsigned int n) 
{
  return src [(m+1)&1][(n+1)&1];
} 

template <class T> 
typename matrix<T, 3>::value_type minor (const matrix<T, 3>& src, unsigned int m, unsigned int n) 
{
  return (m+n)&1 ? src [(m+2)%3][(n+1)%3]*src [(m+1)%3][(n+2)%3] -
                   src [(m+2)%3][(n+2)%3]*src [(m+1)%3][(n+1)%3]
                 : src [(m+2)%3][(n+2)%3]*src [(m+1)%3][(n+1)%3] -
                   src [(m+2)%3][(n+1)%3]*src [(m+1)%3][(n+2)%3];
}
