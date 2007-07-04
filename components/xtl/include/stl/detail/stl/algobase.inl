/*
    Обмен
*/

template <class T> 
inline void swap (T& a,T& b)
{
  T t = a;
  a   = b;
  b   = t;
}

template <class Iter1,class Iter2>
inline void iter_swap (Iter1 a,Iter2 b)
{
  swap (*a,*b);
}

/*
    Возвращение случайного числа  
*/

template <class Distance>
inline Distance random_number (Distance n)
{
  return rand () % n;
}

/*
    Минимум / максимум
*/

template <class T,class Compare>
inline const T& min (const T& a,const T& b,Compare less)
{
  return less (b,a) ? b : a;
}

template <class T,class Compare>
inline const T& max (const T& a,const T& b,Compare less)
{
  return less (a,b) ? b : a;
}

template <class T>
inline const T& min (const T& a,const T& b)
{
  return b < a ? b : a;
}

template <class T>
inline const T& max (const T& a,const T& b)
{
  return a < b ? b : a;
}

template <class T>
inline const T& median (const T& a,const T& b,const T& c) 
{ 
  if (a < b)  return b < c ? b : a < c ? c : a;
  else        return a < c ? a : c < b ? b : c;
}

template <class T,class Compare>
inline const T& median (const T& a,const T& b,const T& c,Compare less) 
{ 
  if (less (a,b)) return less (b,c) ? b : less (a,c) ? c : a;
  else            return less (a,c) ? a : less (c,b) ? b : c;
}

/*
    Тривиальные случаи копирования (копирование примитивных типов данных)
*/

template <class T>
inline T* trivial_copy (const T* first,const T* last,T* result)
{
  memcpy (result,first,sizeof (T) * (last - first)); //no overlap by standart
  return result + (last - first);
}

template <class T>
inline T* trivial_move (const T* first,const T* last,T* result)
{
  memmove (result,first,sizeof (T) * (last - first));
  return result + (last - first);
}

/*
    Копирование диапозона в прямом порядке
*/

template <class InIter,class OutIter>
inline OutIter __copy (InIter first,InIter last,OutIter result,input_iterator_tag)
{
  for (;first!=last;++first,++result) *result = *first;
  return result;
}

template <class InIter,class OutIter>
inline OutIter __copy (InIter first,InIter last,OutIter result,random_access_iterator_tag)
{
  for (typename iterator_traits<InIter>::difference_type n=last-first;n--;++first,++result) 
    *result = *first;
    
  return result;
}

template <class InIter,class OutIter>
inline OutIter copy_aux (InIter first,InIter last,OutIter result,false_type)
{
  return __copy (first,last,result,iterator_category<InIter> ());
}

template <class InIter,class OutIter>
inline OutIter copy_aux (InIter first,InIter last,OutIter result,true_type)
{
  return __copy (first,last,result,iterator_category<InIter> ());
}

template <class T>
inline T* copy_aux (const T* first,const T* last,T* result,true_type)
{
  return trivial_copy (first,last,result);
}

template <class T>
inline T* copy_aux (T* first,T* last,T* result,true_type)
{
  return trivial_copy (first,last,result);
}

template <class InIter,class OutIter>
inline OutIter copy (InIter first,InIter last,OutIter result)
{
  typedef typename iterator_traits<OutIter>::value_type value_type;
  return copy_aux (first,last,result,typename type_traits<value_type>::has_trivial_assignment_operator ());
}

/*
    Копирование n элементов первого диапозона во второй
*/

template <class InIter,class Size,class OutIter>
inline pair<InIter,OutIter> __copy_n (InIter first,Size count,OutIter result,input_iterator_tag) 
{
  for (;count>0;--count,++first,++result)
    *result = *first;

  return pair<InIter,OutIter> (first,result);
}

