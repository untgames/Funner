template <typename T>
SharedQueue<T>::SharedQueue (size_t size)
  : queue_size(size)
{
}

template <typename T>
SharedQueue<T>::~SharedQueue ()
{
  plarium::system::Lock lock (mutex);

  while (!items.empty ())
  {
    T* item = items.front();

    items.pop_front();

    delete item;
  }
}

template <typename T>
bool SharedQueue<T>::Empty () const
{
  plarium::system::Lock lock (const_cast<plarium::system::Mutex&> (mutex));

  return items.empty ();
}

template <typename T>
size_t SharedQueue<T>::Size () const
{
  plarium::system::Lock lock (const_cast<plarium::system::Mutex&> (mutex));

  return items.size();
}

template <typename T>
bool SharedQueue<T>::Enqueue (std::auto_ptr<T>& item)
{
  plarium::system::Lock lock (mutex);

  if (items.size () >= queue_size)
    return false;

  items.push_back (item.release ());

  dequeue_condition.Notify (false);

  return true;
}

template <typename T>
std::auto_ptr<T> SharedQueue<T>::Dequeue (size_t milliseconds_timeout)
{
  plarium::system::Lock lock (mutex);

  if (items.empty ())
    dequeue_condition.Wait (mutex, milliseconds_timeout);

  if (items.empty ())
    return std::auto_ptr<T> (0);

  std::auto_ptr<T> result (items.front ());

  items.pop_front();

  return result;
}

template <typename T>
void SharedQueue<T>::Clear ()
{
  plarium::system::Lock lock (mutex);

  items.clear ();
}

template <typename T>
void SharedQueue<T>::WakeUp ()
{
  dequeue_condition.Notify (false);
}
