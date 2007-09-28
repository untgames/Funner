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

inline reference_counter::operator safe_bool () const
{
  return counter != 0 ? &reference_counter::empty : 0;
}

/*
    Увеличение / уменьшение количества ссылок
*/

inline reference_counter& reference_counter::operator ++ ()
{
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
  if (!--rc)
    fn ();
}

template <class Fn>
inline void release (reference_counter* rc, Fn fn)
{
  release (*rc, fn);
}

template <class T>
inline void release (T* ptr)
{
  if (!--*ptr)
    checked_delete (ptr);
}
