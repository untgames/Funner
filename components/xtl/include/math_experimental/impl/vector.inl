/*
        Конструкторы
*/

template <class T,size_t size>
inline vec<T,size>::vec ()
{
  component_fn<assign<T,T> > ()(*this,T (0));
}

template <class T,size_t size> template <size_t size1>
vec<T,size>::vec (const vec<T,size1>& v) 
{
  component_fn<assign<T,T> > () (*this,v);
}

template <class T,size_t size> 
vec<T,size>::vec (const T& a)
{
  component_fn<assign<T,T> > ()(*this,a);
}

template <class T,size_t size> 
vec<T,size>::vec (const base& b)
  : base (b)
{  
}

template <class type,size_t size> template <size_t size1, size_t size2, class Fn>
	inline vec<type,size>::vec (const vec<type,size1>v1,const vec<type,size2>v2,Fn fn)
{
	fn(*this,v1,v2);
}

template<class type,size_t size> template <size_t size1,class Fn>
	inline vec<type,size>::vec (const vec<type,size1>& v1,const type& arg,Fn fn)
{
	fn(*this,v1,arg);
}

template<class type,size_t size> template <size_t size1,class Fn>
	inline vec<type,size>::vec (const vec<type,size1>& v1,Fn fn)
{
	fn(*this,v1);
}



/*template <class T,size_t size> template <class T1>
vec<T,size>::vec (const T1& a,void (*eval)(vec&,const T1&))
{
  eval (*this,a);
}

template <class T,size_t size> template <class T1,class T2>
vec<T,size>::vec (const T1& a,const T2& b,void (*eval)(vec&,const T1&,const T2&))
{
  eval (*this,a,b);  
}*/

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
  return vec (*this,component_fn<negate <T,T> > ());
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
  component_fn<plus <T,T,T> > () (*this,*this,v);
  return *this; 
}

template <class T,size_t size>
vec<T,size>& vec<T,size>::operator -= (const vec& v) 
{ 
  component_fn<minus<T,T,T> > () (*this,*this,v);
  return *this; 
}

template <class T,size_t size>
vec<T,size>& vec<T,size>::operator *= (const vec& v) 
{ 
  component_fn<multiplies<T,T,T> > () (*this,*this,v);
  return *this; 
}

template <class T,size_t size>
vec<T,size>& vec<T,size>::operator /= (const vec& v) 
{ 
  component_fn<divides<T,T,T> > () (*this,*this,v);
  return *this; 
}
        
template <class T,size_t size>
vec<T,size>& vec<T,size>::operator *= (const T& d)
{ 
  component_fn<multiplies<T,T,T> > () (*this,*this,d);
  return *this; 
}

template <class T,size_t size>
vec<T,size>& vec<T,size>::operator /= (const T& d)
{ 
  component_fn<divides<T,T,T> > () (*this,*this,d);
  return *this; 
}

template <class T,size_t size>
const vec<T,size> vec<T,size>::operator + (const vec& v) const  
{ 
  return vec<T,size>(*this,v,component_fn<plus<T,T,T> > ());
}

template <class T,size_t size>
const vec<T,size> vec<T,size>::operator - (const vec& v) const  
{ 
  return vec (*this,v,component_fn<minus<T,T,T> > ());
}

template <class T,size_t size>
const vec<T,size> vec<T,size>::operator * (const vec& v) const  
{ 
  return vec (*this,v,component_fn<multiplies<T,T,T> > ());
}

template <class T,size_t size>
const vec<T,size> vec<T,size>::operator / (const vec& v) const  
{ 
  return vec<T,size> (*this,v,component_fn<divides<T,T,T> > ());
}

template <class T,size_t size>
const vec<T,size> vec<T,size>::operator * (const T& a) const  
{ 
  return vec (*this,a,component_fn<multiplies<T,T,T> > ());
}

template <class T,size_t size> 
const vec<T,size> vec<T,size>::operator / (const T& a) const  
{ 
  return vec (*this,a,component_fn<divides<T,T,T> > ());
}

/*
        Сравнение векторов
*/

template <class T,size_t size>
bool vec<T,size>::operator == (const vec& v) const
{
  return compare_fn<equal_to<T> >() (*this,v);
}

template <class T,size_t size>
bool vec<T,size>::operator != (const vec& v) const
{
  return compare_fn<not_equal_to<T> >() (*this,v) (*this,v);
}

template <class T,size_t size>
vec<T,size>::operator const vec<T,size-1>& () const
{
  return *(const vec<T,size-1>*)this;
}

template <class T,size_t size>
vec<T,size>::operator vec<T,size-1>& ()
{
  return *(vec<T,size-1>*)this;
}

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
  return vec<T,size> (v,component_fn<absol<T> >());
}

template <class T,size_t size>
const vec<T,size> min (const vec<T,size>& a,const vec<T,size>& b)
{
  return vec<T,size> (a,b,component_fn<min_fn<T> > ());
}

template <class T,size_t size>
const vec<T,size> max (const vec<T,size>& a,const vec<T,size>& b)
{
  return vec<T,size> (a,b,component_fn<max_fn<T> > ());
} 

template <class T,size_t size> 
const vec<T,size> normalize (const vec<T,size>& v)
{
  return vec<T,size> (v,vec_length(v),component_fn<divides <T> > ());
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
