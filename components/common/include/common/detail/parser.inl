/*
    Утилиты
*/

namespace detail
{

template <class Iter, class Fn>
void for_each_child_impl (Iter iter, Fn& fn)
{
  for (; iter; ++iter)
  {
    try
    {
      fn (*iter);
    }
    catch (ParserException&)
    {
      //сообщение уже запротоколировано при генерации исключения
    }
    catch (std::exception& exception)
    {
      try
      {
        iter->Log ().Error (*iter, "%s", exception.what ());
      }
      catch (...)
      {
      }
    }
    catch (...)
    {
      try
      {
        iter->Log ().Error (*iter, "unknown exception");
      }
      catch (...)
      {
      }
    }
  }
}

}

/*
    Полученеи первого потомка
*/

inline ParseNode get_first_child (const ParseNode& node, const char* name)
{
  ParseNode child = node.First (name);

  if (!child)
    raise_parser_exception (node, "No child with name '%s'", name);

  return child;
}

/*
    Получение итератора атрибутов
*/

inline Parser::AttributeIterator make_attribute_iterator (const ParseNode& node)
{
  const char **first = node.Attributes (),
             **last  = first + node.AttributesCount ();

  return Parser::AttributeIterator (first, last);
}

inline Parser::AttributeIterator make_attribute_iterator (const ParseNode& node, const char* name)
{
  return make_attribute_iterator (get_first_child (node, name));
}

/*
    Чтение атрибутов
*/

template <class T>
inline bool try_read (const ParseNode& node, const char* name, T& value)
{
  ParseNode child = node.First (name);

  if (!child)
    return false;

  Parser::AttributeIterator attr_iter = make_attribute_iterator (child);

  return read (attr_iter, value);
}

template <class T>
inline bool try_read (const ParseNode& node, const char* name, T& value, const T& default_value)
{
  if (try_read (node, name, value))
    return true;

  value = default_value;

  return false;
}

template <class T>
inline void read (const ParseNode& node, const char* name, T& value)
{
  ParseNode child = get_first_child (node, name);

  Parser::AttributeIterator attr_iter = make_attribute_iterator (child);

  if (!read (attr_iter, value))
    raise_parser_exception (child, "Bad value");
}

template <class OutIter>
inline void read (const ParseNode& node, const char* name, OutIter first, size_t count)
{
  ParseNode child = get_first_child (node, name);

  Parser::AttributeIterator attr_iter = make_attribute_iterator (child);

  size_t read_count = read_range (attr_iter, first, count);

  if (read_count != count)
    raise_parser_exception (child, "Error at read node item #%u (requested_items_count=%u)", read_count, count);
}

namespace detail
{

//обёртка для хранения возвращаемого значения (для обхода предупреждения g++ 'value may be used uninitialized')
template <class T> struct ResultValueHolder
{
  T value;
  
  ResultValueHolder () : value () {}
};

}

template <class T>
inline T get (const ParseNode& node, const char* name)
{
  detail::ResultValueHolder<T> result;

  read (node, name, result.value);

  return result.value;
}

template <class T>
inline T get (const ParseNode& node, const char* name, const T& default_value)
{
  detail::ResultValueHolder<T> result;

  if (try_read (node, name, result.value))
    return result.value;

//  node.Log ().Warning (node, "Could not read attribute '%s'. Using default value", name);

  return default_value;
}

/*
    Применение функционала ко всем вложенным узлам
*/

template <class Fn>
inline void for_each_child (const ParseNode& node, Fn fn)
{
  detail::for_each_child_impl (Parser::Iterator (node.First ()), fn);
}

template <class Fn>
inline void for_each_child (const ParseNode& node, const char* name, Fn fn)
{
  detail::for_each_child_impl (Parser::NamesakeIterator (node.First (name)), fn);
}
