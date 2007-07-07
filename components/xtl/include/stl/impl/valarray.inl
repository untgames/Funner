/*
    Базовые конструкторы / деструктор
*/

template <class T>
inline valarray<T>::valarray ()
  : first (0), array_size (0), reserved_size (0)
  { }
 
template <class T>
inline valarray<T>::valarray (size_t size)
  : first (0), array_size (0), reserved_size (0)
{
  resize (size);  
}

template <class T>
inline valarray<T>::valarray (const value_type& x,size_t size)
  : first (0), array_size (0), reserved_size (0)
{
  resize (size,x);
}

template <class T>
inline valarray<T>::valarray (const value_type* p,size_t size)
  : first (0), array_size (0), reserved_size (0)
{
  resize (size);  
  copy   (p,p+size,first);
}

template <class T>
inline valarray<T>::valarray (const valarray& x)
  : first (0), array_size (0), reserved_size (0)  
{
  resize (x.size ());
  copy   (x.first,x.first+x.size (),first);
}

template <class T>
inline valarray<T>::~valarray ()
{
  clear ();
}

/*
    Конструктор proxy_array
*/

template <class T> template <class Proxy>
inline valarray<T>::valarray (const proxy_array<T,Proxy>& array)
  : first (0), array_size (0), reserved_size (0)
{
  operator = (array);
}

/*
    Конструкторы используемые для оптимизации возвращаемого значения
*/

template <class T> template <class T1,class UnaryOperation>
inline valarray<T>::valarray (const valarray<T1>& x,UnaryOperation op)
  : first (0), array_size (0), reserved_size (0)
{  
  resize    (x.size ());
  transform (x.first,x.first+x.size (),first,op);
}

template <class T> template <class T1,class T2,class BinOperation>
inline valarray<T>::valarray (const valarray<T1>& x,const valarray<T2>& y,BinOperation op)
  : first (0), array_size (0), reserved_size (0)
{ 
  resize    (stl::min (x.size (),y.size ()));
  transform (x.first,x.first+size (),y.first,first,op);
}

template <class T> template <class T1,class T2,class BinOperation>
inline valarray<T>::valarray (const valarray<T1>& x,const T2& y,BinOperation op)
  : first (0), array_size (0), reserved_size (0)
{ 
  resize (x.size ());

  for (size_t i=0;i<size ();i++) (*this)[i] = op (x [i],y);
}

template <class T> template <class T1,class T2,class BinOperation>
inline valarray<T>::valarray (const T1& x,const valarray<T2>& y,BinOperation op)
  : first (0), array_size (0), reserved_size (0)
{ 
  resize (y.size ());

  for (size_t i=0;i<size ();i++) (*this)[i] = op (x,y [i]);
}

/*
    Изменение размера
*/

template <class T>
void valarray<T>::resize (size_t size)
{
  if (!size)    
  { 
    clear ();
    return;
  }
  
  if (size > reserved_size)
  {
    value_type* new_ptr = 0;
  
    try
    {
      new_ptr = new value_type [size];    
    }
    catch (...)
    {
      clear ();
      throw;
    }    
    
    for (size_t i=0;i<array_size;i++) new_ptr [i] = first [i];

    clear ();

    first         = new_ptr;
    reserved_size = size;    
  }    
  
  array_size = size;
}

template <class T>
void valarray<T>::resize (size_t size,value_type val)
{
  size_t old_size = array_size;
  
  resize (size);
  
  if (old_size < size)  
    fill (first+old_size,first+size,val);
}

template <class T>
void valarray<T>::clear ()
{
  delete [] first;
  
  first         = 0;
  array_size    = 0;
  reserved_size = 0;
}

/*
    Операции индексирования
*/

template <class T>
inline typename valarray<T>::value_type valarray<T>::operator [] (size_t i) const
{
  return first [i];
}

template <class T>
inline typename valarray<T>::value_type& valarray<T>::operator [] (size_t i)
{
  return first [i];
}

/*
    Присваивание
*/

template <class T>
valarray<T>& valarray<T>::operator = (const valarray& x)
{
  if (&x == this)
    return *this;

  if (x.size () != size ())
  {
    clear ();
    resize (x.size ());
  }
    
  copy (x.first,x.first+x.size (),first);
  
  return *this;
}

template <class T>
inline valarray<T>& valarray<T>::operator = (const value_type& x)
{
  fill_n (first,size (),x);
  return *this;
}

template <class T> template <class Proxy> 
inline valarray<T>& valarray<T>::operator = (const proxy_array<T,Proxy>& x)
{
  assign (x.proxy,x.array);
  return *this;
}

/*
    Преобразования for_each/apply 
*/

template <class T> template <class BinOperation> 
inline valarray<T>& valarray<T>::for_each (const valarray& x,BinOperation op)
{
  if (x.size () != size ())
    resize (stl::min (x.size (),size ()));
    
  transform (first,first+size (),x.first,first,op);  
  
  return *this;
}

template <class T> template <class BinOperation> 
inline valarray<T>& valarray<T>::for_each (const value_type& x,BinOperation op)
{   
  for (int i=0;i<size ();i++) op ((*this)[i],x);
  
  return *this;
}

template <class T>
inline valarray<T> valarray<T>::apply (value_type f (value_type)) const
{
  return valarray (*this,f);
}

template <class T>
inline valarray<T> valarray<T>::apply (value_type f (const value_type&)) const
{
  return valarray (*this,f);
}

/*
    Статистики
*/

