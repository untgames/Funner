/*
    Адаптеры предикатов сравнения
*/

template <class Iter1,class Iter2=Iter1>
struct iter_less: public less<typename iterator_traits<Iter1>::value_type,
                              typename iterator_traits<Iter2>::value_type> { };
                             
template <class Iter1,class Iter2=Iter1> 
struct iter_equal: public equal_to<typename iterator_traits<Iter1>::value_type,
                                   typename iterator_traits<Iter2>::value_type> { };

/*
    Поиск
*/

template <class Iter,class T>
Iter find (Iter first,Iter last,const T& val,input_iterator_tag)
{
  while (first != last && *first != val) ++first;
  return first;  
}

template <class Iter,class T>
Iter find (Iter first,Iter last,const T& val,random_access_iterator_tag)
{
  typename iterator_traits<Iter>::difference_type trip_count = (last-first)>>2;  

  while (trip_count--) 
  {
    if (*first == val) return first;
    ++first;    
    if (*first == val) return first;
    ++first;    
    if (*first == val) return first;
    ++first;    
    if (*first == val) return first;
    ++first;    
  }

  switch (last-first) 
  {
    case 3:  if (*first == val) return first; ++first;    
    case 2:  if (*first == val) return first; ++first;
    case 1:  if (*first == val) return first; ++first;
    default: 
    case 0:  return last;
  }
}

template <class Iter,class T>
inline Iter find (Iter first,Iter last,const T& val)
{
  return find (first,last,val,iterator_category<Iter> ());
}

template <class Iter,class Predicate>
Iter find_if (Iter first,Iter last,Predicate equal,input_iterator_tag)
{
  while (first != last && !equal (*first)) ++first;
  return first;
}

template <class Iter,class Predicate>
Iter find_if (Iter first,Iter last,Predicate pred,random_access_iterator_tag)
{
  typename iterator_traits<Iter>::difference_type trip_count = (last-first)>>2;  

  while (trip_count--)
  {
    if (pred (*first)) return first;
    ++first;
    if (pred (*first)) return first;
    ++first;    
    if (pred (*first)) return first;
    ++first;    
    if (pred (*first)) return first;
    ++first;    
  }

  switch (last-first) 
  {
    case 3:  if (pred (*first)) return first; ++first;
    case 2:  if (pred (*first)) return first; ++first;
    case 1:  if (pred (*first)) return first; ++first;
    default: 
    case 0:  return last;
  }
}

template <class Iter,class Predicate>
inline Iter find_if (Iter first,Iter last,Predicate pred)
{
  return find_if (first,last,pred,iterator_category<Iter> ());
}

template <class Iter,class T,class Size>
void count (Iter first,Iter last,const T& value,Size& n)
{
  for (;first!=last;++first)
    if (*first==value)
      ++n;  
} 

template <class Iter,class Predicate,class Size>
void count_if (Iter first,Iter last,Predicate pred,Size& n)
{
  for (;first!=last;++first)
    if (pred (*first)) 
      ++n;
}

template <class Iter,class T>
inline typename iterator_traits<Iter>::difference_type count (Iter first,Iter last,const T& value)
{
  typename iterator_traits<Iter>::difference_type n = 0;  
  
  count (first,last,value,n);
  
  return n;
}

template <class Iter,class Predicate>
inline typename iterator_traits<Iter>::difference_type count_if (Iter first,Iter last,Predicate equal)
{
  typename iterator_traits<Iter>::difference_type n = 0;  
  
  count_if (first,last,equal,n);
  
  return n;  
}

template <class InIter,class Iter,class Predicate>
InIter find_first_of (InIter first1,InIter last1,Iter first2,Iter last2,Predicate equal)
{
  for (;first1!=last1;++first1) 
    for (Iter iter=first2;iter!=last2;++iter)
      if (equal (*first1,*iter))
        return first1;
        
  return last1;
}

template <class InIter,class Iter>
inline InIter find_first_of (InIter first1,InIter last1,Iter first2,Iter last2)
{
  return find_first_of (first1,last1,first2,last2,iter_equal<InIter,Iter> ());
}

template <class Iter1,class Iter2,class Predicate> 
Iter1 find_end (Iter1 first1,Iter1 last1,Iter2 first2,Iter2 last2,Predicate equal,forward_iterator_tag,forward_iterator_tag)
{
  if (first2 == last2)
    return last1;
    
  Iter1 result = last1;
  
  while (1) 
  {
    Iter1 new_result = search (first1,last1,first2,last2,equal);
    
    if (new_result == last1)
      return result;

    first1 = result = new_result;
    ++first1;
  }
}

