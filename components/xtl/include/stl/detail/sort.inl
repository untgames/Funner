const int _ISORT_MAX = 32; //???name???
const int __stl_chunk_size = 7; //????

/*
    Сортировка методом Шелла
*/

template <class Iter,class Compare>
void shell_sort (Iter first,Iter last,Compare less)
{
  typedef typename iterator_traits<Iter>::difference_type size_type;
  typedef typename iterator_traits<Iter>::value_type value_type;
  size_type count = last-first;
   
  switch (count)
  {
    case 0:
    case 1:  
      return;    
    case 2:
      if (less (*(first+1),*first)) swap (*first,*(first+1));
      return;
    default:
    {
      for (size_type inc=count;inc;inc=inc>1 && inc<5 ? 1 : 5*inc/11)
      {
        for (size_type i=inc;i<count;i++)
        {         
          Iter dst = first+i, src = dst-inc, end = src-(i/inc)*inc;
          value_type tmp = *dst; 
          
          for (;src!=end && less (tmp,*src);src-=inc,dst-=inc)
            *dst = *src;
          
          *dst = tmp;
        }
      }
    
      return;
    }  
  } 
}

/*
    Вспомагательное разбиение. Возвращает диапозон эквивалентности [first;second)
*/

//определение медианы
template< class Iter,class Compare> 
inline void __med3 (Iter first,Iter mid,Iter last,Compare less)
{ 
  if (less (*mid,*first)) iter_swap (mid,first);
  if (less (*last,*mid))  iter_swap (last,mid);
  if (less (*mid,*first)) iter_swap (mid,first);
}

template <class Iter,class Compare>
inline void __median (Iter first,Iter mid,Iter last,Compare less)
{ 
    // sort median element to middle
  if (last-first > 40) //медиана из 9-ти элементов
  { 
    volatile int step = (last-first+1) / 8;  //volatile для обхода бага компилятора gcc 4.0 под MacOS
    
    __med3 (first,first+step,first+2*step,less);
    __med3 (mid-step,mid,mid+step,less);
    __med3 (last-2*step,last-step,last,less);
    __med3 (first+step,mid,last-step,less);
  }
  else __med3 (first,mid,last,less);
}

template <class Iter,class Compare>
inline bool __order_equal (Iter a,Iter b,Compare less)
{
  return !less (*a,*b) && !less (*b,*a);
}

template <class Iter>
inline bool __order_equal (Iter a,Iter b)
{
  return !(*a<*b) && !(*b<*a);
}

template <class Iter,class Compare> 
pair<Iter,Iter> __unguarded_partition (Iter first,Iter last,Compare less)
{ 
  Iter mid = first+(last-first) / 2, pfirst = mid, plast = pfirst + 1;  
  
  __median (first,mid,last-1,less); // mid - указывает на середину диапозона

    //определение эквивалентного диапозона
  while (pfirst != first && __order_equal (pfirst-1,pfirst,less)) --pfirst;
  while (plast  != last  && __order_equal (plast,pfirst,less))    ++plast;

  Iter gfirst = plast, glast = pfirst; //эквивалентный диапозон [gfirst;glast)=[plast;pfirst)

  for (;;)
  { 
      //поиск элементов нарушающих порядок следования (относительно диапозона эквивалентности)
    for (;gfirst!=last;++gfirst)
      if      (less (*pfirst,*gfirst)) ;
      else if (less (*gfirst,*pfirst)) break; //найдено нарушение порядка
      else                             iter_swap (plast,gfirst), ++plast; //расширение диапозона эквивалентности
      
    for (;glast!=first;--glast)
      if      (less (*(glast-1),*pfirst))  ;
      else if (less (*pfirst,*(glast-1)))  break; //найдено нарушение порядка
      else                                 iter_swap (--pfirst,glast-1); //рсширение диапозона эквивалентности
      
    if (glast==first && gfirst==last)
      return pair<Iter,Iter> (pfirst,plast); //возвращаем диапозон эквивалентности

    if (glast==first) //достигнут предел слева, сдвигаем диапозон эквивалентности вправо
    {               
      if (plast!=gfirst)
        iter_swap (pfirst,plast);
             
      iter_swap (pfirst,gfirst);

      ++plast;
      ++pfirst;
      ++gfirst;
    }
    else if (gfirst==last) //достигнут предел справа, сдвигаем диапозон эквивалентности влево
    { 
      if (--glast!=--pfirst)
        iter_swap (glast,pfirst);
        
      iter_swap (pfirst,--plast);
    }
    else iter_swap (gfirst,--glast), ++gfirst; //обмен элементов нарушающих порядок
  }
}

