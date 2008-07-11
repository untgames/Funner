
/*
  Вспомогательные операции
*/

namespace detail
{

template <class T, size_t Size> T& get_component (math::vec<T, Size>& v, size_t index)
{
  return v [index];
}

template <class T, size_t Size> const T& get_component (const math::vec<T, Size>& v, size_t index)
{
  return v [index];
}

}

/*
        Конструкторы
*/

template <class T,size_t Size>
inline vec<T,Size>::vec ()
{
  make_unary_operation<vec<T,Size> > (T(0),assign<T,T>(),*this); 
}

template <class T,size_t Size> template <size_t Size1>
vec<T,Size>::vec (const vec<T,Size1>& v) 
{
  make_unary_operation<vec<T,Size> > (v,assign<T,T> (),*this);
}

template <class T,size_t Size> 
vec<T,Size>::vec (const T& a)
{
  make_unary_operation<vec<T,Size> > (a,assign<T,T>(),*this);
}

template <class T,size_t Size> 
vec<T,Size>::vec (const base& b)
  : base (b)
{  
}

template <class type,size_t Size> template <size_t Size1, size_t Size2, class Fn>
  inline vec<type,Size>::vec (const vec<type,Size1>v1,const vec<type,Size2>v2,Fn fn)
{
  fn(*this,v1,v2);
}

template<class type,size_t Size> template <size_t Size1,class Fn>
  inline vec<type,Size>::vec (const vec<type,Size1>& v1,const type& arg,Fn fn)
{
  fn(*this,v1,arg);
}

template<class type,size_t Size> template <size_t Size1,class Fn>
  inline vec<type,Size>::vec (const vec<type,Size1>& v1,Fn fn)
{
  fn(*this,v1);
}


template <class type,size_t Size> template <class Fn>
  inline vec<type,Size>::vec (const type& v1,Fn fn)
{
  fn(*this,v1);
}

template<class type,size_t Size> template <class Fn>
inline vec<type,Size>::vec (const quat<type>& q1,const type& arg,Fn fn)
{
  fn(*this,q1,arg);
}


template <class T,size_t Size>
vec<T,Size>::vec (const T& x1,const T& x2,const T& x3,const T& x4)
{
  switch (Size)
  {
    default:
    {
      for (size_t i=0;i<Size-4;i++)
        (*this)[i+4] = T(0);
    }  
    case 4: (*this)[3] = x4;
    case 3: (*this)[2] = x3;
    case 2: (*this)[1] = x2;
    case 1: (*this)[0] = x1;
    case 0: break;
  }
}

template <class T,size_t Size>
vec<T,Size>::vec (const vec<T,Size-1>& v,const T& a)
{
  for (size_t i=0;i<Size-1;i++)
    (*this)[i] = v [i];

  (*this)[Size-1] = a;
}

/*
        Привсаивание
*/

template <class T,size_t Size>
vec<T,Size>& vec<T,Size>::operator = (const base& b)
{
  base::operator = (b);
  return *this;
} 

template <class T,size_t Size> template<class T1>
vec<T,Size>& vec<T,Size>::operator = (const T1& a)
{
  assign<T,T> fn;
  unary_component_function<vec<T,Size>,assign<T,T> > _fn (fn);
  _fn (*this,a);
  return *this;
}

/*
        Базовые унарные опреации
*/

template <class T,size_t Size>
const vec<T,Size> vec<T,Size>::operator + () const
{ 
  return *this; 
}

template <class T,size_t Size>
const vec<T,Size> vec<T,Size>::operator - () const
{ 
  return make_unary_operation<vec<T,Size> >(*this,negate<T,T> ());
}

template <class T,size_t Size> 
T vec<T,Size>::squared_length () const
{ 
    return binary_accumulation_function<T>()(*this,*this,T(0),plus<T>(),multiplies<T>());
}


template <class T,size_t Size> 
T vec<T,Size>::length () const
{ 
  return sqrt(squared_length());
}

/*
        Базовые бинарные операции
*/

template <class T,size_t Size>
inline T const vec<T,Size>::operator & (const vec<T,Size>& right) const
{ 
  return binary_accumulation_function<T>()(*this,right,T(0),plus<T>(),multiplies<T>());
}

template <class T,size_t Size>
inline vec<T,Size>& vec<T,Size>::operator += (const vec& v) 
{ 
  make_binary_operation<vec<T,Size> >(*this,v,plus<T,T,T> (),*this);
  return *this; 
}

template <class T,size_t Size>
vec<T,Size>& vec<T,Size>::operator -= (const vec& v) 
{ 
  make_binary_operation<vec<T,Size> >(*this,v,minus<T,T,T> (),*this);
  return *this; 
}

template <class T,size_t Size>
vec<T,Size>& vec<T,Size>::operator *= (const T& v) 
{ 
  make_binary_operation<vec<T,Size> >(*this,v,multiplies<T,T,T> (),*this);
  return *this; 
}

template <class T,size_t Size>
vec<T,Size>& vec<T,Size>::operator *= (const vec<T,Size>& v) 
{ 
  make_binary_operation<vec<T,Size> >(*this,v,multiplies<T,T,T> (),*this);
  return *this; 
}