template <class T>
inline typename valarray<T>::value_type valarray<T>::sum () const
{
  return accumulate (first,first+size (),T (0));
}

template <class T>
inline typename valarray<T>::value_type valarray<T>::min () const
{
  return *min_element (first,first+size ());
}

template <class T>
inline typename valarray<T>::value_type valarray<T>::max () const
{
  return *max_element (first,first+size ());
}

/*
    Сдвиги
*/

template <class T>
valarray<T> valarray<T>::shift (int n) const
{
  valarray tmp (value_type (),size ());

  if (n >= 0) 
  {
    if (static_cast<size_t> (n) < size()) 
      copy (first+n,first+size(),tmp.first);
  }
  else 
  {
    if (static_cast<size_t> (-n) < size ())
      copy (first,first+size ()+n,tmp.first-n);
  }
  
  return tmp;
}

template <class T>
valarray<T> valarray<T>::cshift (int m) const
{
  valarray tmp (value_type (),size ());

   //from SGI STL
  // Reduce m to an equivalent number in the range [0, size()).  We
  // have to be careful with negative numbers, since the sign of a % b
  // is unspecified when a < 0.
  
  long n = m;
  
  if (size () < static_cast<size_t> (numeric_limits<long>::max())) n %= long (size());
  if (n < 0) n += size();

  copy (first,first+n,tmp.first+(size ()-n));
  copy (first+n,first+size (),tmp.first);

  return tmp;
}

/*
    Унарные операции
*/

template <class T> 
inline T __valarray_positive (const T& x)
{ 
  return +x; 
}

template <class T>
inline valarray<T> valarray<T>::operator + () const
{
  return valarray (*this,__valarray_positive<T>);
}

template <class T>
inline valarray<T> valarray<T>::operator - () const
{
  return valarray (*this,negate<T> ());
}

template <class T>
inline valarray<T> valarray<T>::operator ~ () const
{
  return valarray (*this,radix_not<T> ());
}

template <class T>
inline valarray<bool> valarray<T>::operator ! () const
{
  return valarray<bool> (*this,logical_not<T> ());
}

/*
    Основные бинарные операции между вектором и скаляром
*/

template <class T> inline valarray<T>& valarray<T>::operator *= (const value_type& x)
{
  return for_each (x,assign_multiplies<T> ());
}

template <class T> inline valarray<T>& valarray<T>::operator /= (const value_type& x)
{
  return for_each (x,assign_divides<T> ());
}

template <class T> inline valarray<T>& valarray<T>::operator %= (const value_type& x)
{
  return for_each (x,assign_modulus<T> ());
}

template <class T> inline valarray<T>& valarray<T>::operator += (const value_type& x)
{
  return for_each (x,assign_plus<T> ());
}

template <class T> inline valarray<T>& valarray<T>::operator -= (const value_type& x)
{
  return for_each (x,assign_minus<T> ());
}

template <class T> inline valarray<T>& valarray<T>::operator ^= (const value_type& x)
{
  return for_each (x,assign_radix_xor<T> ());
}

template <class T> inline valarray<T>& valarray<T>::operator |= (const value_type& x)
{
  return for_each (x,assign_radix_or<T> ());
}

template <class T> inline valarray<T>& valarray<T>::operator &= (const value_type& x)
{
  return for_each (x,assign_radix_and<T> ());
}

template <class T> inline valarray<T>& valarray<T>::operator <<= (const value_type& x)
{
  return for_each (x,assign_radix_left_shift<T> ());
}

template <class T> inline valarray<T>& valarray<T>::operator >>= (const value_type& x)
{
  return for_each (x,assign_radix_right_shift<T> ());
}

template <class T> inline valarray<T> valarray<T>::operator * (const value_type& x) const
{
  return valarray (*this,x,multiplies<T> ());
}

template <class T> inline valarray<T> valarray<T>::operator / (const value_type& x) const
{
  return valarray (*this,x,divides<T> ());
}

template <class T> inline valarray<T> valarray<T>::operator % (const value_type& x) const
{
  return valarray (*this,x,modulus<T> ());
}

template <class T> inline valarray<T> valarray<T>::operator + (const value_type& x) const
{
  return valarray (*this,x,plus<T> ());
}

template <class T> inline valarray<T> valarray<T>::operator - (const value_type& x) const
{
  return valarray (*this,x,minus<T> ());
}

template <class T> inline valarray<T> valarray<T>::operator ^ (const value_type& x) const
{
  return valarray (*this,x,radix_xor<T> ());
}

template <class T> inline valarray<T> valarray<T>::operator | (const value_type& x) const
{
  return valarray (*this,x,radix_or<T> ());
}

template <class T> inline valarray<T> valarray<T>::operator & (const value_type& x) const
{
  return valarray (*this,x,radix_and<T> ());
}

template <class T> inline valarray<T> valarray<T>::operator << (const value_type& x) const
{
  return valarray (*this,x,radix_left_shift<T> ());
}

template <class T> inline valarray<T> valarray<T>::operator >> (const value_type& x) const
{
  return valarray (*this,x,radix_right_shift<T> ());
}

template <class T> inline valarray<T> operator * (const T& x,const valarray<T>& y)
{
  return valarray<T> (x,y,multiplies<T> ());
}

template <class T> inline valarray<T> operator / (const T& x,const valarray<T>& y)
{
  return valarray<T> (x,y,divides<T> ());
}

template <class T> inline valarray<T> operator % (const T& x,const valarray<T>& y)
{
  return valarray<T> (x,y,modulus<T> ());
}