/*
    Перемещение объектов удовлетворяющих условию в начало диапозона
*/

template <class Iter,class Predicate>
Iter partition (Iter first,Iter last,Predicate pred,forward_iterator_tag)
{
  if (first == last) 
    return first;

  while (pred (*first))
    if (++first == last) 
      return first;

  for (Iter next=first;++next!=last;)
    if (pred (*next)) 
    {
      swap (*first,*next);
      ++first;
    }

  return first;  
}

template <class Iter,class Predicate>
Iter partition (Iter first,Iter last,Predicate pred,bidirectional_iterator_tag)
{
  for (;;++first)
  {
    for (;;)
      if      (first == last) return first;
      else if (pred (*first)) ++first;
      else                    break;
      
    --last;
    
    for (;;)
      if      (first == last) return first;
      else if (!pred (*last)) --last;
      else                    break;
      
    iter_swap (first,last);
  }  
}

template <class Iter,class Predicate>
inline Iter partition (Iter first,Iter last,Predicate pred)
{
  return partition (first,last,pred,iterator_category<Iter> ());
}

template <class Iter,class Predicate>
inline Iter inplace_stable_partition 
 (Iter      first,
  Iter      last,
  Predicate pred,
  typename  iterator_traits<Iter>::difference_type len)
{
  if (len == 1)
    return pred (*first) ? last : first;
    
  Iter middle = first;
  
  advance (middle,len/2);
  
  return rotate (inplace_stable_partition (first,middle,pred,len/2),middle,
                 inplace_stable_partition (middle,last,pred,len-len/2));
}

template <class Iter,class Predicate,class Pointer,class Distance>
Iter stable_partition_adaptive 
 (Iter      first,
  Iter      last,
  Predicate pred,
  Distance  len,  
  Pointer   buffer, 
  Distance  buffer_size)
{
  if (len <= buffer_size)
  {
    Iter    result1 = first;
    Pointer result2 = buffer;
    
    for (;first!=last;++first)
      if (pred (*first)) *result1 = *first, ++result1;
      else               *result2 = *first, ++result2;

    copy (buffer,result2,result1);
    
    return result1;
  }
  else 
  {
    Iter middle = first;
    
    advance (middle,len/2);
    
    return rotate (stable_partition_adaptive (first,middle,pred,len/2,buffer,buffer_size),middle,
                   stable_partition_adaptive (middle,last,pred,len-len/2,buffer,buffer_size));
  }
}

template <class Iter,class Predicate>
inline Iter stable_partition (Iter first,Iter last,Predicate pred)
{
  if (first == last)
    return first;
    
  temporary_buffer<typename iterator_traits<Iter>::value_type> buf (last-first);
  
  return buf.size () ? stable_partition_adaptive (first,last,pred,buf.requested_size (),buf.begin (),buf.size ()) :
                       inplace_stable_partition  (first,last,pred,buf.requested_size ());
}

/*
    Размещение объектов упорядоченно относительно n-th
*/   

template <class Iter,class Compare>
void nth_element (Iter first,Iter nth,Iter last,Compare less)
{
  while (last-first>_ISORT_MAX)
  { 
    pair<Iter,Iter> mid = __unguarded_partition (first,last,less);

    if      (mid.second <= nth) first = mid.second;
    else if (mid.first  >  nth) last  = mid.first;
    else return;
  }

  shell_sort (first,last,less);
}

template <class Iter>
inline void nth_element (Iter first,Iter nth,Iter last)
{
  return nth_element (first,nth,last,iter_less<Iter> ());  
}

/*
    Частичная сортировка с перемещением елементов в диапозон [first,middle) или  [result_first;result_last)
*/