template <class InIter,class Size,class OutIter>
inline pair<InIter,OutIter> __copy_n (InIter first,Size count,OutIter result,random_access_iterator_tag) 
{
  InIter last = first + count;
  
  return pair<InIter,OutIter> (last,copy (first,last,result));
}

template <class InIter,class Size,class OutIter>
inline pair<InIter,OutIter> copy_n (InIter first,Size count,OutIter result)
{
  return __copy_n (first,count,result,iterator_category<InIter> ());
}

/*
    Копирование диапозона в обратном порядке
*/

template <class InIter,class OutIter>
inline OutIter __copy_backward (InIter first,InIter last,OutIter result,bidirectional_iterator_tag)
{
  while (first != last) *--result = *--last;
  return result;  
}

template <class InIter,class OutIter>
inline OutIter __copy_backward (InIter first,InIter last,OutIter result,random_access_iterator_tag)
{
  for (typename iterator_traits<InIter>::difference_type n=last-first;n;--n)
    *--result = *--last;    
  return result;
}

template <class InIter,class OutIter>
inline OutIter copy_backward_aux (InIter first,InIter last,OutIter result,false_type)
{
  return __copy_backward (first,last,result,iterator_category<InIter> ());
}

template <class InIter,class OutIter>
inline OutIter copy_backward_aux (InIter first,InIter last,OutIter result,true_type)
{
  return __copy_backward (first,last,result,iterator_category<InIter> ());
}

template <class T>
inline T* copy_backward_aux (const T* first,const T* last,T* result,true_type)
{
  return trivial_move (first,last,result-(last-first));
}

template <class T>
inline T* copy_backward_aux (T* first,T* last,T* result,true_type)
{
  return trivial_move (first,last,result-(last-first));
}

template <class InIter,class OutIter>
inline OutIter copy_backward (InIter first,InIter last,OutIter result)
{
  typedef typename iterator_traits<OutIter>::value_type value_type;
  return copy_backward_aux (first,last,result,typename type_traits<value_type>::has_trivial_assignment_operator ());
}

/*
    Инициализация диапозона 
*/

template <class InIter,class OutIter>
OutIter __uninitialized_copy (InIter first,InIter last,OutIter result,input_iterator_tag)
{
  OutIter cur = result;
    
  try
  {
    for (;first!=last;++first,++cur)
      construct (&*cur,*first);
      
    return cur;
  }
  catch (...)
  {
    destroy (result,cur);
    throw;
  }
}

template <class InIter,class OutIter>
OutIter __uninitialized_copy (InIter first,InIter last,OutIter result,random_access_iterator_tag)
{
  OutIter cur = result;
     
  try
  {
    for (typename iterator_traits<InIter>::difference_type n=last-first;n--;++first,++cur)
      construct (&*cur,*first);
      
    return cur;
  }
  catch (...)
  {
    destroy (result,cur);
    throw;
  }
}

template <class InIter,class OutIter>
inline OutIter uninitialized_copy_aux (InIter first,InIter last,OutIter result,false_type)
{
  return __uninitialized_copy (first,last,result,iterator_category<InIter> ());
}

template <class InIter,class OutIter>
inline OutIter uninitialized_copy_aux (InIter first,InIter last,OutIter result,true_type)
{
  return copy (first,last,result);
}

template <class InIter,class OutIter>
inline OutIter uninitialized_copy (InIter first,InIter last,OutIter result)
{
  return uninitialized_copy_aux (first,last,result,
              typename type_traits<typename iterator_traits<OutIter>::value_type>::is_POD_type ());
}

/*
    Инициализация n первых элементов диапозона (копированием элементов из другого диапозона)
*/

template <class InIter,class Size,class OutIter>
pair<InIter,OutIter> __uninitialized_copy_n (InIter first,Size count,OutIter result,input_iterator_tag)
{
  OutIter cur = result;
  
  try 
  {
    for (;count>0;--count,++first,++cur) 
      construct (&*cur,*first);
      
    return pair<InIter,OutIter> (first,cur);
  }
  catch (...)
  {
    destroy (result,cur);
    throw;
  }
}

