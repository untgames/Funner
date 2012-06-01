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