template <class T> inline valarray<T> operator + (const T& x,const valarray<T>& y)
{
  return valarray<T> (x,y,plus<T> ());
}

template <class T> inline valarray<T> operator - (const T& x,const valarray<T>& y)
{
  return valarray<T> (x,y,minus<T> ());
}

template <class T> inline valarray<T> operator ^ (const T& x,const valarray<T>& y)
{
  return valarray<T> (x,y,radix_xor<T> ());
}

template <class T> inline valarray<T> operator & (const T& x,const valarray<T>& y)
{
  return valarray<T> (x,y,radix_and<T> ());
}

template <class T> inline valarray<T> operator | (const T& x,const valarray<T>& y)
{
  return valarray<T> (x,y,radix_or<T> ());
}

template <class T> inline valarray<T> operator << (const T& x,const valarray<T>& y)
{
  return valarray<T> (x,y,radix_left_shift<T> ());
}

template <class T> inline valarray<T> operator >> (const T& x,const valarray<T>& y)
{
  return valarray<T> (x,y,radix_right_shift<T> ());
}

/*
    Основные бинарные операции между двумя векторами
*/

template <class T> inline valarray<T>& valarray<T>::operator *= (const valarray& x)
{
  return for_each (x,assign_multiplies<T> ());
}

template <class T> inline valarray<T>& valarray<T>::operator /= (const valarray& x)
{
  return for_each (x,assign_divides<T> ());
}

template <class T> inline valarray<T>& valarray<T>::operator %= (const valarray& x)
{
  return for_each (x,assign_modulus<T> ());
}

template <class T> inline valarray<T>& valarray<T>::operator += (const valarray& x)
{
  return for_each (x,assign_plus<T> ());
}

template <class T> inline valarray<T>& valarray<T>::operator -= (const valarray& x)
{
  return for_each (x,assign_minus<T> ());
}

template <class T> inline valarray<T>& valarray<T>::operator ^= (const valarray& x)
{
  return for_each (x,assign_radix_xor<T> ());
}

template <class T> inline valarray<T>& valarray<T>::operator |= (const valarray& x)
{
  return for_each (x,assign_radix_or<T> ());
}

template <class T> inline valarray<T>& valarray<T>::operator &= (const valarray& x)
{
  return for_each (x,assign_radix_and<T> ());
}

template <class T> inline valarray<T>& valarray<T>::operator <<= (const valarray& x)
{
  return for_each (x,assign_radix_left_shift<T> ());
}

template <class T> inline valarray<T>& valarray<T>::operator >>= (const valarray& x)
{
  return for_each (x,assign_radix_right_shift<T> ());
}

template <class T> inline valarray<T> valarray<T>::operator * (const valarray& x) const
{
  return valarray (*this,x,multiplies<T> ());
}

template <class T> inline valarray<T> valarray<T>::operator / (const valarray& x) const
{
  return valarray (*this,x,divides<T> ());
}

template <class T> inline valarray<T> valarray<T>::operator % (const valarray& x) const
{
  return valarray (*this,x,modulus<T> ());
}

template <class T> inline valarray<T> valarray<T>::operator + (const valarray& x) const
{
  return valarray (*this,x,plus<T> ());
}

template <class T> inline valarray<T> valarray<T>::operator - (const valarray& x) const
{
  return valarray (*this,x,minus<T> ());
}

template <class T> inline valarray<T> valarray<T>::operator ^ (const valarray& x) const
{
  return valarray (*this,x,radix_xor<T> ());
}

template <class T> inline valarray<T> valarray<T>::operator | (const valarray& x) const
{
  return valarray (*this,x,radix_or<T> ());
}

template <class T> inline valarray<T> valarray<T>::operator & (const valarray& x) const
{
  return valarray (*this,x,radix_and<T> ());
}

template <class T> inline valarray<T> valarray<T>::operator << (const valarray& x) const
{
  return valarray (*this,x,radix_left_shift<T> ());
}

template <class T> inline valarray<T> valarray<T>::operator >> (const valarray& x) const
{
  return valarray (*this,x,radix_right_shift<T> ());
}

/*
    Логические операции между вектором и скаляром
*/

template <class T> inline valarray<bool> valarray<T>::operator == (const value_type& x) const
{
  return valarray<bool> (*this,x,equal_to<T> ());
}

template <class T> inline valarray<bool> valarray<T>::operator != (const value_type& x) const
{
  return valarray<bool> (*this,x,not_equal_to<T> ());
}

template <class T> inline valarray<bool> valarray<T>::operator < (const value_type& x) const
{
  return valarray<bool> (*this,x,less<T> ());
}

template <class T> inline valarray<bool> valarray<T>::operator > (const value_type& x) const
{
  return valarray<bool> (*this,x,greater<T> ());
}

template <class T> inline valarray<bool> valarray<T>::operator <= (const value_type& x) const
{
  return valarray<bool> (*this,x,less_equal<T> ());
}

template <class T> inline valarray<bool> valarray<T>::operator >= (const value_type& x) const
{
  return valarray<bool> (*this,x,greater_equal<T> ());
}

template <class T> inline valarray<bool> valarray<T>::operator && (const value_type& x) const
{
  return valarray<bool> (*this,x,logical_and<T> ());
}

template <class T> inline valarray<bool> valarray<T>::operator || (const value_type& x) const
{
  return valarray<bool> (*this,x,logical_or<T> ());
}

template <class T> inline valarray<bool> operator == (const T& x,const valarray<T>& y)
{
  return valarray<bool> (x,y,equal_to<T> ());
}

