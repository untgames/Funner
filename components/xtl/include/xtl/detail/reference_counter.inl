/*
    Конструкторы / деструктор / присваивание
*/

inline reference_counter::reference_counter (size_t start_counter)
  : counter (start_counter)
  {}

inline reference_counter::reference_counter (const reference_counter&)
  : counter (1)
  {}

inline reference_counter& reference_counter::operator = (const reference_counter&)
{
  return *this;
}

/*
    Количество ссылок
*/

inline size_t reference_counter::use_count () const
{
  return counter;
}

/*
    Проверка на пустоту
*/

inline bool reference_counter::empty () const
{
  return counter == 0;
}

/*
    Увеличение / уменьшение количества ссылок
*/

inline reference_counter& reference_counter::operator ++ ()
{
  if (counter)
    counter++;

  return *this;
}

inline reference_counter reference_counter::operator ++ (int)
{
  reference_counter tmp (counter);
  
  ++(*this);
 
  return tmp;
}

inline reference_counter& reference_counter::operator -- ()
{
  if (counter)
    counter--;

  return *this;
}

inline reference_counter reference_counter::operator -- (int)
{
  reference_counter tmp (counter);

  --(*this);
 
  return tmp;
}

/*
    Увеличение / уменьшение количества ссылок с вызовом пользовательского функтора при потере ссылок
*/

inline void addref (reference_counter& rc)
{  
  ++rc;
}

inline void addref (reference_counter* rc)
{
  addref (*rc);
}

template <class Fn>
inline void release (reference_counter& rc, Fn fn)
{
  if (rc && !--rc)
    fn ();
}

template <class Fn>
inline void release (reference_counter* rc, Fn fn)
{
  release (*rc, fn);
}

template <class Ptr>
inline void release (const Ptr& ptr)
{
  reference_counter& rc = *ptr;

  if (rc && !--rc)
    checked_delete (ptr);
}

template <class Ptr, class Deleter>
inline void release (const Ptr& ptr, Deleter deleter)
{
  reference_counter& rc = *ptr;

  if (rc && !--rc)
    deleter (ptr);
}

/*
    Взаимодействие с intrusive_ptr
*/

template <class T>
inline void intrusive_ptr_add_ref (T* ptr)
{
  addref (ptr);
}

template <class T>
inline void intrusive_ptr_release (T* ptr)
{
  release (ptr);
}
