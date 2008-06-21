/*
    Àğèôìåòè÷åñèå îïåğàöèè
*/

template <class Arg1,class Arg2,class Result>
inline Result plus<Arg1,Arg2,Result>::operator () (const Arg1& a,const Arg2& b) const
{
  return a + b;
}

template <class Arg1,class Arg2,class Result>
inline Result minus<Arg1,Arg2,Result>::operator () (const Arg1& a,const Arg2& b) const
{
  return a - b;
}

template <class Arg1,class Arg2,class Result>
inline Result multiplies<Arg1,Arg2,Result>::operator () (const Arg1& a,const Arg2& b) const
{
  return a * b;
}

template <class Arg1,class Arg2,class Result>
inline Result divides<Arg1,Arg2,Result>::operator () (const Arg1& a,const Arg2& b) const
{
  return a / b;
}


template <class Arg,class Result>
inline Result assign<Arg,Result>::operator () (const Arg& a) const
{
  return a;
}

template <class Arg,class Result>
inline Result negate<Arg,Result>::operator () (const Arg& a) const
{
  return -a;
}

/*
    ‹®£¨ç¥á¨ª¥ ®¯¥à æ¨¨
*/

template <class Arg1,class Arg2,class Eps>
inline bool equal_to<Arg1,Arg2,Eps>::operator () (const Arg1& a,const Arg2& b,const Eps& eps) const
{
  if ((b>=a-eps)&&(b<=a+eps))
  {
    return 1;
  }
  return 0;
}

/*template <class Arg1,class Arg2,class Eps>
inline bool equal_to<Arg1,Arg2,Eps>::operator () (const Arg1& a,const Arg2& b) const
{
  return a==b;
}

template <class Arg1,class Arg2>
inline bool not_equal_to<Arg1,Arg2>::operator () (const Arg1& a,const Arg2& b) const
{
  return a != b;
}*/

template <class Arg1,class Arg2,class Eps>
inline bool not_equal_to<Arg1,Arg2,Eps>::operator () (const Arg1& a,const Arg2& b,const Eps& eps) const
{
  if ((b>=a-eps)&&(b<=a+eps))
  {
    return 0;
  }
  return 1;
}

//inline bool operator () (const Arg1& a,const Arg2& b,const Eps& eps=Eps(0)) const;

template <class Arg1,class Arg2,class Eps>
inline bool less<Arg1,Arg2,Eps>::operator () (const Arg1& a,const Arg2& b,const Eps& eps) const
{
  return (a+eps) < b;
}

template <class Arg1,class Arg2,class Eps>
inline bool greater<Arg1,Arg2,Eps>::operator () (const Arg1& a,const Arg2& b,const Eps& eps) const
{
  return (a-eps) > b;
}

template <class Arg1,class Arg2,class Eps>
inline bool less_equal<Arg1,Arg2,Eps>::operator () (const Arg1& a,const Arg2& b,const Eps& eps) const
{
  return (a-eps) <= b;
}

template <class Arg1,class Arg2,class Eps>
inline bool greater_equal<Arg1,Arg2,Eps>::operator () (const Arg1& a,const Arg2& b,const Eps& eps) const
{
  return (a+eps) >= b;
}

template <class Arg,class Result>
inline Result absol<Arg,Result>:: operator () (const Arg&a) const
{
  if(a<Arg(0))
  {
    return -a;
  }
  return a;
}

template <class Arg1,class Arg2,class Result>
inline Result min_fn<Arg1,Arg2,Result>::operator () (const Arg1& a,const Arg2& b) const
{
  if (a<b) return a;
  return b;
}

template <class Arg1,class Arg2,class Result>
inline Result max_fn<Arg1,Arg2,Result>::operator () (const Arg1& a,const Arg2& b) const
{
  if (a>b) return a;
  return b;
}

