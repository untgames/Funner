#ifndef XTL_UNWRAP_HEADER
#define XTL_UNWRAP_HEADER

//forward declaration
template <class T> class reference_wrapper;

/*
    Снятие reference_wrapper
*/

template <class T> inline T& unwrap (T& x)
{
  return x;
}

template <class T> inline T& unwrap (const reference_wrapper<T>& x)
{
  return x.get ();
}

template <class T> inline T& unwrap (reference_wrapper<T>& x)
{
  return x.get ();
}

#endif
