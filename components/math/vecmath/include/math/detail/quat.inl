namespace detail
{

#ifdef VECMATH_SSE

__forceinline __m128 to_m128 (const quat<float>& q)
{
  return _mm_loadu_ps (&q.x);
}

#endif


/*
    Бинарные операции
*/

struct quat_add {
  template <class T> 
  void operator () (const quat<T>& a, const quat<T>& b, quat<T>& res) const
  {
    for (int i=0; i<4; i++)
      res [i] = a [i] + b [i];
  }
  
#ifdef VECMATH_SSE
  __forceinline void operator () (const quat<float>& a, const quat<float>& b, quat<float>& res) const
  { 
    _mm_storeu_ps (&res.x, _mm_add_ps (to_m128 (a), to_m128 (b)));
  }
#endif
};

struct quat_sub {
  template <class T> 
  void operator () (const quat<T>& a, const quat<T>& b, quat<T>& res) const
  {
    for (int i=0; i<4; i++)
      res [i] = a [i] - b [i];
  }
  
#ifdef VECMATH_SSE
  __forceinline void operator () (const quat<float>& a, const quat<float>& b, quat<float>& res) const
  { 
    _mm_storeu_ps (&res.x, _mm_sub_ps (to_m128 (a), to_m128 (b)));
  }
#endif
};

struct quat_mul {
  template <class T> 
  void operator () (const quat<T>& a, const quat<T>& b, quat<T>& res) const
  {
    res.x = a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y;
    res.y = a.w*b.y + a.y*b.w + a.z*b.x - a.x*b.z;
    res.z = a.w*b.z + a.z*b.w + a.x*b.y - a.y*b.x;
    res.w = a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z;
  }
  
  template <class T> 
  void operator () (quat<T>& a, const quat<T>& b) const
  {
    (*this)(quat<T> (a), b, a);
  }
};

struct quat_mul_scalar {
  template <class T> 
  void operator () (const quat<T>& a, const T& b, quat<T>& res) const
  {
    for (int i=0; i<4; i++)
      res [i] = a [i] * b;
  }
};

struct quat_div_scalar {
  template <class T> 
  void operator () (const quat<T>& a, const T& b, quat<T>& res) const
  {
    quat_mul_scalar ()(a, T (1) / b, res);
  }
};

/*
    Унарные операции
*/

struct quat_copy {
  template <class T> 
  void operator () (const quat<T>& src, quat<T>& res) const
  {
    for (int i=0; i<4; i++)
      res [i] = src [i];
  }
  
#ifdef VECMATH_SSE
  __forceinline void operator () (const quat<float>& src, quat<float>& res) const
  {
    _mm_storeu_ps (&res.x, to_m128 (src));
  }
#endif
};

struct quat_assign_scalar {
  template <class T> 
  void operator () (const T& value, quat<T>& res) const
  {
    for (int i=0; i<4; i++)
      res [i] = value;
  }
  
#ifdef VECMATH_SSE
  __forceinline void operator () (float a, quat<float>& res) const
  {
    _mm_storeu_ps (&res.x, _mm_set_ps1 (a));
  }
#endif
};

struct quat_neg {
  template <class T> 
  void operator () (const quat<T>& src, quat<T>& res) const
  {
    for (int i=0; i<4; i++)
      res [i] = -src [i]; 
  }
  
#ifdef VECMATH_SSE
  __forceinline void operator () (const quat<float>& src, quat<float>& res) const
  {
    static union {
      size_t i;
      float  f;  
    } mask = {0x80000000};

    _mm_storeu_ps (&res.x, _mm_xor_ps (to_m128 (src), _mm_set_ps1 (mask.f)));
  }
#endif
};

/*
    Умножение на вектор
*/

struct quat_mul_vec {
  template <class T> 
  void operator () (const quat<T>& q, const vector<T, 3>& v, vector<T, 3>& res) const
  {
    T x = v.x * q.w + v.z * q.y - v.y * q.z, 
      y = v.y * q.w + v.x * q.z - v.z * q.x, 
      z = v.z * q.w + v.y * q.x - v.x * q.y, 
      w = v.x * q.x + v.y * q.y + v.z * q.z;

    res [0]  = q.w * x + q.x * w + q.y * z - q.z * y;
    res [1]  = q.w * y + q.y * w + q.z * x - q.x * z;
    res [2]  = q.w * z + q.z * w + q.x * y - q.y * x;
    res     /= norm (q);
  }
  
  template <class T>
  void operator () (const quat<T>& q, const vector<T, 4>& v, vector<T, 4>& res) const
  {
    (*this)(q, (const vector<T, 3>&)v, (vector<T, 3>&)res);
    
    res [3] = 0;
  }
};

struct quat_inverse {
  template <class T>
  void operator () (const quat<T>& src, quat<T>& res) const
  {
    T quat_norm = norm (src);
    
    if (quat_norm > T (0))
    {
      T inv_norm = T (1) / quat_norm;
      
      res.w =  src.w * inv_norm;
      res.x = -src.x * inv_norm;
      res.y = -src.y * inv_norm;
      res.z = -src.z * inv_norm;
    }
    else res.x = res.y = res.z = res.w = 0;
  }
};

}

/*
    Конструкторы
*/

template <class T>
quat<T>::quat ()
  : x (0)
  , y (0)
  , z (0)
  , w (1)
{
}

template <class T> template <class T1, class Fn>
quat<T>::quat (const T1& a, Fn fn, return_value_tag)
{
  fn (a, *this);
}

template <class T> template <class T1, class T2, class Fn>
quat<T>::quat (const T1& a, const T2& b, Fn fn, return_value_tag)
{
  fn (a, b, *this);
}

