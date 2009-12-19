namespace detail
{

#ifdef VECMATH_SSE

__forceinline __m128 to_m128 (const vector<float, 4>& v)
{
  return _mm_loadu_ps (&v.x);
}

#endif

/*
    Покомпонентные векторные операции
*/

struct vec_add {
  template <class T, unsigned int Size>
  void operator () (const vector<T, Size>& a, const vector<T, Size>& b, vector<T, Size>& res) const
  {
    for (unsigned int i=0; i<Size; i++)
      res [i] = a [i] + b [i];
  }
  
#ifdef VECMATH_SSE
  __forceinline void operator () (const vector<float, 4>& a, const vector<float, 4>& b, vector<float, 4>& res) const
  {
    _mm_storeu_ps (&res [0], _mm_add_ps (to_m128 (a), to_m128 (b)));
  }
#endif
};

struct vec_sub {
  template <class T, unsigned int Size>
  void operator () (const vector<T, Size>& a, const vector<T, Size>& b, vector<T, Size>& res) const
  {
    for (unsigned int i=0; i<Size; i++)
      res [i] = a [i] - b [i];
  }
  
#ifdef VECMATH_SSE
  __forceinline void operator () (const vector<float, 4>& a, const vector<float, 4>& b, vector<float, 4>& res) const
  {
    _mm_storeu_ps (&res [0], _mm_sub_ps (to_m128 (a), to_m128 (b)));
  }
#endif
};

struct vec_mul {
  template <class T, unsigned int Size>
  void operator () (const vector<T, Size>& a, const vector<T, Size>& b, vector<T, Size>& res) const
  {
    for (unsigned int i=0; i<Size; i++) res [i] = a [i] * b [i];
  }
  
#ifdef VECMATH_SSE
  __forceinline void operator () (const vector<float, 4>& a, const vector<float, 4>& b, vector<float, 4>& res) const
  {
    _mm_storeu_ps (&res [0], _mm_mul_ps (to_m128 (a), to_m128 (b)));
  }
#endif
};

struct vec_div {
  template <class T, unsigned int Size>
  void operator () (const vector<T, Size>& a, const vector<T, Size>& b, vector<T, Size>& res) const
  {
    for (unsigned int i=0; i<Size; i++)
      res [i] = a [i] / b [i];
  }
  
#ifdef VECMATH_SSE
  __forceinline void operator () (const vector<float, 4>& a, const vector<float, 4>& b, vector<float, 4>& res) const
  {
    _mm_storeu_ps (&res [0], _mm_div_ps (to_m128 (a), to_m128 (b)));
  }
#endif
};

struct vec_mul_scalar {
  template <class T, unsigned int Size>
  void operator () (const vector<T, Size>& a, const T& b, vector<T, Size>& res) const
  {
    for (unsigned int i=0; i<Size; i++)
      res [i] = a [i] * b;
  }
};

struct vec_div_scalar {
  template <class T, unsigned int Size>
  void operator () (const vector<T, Size>& a, const T& b, vector<T, Size>& res) const
  {
    vec_mul_scalar ()(a, T (1) / b, res);
  }
};

/*
    Векторное присваивание
*/

struct vec_copy {
  template <class T, unsigned int Size1, unsigned int Size2>
  void operator () (const vector<T, Size2>& src, vector<T, Size1>& res) const
  {
    const unsigned int s1 = Size1 < Size2 ? Size1 : Size2;
    
    unsigned int i;
  
    for (i=0; i<s1; i++) res [i] = src [i];
    for (; i<Size1; i++) res [i] = 0;
  }
  
#ifdef VECMATH_SSE
  __forceinline void operator () (const vector<float, 4>& a, vector<float, 4>& res) const
  {
    _mm_storeu_ps (&res.x, to_m128 (a));
  }
#endif
};

struct vec_assign_scalar {
  template <class T, unsigned int Size>
  void operator () (const T& a, vector<T, Size>& res) const
  {
    for (unsigned int i=0; i<Size; i++)
      res [i] = a;
  }
  
#ifdef VECMATH_SSE
  inline void operator () (float a, vector<float, 4>& res) const
  {
    _mm_storeu_ps (&res [0], _mm_set_ps1 (a));
  }
#endif
};

/*
        Утилиты
*/

struct vec_neg {
  template <class T, unsigned int Size>
  void operator () (const vector<T, Size>& src, vector<T, Size>& res) const
  {
    for (unsigned int i=0; i<Size; i++)
      res [i] = -src [i];
  }
  
#ifdef VECMATH_SSE
  __forceinline void operator () (const vector<float, 4>& src, vector<float, 4>& res) const
  {
    static union {
      size_t i;
      float  f;  
    } mask = {0x80000000};

    _mm_storeu_ps (&res.x, _mm_xor_ps (to_m128 (src), _mm_set_ps1 (mask.f)));
  }
#endif
};

struct vec_abs {
  template <class T, unsigned int Size>
  void operator () (const vector<T, Size>& src, vector<T, Size>& res) const
  {
    for (unsigned int i=0; i<Size; i++)
      res [i] = src [i] < T (0) ? -src [i] : src [i];
  }

#ifdef VECMATH_SSE    
  __forceinline void operator () (const vector<float, 4>& src, vector<float, 4>& res) const
  {
    static union {
      size_t i;
      float  f;  
    } mask = {0x7FFFFFFF};

    _mm_storeu_ps (&res.x, _mm_and_ps (to_m128 (src), _mm_set_ps1 (mask.f)));
  }
#endif
};

struct vec_min {
  template <class T, unsigned int Size>
  void operator () (const vector<T, Size>& a, const vector<T, Size>& b, vector<T, Size>& res) const
  {
    for (unsigned int i=0; i<Size; i++)
      res [i] = a [i] < b [i] ? a [i] : b [i];
  }

#ifdef VECMATH_SSE  
  __forceinline void operator () (const vector<float, 4>& a, const vector<float, 4>& b, vector<float, 4>& res) const
  {
    _mm_storeu_ps (&res.x, _mm_min_ps (to_m128 (a), to_m128 (b)));
  }
#endif
};

struct vec_max {
  template <class T, unsigned int Size>
  void operator () (const vector<T, Size>& a, const vector<T, Size>& b, vector<T, Size>& res) const
  {
    for (unsigned int i=0; i<Size; i++)
      res [i] = a [i] > b [i] ? a [i] : b [i];
  }
  
#ifdef VECMATH_SSE  
  __forceinline void operator () (const vector<float, 4>& a, const vector<float, 4>& b, vector<float, 4>& res) const
  {
    _mm_storeu_ps (&res.x, _mm_max_ps (to_m128 (a), to_m128 (b)));
  }
#endif
};

/*
    Векторное произведение
*/

struct vec_cross_product {
  template <class T>
  void operator () (const vector<T, 3>& a, const vector<T, 3>& b, vector<T, 3>& res) const
  {
    res [0] = a [1] * b [2] - b [1] * a [2];
    res [1] = b [0] * a [2] - b [2] * a [0];
    res [2] = a [0] * b [1] - b [0] * a [1];
  }

  template <class T>
  void operator () (const vector<T, 4>& a, const vector<T, 4>& b, vector<T, 4>& res) const
  {
    (*this) (reinterpret_cast<const vector<T, 3>&> (a), reinterpret_cast<const vector<T, 3>&> (b), reinterpret_cast<vector<T, 3>&> (res));
  
    res [3] = 0;
  }
  
#ifdef VECMATH_SSE
  __forceinline void operator () (const vector<float, 4>& a, const vector<float, 4>& b, vector<float, 4>& res) const
  {
    __m128 r0, r1, r2;

    r0 = _mm_shuffle_ps (to_m128 (a), to_m128 (a), _MM_SHUFFLE (3, 1, 0, 2));
    r1 = _mm_shuffle_ps (to_m128 (b), to_m128 (b), _MM_SHUFFLE (3, 0, 2, 1));
    r2 = _mm_mul_ps     (r1, r0);

    r0 = _mm_shuffle_ps (r0, r0, _MM_SHUFFLE (3, 1, 0, 2));
    r1 = _mm_shuffle_ps (r1, r1, _MM_SHUFFLE (3, 0, 2, 1));
    r1 = _mm_mul_ps     (r1, r0);

    r1 = _mm_sub_ps     (r1,r2);

    _mm_storeu_ps (&res.x, r1);
  }
#endif
};

}

