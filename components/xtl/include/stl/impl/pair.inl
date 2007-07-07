template <class T1,class T2>
pair<T1,T2>::pair (const T1& a,const T2& b)
  : first (a), second (b)
{ }

template <class T1,class T2> template <class U1,class U2> 
pair<T1,T2>::pair (const pair<U1,U2>& x)
  : first (x.first), second (x.second)
{ }  

template <class T1,class T2>
inline bool pair<T1,T2>::operator == (const pair& x) const
{ 
  return first == x.first && second == x.second;
}

template <class T1,class T2>
inline bool pair<T1,T2>::operator != (const pair& x) const
{
  return !(*this == x);
}

template <class T1,class T2>
inline bool pair<T1,T2>::operator < (const pair& x) const
{ 
  return first < x.first || (!(x.first < first) && second < x.second); 
}

template <class T1,class T2>
inline bool pair<T1,T2>::operator > (const pair& x) const 
{
  return x < *this;
}

template <class T1,class T2>
inline bool pair<T1,T2>::operator <= (const pair& x) const 
{
  return !(x < *this);
}

template <class T1,class T2>
inline bool pair<T1,T2>::operator >= (const pair& x) const
{
  return !(*this < x);
}

template <class T1,class T2>
inline pair<T1,T2> make_pair (const T1& x,const T2& y)
{
  return pair<T1,T2> (x,y);
}