template <class T> inline valarray<bool> operator != (const T& x,const valarray<T>& y)
{
  return valarray<bool> (x,y,not_equal_to<T> ());
}

template <class T> inline valarray<bool> operator < (const T& x,const valarray<T>& y)
{
  return valarray<bool> (x,y,less<T> ());
}

template <class T> inline valarray<bool> operator > (const T& x,const valarray<T>& y)
{
  return valarray<bool> (x,y,greater<T> ());
}

template <class T> inline valarray<bool> operator <= (const T& x,const valarray<T>& y)
{
  return valarray<bool> (x,y,less_equal<T> ());
}

template <class T> inline valarray<bool> operator >= (const T& x,const valarray<T>& y)
{
  return valarray<bool> (x,y,greater_equal<T> ());
}

template <class T> inline valarray<bool> operator && (const T& x,const valarray<T>& y)
{
  return valarray<bool> (x,y,logical_and<T> ());
}

template <class T> inline valarray<bool> operator || (const T& x,const valarray<T>& y)
{
  return valarray<bool> (x,y,logical_or<T> ());
}

/*
    Логические операции между двумя векторами
*/

template <class T> inline valarray<bool> valarray<T>::operator == (const valarray& x) const
{
  return valarray<bool> (*this,x,equal_to<T> ());
}

template <class T> inline valarray<bool> valarray<T>::operator != (const valarray& x) const
{
  return valarray<bool> (*this,x,not_equal_to<T> ());
}

template <class T> inline valarray<bool> valarray<T>::operator < (const valarray& x) const
{
  return valarray<bool> (*this,x,less<T> ());
}

template <class T> inline valarray<bool> valarray<T>::operator > (const valarray& x) const
{
  return valarray<bool> (*this,x,greater<T> ());
}

template <class T> inline valarray<bool> valarray<T>::operator <= (const valarray& x) const
{
  return valarray<bool> (*this,x,less_equal<T> ());
}

template <class T> inline valarray<bool> valarray<T>::operator >= (const valarray& x) const
{
  return valarray<bool> (*this,x,greater_equal<T> ());
}

template <class T> inline valarray<bool> valarray<T>::operator && (const valarray& x) const
{
  return valarray<bool> (*this,x,logical_and<T> ());
}

template <class T> inline valarray<bool> valarray<T>::operator || (const valarray& x) const
{
  return valarray<bool> (*this,x,logical_or<T> ());
}

/*
    Основные математические функции
*/

template <class T> inline valarray<T> abs (const valarray<T>& x)
{
  return x.apply (math_adaptors::abs<T>);
}

template <class T> inline valarray<T> acos (const valarray<T>& x) 
{
  return x.apply (math_adaptors::acos<T>);
}

template <class T> inline valarray<T> asin (const valarray<T>& x) 
{
  return x.apply (math_adaptors::asin<T>);
}

template <class T> inline valarray<T> atan (const valarray<T>& x) 
{
  return x.apply (math_adaptors::atan<T>);
}

template <class T> valarray<T> atan2 (const valarray<T>& x,const valarray<T>& y)
{
  return valarray<T> (x,y,math_adaptors::atan2<T>);
}

template <class T> valarray<T> atan2 (const valarray<T>& x,const T& y)
{
  return valarray<T> (x,y,math_adaptors::atan2<T>);
}

template <class T> valarray<T> atan2 (const T& x,const valarray<T>& y)
{
  return valarray<T> (x,y,math_adaptors::atan2<T>);
}

template <class T> inline valarray<T> cos (const valarray<T>& x) 
{
  return x.apply (math_adaptors::cos<T>);
}

template <class T> inline valarray<T> cosh (const valarray<T>& x) 
{
  return x.apply (math_adaptors::cosh<T>);
}

template <class T> inline valarray<T> exp (const valarray<T>& x) 
{
  return x.apply (math_adaptors::exp<T>);
}

template <class T> inline valarray<T> log (const valarray<T>& x) 
{
  return x.apply (math_adaptors::log<T>);
}

template <class T> inline valarray<T> log10 (const valarray<T>& x) 
{
  return x.apply (math_adaptors::log10<T>);
}

template <class T> valarray<T> pow (const valarray<T>& x,const valarray<T>& y)
{
  return valarray<T> (x,y,math_adaptors::pow<T>);
}

template <class T> valarray<T> pow (const valarray<T>& x,const T& y)
{
  return valarray<T> (x,y,math_adaptors::pow<T>);
}

template <class T> valarray<T> pow (const T& x,const valarray<T>& y)
{
  return valarray<T> (x,y,math_adaptors::pow<T>);
}

template <class T> inline valarray<T> sin (const valarray<T>& x) 
{
  return x.apply (math_adaptors::sin<T>);
}

template <class T> inline valarray<T> sinh (const valarray<T>& x) 
{
  return x.apply (math_adaptors::sinh<T>);
}

template <class T> inline valarray<T> sqrt (const valarray<T>& x) 
{
  return x.apply (math_adaptors::sqrt<T>);
}

template <class T> inline valarray<T> tan (const valarray<T>& x) 
{
  return x.apply (math_adaptors::tan<T>);
}

template <class T> inline valarray<T> tanh (const valarray<T>& x) 
{
  return x.apply (math_adaptors::tanh<T>);
}

/*
    Работа с slice
*/

