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

template <class T,size_t SizeX,size_t SizeY>
inline matrix<T,SizeX,SizeY>::matrix ()
{
  *this=make_unary_operation<matrix<T,SizeX,SizeY> >(T(1), assign<vec<T,SizeY> >());
}

template <class T,size_t SizeX,size_t SizeY>
inline matrix<T,SizeX,SizeY>::matrix (const T& a)
{
  *this=make_unary_operation<matrix<T,SizeX,SizeY> >(a, assign<vec<T,SizeY> >());
}

template<class T,size_t SizeX,size_t SizeY>
  matrix<T,SizeX,SizeY>::matrix (const matrix<T,SizeX,SizeY>& src)
{
  *this=make_unary_operation<matrix<T,SizeX,SizeY> >(src,assign<vec<T,SizeY> >());
}

template <class T,size_t SizeX,size_t SizeY>
inline matrix<T,SizeX,SizeY>::matrix (const T* a)
{
  for (size_t i=0;i<SizeX;i++)
    for (size_t j=0;j<SizeY;j++)
      (*this)[i][j] = *a++;                
}

template<class T,size_t SizeX,size_t SizeY> template <class T1,class T2, class Fn>
inline matrix<T,SizeX,SizeY>::matrix(const T1& a,const T2& b,Fn fn)
{
  fn(*this,a,b);
} 
template <class T,size_t SizeX,size_t SizeY> template <class T1,class Fn>
inline matrix<T,SizeX,SizeY>::matrix(const T1& src,Fn fn)
{
  fn(*this,src);
}

template <class T,size_t SizeX,size_t SizeY> template <class T1>
inline matrix<T,SizeX,SizeY>::matrix (const T1& a,void (*eval)(matrix&,const T1&))
{
  eval (*this,a);
}

/*
	Индексация
*/

template<class T,size_t SizeX,size_t SizeY>
inline const vec<T,SizeX> matrix<T,SizeX,SizeY>::column (size_t j) const
{
  vec<T,SizeX> res;
  for(int i=0;i<SizeX;i++) res[i]=(*this)[i][j];
  return res;
}

/*
	Унарные операторы
*/

template <class T,size_t SizeX,size_t SizeY>
inline const matrix<T,SizeX,SizeY> matrix<T,SizeX,SizeY>::operator -() const
{
  return make_unary_operation<matrix<T,SizeX,SizeY> >(*this,negate<vec<T,SizeY> >());
}

/*
	Бинарные операторы
*/



template <class T,size_t SizeX,size_t SizeY>
inline matrix<T,SizeX,SizeY>& matrix<T,SizeX,SizeY>::operator *= (const T& a)
{
  (*this)=(*this)*a;
  return *this;
}

template<class T,size_t SizeX,size_t SizeY>
inline matrix<T,SizeX,SizeY>& matrix<T,SizeX,SizeY>::operator /= (const T& a)
{
  (*this)=(*this)/a;
  return (*this);
}

template <class T,size_t SizeX,size_t SizeY>
  inline const matrix<T,SizeX,SizeY> matrix<T,SizeX,SizeY>::operator * (const T& a) const
{
  return make_binary_operation<matrix<T,SizeX,SizeY> >(*this,a,multiplies<vec<T,SizeY> >());
}

