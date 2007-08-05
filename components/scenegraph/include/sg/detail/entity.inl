/*
    EntityUpdateLock
*/

inline EntityUpdateLock::EntityUpdateLock (Entity* in_entity)
  : entity (in_entity)
{
  if (entity)
  {
    entity->AddRef ();
    entity->BeginUpdate ();
  }
}

inline EntityUpdateLock::EntityUpdateLock (const EntityUpdateLock& lock)
  : entity (lock.entity)
{
  if (entity)
  {
    entity->AddRef ();
    entity->BeginUpdate ();
  }  
}

inline EntityUpdateLock::~EntityUpdateLock ()
{
  if (!entity)
    return;
    
  entity->EndUpdate ();
  entity->Release ();
}
    
inline EntityUpdateLock& EntityUpdateLock::operator = (const EntityUpdateLock& lock)
{
  if (&lock != this)
    EntityUpdateLock (lock).Swap (*this);
  
  return *this;
}

inline void EntityUpdateLock::Swap (EntityUpdateLock& lock)
{
  Entity* tmp = entity;
  entity      = lock.entity;
  lock.entity = tmp;
}

inline void swap (EntityUpdateLock& a, EntityUpdateLock& b)
{
  a.Swap (b);
}

/*
    Генерация кода посещения родительских классов    
*/

namespace detail
{

template <class T> struct AcceptTracker
{
  void operator () (T& visited, Entity::Visitor& visitor) const { visitor (visited, AcceptTracker<typename T::BaseClass> ()); }
};

template <> struct AcceptTracker<Entity>
{
  template <class T> void operator () (T& visited, Entity::Visitor& visitor) const { visitor (static_cast<Entity&> (visited)); }
};

}

template <class T>
inline void Entity::TrackAccept (T& visited, Visitor& visitor)
{
  detail::AcceptTracker<T> ()(visited, visitor);
}
