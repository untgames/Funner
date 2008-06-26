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

/*
	Конструкторы
*/

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

/*
	Индексация
*/

template<class T,size_t sizeX,size_t sizeY>
inline const vec<T,sizeX> matrix<T,sizeX,sizeY>::column (size_t j) const
{
  vec<T,sizeX> res;
  for(int i=0;i<sizeX;i++) res[i]=(*this)[i][j];
  return res;
}

/*
	Унарные операторы
*/

template <class T,size_t sizeX,size_t sizeY>
inline const matrix<T,sizeX,sizeY> matrix<T,sizeX,sizeY>::operator -() const
{
  return make_unary_operation<matrix<T,sizeX,sizeY> >(*this,negate<vec<T,sizeY> >());
}

/*
	Бинарные операторы
*/



template <class T,size_t sizeX,size_t sizeY>
inline matrix<T,sizeX,sizeY>& matrix<T,sizeX,sizeY>::operator *= (const T& a)
{
  *this=make_binary_operation<matrix<T,sizeX,sizeY> >(*this,vec<T,sizeY> (a),multiplies<vec<T,sizeY> >());
  return *this;
}

template<class T,size_t sizeX,size_t sizeY>
inline matrix<T,sizeX,sizeY>& matrix<T,sizeX,sizeY>::operator /= (const T& a)
{
  (*this)=(*this)/a;
  return (*this);
}

template <class T,size_t sizeX,size_t sizeY>
  inline const matrix<T,sizeX,sizeY> matrix<T,sizeX,sizeY>::operator * (const T& a) const
{
  return make_binary_operation<matrix<T,sizeX,sizeY> >(*this,a,multiplies<vec<T,sizeY> >());
}

template <class T,size_t sizeX,size_t sizeY>
  inline const matrix<T,sizeX,sizeY> matrix<T,sizeX,sizeY>::operator / (const T& a) const
{
  return make_binary_operation<matrix<T,sizeX,sizeY> >(*this,a,divides<vec<T,sizeY> >());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

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


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class T,size_t sizeX,size_t sizeY> template<size_t size2Y>
inline const matrix<T, sizeX,size2Y> matrix<T,sizeX,sizeY>::operator *(const matrix<T, sizeY,size2Y>&b)
{
  matrix<T,sizeX,size2Y> res;
  for(int i=0;i<sizeX;i++)
  {
    for(int j=0;j<size2Y;j++)
    {
      res[i][j]=binary_accumulation_function<T>()((*this)[i],b.column(j),T(0),plus<T> (),multiplies<T> ());
    }
  }
  return res;
}

template<class T,size_t size>
inline matrix<T,size>& operator *=(matrix<T,size>& left,const matrix<T,size>& right)
{
  left=left*right;
  return left;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class T,size_t size>
inline const matrix<T,size> operator /  (const matrix<T,size>& left,const matrix<T,size>&right)
{
  return left*invert(right);
}

template<class T,size_t size>
inline matrix<T,size>& operator /= (matrix<T,size>& left,const matrix<T,size>& right)
{
  return left*=invert(right);
}

/*
	Удаление строки/столбца
*/
template<class T,size_t sizeX,size_t sizeY>
inline const matrix<T,sizeX-1,sizeY> matrix<T,sizeX,sizeY>::delete_row(size_t row) const
{
  matrix<T,sizeX-1,sizeY> res;
  for(int i=0,_i=0;i<sizeX;i++,_i++)
  {
    if (i==row) 
     {
       _i--;
       continue;
     }
    for(int j=0,_j=0;j<sizeY;j++,_j++) res[_i][_j]=(*this)[i][j];
  }
  return res;
}

template<class T,size_t sizeX,size_t sizeY>
inline const matrix<T,sizeX,sizeY-1> matrix<T,sizeX,sizeY>::delete_column(size_t column) const
{
  matrix<T, sizeX,sizeY-1> res;
  for(int i=0,_i=0;i<sizeX;i++,_i++)
  {
    for(int j=0,_j=0;j<sizeY;j++,_j++)
    {
      if (j==column) 
      {
        _j--;
        continue;
      }
      res[_i][_j]=(*this)[i][j];
    }
  }
  return res;
}

template<class T,size_t sizeX,size_t sizeY>
inline const matrix<T,sizeX-1,sizeY-1> matrix<T,sizeX,sizeY>::delete_row_column(size_t row,size_t column) const
{
  matrix<T,sizeX-1,sizeY-1> res=this->delete_row(row).delete_column(column);
  return res;
}



/*
	Сравнение
*/

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


/*
	Утилиты
*/
template<class T,size_t sizeX,size_t sizeY>
inline const matrix<T,sizeY,sizeX> matrix<T,sizeX,sizeY>::transpose ()
{
  matrix<T,sizeY,sizeX> res;
  for (int i=0;i<sizeX;i++)
  {
    for(int j=0;j<sizeY;j++)
    {
      res[i][j]=(*this)[j][i];
    }
  }
  return res;
}

////////////////////////////////////////////////////////////////////////////////////////////

template<class T,size_t size>
inline const matrix<T,size,size> three_angle_view(const matrix<T,size,size>& src)
{
  matrix<T,size> tmp(src);
  for (int i=0;i<size-1;i++)
  {
    if (!tmp[i][i])
    {
      int first_non_zero=0;
      for (int j=i+1;j<size;j++)
      {
        if (tmp[j][i]) first_non_zero=j;
      }
      if (!first_non_zero) continue;
      for(int j=0;j<size;j++)
      {
        T _tmp(tmp[i][j]);
        tmp[i][j]=tmp[first_non_zero][j];
        tmp[first_non_zero][j]=_tmp;
      }
    }
    for(int j=i+1;j<size;j++)
    {
      T coeff=tmp[j][i];
      for(int k=i;k<size;k++) tmp[j][k]-=tmp[i][k]*(coeff/tmp[i][i]);
    }
  }
  return tmp;
}

template<class T,size_t size>
inline const T det(const matrix<T,size,size>& src)
{
  matrix<T,size,size> tmp(three_angle_view(src));
  T res(1);
  for(int i=0;i<size;i++) res*=tmp[i][i];
  return res;
}

template<class T,size_t size>
inline const T mathematical_add(const matrix<T,size>& src,size_t row,size_t column)
{
  T res(1);
  for (int i=0;i<row+column;i++) res*=T(-1);
  res*=src[row][column];
  res*=det(src.delete_row_column(row,column));
  return res;
}

template<class T,size_t size>
inline const matrix<T,size> invert(const matrix<T,size>& src)
{
  matrix<T,size> res;
  T d(det(src));
  for(int i=0;i<size;i++)
  {
    for(int j=0;j<size;j++)
    {
      res[i][j]=mathematical_add(src,j,i)/d;
    }
  }
  return res;
}

template<class T,size_t size>
inline const matrix<T,size> normalize(const matrix<T,size>& src)
{
  return src/det(src);
}