template <class T> template <class T1, class T2, class T3, class Fn>
quat<T>::quat (const T1& a, const T2& b, const T3& c, Fn fn, return_value_tag)
{
  fn (a, b, c, *this);
}

template <class T> 
quat<T>::quat (const value_type& in_x, const value_type& in_y, const value_type& in_z, const value_type& in_w)
  : x (in_x)
  , y (in_y)
  , z (in_z)
  , w (in_w)
{  
}

template <class T>
quat<T>::quat (const value_type& a)
{
  detail::quat_assign_scalar ()(a, *this);
}

template <class T>
quat<T>::quat (const quat& q)
{
  detail::quat_copy ()(q, *this);
}

/*
    Присваивание
*/

template <class T>
quat<T>& quat<T>::operator = (const value_type& a)
{
  detail::quat_assign_scalar ()(a, *this);
  return *this;
}

template <class T>
quat<T>& quat<T>::operator = (const quat<T>& q)
{
  detail::quat_copy ()(q, *this);
  return *this;
}

/*
    Индексирование
*/

template <class T>
typename quat<T>::value_type& quat<T>::operator [] (unsigned int index)
{
  return (&x)[index];
}

template <class T>
const typename quat<T>::value_type& quat<T>::operator [] (unsigned int index) const
{
  return (&x)[index];
}

/*
    Унарные операции
*/

template <class T>
const quat<T>& quat<T>::operator + () const
{
  return *this;
}

template <class T>
quat<T> quat<T>::operator - () const
{
  return quat (*this, detail::quat_neg (), return_value_tag ());
}

/*
    Бинарные операции
*/

template <class T>
quat<T>& quat<T>::operator += (const quat<T>& q)
{
  detail::quat_add ()(*this, q, *this);
  return *this;
}

template <class T>
quat<T>& quat<T>::operator -= (const quat<T>& q)
{
  detail::quat_sub ()(*this, q, *this);
  return *this;
}

template <class T>
quat<T>& quat<T>::operator *= (const value_type& a)
{
  detail::quat_mul_scalar ()(*this, a, *this);
  return *this;
}

template <class T>
quat<T>& quat<T>::operator /= (const value_type& a)
{
  detail::quat_div_scalar ()(*this, a, *this);
  return *this;
}

template <class T>  
quat<T>& quat<T>::operator *= (const quat<T>& q)
{  
  detail::quat_mul ()(*this, q);
  return *this;
}

template <class T>
quat<T> quat<T>::operator + (const quat<T>& q) const
{
  return quat (*this, q, detail::quat_add (), return_value_tag ());
}

template <class T>
quat<T> quat<T>::operator - (const quat<T>& q) const
{
  return quat (*this, q, detail::quat_sub (), return_value_tag ());
}

template <class T>
quat<T> quat<T>::operator * (const quat<T>& q) const
{
  return quat (*this, q, detail::quat_mul (), return_value_tag ());
}

template <class T> 
quat<T> quat<T>::operator * (const value_type& a) const
{
  return quat (*this, a, detail::quat_mul_scalar (), return_value_tag ());
}

template <class T>
quat<T> operator * (const typename quat<T>::value_type& a, const quat<T>& q)
{
  return q * a;
}

template <class T> 
quat<T> quat<T>::operator / (const value_type& a) const
{
  return quat (*this, a, detail::quat_div_scalar (), return_value_tag ());
}

/*
    Сравнение
*/

template <class T>  
bool quat<T>::operator == (const quat<T>& q) const
{
  for (int i=0; i<4; i++)
    if ((*this) [i] != q [i])
      return false;

  return true;
}

template <class T>  
bool quat<T>::operator != (const quat<T>& q) const
{
  return !(*this == q);
}

template <class T> 
bool equal (const quat<T>& a, const quat<T>& b, const T& eps)
{
  for (int i=0; i<4; i++)
  {
    T t = a [i] - b [i];

    if (t > eps || t < -eps)
      return false;  
  }    

  return true;
}

/*
    Умножение кватерниона на вектор
*/

template <class T>
vector<T, 3> quat<T>::operator * (const vector<T, 3>& v) const
{
  return vector<T, 3> (*this, v, detail::quat_mul_vec (), return_value_tag ());
} 

template <class T>
vector<T, 4> quat<T>::operator * (const vector<T, 4>& v) const
{
  return vector<T, 4> (*this, v, detail::quat_mul_vec (), return_value_tag ());
}

template <class T, unsigned int Size>
vector<T, Size> operator * (const vector<T, Size>& v, const quat<T>& q)
{
  return q * v;
}

/*
    Скалярное произведение
*/

template <class T>
typename quat<T>::value_type inner (const quat<T>& a, const quat<T>& b)
{
  T result = 0;

  for (int i=0; i<4; i++)
    result += a [i] * b [i];

  return result;  
}

/*
    Длина / норма / нормирование
*/

template <class T>
typename quat<T>::value_type norm (const quat<T>& q)
{
  return inner (q, q);
}

template <class T> 
typename quat<T>::value_type length (const quat<T>& v)
{
  return sqrt (norm (v));
}

inline float length (const quat<float>& v)
{
  return sqrtf (norm (v));
}

#ifdef _MSC_VER

inline long double length (const quat<long double>& v)
{
  return sqrtl (norm (v));
}

#endif

template <class T>
quat<T> normalize (const quat<T>& q)
{
  return quat<T> (q, length (q), detail::quat_div_scalar (), return_value_tag ());
}

/*
    Нахождение обратного кватерниона
*/

template <class T>
quat<T> inverse (const quat<T>& q)
{
  return quat<T> (q, detail::quat_inverse (), return_value_tag ());
}

/*
    Приведение к строке
*/

template <class String, class T>
void to_string (String& buffer, const quat<T>& value)
{
  detail::to_string_helper<4> (buffer, &value [0]);
}
