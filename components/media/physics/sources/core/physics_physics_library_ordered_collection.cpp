#include "shared.h"

using namespace media::physics;

namespace
{

template <class T> class CollectionBaseClass
{
  public:
    typedef xtl::iterator<T>       Iterator;
    typedef xtl::iterator<const T> ConstIterator;

  protected:
    virtual ~CollectionBaseClass () {}
};

}

/*
   Реализация коллекции физической библиотеки
*/

template <class T>
struct PhysicsLibraryOrderedCollection<T>::Impl : public xtl::reference_counter
{
  typedef media::CollectionImpl<T, CollectionBaseClass<T> > CollectionHolder;

  CollectionHolder collection;
};


/*
   Конструкторы / деструктор / присваивание
*/

template <class T>
PhysicsLibraryOrderedCollection<T>::PhysicsLibraryOrderedCollection ()
  : impl (new Impl)
{
}

template <class T>
PhysicsLibraryOrderedCollection<T>::PhysicsLibraryOrderedCollection (const PhysicsLibraryOrderedCollection& source)
  : impl (source.impl)
{
  addref (impl);
}

template <class T>
PhysicsLibraryOrderedCollection<T>::~PhysicsLibraryOrderedCollection ()
{
  release (impl);
}

template <class T>
PhysicsLibraryOrderedCollection<T>& PhysicsLibraryOrderedCollection<T>::operator = (const PhysicsLibraryOrderedCollection<T>& source)
{
  PhysicsLibraryOrderedCollection<T> (source).Swap (*this);

  return *this;
}
    
/*
   Количество элементов / проверка на пустоту
*/

template <class T>
size_t PhysicsLibraryOrderedCollection<T>::Size () const
{
  return impl->collection.Size ();
}

template <class T>
bool PhysicsLibraryOrderedCollection<T>::IsEmpty () const
{
  return impl->collection.IsEmpty ();
}

/*
   Перебор
*/

template <class T>
typename PhysicsLibraryOrderedCollection<T>::Iterator PhysicsLibraryOrderedCollection<T>::CreateIterator ()
{
  return impl->collection.CreateIterator ();
}

template <class T>
typename PhysicsLibraryOrderedCollection<T>::ConstIterator PhysicsLibraryOrderedCollection<T>::CreateIterator () const
{
  return impl->collection.CreateIterator ();
}

/*
   Доступ к элементу по индексу
*/

template <class T>
const T& PhysicsLibraryOrderedCollection<T>::operator [] (size_t index) const
{
  return const_cast<PhysicsLibraryOrderedCollection&> (*this) [index];
}

template <class T>
T& PhysicsLibraryOrderedCollection<T>::operator [] (size_t index)
{
  return impl->collection [index];
}

/*
   Резервирование элементов
*/

template <class T>
size_t PhysicsLibraryOrderedCollection<T>::Capacity () const
{
  return impl->collection.Capacity ();
}

template <class T>
void PhysicsLibraryOrderedCollection<T>::Reserve  (size_t count)
{
  impl->collection.Reserve (count);
}

/*
   Добавление элементов в коллекцию
*/

template <class T>
size_t PhysicsLibraryOrderedCollection<T>::Add (const Item& item)
{
  return impl->collection.Add (item);
}

/*
   Удаление элементов из коллекции
*/

template <class T>
void PhysicsLibraryOrderedCollection<T>::Remove (size_t index)
{
  impl->collection.Remove (index);
}

/*
   Очистка коллекции
*/

template <class T>
void PhysicsLibraryOrderedCollection<T>::Clear ()
{
  impl->collection.Clear ();
}

/*
   Обмен
*/

template <class T>
void PhysicsLibraryOrderedCollection<T>::Swap (PhysicsLibraryOrderedCollection<T>& source)
{
  stl::swap (impl, source.impl);
}

namespace media
{

namespace physics
{

template class PhysicsLibraryOrderedCollection <CollisionFilter>;

/*
   Обмен
*/

template <class T>
void swap (PhysicsLibraryOrderedCollection<T>& collection1, PhysicsLibraryOrderedCollection<T>& collection2)
{
  collection1.Swap (collection2);
}

}

}
