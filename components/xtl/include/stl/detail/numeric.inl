/*
    Статистическая функция (по умолчанию сумма элементов интервала)
*/

template <class InIter,class T,class BinOperation>
inline T accumulate (InIter first,InIter last,T init,BinOperation binary_op)
{
  for (;first!=last;++first) init = binary_op (init,*first);
  return init;
}

template <class InIter,class T>
inline T accumulate (InIter first,InIter last,T init)
{
  return accumulate (first,last,init,plus<T,typename iterator_traits<InIter>::value_type> ());
}

template <class InIter,class T>
T average (InIter first,InIter last,T init)
{
  int n = 0;
  
  for (;first!=last;++first,++n) init = binary_op (init,*first);
  
  return n ? init / n : 0;
}

/*
    Скалярное произведение
*/

template <class InIter1,class InIter2,class T,class BinOperation1,class BinOperation2>
T inner_product (InIter1 first1,InIter1 last1,InIter2 first2,T init,BinOperation1 op1,BinOperation2 op2)
{
  for (;first1!=last1;++first1,++first2) init = op1 (init,op2 (*first1,*first2));
  return init;
}

template <class InIter1,class InIter2,class T>
inline T inner_product (InIter1 first1,InIter1 last1,InIter2 first2,T init)
{
  for (;first1!=last1;++first1,++first2) init = init + *first1 * *first2;
  return init;
}

/*
    Вычисление суммы ряда
*/

template <class InIter,class OutIter,class BinOperation>
OutIter partial_sum (InIter first,InIter last,OutIter result,BinOperation op)
{
  if (first == last) 
    return result;

  for (typename iterator_traits<InIter>::value_type value=*result=*first;++first!=last;)
    *++result = value = op (value,*first);

  return ++result;
}

template <class InIter,class OutIter>
inline OutIter partial_sum (InIter first,InIter last,OutIter result)
{
  return partial_sum (first,last,result,plus<typename iterator_traits<InIter>::value_type> ());
}

/*
    Вычисление разности смежных элементов
*/

template <class InIter,class OutIter,class BinOperation>
OutIter adjacent_difference (InIter first,InIter last,OutIter result,BinOperation op)
{
  if (first == last) 
    return result;

  typedef typename iterator_traits<InIter>::value_type value_type;

  for (value_type value=*result=*first;++first!=last;)
  {
    value_type tmp = *first;

    *++result = op (tmp,value);
    value     = tmp;
  }

  return ++result;
}

template <class InIter,class OutIter>
inline OutIter adjacent_difference (InIter first,InIter last,OutIter result)
{
  return adjacent_difference (first,last,result,minus<typename iterator_traits<InIter>::value_type> ());
}

/*
    Возведение в целую степень (время работы O(logN))
*/

template <class T,class Integer,class MonoidOperation>
inline T power (T x,Integer n,MonoidOperation op)
{
  if (n <= 0)
    return identity_element (op);

  while ((n & 1) == 0)
  {
    n >>= 1;
    x   = op (x, x);
  }

  T result = x;

  n >>= 1;

  while (n != 0)
  {
    x = op (x, x);

    if ((n & 1) != 0)
      result = op (result, x);

    n >>= 1;
  }

  return result;
}

template <class T,class Integer>
inline T power (T x,Integer n)
{
  return power (x,n,multiplies<T> ());
}

/*
    Генерация числовой последовательности
*/

template <class FwdIter,class T>
void iota (FwdIter first,FwdIter last,T value)
{
  while (first!=last) *first++ = value++;
}