template <class Iter1,class Iter2,class Predicate> 
Iter1 find_end (Iter1 first1,Iter1 last1,Iter2 first2,Iter2 last2,Predicate equal,bidirectional_iterator_tag,bidirectional_iterator_tag)
{
  typedef reverse_iterator<Iter1> RevIter1;
  typedef reverse_iterator<Iter2> RevIter2;

  RevIter1 rlast1 (first1);
  RevIter2 rlast2 (first2);
  RevIter1 rresult = search (RevIter1 (last1),rlast1,RevIter2 (last2),rlast2,equal);

  if (rresult == rlast1)
    return last1;
    
  Iter1 result = rresult.base ();
  
  advance (result,-distance (first2,last2));
  
  return result;
}

template <class Iter1,class Iter2,class Predicate> 
inline Iter1 find_end (Iter1 first1,Iter1 last1,Iter2 first2,Iter2 last2,Predicate equal)
{
  return find_end (first1,last1,first2,last2,equal,iterator_category<Iter1> (),iterator_category<Iter2> ());
}

template <class Iter1,class Iter2> 
inline Iter1 find_end (Iter1 first1,Iter1 last1,Iter2 first2,Iter2 last2)
{
  return find_end (first1,last1,first2,last2,iter_equal<Iter1,Iter2> ());
}

template <class Iter,class Predicate>
Iter adjacent_find (Iter first,Iter last,Predicate equal)
{
  if (first == last) 
    return last;

  for (Iter next=first;++next!=last;first=next)
    if (equal (*first,*next))
      return first;
      
  return last;  
}

template <class Iter>
inline Iter adjacent_find (Iter first,Iter last)
{
  return adjacent_find (first,last,iter_equal<Iter> ());
}

template <class Iter1,class Iter2,class Predicate>
Iter1 search (Iter1 first1,Iter1 last1,Iter2 first2,Iter2 last2,Predicate equal)
{
  if (first1 == last1 || first2 == last2)
    return first1;
    
  // Test for a pattern of length 1
  Iter2 tmp = first2;
  ++tmp;
  
  if (tmp == last2) 
  {
    while (first1 != last1 && !equal (*first1,*first2)) ++first1;
    return first1;
  } 

  // General case.
  
  Iter2 p1 = tmp;
  
  while (first1 != last1) 
  {     
    for (;first1!=last1 && !equal (*first1,*first2);++first1); //поиск первого совпадения
                           
    if (first1 == last1)
      return last1;

    Iter2 p = p1;
    Iter1 current = first1;     
    
    if (++current == last1) 
      return last1;          

    while (equal (*current,*p))
    {    
      if (++p == last2)       return first1; //возвращение позиции соответствия     
      if (++current == last1) return last1;  //поиск закончен неудачей
    }    
    
    ++first1;    
  }
  
  return first1;  
}

template <class Iter1,class Iter2>
inline Iter1 search (Iter1 first1,Iter1 last1,Iter2 first2,Iter2 last2)
{
  return search (first1,last1,first2,last2,iter_equal<Iter1,Iter2> ());
}

template <class Iter,class Int,class T,class Predicate>
Iter search_n (Iter first,Iter last,Int count,const T& val,Predicate equal)
{
  if (count <= 0)
    return first;   
  
  for (;first!=last && !equal (*first,val);++first);
  
  while (first != last) 
  {
    Int  n = count - 1;
    Iter i = first;
    ++i;
    
    while (i != last && n != 0 && equal (*i,val)) ++i, --n;

    if (!n) 
      return first;

    for (;i!=last && !equal (*i,val);++i);
    
    first = i;      
  }
  
  return last;
}

template <class Iter,class Int,class T>
inline Iter search_n (Iter first,Iter last,Int count,const T& val)
{
  return search_n (first,last,count,val,equal_to<typename iterator_traits<Iter>::value_type,T> ());
}

template <class Iter,class Compare> 
Iter max_element (Iter first,Iter last,Compare less)
{
  if (first == last) 
    return first;
    
  Iter result = first;
  
  while (++first != last) 
    if (less (*result,*first))
      result = first;
      
  return result;  
}

template <class Iter> 
inline Iter max_element (Iter first,Iter last)
{
  return max_element (first,last,iter_less<Iter> ());
}

template <class Iter,class Compare> 
Iter min_element (Iter first,Iter last,Compare less)
{
  if (first == last) 
    return first;
    
  Iter result = first;
  
  while (++first != last) 
    if (less (*first,*result))
      result = first;
      
  return result;
}

template <class Iter> 
inline Iter min_element (Iter first,Iter last)
{
  return min_element (first,last,iter_less<Iter> ());
}

