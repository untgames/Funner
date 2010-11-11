template <class Pred>
void Condition::Wait (Mutex& lock, Pred pred)
{
  while (!pred ()) Wait (lock);
}

template <class Pred>
void Condition::Wait (Mutex& lock, Pred pred, size_t wait_in_milliseconds)
{
  while (!pred ()) Wait (lock, wait_in_milliseconds);
}

template <class Pred>
bool Condition::TryWait (Mutex& lock, Pred pred, size_t wait_in_milliseconds)
{
  while (!pred ())
    if (!TryWait (lock, wait_in_milliseconds))
      return false;
      
  return true;
}
