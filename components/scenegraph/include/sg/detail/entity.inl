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
    Попытка посещения объекта
*/

namespace detail
{

struct touch_on_default_action
{
  touch_on_default_action (bool& in_flag) : flag (&in_flag)  {}
  
  template <class T> void operator () (T&, Entity::Visitor&) { *flag = true; }
  
  bool* flag;
};

}

template <class T>
inline bool Entity::TryAccept (T& visited, Visitor& visitor)
{
  bool is_not_processed = false;
  
  visitor (visited, detail::touch_on_default_action (is_not_processed));

  return !is_not_processed;
}