template <class Iter,class T,class Compare>
Iter lower_bound (Iter first,Iter last,const T& val,Compare less)
{
  typename iterator_traits<Iter>::difference_type len = distance (first,last), half;
  Iter middle;   
  
  while (len)
  {
    half   = len >> 1;
    middle = first;
    
    advance (middle,half);
    
    if (less (*middle,val)) 
    {
      first = middle;
      ++first;      
      len -= half+1;     
    }
    else len = half;
  }
  
  return first; 
}

template <class Iter,class T>
inline Iter lower_bound (Iter first,Iter last,const T& val)
{
  return lower_bound (first,last,val,less<typename iterator_traits<Iter>::value_type,T> ());
}

template <class Iter,class T,class Compare>
Iter upper_bound (Iter first,Iter last,const T& val,Compare less)
{
  typename iterator_traits<Iter>::difference_type len = distance (first,last), half;
  Iter middle;

  while (len) 
  {
    half   = len >> 1;
    middle = first;
    
    advance (middle,half);
    
    if (less (val,*middle)) len = half;
    else 
    {
      first = middle;
      ++first;
      len -= half + 1;
    }
  }
  
  return first;
}

template <class Iter,class T>
inline Iter upper_bound (Iter first,Iter last,const T& val)
{
  return upper_bound (first,last,val,less<T,typename iterator_traits<Iter>::value_type> ());
}

template <class Iter,class T,class Compare>
inline bool binary_search (Iter first,Iter last,const T& val,Compare less)
{
  Iter i = lower_bound (first,last,val,less);
  return i != last && !less (val,*i);
}

template <class Iter,class T>
inline bool binary_search (Iter first,Iter last,const T& val)
{
  return binary_search (first,last,val,less<typename iterator_traits<Iter>::value_type,T> ());
}

template <class Iter,class T,class Compare>
pair<Iter,Iter> equal_range (Iter first,Iter last,const T& val,Compare less)
{
  typename iterator_traits<Iter>::difference_type len = distance (first,last), half;
  Iter middle, left, right;
  
  while (len) 
  {
    half   = len >> 1;
    middle = first;
    
    advance (middle,half);
    
    if (less (*middle,val)) 
    {
      first = middle;
      ++first;
      len -= half + 1;
    }
    else if (less (val,*middle)) len = half;
    else 
    {
      left =  lower_bound (first,middle,val,less);
      advance (first, len);
      right = upper_bound (++middle,first,val,less);
      
      return pair<Iter,Iter> (left,right);
    }
  }
  
  return pair<Iter,Iter> (first,first);
}

template <class Iter,class T>
pair<Iter,Iter> equal_range (Iter first,Iter last,const T& val)
{
  typename iterator_traits<Iter>::difference_type len = distance (first,last), half;
  Iter middle, left, right;
  
  while (len) 
  {
    half   = len >> 1;
    middle = first;
    
    advance (middle,half);
    
    if (*middle < val) 
    {
      first = middle;
      ++first;
      len -= half + 1;
    }
    else if (val < *middle) len = half;
    else 
    {
      left =  lower_bound (first,middle,val);
      advance (first, len);
      right = upper_bound (++middle,first,val);
      
      return pair<Iter,Iter> (left,right);
    }
  }
  
  return pair<Iter,Iter> (first,first);
}

/*
    Копирование, перемещение, замена
*/

template <class InIter,class OutIter,class T>
OutIter remove_copy (InIter first,InIter last,OutIter result,const T& value)
{
  for (;first!=last;++first)
    if (*first != value)
    {
      *result = *first;
      ++result;
    }
    
  return result;
}

template <class InIter,class OutIter,class Predicate>
OutIter remove_copy_if (InIter first,InIter last,OutIter result,Predicate pred)
{
  for (;first!=last;++first)
    if (!pred (*first)) 
    {
      *result = *first;
      ++result;
    }
    
  return result;
}

template <class Iter,class T>
inline Iter remove (Iter first,Iter last,const T& value)
{
  first = find (first,last,value);
  
  Iter i = first;
  
  return first == last ? first : remove_copy (++i,last,first,value);  
}

template <class Iter,class Predicate>
inline Iter remove_if (Iter first,Iter last,Predicate pred)
{
  first = find_if (first,last,pred);  
  
  Iter i = first;
  
  return first == last ? first : remove_copy_if (++i,last,first,pred);
}

template <class Iter,class T>
void replace (Iter first,Iter last,const T& old_value,const T& new_value)
{
  for (;first!=last;++first)
    if (*first == old_value)
      *first = new_value;
}

