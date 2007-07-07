/*
    Помещение элемента last-1 в кучу [first;last-1)
*/

template <class Iter,class Compare>
inline void __push_heap 
 (Iter    first,
  typename iterator_traits<Iter>::difference_type   holeIndex,
  typename iterator_traits<Iter>::difference_type   topIndex,
  const typename iterator_traits<Iter>::value_type& value, //value - не из интервала!
  Compare less)
{
  typename iterator_traits<Iter>::difference_type parent = (holeIndex - 1) / 2;
  
  while (holeIndex > topIndex && less (first [parent],value)) 
  {
    first [holeIndex] = first [parent];
    holeIndex         = parent;
    parent            = (holeIndex - 1) / 2;
  }
  
  first [holeIndex] = value;
}

template <class Iter,class Compare>
inline void push_heap (Iter first,Iter last,Compare less)
{
  typedef typename iterator_traits<Iter>::difference_type Distance;

  __push_heap (first,Distance ((last-first)-1),Distance (0),copy (last [-1]),less);
}

template <class Iter>
inline void push_heap (Iter first,Iter last)
{
  push_heap (first,last,iter_less<Iter> ());
}

/*
    Извлечение наибольшего элемента из кучи [first;last) и помещение в last-1
*/

template <class Iter,class Compare>
inline void __adjust_heap 
 (Iter    first,
  typename iterator_traits<Iter>::difference_type   holeIndex,
  typename iterator_traits<Iter>::difference_type   len,
  const typename iterator_traits<Iter>::value_type& value, //value - не из интервала!
  Compare less)
{
  typename iterator_traits<Iter>::difference_type topIndex = holeIndex, secondChild = 2 * holeIndex + 2;
  
  while (secondChild < len) 
  {
    if (less (first [secondChild],first [secondChild-1]))
      secondChild--;
      
    first [holeIndex] = first [secondChild];    
    holeIndex         = secondChild;
    secondChild       = 2 * (secondChild + 1);
  }
  
  if (secondChild == len)
  {
    first [holeIndex] = first [secondChild-1];
    holeIndex         = secondChild - 1;
  }
  
  __push_heap (first,holeIndex,topIndex,value,less);
}

template <class Iter,class Compare>
inline void pop_heap (Iter first,Iter last,Compare less)
{           
  typename iterator_traits<Iter>::value_type val = last [-1];
  
  last [-1] = *first;    
  
  __adjust_heap (first,0,last-first-1,val,less);
}

template <class Iter>
inline void pop_heap (Iter first,Iter last)
{
  pop_heap (first,last,iter_less<Iter> ());
}

/*
    Создание кучи из диапозона [first;last)
*/

template <class Iter,class Compare>
inline void make_heap (Iter first,Iter last,Compare less)
{
  if (last - first < 2) 
    return;

  typename iterator_traits<Iter>::difference_type len = last-first, parent = (len-2)/2;
    
  for (;;)
  { 
    __adjust_heap (first,parent,len,copy (first [parent]),less);
    
    if (parent == 0) 
      return;
      
    parent--;
  }
}

template <class Iter>
inline void make_heap (Iter first,Iter last)
{
  make_heap (first,last,iter_less<Iter> ()); 
}

/*
    Преобразование кучи [first;last) в отсортированный диапозон
*/

template <class Iter,class Compare>
inline void sort_heap (Iter first,Iter last,Compare less)
{
  while (last-first > 1)
    pop_heap (first,last--,less);
}

template <class Iter>
inline void sort_heap (Iter first,Iter last)
{
  sort_heap (first,last,iter_less<Iter> ());
}

/*
    Предикат определяющий является ли [first;last) кучей
*/    

template <class Iter,class Compare>
inline bool is_heap (Iter first,Iter last,Compare less)
{
  typedef typename iterator_traits<Iter>::difference_type Distance; 

  for (Distance count=last-first,parent=0,child=1;child<count;++child) 
  {
    if (less (first [parent],first [child])) 
      return false;
      
    if (!(child & 1))
      ++parent;
  }
  
  return true;  
}

template <class Iter>
inline bool is_heap (Iter first,Iter last)
{
  return is_heap (first,last,iter_less<Iter> ());
}
