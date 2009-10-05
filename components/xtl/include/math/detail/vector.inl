/*
        Конструкторы
*/

template <class T,size_t size>
inline vec<T,size>::vec ()
{
  vec_assign_scalar (*this,T (0));
}

template <class T,size_t size> template <size_t size1>
vec<T,size>::vec (const vec<T,size1>& v) 
{
  vec_copy (*this,v);
}

template <class T,size_t size> 
vec<T,size>::vec (const T& a)
{
  vec_assign_scalar (*this,a);
}

template <class T,size_t size> 
vec<T,size>::vec (const base& b)
  : base (b)
{  
}

template <class T,size_t size> template <class T1>
vec<T,size>::vec (const T1& a,void (*eval)(vec&,const T1&))
{
  eval (*this,a);
}

template <class T,size_t size> template <class T1,class T2>
vec<T,size>::vec (const T1& a,const T2& b,void (*eval)(vec&,const T1&,const T2&))
{
  eval (*this,a,b);  
}

template <class T,size_t size>
vec<T,size>::vec (const T& x1,const T& x2,const T& x3,const T& x4)
{
  switch (size)
  {
    default:
    {
      for (size_t i=0;i<size-4;i++)
        (*this)[i+4] = T(0);
    }  
    case 4: (*this)[3] = x4;
    case 3: (*this)[2] = x3;
    case 2: (*this)[1] = x2;
    case 1: (*this)[0] = x1;
    case 0: break;
  }
}

template <class T,size_t size>
vec<T,size>::vec (const vec<T,size-1>& v,const T& a)
{
  for (size_t i=0;i<size-1;i++)
    (*this)[i] = v [i];

  (*this)[size-1] = a;
}

/*
        Привсаивание
*/

template <class T,size_t size> template <size_t size1>
vec<T,size>& vec<T,size>::operator = (const vec<T,size1>& v)
{
  vec_copy (*this,v);
  return *this;
} 

template <class T,size_t size>
vec<T,size>& vec<T,size>::operator = (const base& b)
{
  base::operator = (b);
  return *this;
} 

template <class T,size_t size> 
vec<T,size>& vec<T,size>::operator = (const T& a)
{
  vec_assign_scalar (*this,a);
  return *this;
}

/*
        Базовые унарные опреации
*/

template <class T,size_t size>
const vec<T,size>& vec<T,size>::operator + () const
{ 
  return *this; 
}

template <class T,size_t size>
const vec<T,size> vec<T,size>::operator - () const
{ 
  return vec (*this,vec_neg<T,size>);
}

template <class T,size_t size> 
T vec<T,size>::operator ~ () const
{ 
  return vec_length (*this); 
}

/*
        Базовые бинарные операции
*/

template <class T,size_t size>
T vec<T,size>::operator & (const vec& v) const
{ 
  return vec_dot_product (*this,v);
}

template <class T,size_t size>
vec<T,size>& vec<T,size>::operator += (const vec& v) 
{ 
  vec_add (*this,*this,v);
  return *this; 
}

template <class T,size_t size>
vec<T,size>& vec<T,size>::operator -= (const vec& v) 
{ 
  vec_sub (*this,*this,v);
  return *this; 
}

template <class T,size_t size>
vec<T,size>& vec<T,size>::operator *= (const vec& v) 
{ 
  vec_mul (*this,*this,v);
  return *this; 
}

template <class T,size_t size>
vec<T,size>& vec<T,size>::operator /= (const vec& v) 
{ 
  vec_div (*this,*this,v);
  return *this; 
}
        
template <class T,size_t size>
vec<T,size>& vec<T,size>::operator *= (const T& d)
{ 
  vec_mul_scalar (*this,*this,d);
  return *this; 
}

template <class T,size_t size>
vec<T,size>& vec<T,size>::operator /= (const T& d)
{ 
  vec_div_scalar (*this,*this,d);
  return *this; 
}