template <class Iter,class Predicate,class T>
void replace_if (Iter first,Iter last,Predicate pred,const T& new_value)
{
  for (;first!=last;++first)
    if (pred(*first))
      *first = new_value;
}

template <class InIter,class OutIter,class T>
OutIter replace_copy (InIter first,InIter last,OutIter result,const T& old_value,const T& new_value)
{
  for (;first!=last;++first,++result)
    *result = *first == old_value ? new_value : *first;
    
  return result;
}

template <class InIter,class OutIter,class Predicate,class T>
OutIter replace_copy_if (InIter first,InIter last,OutIter result,Predicate pred,const T& new_value)
{
  for (;first!=last;++first,++result)
    *result = pred (*first) ? new_value : *first;
    
  return result;
}

template <class InIter,class OutIter,class Predicate>
OutIter unique_copy (InIter first,InIter last,OutIter result,Predicate equal,output_iterator_tag)
{
  if (first == last) 
    return result;  

  const typename iterator_traits<InIter>::value_type value = *first;

  *result = value;
  
  while (++first != last)
    if (!equal (value,*first)) 
    {
      value = *first;
      *++result = value;
    }
    
  return ++result;
}

template <class InIter,class OutIter,class Predicate>
OutIter unique_copy (InIter first,InIter last,OutIter result,Predicate equal,forward_iterator_tag)
{
  if (first == last) 
    return result;  

  *result = *first;
  
  while (++first!=last)
    if (!equal (*result,*first))
      *++result = *first;
      
  return ++result;
}

template <class InIter,class OutIter,class Predicate>
inline OutIter unique_copy (InIter first,InIter last,OutIter result,Predicate equal)
{
  return unique_copy (first,last,result,equal,iterator_category<OutIter> ());
}

template <class InIter,class OutIter>
inline OutIter unique_copy (InIter first,InIter last,OutIter result)
{
  return unique_copy (first,last,result,iter_equal<InIter> ());
}

template <class Iter,class Predicate>
inline Iter unique (Iter first,Iter last,Predicate pred)
{
  first = adjacent_find (first,last,pred);
  return unique_copy (first,last,first,pred);
}

template <class Iter>
inline Iter unique (Iter first,Iter last)
{
  return unique (first,last,iter_equal<Iter> ());
}

template <class Iter1,class Iter2>
Iter2 swap_ranges (Iter1 first1,Iter1 last1,Iter2 first2)
{
  for (;first1!=last1;++first1,++first2)
    iter_swap (first1,first2);
    
  return first2;
}

/*
    Преобразования интервала
*/

template <class Iter>
void reverse (Iter first,Iter last,bidirectional_iterator_tag)
{
  for (;;)
    if (first==last || first==--last) return;
    else iter_swap (first++,last);
}

template <class Iter>
void reverse (Iter first,Iter last,random_access_iterator_tag)
{
  while (first < last) iter_swap (first++,--last);
}

template <class Iter>
inline void reverse (Iter first,Iter last)
{
  reverse (first,last,iterator_category<Iter> ());
}

template <class InIter,class OutIter>
OutIter reverse_copy (InIter first,InIter last,OutIter result)
{
  for (;first!=last;*result=*--last,++result);
  return result;  
}

template <class Iter>
Iter rotate (Iter first,Iter middle,Iter last,forward_iterator_tag)
{
  if (first == middle)
    return last;
    
  if (last == middle)
    return first;

  Iter first2 = middle;
  
  do 
  {
    swap (*first++,*first2++);
    
    if (first == middle)
      middle = first2;
  } while (first2 != last);

  Iter new_middle = first;

  first2 = middle;

  while (first2 != last) 
  {
    swap (*first++,*first2++);
    
    if      (first  == middle) middle = first2;
    else if (first2 == last)   first2 = middle;
  }

  return new_middle;
}

template <class Iter>
Iter rotate (Iter first,Iter middle,Iter last,bidirectional_iterator_tag)
{
  if (first == middle)
    return last;
    
  if (last == middle)
    return first;

  reverse (first,middle,bidirectional_iterator_tag ());
  reverse (middle,last,bidirectional_iterator_tag ());

  while (first != middle && middle != last)
    swap (*first++,*--last);

  if (first == middle) 
  {
    reverse (middle,last,bidirectional_iterator_tag ());
    return last;
  }
  else 
  {
    reverse (first,middle,bidirectional_iterator_tag ());
    return first;
  }
}

template <class Int>
inline Int gcd (Int count,Int factor)
{
  while (factor)
  {
    Int t  = count % factor;
    count  = factor;
    factor = t;
  }
  
  return count;
}

