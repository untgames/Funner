/*
    raw_storage_iterator
*/

template <class FwdIter,class T>
inline raw_storage_iterator<FwdIter,T>::raw_storage_iterator (FwdIter _iter)
  : iter (_iter)
  { }
    
template <class FwdIter,class T>
inline raw_storage_iterator<FwdIter,T>& raw_storage_iterator<FwdIter,T>::operator = (const T& obj)
{
  construct (&*iter,obj);
  return *this;
}

template <class FwdIter,class T>
inline raw_storage_iterator<FwdIter,T>& raw_storage_iterator<FwdIter,T>::operator ++ ()
{
  ++iter;
  return *this;
}

template <class FwdIter,class T>
inline raw_storage_iterator<FwdIter,T> raw_storage_iterator<FwdIter,T>::operator ++ (int)
{
  raw_storage_iterator tmp = *this;
  
  ++iter;
  
  return tmp;
}
    
template <class FwdIter,class T>
inline raw_storage_iterator<FwdIter,T>& raw_storage_iterator<FwdIter,T>::operator * ()
{
  return *this;
}