template <class InIter,class Size,class OutIter>
inline pair<InIter,OutIter> __uninitialized_copy_n (InIter first,Size count,OutIter result,random_access_iterator_tag)
{
  InIter last = first + count;
  return pair<InIter,OutIter> (last,uninitialized_copy (first,last,result));
}

template <class _InputIter, class _Size, class _ForwardIter>
inline pair<_InputIter, _ForwardIter>
__uninitialized_copy_n(_InputIter __first, _Size __count,
                     _ForwardIter __result) {
  return __uninitialized_copy_n(__first, __count, __result,
                                __ITERATOR_CATEGORY(__first));
}

template <class InIter,class Size,class OutIter>
inline pair<InIter,OutIter> uninitialized_copy_n (InIter first,Size count,OutIter result) 
{
  return __uninitialized_copy_n (first,count,result,iterator_category<InIter> ());
}

/*
    Заполнение диапозона
*/

template <class Iter,class T>
inline void fill (Iter first,Iter last,const T& val)
{
  for (;first!=last;++first) *first = val;
}

inline void fill (char* first,char* last,int c)
{
  memset (first,c,last-first);
}

template <class Iter,class Size,class T>
inline Iter fill_n (Iter first,Size n,const T& value)
{
  for (;n>0;--n,++first) *first = value;
  return first;
}

/*
    Инициализация диапозона значением value
*/

template <class Iter,class T>
void __uninitialized_fill (Iter first,Iter last,const T& value,input_iterator_tag)
{
  Iter cur = first;
  
  try 
  {
    for (;cur!=last;++cur)
      construct (&*cur,value);
  }
  catch (...)
  {
    destroy (first,cur);
    throw;
  }
}

template <class Iter,class T>
void __uninitialized_fill (Iter first,Iter last,const T& value,random_access_iterator_tag)
{
  Iter cur = first;
  
  try 
  {
    for (typename iterator_traits<Iter>::difference_type n=last-first;n;--n,++cur)
      construct (&*cur,value);
  }
  catch (...)
  {
    destroy (first,cur);
    throw;
  }
}

template <class Iter,class T>
inline void uninitialized_fill_aux (Iter first,Iter last,const T& value,false_type)
{
  return __uninitialized_fill (first,last,value,iterator_category<Iter> ());
}

template <class Iter,class T>
inline void uninitialized_fill_aux (Iter first,Iter last,const T& value,true_type)
{
  return fill (first,last,value);
}

template <class Iter,class T>
inline void uninitialized_fill (Iter first,Iter last,const T& value)
{
  typedef typename iterator_traits<Iter>::value_type value_type;
  return uninitialized_fill_aux (first,last,value,typename type_traits<value_type>::is_POD_type ());
}

inline void unitialized_fill (signed char* first,signed char* last,int c)
{
  memset (first,c,last-first);
}

inline void unitialized_fill (unsigned char* first,unsigned char* last,int c)
{
  memset (first,c,last-first);
}

/*
    Инициализация первых n элементов диапозона значением value
*/

template <class Iter,class Size,class T>
Iter __uninitialized_fill_n (Iter first,Size count,const T& x,false_type)
{
  Iter cur = first;
  
  try 
  {
    for (;count>0;--count,++cur)
      construct (&*cur,x);
      
    return cur;
  }
  catch (...)
  {
    destroy (first,cur);
    throw;
  }
}

template <class Iter,class Size,class T>
inline Iter __uninitialized_fill_n (Iter first,Size count,const T& x,true_type)
{
  return fill_n (first,count,x);
}

template <class Iter,class Size,class T>
inline Iter uninitialized_fill_n (Iter first,Size count,const T& value)
{
  typedef typename iterator_traits<Iter>::value_type value_type;
  return __uninitialized_fill_n (first,count,value,typename type_traits<value_type>::is_POD_type ());
}

