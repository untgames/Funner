/*
    Блокировка в пределах блока    
*/

inline Lock::Lock (Lockable& in_lockable)
  : lockable (in_lockable)
{
  lockable.Lock ();
}

inline Lock::~Lock ()
{
  lockable.Unlock ();
}

/*
    Функции для взаимодействия с XTL
*/

inline void lock (Lockable& lockable)
{
  lockable.Lock ();
}

inline void unlock (Lockable& lockable)
{
  lockable.Unlock ();
}
