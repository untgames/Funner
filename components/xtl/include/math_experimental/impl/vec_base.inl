

/*
	Шаблонный класс покомпонентных векторных операций
*/


template <class Fn> template <class T,size_t size> 
	inline void component_fn<Fn>::operator() (vec<T,size>& res,const vec<T,size>& a,const vec<T,size>& b)
{
  for (size_t i=0;i<size;i++) res [i] = Fn ()(a [i],b [i]);
}

template<class Fn> template <class T,size_t size> 
	inline void component_fn<Fn>::operator() (vec<T,size>& res,const vec<T,size>& a,const T& b)
{
  for (size_t i=0;i<size;i++) res [i] = Fn ()(a [i],b);
}

template <class Fn> template <class T,size_t size>
	inline void component_fn<Fn>::operator() (vec<T,size>& res,const vec<T,size>& src)
{
  for (size_t i=0;i<size;i++) res [i] = Fn ()(src [i]);
}

template<class Fn> template <class T,size_t size> 
	inline void component_fn<Fn>::operator() (vec<T,size>& res,const T& src)
{
  for (size_t i=0;i<size;i++) res [i] = Fn ()(src);
}

/*
	Шаблонный класс операций сравнения
*/

template <class Fn> template<class T,size_t size>
inline bool compare_fn<Fn>::operator () (const vec<T,size>& a,const vec<T,size>& b,const T& eps)
{
  for(size_t i=0;i<size;i++)
  {
    if (!Fn()(a[i],b[i]),eps) return 0;
  }
  return 1;
}
    
/*
        Утилиты
*/

template <class T,size_t size> 
T vec_dot_product (const vec<T,size>& a,const vec<T,size>& b)
{
  T result = T (0);

  for (size_t i=0;i<size;i++)
    result += a [i] * b [i];

  return result;  
}


/*
        Длина и нормализация
*/

template <class T,size_t size> 
T vec_length (const vec<T,size>& v)
{
  return sqrt (vec_qlength (v));
}

template <size_t size>
float vec_length (const vec<float,size>& v)
{
  return sqrtf (vec_qlength (v));
}

template <size_t size>
long double vec_length (const vec<long double,size>& v)
{
  return sqrtl (vec_qlength (v));
}

template <class T,size_t size> 
T vec_qlength (const vec<T,size>& v)
{
  return vec_dot_product (v,v);
}

/*
    Векторное произведение
*/

template <class T>
void vec_cross_product (vec<T,3>& res,const vec<T,3>& a,const vec<T,3>& b)
{
  res [0] = a [1] * b [2] - b [1] * a [2];
  res [1] = b [0] * a [2] - b [2] * a [0];
  res [2] = a [0] * b [1] - b [0] * a [1];
}

template <class T>
void vec_cross_product (vec<T,4>& res,const vec<T,4>& a,const vec<T,4>& b)
{
  vec_cross_product ((vec<T,3>&)res,(const vec<T,3>&)a,(const vec<T,3>&)b);
  
  res [3] = T (0);
}