template <class T,size_t SizeX,size_t SizeY>
  inline const matrix<T,SizeX,SizeY> matrix<T,SizeX,SizeY>::operator / (const T& a) const
{
  return make_binary_operation<matrix<T,SizeX,SizeY> >(*this,a,divides<vec<T,SizeY>,T >());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T,size_t SizeX,size_t SizeY>   
inline const matrix<T,SizeX,SizeY> matrix<T,SizeX,SizeY>::operator + (const matrix<T,SizeX,SizeY>& m) const
{
  return make_binary_operation<matrix<T,SizeX,SizeY> >(*this,m,plus<vec<T,SizeY> >());
}

template <class T,size_t SizeX,size_t SizeY>   
inline const matrix<T,SizeX,SizeY> matrix<T,SizeX,SizeY>::operator - (const matrix<T,SizeX,SizeY>& m) const
{
  return make_binary_operation<matrix<T,SizeX,SizeY> >(*this,m,minus<vec<T,SizeY> >());
}

template <class T,size_t SizeX,size_t SizeY> 
inline matrix<T,SizeX,SizeY>& matrix<T,SizeX,SizeY>::operator += (const matrix<T,SizeX,SizeY>& a)
{
  *this=make_binary_operation<matrix<T,SizeX,SizeY> >(*this,a,plus<vec<T,SizeY> >());
  return *this;
}

template <class T,size_t SizeX,size_t SizeY>   
inline matrix<T,SizeX,SizeY>& matrix<T,SizeX,SizeY>::operator -= (const matrix<T,SizeX,SizeY>& a)
{
  *this=make_binary_operation<matrix<T,SizeX,SizeY> >(*this,a,minus<vec<T,SizeY> >());
  return *this;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class T,size_t SizeX,size_t SizeY> template<size_t Size2Y>
inline const matrix<T, SizeX,Size2Y> matrix<T,SizeX,SizeY>::operator *(const matrix<T, SizeY,Size2Y>&b) const
{
  matrix<T,SizeX,Size2Y> res;
  for(int i=0;i<SizeX;i++)
  {
    for(int j=0;j<Size2Y;j++)
    {
      res[i][j]=binary_accumulation_function<T>()((*this)[i],b.column(j),T(0),plus<T> (),multiplies<T> ());
    }
  }
  return res;
}

template<class T,size_t Size>
inline matrix<T,Size>& operator *=(matrix<T,Size>& left,const matrix<T,Size>& right)
{
  left=left*right;
  return left;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class T,size_t Size>
inline const matrix<T,Size> operator /  (const matrix<T,Size>& left,const matrix<T,Size>& right)
{
  matrix<T,Size> tmp(invert(right));
  return left*tmp;
}

template<class T,size_t Size>
inline matrix<T,Size>& operator /= (matrix<T,Size>& left,const matrix<T,Size>& right)
{
  return left*=invert(right);
}

/*
	Удаление строки/столбца
*/
template<class T,size_t SizeX,size_t SizeY>
inline const matrix<T,SizeX-1,SizeY> matrix<T,SizeX,SizeY>::delete_row(size_t row) const
{
  matrix<T,SizeX-1,SizeY> res;
  for(int i=0,i_=0;i<SizeX;i++,i_++)
  {
    if (i==row) 
     {
       i_--;
       continue;
     }
    for(int j=0,j_=0;j<SizeY;j++,j_++) res[i_][j_]=(*this)[i][j];
  }
  return res;
}

template<class T,size_t SizeX,size_t SizeY>
inline const matrix<T,SizeX,SizeY-1> matrix<T,SizeX,SizeY>::delete_column(size_t column) const
{
  matrix<T, SizeX,SizeY-1> res;
  for(int i=0,i_=0;i<SizeX;i++,i_++)
  {
    for(int j=0,j_=0;j<SizeY;j++,j_++)
    {
      if (j==column) 
      {
        j_--;
        continue;
      }
      res[i_][j_]=(*this)[i][j];
    }
  }
  return res;
}

template<class T,size_t SizeX,size_t SizeY>
inline const matrix<T,SizeX-1,SizeY-1> matrix<T,SizeX,SizeY>::delete_row_column(size_t row,size_t column) const
{
  matrix<T,SizeX-1,SizeY-1> res=this->delete_row(row).delete_column(column);
  return res;
}



/*
	Сравнение
*/

template <class T,size_t SizeX,size_t SizeY>
inline bool matrix<T,SizeX,SizeY>::operator == (const matrix<T,SizeX,SizeY>& v) const
{
  return compare_function<vec<T,SizeY>, equal_to<vec<T,SizeY> > >() (*this,v);
}

template <class T,size_t SizeX,size_t SizeY>
inline bool matrix<T,SizeX,SizeY>::operator != (const matrix<T,SizeX,SizeY>& v) const
{
  return compare_function<vec<T,SizeY>, not_equal_to<vec<T,SizeY> > >() (*this,v);
}


/*
	Утилиты
*/
template<class T,size_t SizeX,size_t SizeY>
inline const matrix<T,SizeY,SizeX> matrix<T,SizeX,SizeY>::transpose ()
{
  matrix<T,SizeY,SizeX> res;
  for (int i=0;i<SizeX;i++)
  {
    for(int j=0;j<SizeY;j++)
    {
      res[i][j]=(*this)[j][i];
    }
  }
  return res;
}

////////////////////////////////////////////////////////////////////////////////////////////

template<class T,size_t Size>
inline const matrix<T,Size,Size> three_angle_view(const matrix<T,Size,Size>& src,int& num_of_changes)
{
  matrix<T,Size> tmp(src);
  num_of_changes=0;
  for (int i=0;i<Size-1;i++)
  {
    if (!tmp[i][i])
    {
      int first_non_zero=0;
      for (int j=i+1;j<Size;j++)
      {
        if (tmp[j][i]) first_non_zero=j;
      }
      if (!first_non_zero) continue;
      num_of_changes++;
      for(int j=0;j<Size;j++)
      {
        T _tmp(tmp[i][j]);
        tmp[i][j]=tmp[first_non_zero][j];
        tmp[first_non_zero][j]=_tmp;
      }
    }
    for(int j=i+1;j<Size;j++)
    {
      T coeff=tmp[j][i];
      for(int k=i;k<Size;k++) tmp[j][k]-=tmp[i][k]*(coeff/tmp[i][i]);
    }
  }
  return tmp;
}

template<class T,size_t Size>
inline const T det(const matrix<T,Size,Size>& src)
{
  int num_of_changes;
  matrix<T,Size,Size> tmp(three_angle_view(src,num_of_changes));
  T res(1);
  for(int i=0;i<num_of_changes;i++) res*=T(-1);
  for(int i=0;i<Size;i++) res*=tmp[i][i];
  return res;
}

template<class T,size_t Size>
inline const T mathematical_add(const matrix<T,Size>& src,size_t row,size_t column)
{
  T res(1);
  for (int i=0;i<row+column;i++) res*=T(-1);
  res*=det(src.delete_row_column(row,column));
  return res;
}

template<class T,size_t Size>
inline const matrix<T,Size> invert(const matrix<T,Size>& src)
{
  matrix<T,Size> res;
  T d(det(src));
  for(int i=0;i<Size;i++)
  {
    for(int j=0;j<Size;j++)
    {
      res[i][j]=mathematical_add(src,j,i)/d;
    }
  }
  return res;
}

template<class T,size_t Size>
inline const matrix<T,Size> normalize(const matrix<T,Size>& src)
{
  return src/sqrt(det(src));
}