inline signed char* uninitialized_fill_n (signed char* first,size_t count,int value)
{
  memset (first,value,count);
  return first+count;
}

inline unsigned char* uninitialized_fill_n (unsigned char* first,size_t count,int value)
{
  memset (first,value,count);
  return first+count;
}

/*
    Определение первого несовпадающего элемента диапозонов
*/

template <class Iter1,class Iter2>
inline pair<Iter1,Iter2> mismatch (Iter1 first1,Iter1 last1,Iter2 first2)
{
  while (first1 != last1 && *first1 == *first2) ++first1, ++first2;  
  return pair<Iter1,Iter2> (first1,first2);  
}

template <class Iter1,class Iter2,class BinPredicate>
inline pair<Iter1,Iter2> mismatch (Iter1 first1,Iter1 last1,Iter2 first2,BinPredicate comp)
{
  while (first1 != last1 && comp (*first1,*first2)) ++first1, ++first2;
  return pair<Iter1,Iter2> (first1,first2);
}

/*
    Предикат эквивалентности диапозонов
*/

template <class Iter1,class Iter2,class BinPredicate>
inline bool equal (Iter1 first1,Iter1 last1,Iter2 first2,BinPredicate comp)
{
  for (;first1!=last1;++first1,++first2)
    if (!comp (*first1,*first2))
      return false;     
      
  return true;    
}

template <class Iter1,class Iter2>
inline bool equal (Iter1 first1,Iter1 last1,Iter2 first2)
{
  for (;first1!=last1;++first1,++first2)
    if (*first1 != *first2)
      return false;
      
  return true;    
}

inline bool equal (const signed char* first1,const signed char* last1,const signed char* first2)
{
  return memcmp (first1,first2,last1-first1) == 0;
}

inline bool equal (const unsigned char* first1,const unsigned char* last1,const unsigned char* first2)
{
  return memcmp (first1,first2,last1-first1) == 0;
}

/*
    Лексикографическое сравнение диапозонов
*/

template <class Iter1,class Iter2>
inline int lexicographical_compare_3way (Iter1 first1,Iter1 last1,Iter2 first2,Iter2 last2)
{
  for (;first1 != last1 && first2 != last2;++first1,++first2)  
  {
    if (*first1 < *first2) return -1;
    if (*first2 < *first1) return 1;
  }
  
  return first2 == last2 ? !(first1 == last1) : -1; 
}

template <class T>
inline int lexicographical_compare_3way 
 (const unsigned char* first1,
  const unsigned char* last1,
  const unsigned char* first2,
  const unsigned char* last2)
{
  ptrdiff_t len1 = last1 - first1,
            len2 = last2 - first2;
            
  int result = memcmp (first1,first2,min (len1,len2));
  
  return result != 0 ? result : (len1 == len2 ? 0 : (len1 < len2 ? -1 : 1));
}

template <class T>
inline int lexicographical_compare_3way 
 (const signed char* first1,
  const signed char* last1,
  const signed char* first2,
  const signed char* last2)
{
  return lexicographical_compare_3way ((const unsigned char*)first1,(const unsigned char*)last1,
                                       (const unsigned char*)first2,(const unsigned char*)last2);
}                                  

template <class Iter1,class Iter2>
inline bool lexicographical_compare (Iter1 first1,Iter1 last1,Iter2 first2,Iter2 last2)
{
  return lexicographical_compare_3way (first1,last1,first2,last2) < 0;
}

template <class Iter1,class Iter2,class Compare>
inline bool lexicographical_compare (Iter1 first1,Iter1 last1,Iter2 first2,Iter2 last2,Compare comp)
{
  for (;first1 != last1 && first2 != last2;++first1,++first2) 
  {
    if (comp (*first1,*first2)) return true;
    if (comp (*first2,*first1)) return false;
  }
  
  return first1 == last1 && first2 != last2;
}
