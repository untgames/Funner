#include "shared.h"

using namespace render::manager;

/*
    DynamicPrimitiveList
*/

template <class T>
DynamicPrimitiveList<T>::DynamicPrimitiveList (Impl* in_impl)
  : impl (in_impl)
{
}

template <class T>
DynamicPrimitiveList<T>::DynamicPrimitiveList (const DynamicPrimitiveList& list)
  : impl (list.impl)
{
  addref (impl);
}

template <class T>
DynamicPrimitiveList<T>::~DynamicPrimitiveList ()
{
  release (impl);
}

template <class T>
DynamicPrimitiveList<T>& DynamicPrimitiveList<T>::operator = (const DynamicPrimitiveList& list)
{
  DynamicPrimitiveList (list).Swap (*this);
  return *this;
}

template <class T>
size_t DynamicPrimitiveList<T>::Size () const
{
  return impl->Size ();
}

template <class T>
void DynamicPrimitiveList<T>::SetMaterial (const char* material)
{
  impl->SetMaterial (material);
}

template <class T>
const char* DynamicPrimitiveList<T>::Material () const
{
  return impl->Material ();
}

template <class T>
size_t DynamicPrimitiveList<T>::Add (size_t count, const Item* items)
{
  return impl->Add (count, items);
}

template <class T>
void DynamicPrimitiveList<T>::Update (size_t first, size_t count, const Item* items)
{
  impl->Update (first, count, items);
}

template <class T>
void DynamicPrimitiveList<T>::Remove (size_t first, size_t count)
{
  impl->Remove (first, count);
}

template <class T>
void DynamicPrimitiveList<T>::Clear ()
{
  impl->Clear ();
}

template <class T>
void DynamicPrimitiveList<T>::Reserve (size_t count)
{
  impl->Reserve (count);
}

template <class T>
size_t DynamicPrimitiveList<T>::Capacity () const
{
  return impl->Capacity ();
}

template <class T>
void DynamicPrimitiveList<T>::Swap (DynamicPrimitiveList& list)
{
  stl::swap (impl, list.impl);
}

namespace render
{

namespace manager
{

template <class T>
void swap (DynamicPrimitiveList<T>& list1, DynamicPrimitiveList<T>& list2)
{
  list1.Swap (list2);
}

}

}