template <class T,size_t size>
const vec<T,size> vec<T,size>::operator + (const vec& v) const  
{ 
  return vec (*this,v,vec_add<T,size>);
}

template <class T,size_t size>
const vec<T,size> vec<T,size>::operator - (const vec& v) const  
{ 
  return vec (*this,v,vec_sub<T,size>);
}

template <class T,size_t size>
const vec<T,size> vec<T,size>::operator * (const vec& v) const  
{ 
  return vec (*this,v,vec_mul<T,size>);
}

template <class T,size_t size>
const vec<T,size> vec<T,size>::operator / (const vec& v) const  
{ 
  return vec (*this,v,vec_div<T,size>);
}

template <class T,size_t size>
const vec<T,size> vec<T,size>::operator * (const T& a) const  
{ 
  return vec (*this,a,vec_mul_scalar<T,size>);
}

template <class T,size_t size> 
const vec<T,size> vec<T,size>::operator / (const T& a) const  
{ 
  return vec (*this,a,vec_div_scalar<T,size>);
}

/*
        Сравнение векторов
*/

template <class T,size_t size>
bool vec<T,size>::operator == (const vec& v) const
{
  return vec_equal (*this,v);
}

template <class T,size_t size>
bool vec<T,size>::operator != (const vec& v) const
{
  return vec_nequal (*this,v);
}

/*template <class T,size_t size>
vec<T,size>::operator const vec<T,size-1>& () const
{
  return *(const vec<T,size-1>*)this;
}

template <class T,size_t size>
vec<T,size>::operator vec<T,size-1>& ()
{
  return *(vec<T,size-1>*)this;
} */

/*
        Утилиты
*/

template <class T,size_t size> 
T dot (const vec<T,size>& a,const vec<T,size>& b)
{
  return vec_dot_product (a,b);
}

template <class T,size_t size> 
const vec<T,size> abs (const vec<T,size>& v)
{
  return vec<T,size> (v,vec_abs<T,size>);
}

template <class T,size_t size>
const vec<T,size> min (const vec<T,size>& a,const vec<T,size>& b)
{
  return vec<T,size> (a,b,vec_min<T,size>);
}

template <class T,size_t size>
const vec<T,size> max (const vec<T,size>& a,const vec<T,size>& b)
{
  return vec<T,size> (a,b,vec_max<T,size>);
} 

template <class T,size_t size> 
const vec<T,size> normalize (const vec<T,size>& v)
{
  return vec<T,size> (v,vec_normalize<T,size>);
}

template <class T,size_t size> 
T length (const vec<T,size>& v)
{
  return vec_length (v);
}

template <class T,size_t size> 
T qlen (const vec<T,size>& v)
{
  return vec_qlength (v);
}

template <class T,size_t size> 
bool equal (const vec<T,size>& a,const vec<T,size>& b,const T& eps)
{
  return vec_equal (a,b,eps);
}

template <class T,size_t size>
T angle (const vec<T,size>& a,const vec<T,size>& b)
{
  T ang = acos((a&b)/sqrt(qlen(a)*qlen(b)));

  return _isnan(ang)?0:ang;
}

/*
    Векторное произведение
*/

template <class type>
vec<type,3> cross (const vec<type,3>& a,const vec<type,3>& b)
{
  return vec<type,3> (a,b,vec_cross_product<type>);
}

template <class type>
vec<type,4> cross (const vec<type,4>& a,const vec<type,4>& b)
{
  return vec<type,4> (a,b,vec_cross_product<type>);
}

template <class type>
vec<type,3> operator ^ (const vec<type,3>& a,const vec<type,3>& b)
{
  return vec<type,3> (a,b,vec_cross_product<type>);
}

template <class type>
vec<type,4> operator ^ (const vec<type,4>& a,const vec<type,4>& b)
{
  return vec<type,4> (a,b,vec_cross_product<type>);
}