template <class Iter,class Compare>
void partial_sort (Iter first,Iter middle,Iter last,Compare less)
{
  make_heap (first,middle,less);
  
  for (Iter i=middle;i!=last;++i)
    if (less (*i,*first))
    { 
      typename iterator_traits<Iter>::value_type val = *i;
  
      *i = *first;
      __adjust_heap (first,0,middle-first,val,less);
    }
      
  sort_heap (first,middle,less);
}

template <class Iter>
inline void partial_sort (Iter first,Iter middle,Iter last)
{
  return partial_sort (first,middle,last,iter_less<Iter> ());
}

template <class InIter,class OutIter,class Compare>
OutIter partial_sort_copy (InIter first,InIter last,OutIter result_first,OutIter result_last,Compare less)
{
  if (result_first == result_last) 
    return result_last;
    
  OutIter result_real_last = result_first;
  
  while (first != last && result_real_last != result_last) 
  {
    *result_real_last = *first;
    ++result_real_last;
    ++first;
  }
  
  make_heap (result_first,result_real_last,less);
  
  while (first != last) 
  { 
    if (less (*first,*result_first))
      __adjust_heap (result_first,0,result_real_last-result_first,copy (*first),less);
      
    ++first;
  }
  
  sort_heap (result_first,result_real_last,less);
  
  return result_real_last;
}

template <class InIter,class OutIter>
inline OutIter partial_sort_copy (InIter first,InIter last,OutIter result_first,OutIter result_last)
{
  return partial_sort_copy (first,last,result_first,result_last,iter_less<InIter,OutIter> ());
}

/*
    Полная сортировка интервала
*/

template <class Iter,class Compare>
void __sort (Iter first,Iter last,typename iterator_traits<Iter>::difference_type ideal,Compare less)
{
  typename iterator_traits<Iter>::difference_type count;
  
  for (;(count=last-first) > _ISORT_MAX && ideal;)
  { 
    pair<Iter,Iter> mid = __unguarded_partition (first,last,less);
    
    ideal = (ideal + ideal/2)/2; // allow 1.5*log2 (N) divisions

    if (mid.first-first < last-mid.second)  //сортировка большей половины
    {
      __sort (first,mid.first,ideal,less);
      first = mid.second;
    }  
    else
    {
      __sort (mid.second,last,ideal,less);
      last = mid.first;
    }
  }

  if (count > _ISORT_MAX)
  { 
      //сортировка с помощью кучи (если количество разбиений велико)
    make_heap (first,last,less);
    sort_heap (first,last,less);
  }
  else if (count) shell_sort (first,last,less); //сортировка методом Шелла
}

template <class Iter,class Compare>
inline void sort (Iter first,Iter last,Compare less)
{
  __sort (first,last,last-first,less);
}

template <class Iter>
inline void sort (Iter first,Iter last)
{
  __sort (first,last,last-first,iter_less<Iter> ());
}

/*
    Слияние диапозонов с сохранением относительной упорядоченности
*/

template <class Iter1,class Iter2,class OutIter,class Compare>
OutIter merge (Iter1 first1,Iter1 last1,Iter2 first2,Iter2 last2,OutIter result,Compare less)
{
  for (;first1 != last1 && first2 != last2;++result) 
  {
    if (less (*first2,*first1)) *result = *first2, ++first2;
    else                        *result = *first1, ++first1;
  }
  
  return copy (first2,last2,copy (first1,last1,result));
}

template <class Iter1,class Iter2,class OutIter>
inline OutIter merge (Iter1 first1,Iter1 last1,Iter2 first2,Iter2 last2,OutIter result)
{
  return merge (first1,last1,first2,last2,result,iter_less<Iter1,Iter2> ());
}

