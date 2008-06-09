/*
    Конструктор
*/

template <class T, class Allocator>
inline basic_string_buffer<T, Allocator>::basic_string_buffer (const allocator_type& allocator)
  : base (allocator)
  {}

/*
    Аллокатор
*/

template <class T, class Allocator>
inline typename basic_string_buffer<T, Allocator>::allocator_type basic_string_buffer<T, Allocator>::get_allocator () const
{
  return base::get_allocator ();
}

/*
    Размер буфера / объём буфера (количество доступной памяти)
*/

template <class T, class Allocator>
inline typename basic_string_buffer<T, Allocator>::size_type basic_string_buffer<T, Allocator>::size () const
{
  size_type s = base::size ();

  return s ? s - 1 : 0;
}

template <class T, class Allocator>
inline typename basic_string_buffer<T, Allocator>::size_type basic_string_buffer<T, Allocator>::capacity () const
{
  size_type s = base::capacity ();

  return s ? s - 1 : 0;
}

/*
    Проверка на пустоту
*/

template <class T, class Allocator>
inline bool basic_string_buffer<T, Allocator>::empty () const
{
  return size () == 0;
}

/*
    Данные
*/

template <class T, class Allocator>
inline typename basic_string_buffer<T, Allocator>::const_pointer basic_string_buffer<T, Allocator>::data () const
{
  static value_type empty_buffer;

  const_pointer p = base::data ();

  return p ? p : &empty_buffer;
}

template <class T, class Allocator>
inline typename basic_string_buffer<T, Allocator>::pointer basic_string_buffer<T, Allocator>::data ()
{
  return const_cast<pointer> (const_cast<const basic_string_buffer&> (*this).data ());
}

/*

*/

template <class T, class Allocator>
void basic_string_buffer<T, Allocator>::terminate_string ()
{
  try
  {
    size_type s = base::size ();

    if (!s)
      return;
      
    data () [s - 1] = T (0);    
  }
  catch (...)
  {
    clear ();
    throw;
  }
}

/*
    Изменение размера буфера / резервирование памяти
*/

template <class T, class Allocator>
inline void basic_string_buffer<T, Allocator>::resize (size_type new_size, bool need_copy)
{
  base::resize (new_size + 1, need_copy);

  terminate_string ();
}

template <class T, class Allocator>
inline void basic_string_buffer<T, Allocator>::reserve (size_type new_size, bool need_copy)
{
  base::reserve (new_size + 1, need_copy);
}

/*
    Очистка буфера
*/

template <class T, class Allocator>
inline void basic_string_buffer<T, Allocator>::clear ()
{
  base::resize (0, false);
}

/*
    Добавление строки
*/

template <class T, class Allocator>
inline typename basic_string_buffer<T, Allocator>::size_type basic_string_buffer<T, Allocator>::append (const_pointer string)
{
  return string ? append (string, xstrlen (string)) : 0;
}

template <class T, class Allocator>
inline typename basic_string_buffer<T, Allocator>::size_type basic_string_buffer<T, Allocator>::append
 (const_pointer string,
  size_type     length)
{
  if (!string || !length)
    return 0;

  size_type current_size = size ();
  
  resize (current_size + length);

  memcpy (data () + current_size, string, length * sizeof (T));

  return length;
}

template <class T, class Allocator>
inline typename basic_string_buffer<T, Allocator>::size_type basic_string_buffer<T, Allocator>::append_format (const_pointer format, ...)
{
  va_list args;
  
  va_start (args, format);
  
  return append_vformat (format, args);
}

template <class T, class Allocator>
typename basic_string_buffer<T, Allocator>::size_type basic_string_buffer<T, Allocator>::append_vformat
 (const_pointer format,
  va_list       args)
{
  if (!format)
    return 0;

  int append_size = xvsnprintf (0, 0, format, args);

  if (append_size <= 0)
    return 0;

  size_type current_size = size ();

  try
  {
    resize (current_size + append_size);

    xvsnprintf (data () + current_size, append_size + 1, format, args);

    return (size_type)append_size;
  }
  catch (...)
  {
    clear ();
    throw;
  }
}

template <class T, class Allocator>
inline basic_string_buffer<T, Allocator>& basic_string_buffer<T, Allocator>::operator += (const_pointer string)
{
  append (string);

  return *this;
}

template <class T, class Allocator>
inline basic_string_buffer<T, Allocator> basic_string_buffer<T, Allocator>::operator + (const_pointer string) const
{
  return basic_string_buffer (*this) += string;
}

/*
    Обмен
*/

template <class T, class Allocator>
inline void basic_string_buffer<T, Allocator>::swap (basic_string_buffer& sb)
{
  base::swap (sb);
}

template <class T, class Allocator>
inline void swap (basic_string_buffer<T, Allocator>& sb1, basic_string_buffer<T, Allocator>& sb2)
{
  sb1.swap (sb2);
}
