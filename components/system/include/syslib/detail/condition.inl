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