template <class Iter,class Compare>
void merge_without_buffer
 (Iter     first,
  Iter     middle,
  Iter     last,
  typename iterator_traits<Iter>::difference_type len1,
  typename iterator_traits<Iter>::difference_type len2,
  Compare  less) 
{
  if (!len1 || !len2)
    return;
    
  if (len1+len2 == 2) 
  {
    if (less (*middle,*first))
      iter_swap (first,middle);
      
    return;
  }
  
  typedef typename iterator_traits<Iter>::difference_type Distance;
  
  Iter     first_cut = first, second_cut = middle;
  Distance len11 = 0, len22 = 0;

  if (len1 > len2) 
  {
    len11 = len1 / 2;
    
    advance (first_cut,len11);
    
    second_cut = lower_bound (middle,last,*first_cut,less);
    
    distance (middle,second_cut,len22);
  }
  else 
  {
    len22 = len2 / 2;
    
    advance (second_cut,len22);
    
    first_cut = upper_bound (first,middle,*second_cut,less);
    
    distance (first,first_cut,len11);
  }
  
  Iter new_middle = rotate (first_cut,middle,second_cut);
  
  merge_without_buffer (first,first_cut,new_middle,len11,len22,less);
  merge_without_buffer (new_middle,second_cut,last,len1-len11,len2-len22,less);
}

template <class Iter1,class Iter2,class OutIter,class Compare>
OutIter merge_backward (Iter1 first1,Iter1 last1,Iter2 first2,Iter2 last2,OutIter result,Compare less) 
{
  if (first1 == last1)
    return copy_backward (first2,last2,result);
    
  if (first2 == last2)
    return copy_backward (first1,last1,result);
    
  for (--last1,--last2;;)
  {
    if (less (*last2,*last1))
    {
      *--result = *last1;
      
      if (first1 == last1)
        return copy_backward (first2,++last2,result);
        
      --last1;
    }
    else 
    {
      *--result = *last2;
      
      if (first2 == last2)
        return copy_backward (first1,++last1,result);
        
      --last2;
    }
  }
}

template <class Iter1,class Iter2,class Distance>
Iter1 rotate_adaptive 
 (Iter1      first,
  Iter1      middle,
  Iter1      last,
  Distance   len1,
  Distance   len2,
  Iter2      buffer,
  Distance   buffer_size)
{
  Iter2 buffer_end;
  
  if (len1 > len2 && len2 <= buffer_size) 
  {
    buffer_end = copy (middle,last,buffer);
    copy_backward (first,middle,last);
    
    return copy (buffer,buffer_end,first);
  }
  else if (len1 <= buffer_size) 
  {
    buffer_end = copy (first,middle,buffer);
    
    copy (middle, last, first);
    
    return copy_backward (buffer,buffer_end,last);
  }
  else return rotate (first,middle,last);
}

template <class Iter,class Compare>
void merge_adaptive 
 (Iter     first,
  Iter     middle,
  Iter     last,
  typename iterator_traits<Iter>::difference_type len1,
  typename iterator_traits<Iter>::difference_type len2,
  typename iterator_traits<Iter>::pointer         buffer,
  typename iterator_traits<Iter>::difference_type buffer_size,
  Compare  less)
{  
  typedef typename iterator_traits<Iter>::difference_type Distance;

  if (len1 <= len2 && len1 <= buffer_size)
  {
    merge (buffer,copy (first,middle,buffer),middle,last,first,less);
  }
  else if (len2 <= buffer_size) 
  {
    merge_backward (first,middle,buffer,copy (middle,last,buffer),last,less);
  }
  else 
  {
    Iter     first_cut = first, second_cut = middle;
    Distance len11 = 0, len22 = 0;

    if (len1 > len2) 
    {
      len11 = len1 / 2;
      
      advance (first_cut,len11);      
      second_cut = lower_bound (middle,last,*first_cut,less);
      distance (middle,second_cut,len22);   
    }
    else 
    {
      len22 = len2 / 2;
      
      advance (second_cut,len22);
      first_cut = upper_bound (first,middle,*second_cut,less);
      distance (first,first_cut,len11);
    }
    
    Iter new_middle = rotate_adaptive (first_cut,middle,second_cut,len1-len11,len22,buffer,buffer_size);
    
    merge_adaptive (first,first_cut,new_middle,len11,len22,buffer,buffer_size,less);
    merge_adaptive (new_middle,second_cut,last,len1-len11,len2-len22,buffer,buffer_size,less);
  }
}

