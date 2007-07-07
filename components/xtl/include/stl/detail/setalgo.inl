/*
    Теоретико-множественные операции
*/

template <class Iter1,class Iter2,class OutIter,class Compare1,class Compare2>
inline OutIter set_union 
 (Iter1    first1,
  Iter1    last1,
  Iter2    first2,
  Iter2    last2,
  OutIter  result,
  Compare1 less1,
  Compare2 less2)
{
  for (;first1 != last1 && first2 != last2;++result) 
  {
    if      (less1 (*first1,*first2)) *result = *first1, ++first1;
    else if (less2 (*first2,*first1)) *result = *first2, ++first2;
    else                              *result = *first1, ++first1, ++first2;
  }
  
  return copy (first2,last2,copy (first1,last1,result));
}

template <class Iter1,class Iter2,class OutIter>
inline OutIter set_union (Iter1 first1,Iter1 last1,Iter2 first2,Iter2 last2,OutIter result)
{
  return set_union (first1,last1,first2,last2,result,iter_less<Iter1,Iter2> (),iter_less<Iter2,Iter1> ());
}

template <class Iter1,class Iter2,class OutIter,class Compare>
inline OutIter set_union (Iter1 first1,Iter1 last1,Iter2 first2,Iter2 last2,OutIter result,Compare less)
{
  return set_union (first1,last1,first2,last2,result,less,less);
}

template <class Iter1,class Iter2,class OutIter,class Compare1,class Compare2>
inline OutIter set_intersection 
 (Iter1    first1,
  Iter1    last1,
  Iter2    first2,
  Iter2    last2,
  OutIter  result,
  Compare1 less1,
  Compare2 less2)
{
  while (first1 != last1 && first2 != last2)  
    if      (less1 (*first1,*first2)) ++first1;
    else if (less2 (*first2,*first1)) ++first2;
    else 
    {
      *result = *first1;
      
      ++first1;
      ++first2;
      ++result;
    }
    
  return result;
}

template <class Iter1,class Iter2,class OutIter>
inline OutIter set_intersection (Iter1 first1,Iter1 last1,Iter2 first2,Iter2 last2,OutIter result)
{
  return set_intersection (first1,last1,first2,last2,result,iter_less<Iter1,Iter2> (),iter_less<Iter2,Iter1> ());
}

template <class Iter1,class Iter2,class OutIter,class Compare>
inline OutIter set_intersection (Iter1 first1,Iter1 last1,Iter2 first2,Iter2 last2,OutIter result,Compare less)
{
  return set_intersection (first1,last1,first2,last2,result,less,less);
}

template <class Iter1,class Iter2,class OutIter,class Compare1,class Compare2>
inline OutIter set_difference 
 (Iter1    first1,
  Iter1    last1,
  Iter2    first2,
  Iter2    last2,
  OutIter  result,
  Compare1 less1,
  Compare2 less2)
{
  while (first1 != last1 && first2 != last2)
    if      (less1 (*first1,*first2)) *result = *first1, ++first1, ++result;
    else if (less2 (*first2,*first1)) ++first2;
    else                              ++first1,  ++first2;

  return copy (first1,last1,result);
}

template <class Iter1,class Iter2,class OutIter>
inline OutIter set_difference (Iter1 first1,Iter1 last1,Iter2 first2,Iter2 last2,OutIter result)
{
  return set_difference (first1,last1,first2,last2,result,iter_less<Iter1,Iter2> (),iter_less<Iter2,Iter1> ());
}

template <class Iter1,class Iter2,class OutIter,class Compare>
inline OutIter set_difference (Iter1 first1,Iter1 last1,Iter2 first2,Iter2 last2,OutIter result,Compare less)
{
  return set_difference (first1,last1,first2,last2,result,less,less);
}

template <class Iter1,class Iter2,class OutIter,class Compare1,class Compare2>
inline OutIter set_symmetric_difference 
 (Iter1    first1,
  Iter1    last1,
  Iter2    first2,
  Iter2    last2,
  OutIter  result,
  Compare1 less1,
  Compare2 less2)
{
  while (first1 != last1 && first2 != last2)
    if      (less1 (*first1,*first2)) *result = *first1, ++first1, ++result;
    else if (less2 (*first2,*first1)) *result = *first2, ++first2, ++result;
    else                              ++first1, ++first2;

  return copy (first2,last2,copy (first1,last1,result));
}

template <class Iter1,class Iter2,class OutIter>
inline OutIter set_symmetric_difference (Iter1 first1,Iter1 last1,Iter2 first2,Iter2 last2,OutIter result)
{
  return set_symmetric_difference (first1,last1,first2,last2,result,iter_less<Iter1,Iter2> (),iter_less<Iter2,Iter1> ());
}

template <class Iter1,class Iter2,class OutIter,class Compare>
inline OutIter set_symmetric_difference (Iter1 first1,Iter1 last1,Iter2 first2,Iter2 last2,OutIter result,Compare less)
{
  return set_symmetric_difference (first1,last1,first2,last2,result,less,less);
}

template <class Iter1,class Iter2,class Compare1,class Compare2>
inline bool includes (Iter1 first1,Iter1 last1,Iter2 first2,Iter2 last2,Compare1 less1,Compare2 less2)
{
  while (first1 != last1 && first2 != last2)
    if      (less2 (*first2,*first1)) return false;
    else if (less1 (*first1,*first2)) ++first1;
    else                              ++first1, ++first2;

  return first2 == last2;
}

template <class Iter1,class Iter2>
inline bool includes (Iter1 first1,Iter1 last1,Iter2 first2,Iter2 last2)
{
  return includes (first1,last1,first2,last2,iter_less<Iter1,Iter2> (),iter_less<Iter2,Iter1> ());
}

template <class Iter1,class Iter2,class Compare>
inline bool includes (Iter1 first1,Iter1 last1,Iter2 first2,Iter2 last2,Compare less)
{
  return includes (first1,last1,first2,last2,less,less);
}
