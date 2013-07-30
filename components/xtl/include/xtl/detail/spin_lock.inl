/*
    Конструктор / деструктор
*/

inline spin_lock::spin_lock ()
  : counter (1)
{
}

inline spin_lock::~spin_lock ()
{
  lock ();
}

/*
    Запирание / отпирание
*/

inline void spin_lock::lock ()
{
  while (!try_lock ());
}

inline bool spin_lock::try_lock ()
{
  return atomic_compare_and_swap (counter, 1, 0) == 1;
}

inline void spin_lock::unlock ()
{
  atomic_compare_and_swap (counter, 0, 1);
}

inline void lock (spin_lock& sl)
{
  sl.lock ();
}

inline void unlock (spin_lock& sl)
{
  sl.unlock ();
}