template <class T> template <class Func>
inline void valarray<T>::for_each (const slice& s,const valarray& x,Func fn)
{ 
  if (s.start () > size ())
    return ;    

    //коррекция диапозона элементов (отсечения выхода за границы)
  size_t count = stl::min (stl::min ((size ()-s.start ())/s.stride (),x.size ()),s.size ());
    
  value_type*       dst = first + s.start ();
  const value_type* src = x.first;  
  
  for (size_t step=s.stride ();count--;dst+=step,src++) fn (*dst,*src);
}

template <class T> template <class Func>
inline void valarray<T>::for_each (const slice& s,const value_type& val,Func fn)
{ 
  if (s.start () > size ())
    return ;

    //коррекция диапозона элементов (отсечения выхода за границы)    
  size_t count = stl::min ((size ()-s.start ())/s.stride (),s.size ());

  value_type* dst = first + s.start ();
  
  for (const size_t step=s.stride ();count--;dst+=step) fn (*dst,val);
}

template <class T>
inline void valarray<T>::assign (const slice& s,const valarray& x)
{     
  clear ();

  if (s.start () > x.size ())
    return ;

    //коррекция диапозона элементов (отсечения выхода за границы)
  size_t count = stl::min ((x.size ()-s.start ())/s.stride (),s.size ());

  resize (count);
  
  value_type*       dst = first;
  const value_type* src = x.first+s.start ();
    
  for (const size_t step=s.stride ();count--;src+=step,dst++) *dst = *src;
}

template <class T> 
inline valarray<T> valarray<T>::operator [] (slice s) const
{
  return slice_array<T> (s,*this);   
}

template <class T> 
inline slice_array<T> valarray<T>::operator [] (slice s)
{
  return slice_array<T> (s,*this);
}

/*
    Работа gslice_array (general slice array)
*/

template <class T> template <class Func>
inline void valarray<T>::for_each (const gslice& s,const valarray& x,Func fn)
{   
  size_t       count   = stl::min (s.size ().size (),s.stride ().size ()),
               start   = s.start ();
  const size_t *len    = s.size ().first,
               *stride = s.stride ().first;

  while (count--) for_each (slice (start,*len++,*stride++),x,fn);
}

template <class T> template <class Func>
inline void valarray<T>::for_each (const gslice& s,const value_type& x,Func fn)
{   
  size_t       count   = stl::min (s.size ().size (),s.stride ().size ()),
               start   = s.start ();
  const size_t *len    = s.size ().first,
               *stride = s.stride ().first;

  while (count--) for_each (slice (start,*len++,*stride++),x,fn);
}

template <class T>
void valarray<T>::assign (const gslice& s,const valarray& x)
{   
  clear ();
  
  if (s.start () > x.size ())
    return ;
    
  size_t slice_count = stl::min (s.size ().size (),s.stride ().size ()),
         max_size    = x.size ()-s.start (); //максимально возможный размер среза
  
    //рассчёт нового размера массива
    
  size_t new_size = 0;

  const size_t *len    = s.size ().first,
               *stride = s.stride ().first;  
                
  for (size_t i=0;i<slice_count;i++,len++,stride++)
    new_size += stl::min (max_size / *stride,*len);
    
  resize (new_size,0);
        
    //копирование
    
  value_type *dst = first;  
  
  len    = s.size ().first,
  stride = s.stride ().first;

  for (size_t i=0;i<slice_count;i++,len++,stride++)
  { 
    const value_type* src  = x.first+s.start ();
    size_t            size = stl::min (max_size / *stride,*len);    
    
    for (const size_t step=*stride;size--;src+=step,dst++) *dst = *src;
  }  
}

template <class T> 
inline valarray<T> valarray<T>::operator [] (const gslice& s) const
{
  return gslice_array<T> (s,*this);
}

template <class T> 
inline gslice_array<T> valarray<T>::operator [] (const gslice& s)
{
  return gslice_array<T> (s,*this);
}

/*
    Работа с mask_array
*/

template <class T> template <class Func>
inline void valarray<T>::for_each (const valarray<bool>& mask,const valarray& x,Func fn)
{ 
  size_t            count = stl::min (stl::min (mask.size (),size ()),x.size ());
  value_type*       dst   = first;
  const value_type* src   = x.first;
  bool*             flag  = mask.first;
  
  for (;count--;dst++,src++,flag++)
    if (*flag)
      fn (*dst,*src);
}

template <class T> template <class Func>
inline void valarray<T>::for_each (const valarray<bool>& mask,const value_type& val,Func fn)
{ 
  size_t      count = stl::min (mask.size (),size ());
  value_type* dst   = first;
  bool*       flag  = mask.first;
  
  for (;count--;dst++,flag++)
    if (*flag)
      fn (*dst,val);
}

template <class T>
inline void valarray<T>::assign (const valarray<bool>& mask,const valarray& x)
{ 
  size_t count = stl::min (mask.size (),x.size ());  
  
  clear ();
  resize (count);
    
  value_type*       dst   = first;
  const value_type* src   = x.first;  
  bool*             flag  = mask.first; 
  
  for (;count--;dst++,src++,flag++)
    if (*flag)
      *dst = *src;  
}

template <class T> 
inline valarray<T> valarray<T>::operator [] (const valarray<bool>& x) const
{
  return mask_array<T> (x,*this);   
}

template <class T> 
inline mask_array<T> valarray<T>::operator [] (const valarray<bool>& x)
{
  return mask_array<T> (x,*this);
}

/*
    Работа с indirect_array
*/

template <class T> template <class Func>
inline void valarray<T>::for_each (const valarray<size_t>& ref,const valarray& x,Func fn)
{ 
  const value_type* src   = x.first;
  const size_t*     index = ref.first;
  size_t            count = stl::min (ref.size (),x.size ());
 
  while (count--) fn (first [*index++],*src++);
}

