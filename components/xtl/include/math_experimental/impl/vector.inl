
/*
	��������������� ��������
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
        ������������
*/

template <class T,size_t size>
inline vec<T,size>::vec ()
{
  *this=make_unary_operation<vec<T,size> > (T(0),assign<T,T>()); 
}

template <class T,size_t size> template <size_t size1>
vec<T,size>::vec (const vec<T,size1>& v) 
{
  *this=make_unary_operation<vec<T,size> > (v,assign<T,T> ());
}

template <class T,size_t size> 
vec<T,size>::vec (const T& a)
{
  *this=make_unary_operation<vec<T,size> > (a,assign<T,T>());
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


template <class type,size_t size> template <class Fn>
	inline vec<type,size>::vec (const type& v1,Fn fn)
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
        ������������
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
        ������� ������� ��������
*/

template <class T,size_t size>
const vec<T,size> vec<T,size>::operator + () const
{ 
  return *this; 
}

template <class T,size_t size>
const vec<T,size> vec<T,size>::operator - () const
{ 
  return make_unary_operation<vec<T,size> >(*this,negate<T,T> ());
}

template <class T,size_t size> 
T vec<T,size>::squared_length () const
{ 
    return binary_accumulation_function<T>()(*this,*this,T(0),plus<T>(),multiplies<T>());
}


template <class T,size_t size> 
T vec<T,size>::length () const
{ 
  return sqrt(squared_length());
}

/*
        ������� �������� ��������
*/

template <class T,size_t size>
T vec<T,size>::operator & (const vec& v) const
{ 
  return binary_accumulation_function<T>()(*this,v,T(0),plus<T>(),multiplies<T>());
}

template <class T,size_t size>
vec<T,size>& vec<T,size>::operator += (const vec& v) 
{ 
  *this=make_binary_operation<vec<T,size> >(*this,v,plus<T,T,T> ());
  return *this; 
}

template <class T,size_t size>
vec<T,size>& vec<T,size>::operator -= (const vec& v) 
{ 
  *this=make_binary_operation<vec<T,size> >(*this,v,minus<T,T,T> ());
  return *this; 
}

template <class T,size_t size> template<class T2>
vec<T,size>& vec<T,size>::operator *= (const T2& v) 
{ 
  *this=make_binary_operation<vec<T,size> >(*this,v,multiplies<T,T,T> ());
  return *this; 
}

template <class T,size_t size> template<class T2>
vec<T,size>& vec<T,size>::operator /= (const T2& v) 
{ 
  *this=make_binary_operation<vec<T,size> >(*this,v,divides<T,T,T> ());
  return *this; 
}
        
/*template <class T,size_t size>
vec<T,size>& vec<T,size>::operator *= (const T& d)
{ 
  *this=make_binary_operation<vec<T,size> >(*this,d,multiplies<T,T,T> ());
  return *this; 
}

template <class T,size_t size>
vec<T,size>& vec<T,size>::operator /= (const T& d)
{ 
  *this=make_binary_operation<vec<T,size> >(*this,d,divides<T,T,T> ());
  return *this; 
} */

template <class T,size_t size>
const vec<T,size> vec<T,size>::operator + (const vec& v) const  
{ 
  return make_binary_operation<vec<T,size> >(*this,v,plus<T,T,T> ());
}

template <class T,size_t size>
const vec<T,size> vec<T,size>::operator - (const vec& v) const  
{ 
  return make_binary_operation<vec<T,size> >(*this,v,minus<T,T,T> ());
}

/*template <class T,size_t size>
const vec<T,size> vec<T,size>::operator * (const vec& v) const  
{ 
  return make_binary_operation<vec<T,size> >(*this,v,multiplies<T,T,T> ());
}

template <class T,size_t size>
const vec<T,size> vec<T,size>::operator / (const vec& v) const  
{ 
  return make_binary_operation<vec<T,size> >(*this,v,multiplies<T,T,T> ());
}*/

template <class T,size_t size> template<class T2>
const vec<T,size> vec<T,size>::operator * (const T2& a) const  
{ 
  return make_binary_operation<vec<T,size> >(*this,a,multiplies<T,T,T> ());
}

template <class T,size_t size> template<class T2>
const vec<T,size> vec<T,size>::operator / (const T2& a) const  
{ 
  return make_binary_operation<vec<T,size> >(*this,a,divides<T,T,T> ());
}

/*
        ��������� ��������
*/

template <class T,size_t size>
bool vec<T,size>::operator == (const vec& v) const
{
  return compare_function<T,equal_to<T> >() (*this,v);
}

template <class T,size_t size>
bool vec<T,size>::operator != (const vec& v) const
{
  return compare_function<T,not_equal_to<T> >() (*this,v);
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
	��������� ������������
*/




/*
        �������
*/

/*template <class T,size_t size> 
T scalar_multiply (const vec<T,size>& a,const vec<T,size>& b)
{
  return vec_dot_product (a,b);
} */

template <class T,size_t size> 
const vec<T,size> vec<T,size>::abs ()
{
  return make_unary_operation<vec<T,size> >(*this,absol<T> ());
}

template <class T,size_t size>
const vec<T,size> min (const vec<T,size>& a,const vec<T,size>& b)
{
  return make_binary_operation<vec<T,size> >(a,b,min_fn<T> ());
}

template <class T,size_t size>
const vec<T,size> max (const vec<T,size>& a,const vec<T,size>& b)
{
  return make_binary_operation<vec<T,size> >(a,b,max_fn<T> ());
} 

template <class T,size_t size> 
const vec<T,size> vec<T,size>::normalize() const
{
  return make_binary_operation<vec<T,size> > (*this,length(),divides <T> ());
}

template <class T,size_t size> 
bool vec<T,size>::equal (const vec<T,size>& b,const T& eps)
{
  return compare_function<T,equal_to<T> >() (*this,b,eps);
}

template <class T,size_t size>
T angle (const vec<T,size>& a,const vec<T,size>& b)
{
  T ang = acos((a&b)/sqrt(a.squared_length()*b.squared_length()));

  return isnan(ang)?0:ang;
}

/*
    ��������� ������������
*/


template <class T>
vec<T,3> operator ^ (const vec<T,3>& a,const vec<T,3>& b)
{
  vec<T,3> res;
  res [0] = a [1] * b [2] - b [1] * a [2];
  res [1] = b [0] * a [2] - b [2] * a [0];
  res [2] = a [0] * b [1] - b [0] * a [1];

  return res;
} 

template <class type>
vec<type,4> operator ^ (const vec<type,4>& a,const vec<type,4>& b)
{
  return vec<type,4> (a,b,vec_cross_product<type>);
}
