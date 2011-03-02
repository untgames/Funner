#include "shared.h"

using namespace media::physics;

/*
   Реализация коллекции физической библиотеки
*/

template <class T>
struct PhysicsLibraryCollection<T>::Impl : public xtl::reference_counter
{
  typedef media::ResourceLibrary<T> LibraryHolder;

  LibraryHolder library;
};


/*
   Конструкторы / деструктор / присваивание
*/

template <class T>
PhysicsLibraryCollection<T>::PhysicsLibraryCollection ()
  : impl (new Impl)
{
}

template <class T>
PhysicsLibraryCollection<T>::PhysicsLibraryCollection (const PhysicsLibraryCollection& source)
  : impl (source.impl)
{
  addref (impl);
}

template <class T>
PhysicsLibraryCollection<T>::~PhysicsLibraryCollection ()
{
  release (impl);
}

template <class T>
PhysicsLibraryCollection<T>& PhysicsLibraryCollection<T>::operator = (const PhysicsLibraryCollection<T>& source)
{
  PhysicsLibraryCollection<T> (source).Swap (*this);

  return *this;
}
    
/*
   Количество элементов в библиотеке / проверка на пустоту
*/

template <class T>
size_t PhysicsLibraryCollection<T>::Size () const
{
  return impl->library.Size ();
}

template <class T>
bool PhysicsLibraryCollection<T>::IsEmpty () const
{
  return impl->library.IsEmpty ();
}

/*
   Перебор
*/

template <class T>
typename PhysicsLibraryCollection<T>::Iterator PhysicsLibraryCollection<T>::CreateIterator ()
{
  return impl->library.CreateIterator ();
}

template <class T>
typename PhysicsLibraryCollection<T>::ConstIterator PhysicsLibraryCollection<T>::CreateIterator () const
{
  return impl->library.CreateConstIterator ();
}

/*
   Получение идентификатора элемента библиотеки
*/

template <class T>
const char* PhysicsLibraryCollection<T>::ItemId (const ConstIterator& iter) const
{
  return impl->library.ItemId (iter);
}

/*
   Поиск элемента в библиотеке
*/

template <class T>
typename PhysicsLibraryCollection<T>::Item* PhysicsLibraryCollection<T>::Find (const char* name)
{
  return impl->library.Find (name);
}

template <class T>
const typename PhysicsLibraryCollection<T>::Item* PhysicsLibraryCollection<T>::Find (const char* name) const
{
  return const_cast<PhysicsLibraryCollection&> (*this).Find (name);
}

/*
   Добавление / удаление элементов
*/

template <class T>
void PhysicsLibraryCollection<T>::Attach (const char* id, Item& item)
{
  impl->library.Insert (id, item);
}

template <class T>
void PhysicsLibraryCollection<T>::Detach (const char* id)
{
  impl->library.Remove (id);
}

template <class T>
void PhysicsLibraryCollection<T>::DetachAll ()
{
  impl->library.Clear ();
}

/*
   Обмен
*/

template <class T>
void PhysicsLibraryCollection<T>::Swap (PhysicsLibraryCollection<T>& source)
{
  stl::swap (impl, source.impl);
}

namespace media
{

namespace physics
{

/*
   Обмен
*/

template <class T>
void swap (PhysicsLibraryCollection<T>& collection1, PhysicsLibraryCollection<T>& collection2)
{
  collection1.Swap (collection2);
}

}

}