template <class T> template <class Func>
inline void valarray<T>::for_each (const valarray<size_t>& ref,const value_type& val,Func fn)
{ 
  const size_t *index = ref.first;
  size_t        count = ref.size ();
  
  while (count--) fn (first [*index++],val);
}

template <class T>
inline void valarray<T>::assign (const valarray<size_t>& ref,const valarray& x)
{     
  size_t count = ref.size ();

  clear  ();
  resize (count);
  
  value_type*       dst   = first;
  const value_type* src   = x.first;
  const size_t*     index = ref.first;
    
  while (count--) *dst++ = src [*index++];
}

template <class T> 
inline valarray<T> valarray<T>::operator [] (const valarray<size_t>& x) const
{
  return indirect_array<T> (x,*this);   
}

template <class T> 
inline indirect_array<T> valarray<T>::operator [] (const valarray<size_t>& x)
{
  return indirect_array<T> (x,*this);
}

/*
    Proxy array
*/

template <class T,class Proxy>
inline proxy_array<T,Proxy>::proxy_array (const Proxy& _proxy,array_type& _array)
  : proxy (_proxy), array (_array)
  { }
  
template <class T,class Proxy> template <class BinOperation>
inline const proxy_array<T,Proxy>& proxy_array<T,Proxy>::for_each (const array_type& x,BinOperation op) const
{
  array.for_each (proxy,x,op);
  return *this;
}

template <class T,class Proxy> template <class BinOperation>
inline const proxy_array<T,Proxy>& proxy_array<T,Proxy>::for_each (value_type x,BinOperation op) const
{
  array.for_each (proxy,x,op);
  return *this;
}

template <class T,class Proxy>
inline const proxy_array<T,Proxy>& proxy_array<T,Proxy>::operator = (const array_type& x) const
{
  return for_each (x,assign<T> ());
}

template <class T,class Proxy>
inline const proxy_array<T,Proxy>& proxy_array<T,Proxy>::operator = (const value_type& x) const
{
  return for_each (x,assign<T> ());
}

template <class T,class Proxy>
inline const proxy_array<T,Proxy>& proxy_array<T,Proxy>::operator *= (const array_type& x) const
{
  return for_each (x,assign_multiplies<T> ());
}

template <class T,class Proxy>
inline const proxy_array<T,Proxy>& proxy_array<T,Proxy>::operator /= (const array_type& x) const
{
  return for_each (x,assign_divides<T> ());
}

template <class T,class Proxy>
inline const proxy_array<T,Proxy>& proxy_array<T,Proxy>::operator %= (const array_type& x) const
{
  return for_each (x,assign_modulus<T> ());
}

template <class T,class Proxy>
inline const proxy_array<T,Proxy>& proxy_array<T,Proxy>::operator += (const array_type& x) const
{
  return for_each (x,assign_plus<T> ());
}

template <class T,class Proxy>
inline const proxy_array<T,Proxy>& proxy_array<T,Proxy>::operator -= (const array_type& x) const
{
  return for_each (x,assign_minus<T> ());
}

template <class T,class Proxy>
inline const proxy_array<T,Proxy>& proxy_array<T,Proxy>::operator ^= (const array_type& x) const
{
  return for_each (x,assign_radix_xor<T> ());
}

template <class T,class Proxy>
inline const proxy_array<T,Proxy>& proxy_array<T,Proxy>::operator &= (const array_type& x) const
{
  return for_each (x,assign_radix_and<T> ());
}

template <class T,class Proxy>
inline const proxy_array<T,Proxy>& proxy_array<T,Proxy>::operator |= (const array_type& x) const
{
  return for_each (x,assign_radix_or<T> ());
}

template <class T,class Proxy>
inline const proxy_array<T,Proxy>& proxy_array<T,Proxy>::operator <<= (const array_type& x) const
{
  return for_each (x,assign_radix_left_shift<T> ());
}

template <class T,class Proxy>
inline const proxy_array<T,Proxy>& proxy_array<T,Proxy>::operator >>= (const array_type& x) const
{
  return for_each (x,assign_radix_right_shift<T> ());
}

template <class T,class Proxy>
inline typename proxy_array<T,Proxy>::array_type proxy_array<T,Proxy>::operator + () const
{
  return +valarray<T> (*this);
}

template <class T,class Proxy>
inline typename proxy_array<T,Proxy>::array_type proxy_array<T,Proxy>::operator - () const
{
  return -valarray<T> (*this);
}

template <class T,class Proxy>
inline typename proxy_array<T,Proxy>::array_type proxy_array<T,Proxy>::operator ~ () const
{
  return ~valarray<T> (*this);
}
    
template <class T,class Proxy>    
inline typename proxy_array<T,Proxy>::array_type proxy_array<T,Proxy>::operator ! () const
{
  return !valarray<T> (*this);
}

template <class T,class Proxy>
inline typename proxy_array<T,Proxy>::array_type proxy_array<T,Proxy>::operator * (const value_type& x) const
{
  return array_type (*this) * x;
}
    
template <class T,class Proxy>    
inline typename proxy_array<T,Proxy>::array_type proxy_array<T,Proxy>::operator / (const value_type& x) const
{
  return array_type (*this) / x;
}
    
template <class T,class Proxy>    
inline typename proxy_array<T,Proxy>::array_type proxy_array<T,Proxy>::operator % (const value_type& x) const
{
  return array_type (*this) % x;
}
    