template <class Iter,class Compare>
void inplace_merge (Iter first,Iter middle,Iter last,Compare less)
{
  if (first==middle || middle==last)
    return;

  typename iterator_traits<Iter>::difference_type len1 = distance (first,middle),
                                                  len2 = distance (middle,last);

  temporary_buffer<typename iterator_traits<Iter>::value_type> buf (last-first);
  
  if (!buf.begin ())  merge_without_buffer (first,middle,last,len1,len2,less);
  else                merge_adaptive (first,middle,last,len1,len2,buf.begin (),buf.size (),less);
}

template <class Iter>
inline void inplace_merge (Iter first,Iter middle,Iter last)
{
  inplace_merge (first,middle,last,iter_less<Iter> ());
}

/*
    Stable sort
*/

template <class Iter,class Compare>
inline void inplace_stable_sort (Iter first,Iter last,Compare less) 
{
  if (last-first < _ISORT_MAX) 
  {
    shell_sort (first,last,less);
    return;
  }
  
  Iter middle = first+(last-first)/2;
  
  inplace_stable_sort  (first,middle,less);
  inplace_stable_sort  (middle,last,less);
  merge_without_buffer (first,middle,last,middle-first,last-middle,less);
}

template <class InIter,class OutIter,class Distance,class Compare>
void merge_sort_loop (InIter first,InIter last,OutIter result,Distance step_size,Compare less) 
{
  Distance two_step = 2 * step_size;

  while (last - first >= two_step) 
  {
    result  = merge (first,first+step_size,first+step_size,first+two_step,result,less);
    first  += two_step;
  }
  
  step_size = min (Distance (last-first),step_size);

  merge (first,first+step_size,first+step_size,last,result,less);
}

template <class Iter,class Distance,class Compare>
void chunk_insertion_sort (Iter first,Iter last,Distance chunk_size,Compare less)
{
  while (last-first >= chunk_size) 
  {
    shell_sort (first,first+chunk_size,less);
    first += chunk_size;
  }
  
  shell_sort (first,last,less);
}

template <class Iter,class Pointer,class Compare>
void merge_sort_with_buffer (Iter first,Iter last,Pointer buffer,Compare less) 
{
  typedef typename iterator_traits<Iter>::difference_type Distance;

  Distance len         = last-first, step_size = __stl_chunk_size;
  Pointer  buffer_last = buffer+len;

  chunk_insertion_sort (first,last,step_size,less);

  while (step_size < len) 
  {
    merge_sort_loop (first,last,buffer,step_size,less);
    step_size *= 2;
    merge_sort_loop (buffer,buffer_last,first,step_size,less);
    step_size *= 2;
  }
}

template <class Iter,class Pointer,class Distance,class Compare>
inline void stable_sort_adaptive 
 (Iter     first, 
  Iter     last,
  Pointer  buffer,
  Distance buffer_size, 
  Compare  less) 
{
  Distance len = (last-first+1)/2;
  
  Iter middle = first+len;
  
  if (len > buffer_size) 
  {
    stable_sort_adaptive (first,middle,buffer,buffer_size,less);
    stable_sort_adaptive (middle,last,buffer,buffer_size,less);
  }
  else 
  {
    merge_sort_with_buffer (first,middle,buffer,less);
    merge_sort_with_buffer (middle,last,buffer,less);
  }
  
  merge_adaptive (first,middle,last,Distance (middle-first),Distance (last-middle),buffer,buffer_size,less);
}

template <class Iter,class Compare>
inline void stable_sort (Iter first,Iter last,Compare less) 
{
  temporary_buffer<typename iterator_traits<Iter>::value_type> buf (last-first);
     
  if (!buf.begin ()) inplace_stable_sort  (first,last,less);
  else               stable_sort_adaptive (first,last,buf.begin (),buf.size (),less);
}

template <class Iter>
inline void stable_sort (Iter first,Iter last)
{
  return stable_sort (first,last,iter_less<Iter> ());
}

/*
    Предикат определяющий отсортирован ли интервал [first;last)
*/

template <class Iter,class Compare>
bool is_sorted (Iter first,Iter last,Compare less)
{
  if (first == last)
    return true;  
  
  for (Iter next=first;++next!=last;first=next) 
    if (less (*next,*first))
      return false;

  return true;  
}

template <class Iter>
inline bool is_sorted (Iter first,Iter last)
{
  return is_sorted (first,last,iter_less<Iter> ());
}