template <class T,size_t Size>
vec<T,Size>& vec<T,Size>::operator /= (const T& v) 
{ 
  make_binary_operation<vec<T,Size> >(*this,v,divides<T,T,T> (),*this);
  return *this; 
}

template <class T,size_t Size>
vec<T,Size>& vec<T,Size>::operator /= (const vec<T,Size>& v) 
{ 
  make_binary_operation<vec<T,Size> >(*this,v,divides<T,T,T> (),*this);
  return *this; 
}
        
template <class T,size_t Size>
const vec<T,Size> vec<T,Size>::operator + (const vec<T,Size>& v) const  
{ 
  return make_binary_operation<vec<T,Size> >(*this,v,plus<T,T,T> ());
}

template <class T,size_t Size>
const vec<T,Size> vec<T,Size>::operator - (const vec<T,Size>& v) const  
{ 
  return make_binary_operation<vec<T,Size> >(*this,v,minus<T,T,T> ());
}

template <class T,size_t Size>
const vec<T,Size> vec<T,Size>::operator * (const T& a) const  
{ 
  return make_binary_operation<vec<T,Size> >(*this,a,multiplies<T,T,T> ());
}

template <class T,size_t Size>
const vec<T,Size> vec<T,Size>::operator * (const vec<T,Size>& a) const  
{ 
  return make_binary_operation<vec<T,Size> >(*this,a,multiplies<T,T,T> ());
}

template <class T,size_t Size>
const vec<T,Size> vec<T,Size>::operator / (const T& a) const  
{ 
  return make_binary_operation<vec<T,Size> >(*this,a,divides<T,T,T> ());
}

template <class T,size_t Size>
const vec<T,Size> vec<T,Size>::operator / (const vec<T,Size>& a) const  
{ 
  return make_binary_operation<vec<T,Size> >(*this,a,divides<T,T,T> ());
}

/*
        Сравнение векторов
*/

template <class T,size_t Size>
bool vec<T,Size>::operator == (const vec& v) const
{
  return compare_function<T,equal_to<T> >() (*this,v);
}

template <class T,size_t Size>
bool vec<T,Size>::operator != (const vec& v) const
{
  return compare_function<T,not_equal_to<T> >() (*this,v);
}

template <class T,size_t Size>
bool vec<T,Size>::operator < (const vec& v) const
{
  return compare_function<T,less<T> >() (*this,v);
}

template <class T,size_t Size>
bool vec<T,Size>::operator > (const vec& v) const
{
  return compare_function<T,greater<T> >() (*this,v);
}

template <class T,size_t Size>
bool vec<T,Size>::operator <= (const vec& v) const
{
  return compare_function<T,less_equal<T> >() (*this,v);
}

template <class T,size_t Size>
bool vec<T,Size>::operator >= (const vec& v) const
{
  return compare_function<T,greater_equal<T> >() (*this,v);
}


/*
	Приведение типов
*/

template<class T,size_t Size>
  const matrix<T,Size,1>  to_matrix_column (const vec<T,Size>& src)
{
  matrix<T,Size,1> res;
  for(int i=0;i<Size;i++) res[i][0]=src[i];
  return res;
}

template<class T,size_t Size>
  const matrix<T,1,Size> to_matrix_row (const vec<T,Size>& src)
{
  matrix<T,1,Size> res;
  res[0]=(src);
  return res;
}


/*
  Векторное произведение
*/




/*
        Утилиты
*/


template <class T,size_t Size> 
inline const vec<T,Size> abs (const vec<T,Size>& src)
{
  return make_unary_operation<vec<T,Size> >(src,absol<T> ());
}

template <class T,size_t Size>
const vec<T,Size> min (const vec<T,Size>& a,const vec<T,Size>& b)
{
  return make_binary_operation<vec<T,Size> >(a,b,min_fn<T> ());
}

template <class T,size_t Size>
const vec<T,Size> max (const vec<T,Size>& a,const vec<T,Size>& b)
{
  return make_binary_operation<vec<T,Size> >(a,b,max_fn<T> ());
} 

template <class T,size_t Size> 
const vec<T,Size> normalize(const vec<T, Size>& src)
{
  return make_binary_operation<vec<T,Size> > (src,src.length(),divides <T> ());
}

template <class T,size_t Size> 
bool vec<T,Size>::equal (const vec<T,Size>& b,const T& eps)
{
  return compare_function<T,equal_to<T> >() (*this,b,eps);
}

template <class T,size_t Size>
const T angle (const vec<T,Size>& a,const vec<T,Size>& b)
{
  T ang = acos((a&b)/sqrt(a.squared_length()*b.squared_length()));

  return _isnan(ang)?0:ang;
}

/*
    Векторное произведение
*/


template <class T>
const vec<T,3> operator ^ (const vec<T,3>& a,const vec<T,3>& b)
{
  vec<T,3> res;
  res [0] = a [1] * b [2] - b [1] * a [2];
  res [1] = b [0] * a [2] - b [2] * a [0];
  res [2] = a [0] * b [1] - b [0] * a [1];

  return res;
} 