template <class T,class Proxy>    
inline typename proxy_array<T,Proxy>::array_type proxy_array<T,Proxy>::operator + (const value_type& x) const
{
  return array_type (*this) + x;
}
    
template <class T,class Proxy>    
inline typename proxy_array<T,Proxy>::array_type proxy_array<T,Proxy>::operator - (const value_type& x) const
{
  return array_type (*this) - x;
}
    
template <class T,class Proxy>    
inline typename proxy_array<T,Proxy>::array_type proxy_array<T,Proxy>::operator ^ (const value_type& x) const
{
  return array_type (*this) ^ x;
}
    
template <class T,class Proxy>    
inline typename proxy_array<T,Proxy>::array_type proxy_array<T,Proxy>::operator & (const value_type& x) const
{
  return array_type (*this) & x;
}
    
template <class T,class Proxy>    
inline typename proxy_array<T,Proxy>::array_type proxy_array<T,Proxy>::operator | (const value_type& x) const
{
  return array_type (*this) | x;
}
    
template <class T,class Proxy>    
inline typename proxy_array<T,Proxy>::array_type proxy_array<T,Proxy>::operator << (const value_type& x) const
{
  return array_type (*this) << x;
}

template <class T,class Proxy>    
inline typename proxy_array<T,Proxy>::array_type proxy_array<T,Proxy>::operator >> (const value_type& x) const
{
  return array_type (*this) >> x;
}

template <class T,class Proxy> 
inline valarray<T> operator * (const T& x,const proxy_array<T,Proxy>& y)
{
  return x * valarray<T> (y);
}

template <class T,class Proxy> 
inline valarray<T> operator / (const T& x,const proxy_array<T,Proxy>& y)
{
  return x / valarray<T> (y);
}

template <class T,class Proxy> 
inline valarray<T> operator % (const T& x,const proxy_array<T,Proxy>& y)
{
  return x % valarray<T> (y);
}

template <class T,class Proxy> 
inline valarray<T> operator + (const T& x,const proxy_array<T,Proxy>& y)
{
  return x + valarray<T> (y);
}

template <class T,class Proxy> 
inline valarray<T> operator - (const T& x,const proxy_array<T,Proxy>& y)
{
  return x - valarray<T> (y);
}

template <class T,class Proxy> 
inline valarray<T> operator ^ (const T& x,const proxy_array<T,Proxy>& y)
{
  return x ^ valarray<T> (y);
}

template <class T,class Proxy> 
inline valarray<T> operator & (const T& x,const proxy_array<T,Proxy>& y)
{
  return x & valarray<T> (y);
}

template <class T,class Proxy> 
inline valarray<T> operator | (const T& x,const proxy_array<T,Proxy>& y)
{
  return x | valarray<T> (y);
}

template <class T,class Proxy> 
inline valarray<T> operator << (const T& x,const proxy_array<T,Proxy>& y)
{
  return x << valarray<T> (y);
}

template <class T,class Proxy> 
inline valarray<T> operator >> (const T& x,const proxy_array<T,Proxy>& y)
{
  return x >> valarray<T> (y);
}   

template <class T,class Proxy>
inline typename proxy_array<T,Proxy>::array_type proxy_array<T,Proxy>::operator * (const array_type& x) const
{
  return array_type (*this) * x;
}
    
template <class T,class Proxy>    
inline typename proxy_array<T,Proxy>::array_type proxy_array<T,Proxy>::operator / (const array_type& x) const
{
  return array_type (*this) / x;
}
    
template <class T,class Proxy>    
inline typename proxy_array<T,Proxy>::array_type proxy_array<T,Proxy>::operator % (const array_type& x) const
{
  return array_type (*this) % x;
}
    
template <class T,class Proxy>    
inline typename proxy_array<T,Proxy>::array_type proxy_array<T,Proxy>::operator + (const array_type& x) const
{
  return array_type (*this) + x;
}
    
template <class T,class Proxy>    
inline typename proxy_array<T,Proxy>::array_type proxy_array<T,Proxy>::operator - (const array_type& x) const
{
  return array_type (*this) - x;
}
    
template <class T,class Proxy>    
inline typename proxy_array<T,Proxy>::array_type proxy_array<T,Proxy>::operator ^ (const array_type& x) const
{
  return array_type (*this) ^ x;
}
    
template <class T,class Proxy>    
inline typename proxy_array<T,Proxy>::array_type proxy_array<T,Proxy>::operator & (const array_type& x) const
{
  return array_type (*this) & x;
}
    
template <class T,class Proxy>    
inline typename proxy_array<T,Proxy>::array_type proxy_array<T,Proxy>::operator | (const array_type& x) const
{
  return array_type (*this) | x;
}
    
template <class T,class Proxy>    
inline typename proxy_array<T,Proxy>::array_type proxy_array<T,Proxy>::operator << (const array_type& x) const
{
  return array_type (*this) << x;
}
    
template <class T,class Proxy>    
inline typename proxy_array<T,Proxy>::array_type proxy_array<T,Proxy>::operator >> (const array_type& x) const
{
  return array_type (*this) >> x;
}

template <class T,class Proxy>    
inline valarray<bool> proxy_array<T,Proxy>::operator == (const value_type& x) const
{
  return array_type (*this) == x;
}
    
template <class T,class Proxy>        
inline valarray<bool> proxy_array<T,Proxy>::operator != (const value_type& x) const
{
  return array_type (*this) != x;
}
    
