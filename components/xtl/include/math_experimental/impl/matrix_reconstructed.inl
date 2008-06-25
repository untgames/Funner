namespace detail
{
template <class T, size_t SizeX, size_t SizeY> 
  vec<T, SizeY>& get_component (math::matrix<T, SizeX,SizeY>& v, size_t index)
{
  return v [index];
}

template <class T, size_t SizeX, size_t SizeY> const
  vec<T, SizeY>& get_component (const math::matrix<T, SizeX,SizeY>& v, size_t index)
{
  return v [index];
}

}


template <class T,size_t sizeX,size_t sizeY>
inline matrix<T,sizeX,sizeY>::matrix ()
{
  *this=make_unary_operation<matrix<T,sizeX,sizeY> >(T(1), assign<vec<T,sizeY> >());
}

template <class T,size_t sizeX,size_t sizeY>
inline matrix<T,sizeX,sizeY>::matrix (const T& a)
{
  *this=make_unary_operation<matrix<T,sizeX,sizeY> >(a, assign<vec<T,sizeY> >());//matrix_assign_scalar (*this,a);
}

template<class T,size_t sizeX,size_t sizeY>
  matrix<T,sizeX,sizeY>::matrix (const matrix<T,sizeX,sizeY>& src)
{
  *this=make_unary_operation<matrix<T,sizeX,sizeY> >(src,assign<vec<T,sizeY> >());
}

template <class T,size_t sizeX,size_t sizeY>
inline matrix<T,sizeX,sizeY>::matrix (const T* a)
{
  for (size_t i=0;i<sizeX;i++)
    for (size_t j=0;j<sizeY;j++)
      (*this)[i][j] = *a++;                
}

template<class T,size_t sizeX,size_t sizeY> template <class Fn>
inline matrix<T,sizeX,sizeY>::matrix(const matrix<T,sizeX,sizeY>& a,const matrix<T,sizeX,sizeY>& b,Fn fn)
{
  fn(*this,a,b);
} 
template <class T,size_t sizeX,size_t sizeY> template <class T1,class Fn>
inline matrix<T,sizeX,sizeY>::matrix(const T1& src,Fn fn)
{
  fn(*this,src);
}

template <class T,size_t sizeX,size_t sizeY> template <class T1>
inline matrix<T,sizeX,sizeY>::matrix (const T1& a,void (*eval)(matrix&,const T1&))
{
  eval (*this,a);
}

template <class T,size_t sizeX,size_t sizeY>
inline const matrix<T,sizeX,sizeY> matrix<T,sizeX,sizeY>::operator -() const
{
  return make_unary_operation<matrix<T,sizeX,sizeY> >(*this,negate<vec<T,sizeY> >());
}

template <class T,size_t sizeX,size_t sizeY> 
inline matrix<T,sizeX,sizeY>& matrix<T,sizeX,sizeY>::operator += (const matrix<T,sizeX,sizeY>& a)
{
  *this=make_binary_operation<matrix<T,sizeX,sizeY> >(*this,a,plus<vec<T,sizeY> >());
  return *this;
}
template <class T,size_t sizeX,size_t sizeY>   
inline matrix<T,sizeX,sizeY>& matrix<T,sizeX,sizeY>::operator -= (const matrix<T,sizeX,sizeY>& a)
{
  *this=make_binary_operation<matrix<T,sizeX,sizeY> >(*this,a,minus<vec<T,sizeY> >());
  return *this;
}

template <class T,size_t sizeX,size_t sizeY>   
inline matrix<T,sizeX,sizeY>& matrix<T,sizeX,sizeY>::operator *= (const matrix<T,sizeX,sizeY>& a)
{
  *this=make_binary_operation<matrix<T,sizeX,sizeY> >(*this,a,multiplies<vec<T,sizeY> >());
  return *this;
}

/*template <class T,size_t sizeX,size_t sizeY>
inline matrix<T,sizeX,sizeY>& matrix<T,sizeX,sizeY>::operator *= (const T& a)
{
  *this=make_binary_operation<matrix<T,sizeX,sizeY> >(*this,vec<T,sizeY> (a),multiplies<vec<T,sizeY> >());
  return *this;
}*/

/*template <class T,size_t sizeX,size_t sizeY>
inline matrix<T,sizeX,sizeY>& matrix<T,sizeX,sizeY>::operator /= (const T& a)
{
  *this=make_binary_operation<matrix<T,sizeX,sizeY> >(*this,vec<T,sizeY> (a),divides<vec<T,sizeY> >());
  return *this;
}*/

template <class T,size_t sizeX,size_t sizeY>
inline matrix<T,sizeX,sizeY>& matrix<T,sizeX,sizeY>::operator /= (const matrix<T,sizeX,sizeY>& a)
{
  *this=make_binary_operation<matrix<T,sizeX,sizeY> >(*this,a,divides<vec<T,sizeY> >());
  return *this;
}


template <class T,size_t sizeX,size_t sizeY>   
inline const matrix<T,sizeX,sizeY> matrix<T,sizeX,sizeY>::operator + (const matrix<T,sizeX,sizeY>& m) const
{
  return make_binary_operation<matrix<T,sizeX,sizeY> >(*this,m,plus<vec<T,sizeY> >());
}

template <class T,size_t sizeX,size_t sizeY>   
inline const matrix<T,sizeX,sizeY> matrix<T,sizeX,sizeY>::operator - (const matrix<T,sizeX,sizeY>& m) const
{
  return make_binary_operation<matrix<T,sizeX,sizeY> >(*this,m,minus<vec<T,sizeY> >());
}

template <class T,size_t sizeX,size_t sizeY>
inline const matrix<T,sizeX,sizeY> matrix<T,sizeX,sizeY>::operator / (const matrix<T,sizeX,sizeY>& a) const
{
  return make_binary_operation<matrix<T,sizeX,sizeY> >(*this,a,divides<vec<T,sizeY> >());
}


template <class T,size_t sizeX,size_t sizeY>   
inline const matrix<T,sizeX,sizeY> matrix<T,sizeX,sizeY>::operator * (const matrix<T,sizeX,sizeY>& a) const
{
  return make_binary_operation<matrix<T,sizeX,sizeY> >(*this,a,multiplies<vec<T,sizeY> >());
}

template <class T,size_t sizeX,size_t sizeY>
inline bool matrix<T,sizeX,sizeY>::operator == (const matrix<T,sizeX,sizeY>& v) const
{
  return compare_function<vec<T,sizeY>, equal_to<vec<T,sizeY> > >() (*this,v);
}

template <class T,size_t sizeX,size_t sizeY>
inline bool matrix<T,sizeX,sizeY>::operator != (const matrix<T,sizeX,sizeY>& v) const
{
  return compare_function<vec<T,sizeY>, not_equal_to<vec<T,sizeY> > >() (*this,v);
}