/*
    Конструкторы
*/

template <class T, unsigned int Size>
vector<T, Size>::vector ()
{
  detail::vec_assign_scalar ()(T (0), *this);
}

template <class T, unsigned int Size> template <unsigned int Size1>
vector<T, Size>::vector (const vector<T, Size1>& v) 
{
  detail::vec_copy ()(v, *this);
}

template <class T, unsigned int Size> 
vector<T, Size>::vector (const T& a)
{
  detail::vec_assign_scalar ()(a, *this);
}

template <class T, unsigned int Size> 
vector<T, Size>::vector (const base& b)
  : base (b)
{  
}

template <class T, unsigned int Size> template <class T1, class Fn>
vector<T, Size>::vector (const T1& a, Fn fn, return_value_tag)
{
  fn (a, *this);
}

template <class T, unsigned int Size> template <class T1, class T2, class Fn>
vector<T, Size>::vector (const T1& a, const T2& b, Fn fn, return_value_tag)
{
  fn (a, b, *this);
}

template <class T, unsigned int Size> template <class T1, class T2, class T3, class Fn>
vector<T, Size>::vector (const T1& a, const T2& b, const T3& c, Fn fn, return_value_tag)
{
  fn (a, b, c, *this);
}

template <class T, unsigned int Size>
vector<T, Size>::vector (const T& x1, const T& x2, const T& x3, const T& x4)
{
  switch (Size)
  {
    default:
    {
      for (unsigned int i=0; i<Size-4; i++)
        (*this)[i+4] = T(0);
    }  
    case 4: (*this)[3] = x4;
    case 3: (*this)[2] = x3;
    case 2: (*this)[1] = x2;
    case 1: (*this)[0] = x1;
    case 0: break;
  }
}

