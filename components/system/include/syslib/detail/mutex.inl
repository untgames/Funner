/*
    Блокировка в пределах блока    
*/

inline Lock::Lock (Mutex& in_Mutex)
  : mutex (in_Mutex)
{
  mutex.Lock ();
}

inline Lock::~Lock ()
{
  mutex.Unlock ();
}

/*
    Функции для взаимодействия с XTL
*/

inline void lock (Mutex& mutex)
{
  mutex.Lock ();
}

inline void unlock (Mutex& mutex)
{
  mutex.Unlock ();
}
