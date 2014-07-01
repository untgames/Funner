#include "shared.h"

using namespace render::manager;

/*
    SimplePrimitiveList
*/

template <class T>
SimplePrimitiveList<T>::SimplePrimitiveList (Impl* in_impl)
  : impl (in_impl)
{
}

template <class T>
SimplePrimitiveList<T>::SimplePrimitiveList (const SimplePrimitiveList& list)
  : impl (list.impl)
{
  addref (impl);
}

template <class T>
SimplePrimitiveList<T>::~SimplePrimitiveList ()
{
  release (impl);
}

template <class T>
SimplePrimitiveList<T>& SimplePrimitiveList<T>::operator = (const SimplePrimitiveList& list)
{
  SimplePrimitiveList (list).Swap (*this);
  return *this;
}

template <class T>
size_t SimplePrimitiveList<T>::Size () const
{
  return impl->Size ();
}

template <class T>
void SimplePrimitiveList<T>::SetMaterial (const char* material)
{
  impl->SetMaterial (material);
}

template <class T>
const char* SimplePrimitiveList<T>::Material () const
{
  return impl->Material ();
}

template <class T>
size_t SimplePrimitiveList<T>::Add (size_t count, const Item* items)
{
  return impl->Add (count, items);
}

template <class T>
void SimplePrimitiveList<T>::Update (size_t first, size_t count, const Item* items)
{
  impl->Update (first, count, items);
}

template <class T>
void SimplePrimitiveList<T>::Remove (size_t first, size_t count)
{
  impl->Remove (first, count);
}

template <class T>
void SimplePrimitiveList<T>::Clear ()
{
  impl->Clear ();
}

template <class T>
void SimplePrimitiveList<T>::Reserve (size_t count)
{
  impl->Reserve (count);
}

template <class T>
size_t SimplePrimitiveList<T>::Capacity () const
{
  return impl->Capacity ();
}

template <class T>
void SimplePrimitiveList<T>::RemoveFromPrimitive ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

template <class T>
void SimplePrimitiveList<T>::Swap (SimplePrimitiveList& list)
{
  stl::swap (impl, list.impl);
}

namespace render
{

namespace manager
{

template <class T>
void swap (SimplePrimitiveList<T>& list1, SimplePrimitiveList<T>& list2)
{
  list1.Swap (list2);
}

}

}

/*
    Инстанцирование
*/

template class SimplePrimitiveList<Sprite>;
template class SimplePrimitiveList<Line>;