template <class T, unsigned int Size>
vector<T, Size>::vector (const vector<value_type, size-1>& v, const value_type& a)
{
  for (unsigned int i=0;i<Size-1;i++)
    (*this)[i] = v [i];

  (*this)[Size-1] = a;
}

/*
    Присваивание
*/

template <class T, unsigned int Size> template <unsigned int Size1>
vector<T, Size>& vector<T, Size>::operator = (const vector<T, Size1>& v)
{
  detail::vec_copy ()(v, *this);
  return *this;
} 

template <class T, unsigned int Size>
vector<T, Size>& vector<T, Size>::operator = (const base& b)
{
  base::operator = (b);
  return *this;
} 

template <class T, unsigned int Size> 
vector<T, Size>& vector<T, Size>::operator = (const T& a)
{
  detail::vec_assign_scalar ()(a, *this);
  return *this;
}

/*
    Операции доступа
*/

template <class T, unsigned int Size>
typename vector<T, Size>::value_type& vector<T, Size>::operator [] (unsigned int index)
{
  return static_cast<value_type*> (&this->x) [index];
}

template <class T, unsigned int Size>
const typename vector<T, Size>::value_type& vector<T, Size>::operator [] (unsigned int index) const
{
  return static_cast<const value_type*> (&this->x) [index];
}

/*
    Базовые унарные опреации
*/

template <class T, unsigned int Size>
const vector<T, Size>& vector<T, Size>::operator + () const
{ 
  return *this;
}

template <class T, unsigned int Size>
vector<T, Size> vector<T, Size>::operator - () const
{ 
  return vector (*this, detail::vec_neg (), return_value_tag ());
}

/*
    Базовые бинарные операции
*/

template <class T, unsigned int Size>
vector<T, Size>& vector<T, Size>::operator += (const vector& v) 
{ 
  detail::vec_add ()(*this, v, *this);
  return *this; 
}

template <class T, unsigned int Size>
vector<T, Size>& vector<T, Size>::operator -= (const vector& v) 
{ 
  detail::vec_sub ()(*this, v, *this);
  return *this; 
}

template <class T, unsigned int Size>
vector<T, Size>& vector<T, Size>::operator *= (const vector& v) 
{ 
  detail::vec_mul ()(*this, v, *this);
  return *this; 
}

template <class T, unsigned int Size>
vector<T, Size>& vector<T, Size>::operator /= (const vector& v) 
{ 
  detail::vec_div ()(*this, v, *this);
  return *this; 
}
        
template <class T, unsigned int Size>
vector<T, Size>& vector<T, Size>::operator *= (const value_type& d)
{ 
  detail::vec_mul_scalar ()(*this, d, *this);
  return *this; 
}

template <class T, unsigned int Size>
vector<T, Size>& vector<T, Size>::operator /= (const value_type& d)
{ 
  detail::vec_div_scalar ()(*this, d, *this);
  return *this; 
}