template <class T,class Proxy>        
inline valarray<bool> proxy_array<T,Proxy>::operator < (const value_type& x) const
{
  return array_type (*this) < x;
}
    
template <class T,class Proxy>        
inline valarray<bool> proxy_array<T,Proxy>::operator > (const value_type& x) const
{
  return array_type (*this) > x;
}
    
template <class T,class Proxy>        
inline valarray<bool> proxy_array<T,Proxy>::operator <= (const value_type& x) const
{
  return array_type (*this) <= x;
}
    
template <class T,class Proxy>        
inline valarray<bool> proxy_array<T,Proxy>::operator >= (const value_type& x) const
{
  return array_type (*this) >= x;
}
    
template <class T,class Proxy>        
inline valarray<bool> proxy_array<T,Proxy>::operator && (const value_type& x) const
{
  return array_type (*this) && x;
}
    
template <class T,class Proxy>        
inline valarray<bool> proxy_array<T,Proxy>::operator || (const value_type& x) const
{
  return array_type (*this) || x;
}

template <class T,class Proxy> 
inline valarray<bool> operator == (const T& x,const proxy_array<T,Proxy>& y)
{
  return x == valarray<T> (y);
}

template <class T,class Proxy> 
inline valarray<bool> operator != (const T& x,const proxy_array<T,Proxy>& y)
{
  return x != valarray<T> (y);
}

template <class T,class Proxy> 
inline valarray<bool> operator < (const T& x,const proxy_array<T,Proxy>& y)
{
  return x < valarray<T> (y);
}

template <class T,class Proxy> 
inline valarray<bool> operator > (const T& x,const proxy_array<T,Proxy>& y)
{
  return x > valarray<T> (y);
}

template <class T,class Proxy> 
inline valarray<bool> operator <= (const T& x,const proxy_array<T,Proxy>& y)
{
  return x <= valarray<T> (y);
}

template <class T,class Proxy> 
inline valarray<bool> operator >= (const T& x,const proxy_array<T,Proxy>& y)
{
  return x >= valarray<T> (y);
}

template <class T,class Proxy> 
inline valarray<bool> operator && (const T& x,const proxy_array<T,Proxy>& y)
{
  return x && valarray<T> (y);
}

template <class T,class Proxy> 
inline valarray<bool> operator || (const T& x,const proxy_array<T,Proxy>& y)
{
  return x || valarray<T> (y);
}

template <class T,class Proxy>    
inline valarray<bool> proxy_array<T,Proxy>::operator == (const array_type& x) const
{
  return array_type (*this) == x;
}
    
template <class T,class Proxy>        
inline valarray<bool> proxy_array<T,Proxy>::operator != (const array_type& x) const
{
  return array_type (*this) != x;
}
    
template <class T,class Proxy>        
inline valarray<bool> proxy_array<T,Proxy>::operator < (const array_type& x) const
{
  return array_type (*this) < x;
}
    
template <class T,class Proxy>        
inline valarray<bool> proxy_array<T,Proxy>::operator > (const array_type& x) const
{
  return array_type (*this) > x;
}
    
template <class T,class Proxy>        
inline valarray<bool> proxy_array<T,Proxy>::operator <= (const array_type& x) const
{
  return array_type (*this) <= x;
}
    
template <class T,class Proxy>        
inline valarray<bool> proxy_array<T,Proxy>::operator >= (const array_type& x) const
{
  return array_type (*this) >= x;
}
    
template <class T,class Proxy>        
inline valarray<bool> proxy_array<T,Proxy>::operator && (const array_type& x) const
{
  return array_type (*this) && x;
}
    
template <class T,class Proxy>        
inline valarray<bool> proxy_array<T,Proxy>::operator || (const array_type& x) const
{
  return array_type (*this) || x;
}

/*
    slice & slice_array
*/

inline slice::slice ()
  : slice_start (0), slice_size (0), slice_stride (0)
  { }
  
inline slice::slice (size_t _start,size_t _size,size_t _stride)
  : slice_start (_start), slice_size (_size), slice_stride (_stride)
  { }

inline size_t slice::start () const
{
  return slice_start;  
}

inline size_t slice::size () const
{
  return slice_size;
}

inline size_t slice::stride () const
{
  return slice_stride;
}

template <class T> 
inline slice_array<T>::slice_array (const slice& _slice,valarray<T>& _array)
  : proxy_array<T,slice> (_slice,_array)
  { }
  
/*
    gslice & gslice_array
*/  

inline gslice::gslice ()
  : gslice_start (0)
  { }
    
inline gslice::gslice (size_t start,const valarray<size_t>& lengths,const valarray<size_t>& strides)
  : gslice_start (start), gslice_lengths (lengths), gslice_strides (strides)
  { }

inline size_t gslice::start () const
{
  return gslice_start;
}

inline const valarray<size_t>& gslice::size () const
{
  return gslice_lengths;
}

inline const valarray<size_t>& gslice::stride () const
{
  return gslice_strides;
}

template <class T>
inline gslice_array<T>::gslice_array (const gslice& _slice,valarray<T>& _array)
  : proxy_array<T,gslice> (_slice,_array)
  { }

/*
    indirect_array
*/

template <class T> 
inline indirect_array<T>::indirect_array (const valarray<size_t>& _proxy,valarray<T>& _array)
  : proxy_array<T,valarray<size_t> > (_proxy,_array)
  { }

/*
    mask_array
*/  

template <class T> 
inline mask_array<T>::mask_array (const valarray<bool>& _mask,valarray<T>& _array)
  : proxy_array<T,valarray<bool> > (_mask,_array)
  { }
