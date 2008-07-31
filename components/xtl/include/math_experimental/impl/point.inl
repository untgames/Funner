/*
	Конструкторы
*/
template<class T>
inline point<T>::point()
{
  x=T(0);
  y=T(0);
  z=T(0);
}

template<class T>
inline point<T>::point(const T& x_, const T& y_, const T& z_=T(0))
{
  x=x_;
  y=y_;
  z=z_;
}

template<class T>
inline point<T>::point(const point<T>& p)
{
  make_unary_operation<point<T> >(p,assign<T>(),*this);
}

template<class T>
inline point<T>& point<T>::move (const T& x_,const T& y_, const T& z_)
{
  x+=x_;
  y+=y_;
  z+=z_;
  return *this;
}

template<class T>
inline point<T>& point<T>::operator= (const point<T>& p)
{
  make_unary_operation<point<T> >(p,assign<T>(),*this);
  return *this;
}

template<class T>
inline const vec<T,3> create_vector(const point<T>& p1,const point<T>& p2)
{
  vec<T,3> v;
  make_binary_operation(p1,p2,minus<T>(),v);
  return v;
}