template <class T, unsigned int Size>
vector<T, Size> vector<T, Size>::operator + (const vector& v) const  
{ 
  return vector (*this, v, detail::vec_add (), return_value_tag ());
}

template <class T, unsigned int Size>
vector<T, Size> vector<T, Size>::operator - (const vector& v) const  
{ 
  return vector (*this, v, detail::vec_sub (), return_value_tag ());
}

template <class T, unsigned int Size>
vector<T, Size> vector<T, Size>::operator * (const vector& v) const  
{ 
  return vector (*this, v, detail::vec_mul (), return_value_tag ());
}

template <class T, unsigned int Size>
vector<T, Size> vector<T, Size>::operator / (const vector& v) const  
{ 
  return vector (*this, v, detail::vec_div (), return_value_tag ());
}

template <class T, unsigned int Size>
vector<T, Size> vector<T, Size>::operator * (const T& a) const  
{ 
  return vector (*this, a, detail::vec_mul_scalar (), return_value_tag ());
}

template <class T, unsigned int Size> 
vector<T, Size> operator * (const T& a, const vector<T, Size>& v)
{
  return v * a;
}

template <class T, unsigned int Size> 
vector<T, Size> vector<T, Size>::operator / (const T& a) const  
{ 
  return vector (*this, a, detail::vec_div_scalar (), return_value_tag ());
}

/*
    Сравнение векторов
*/

template <class T, unsigned int Size>
bool vector<T, Size>::operator == (const vector& v) const
{
  for (unsigned int i=0; i<Size; i++)
    if ((*this)[i] != v [i])
      return false;

  return true;
}

template <class T, unsigned int Size>
bool vector<T, Size>::operator != (const vector& v) const
{
  return !(*this == v);
}

/*
    Утилиты
*/

template <class T, unsigned int Size> 
typename vector<T, Size>::value_type dot (const vector<T, Size>& a, const vector<T, Size>& b)
{
  T result = 0;

  for (unsigned int i=0; i<Size; i++)
    result += a [i] * b [i];

  return result;
}

template <class T, unsigned int Size> 
vector<T, Size> abs (const vector<T, Size>& v)
{
  return vector<T, Size> (v, detail::vec_abs (), return_value_tag ());
}

template <class T, unsigned int Size>
vector<T, Size> min (const vector<T, Size>& a, const vector<T, Size>& b)
{
  return vector<T, Size> (a, b, detail::vec_min (), return_value_tag ());
}

template <class T, unsigned int Size>
vector<T, Size> max (const vector<T, Size>& a, const vector<T, Size>& b)
{
  return vector<T, Size> (a, b, detail::vec_max (), return_value_tag ());
} 

/*
    Длина и нормализация
*/

template <class T, unsigned int Size> 
typename vector<T, Size>::value_type qlen (const vector<T, Size>& v)
{
  return dot (v, v);
}

template <class T, unsigned int Size>
T length (const vector<T, Size>& v)
{
  return sqrt (qlen (v));
}

template <unsigned int Size>
float length (const vector<float, Size>& v)
{
  return sqrtf (qlen (v));
}

template <unsigned int Size>
long double length (const vector<long double, Size>& v)
{
  return sqrtl (qlen (v));
}

template <class T, unsigned int Size> 
vector<T, Size> normalize (const vector<T, Size>& v)
{
  return vector<T, Size> (v, length (v), detail::vec_div_scalar (), return_value_tag ());
}

template <class T, unsigned int Size> 
bool equal (const vector<T, Size>& a, const vector<T, Size>& b, const T& eps)
{
  vector<T, Size> tmp (a, b, detail::vec_sub (), return_value_tag ());

  detail::vec_abs ()(tmp, tmp);

  for (unsigned int i=0; i<Size; i++)
    if (tmp [i] > eps)
      return false;

  return true;
}

/*
    Векторное произведение
*/

template <class T>
vector<T, 3> cross (const vector<T, 3>& a, const vector<T, 3>& b)
{
  return vector<T, 3> (a, b, detail::vec_cross_product (), return_value_tag ());
}

template <class T>
vector<T, 4> cross (const vector<T, 4>& a, const vector<T, 4>& b)
{
  return vector<T, 4> (a, b, detail::vec_cross_product (), return_value_tag ());
}