template <class Iter>
Iter rotate (Iter first,Iter middle,Iter last,random_access_iterator_tag)
{
  typedef typename iterator_traits<Iter>::difference_type Distance;
  typedef typename iterator_traits<Iter>::value_type T;

  Distance count      = last-first, 
           left_part  = middle-first,
           right_part = count-left_part;
           
  Iter result = first+(last-middle);           
           
  if (!left_part) 
    return last;
  
  if (left_part == right_part) 
  {
    swap_ranges (first,middle,middle);
    return result;
  } 
           
  Distance d = gcd (count,left_part);   
  
  if (left_part < right_part)  
  {
    for (Distance i=d;i--;++first)
    {
      T    tmp = *first;
      Iter p   = first;
      
      for (Distance j=right_part/d;j--;)
      {
        if (p > first+right_part) 
        {
          *p  = *(p-right_part);
          p  -= right_part;
        }

        *p  = *(p+left_part);
        p  += left_part;
      }
      
      *p = tmp;
    }      
  }
  else
  {
    for (Distance i=d;i--;++first)
    {
      T    tmp = *first;
      Iter p   = first;
      
      for (Distance j=left_part/d-1;j--;)
      {
        if (p < last-left_part) 
        {
          *p  = *(p+left_part);
          p  += left_part;
        }

        *p  = *(p-right_part);
        p  -= right_part;
      }      
      
      *p = tmp;
    }    
  }

  return result;
}

template <class Iter>
inline Iter rotate (Iter first,Iter middle,Iter last)
{
  return rotate (first,middle,last,iterator_category<Iter> ());
}

template <class InIter,class OutIter>
inline OutIter rotate_copy (InIter first,InIter middle,InIter last,OutIter result)
{
  return copy (first,middle,copy (middle,last,result));
}

template <class Iter,class Compare>
bool next_permutation (Iter first,Iter last,Compare less)
{
  if (first == last)
    return false;
    
  Iter i=first; 
  
  if (++i==last)
    return false;
    
  for(i=last,--i;;) 
  {
    Iter ii = i--;
    
    if (less (*i,*ii)) 
    {
      Iter j = last;
      
      while (!less (*i,*--j));
      
      iter_swap (i,j);
      reverse   (ii,last);
      
      return true;
    }
    
    if (i == first) 
    {
      reverse (first,last);
      return false;
    }
  }
}

template <class Iter>
inline bool next_permutation (Iter first,Iter last)
{
  return next_permutation (first,last,iter_less<Iter> ());
}

template <class Iter,class Compare>
bool prev_permutation (Iter first,Iter last,Compare less)
{
  if (first == last)
    return false;
    
  Iter i = first;
    
  if (++i == last)
    return false;
    
  for(i=last,--i;;) 
  {
    Iter ii = i--;

    if (less (*ii,*i)) 
    {
      Iter j = last;
      
      while (!less (*--j,*i));
      
      iter_swap (i,j);
      reverse   (ii,last);
      
      return true;
    }
    
    if (i == first) 
    {
      reverse (first,last);
      return false;
    }
  }
}

template <class Iter>
inline bool prev_permutation (Iter first,Iter last)
{
  return prev_permutation (first,last,iter_less<Iter> ());
}

template <class Iter,class Rand>
void random_shuffle (Iter first,Iter last,Rand random)
{
  if (first == last) 
    return;
    
  for (Iter i=first+1;i!=last;++i)
    iter_swap (i,first+random ((i-first)+1));
}

template <class Iter>
void random_shuffle (Iter first,Iter last)
{
  if (first == last) 
    return;
    
  for (Iter i=first+1;i!=last;++i)
    iter_swap (i,first+random_number ((i-first)+1));
}

template <class Iter,class Func>
inline Func for_each (Iter first,Iter last,Func fn)
{
  for (;first!=last;++first) fn (*first);
  return fn;
}

template <class InIter,class OutIter,class UnaryOperation>
inline OutIter transform (InIter first,InIter last,OutIter result,UnaryOperation op)
{
  for (;first!=last;++first,++result) *result = op (*first);
  return result;
}

template <class InIter1,class InIter2,class OutIter,class BinOperation>
inline OutIter transform (InIter1 first1,InIter1 last1,InIter2 first2,OutIter result,BinOperation op)
{ 
  for (;first1!=last1;++first1,++first2,++result) *result = op (*first1,*first2);
  return result;  
}

/*
    Генераторы
*/

template <class Iter,class Generator> 
void generate (Iter first,Iter last,Generator gen)
{
  for (;first!=last;++first) *first = gen();
}

template <class Iter,class Size,class Generator> 
void generate_n (Iter first,Size count,Generator gen)
{
  generate (first,first+count,gen);
}

