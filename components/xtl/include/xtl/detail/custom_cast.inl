namespace detail
{

struct type_converter_item
{
  const std::type_info* source_type;
  const std::type_info* target_type;
  void*                 handler;
  type_converter_item*  next;

  type_converter_item () : source_type (), target_type (), handler (), next () {}
};

struct type_converter_table
{
  enum { table_size = 24593 }; //простое число для размера хэш-таблицы

  type_converter_item* items [table_size];

  type_converter_table () { memset (items, 0, sizeof (items)); }
};

inline size_t get_hash (const std::type_info* t1, const std::type_info* t2)
{
  return size_t (t1) * size_t (t2);
}

template <class T> struct identity_converter_impl: public type_converter_item
{
  identity_converter_impl ()
  {
    source_type = &typeid (T);
    target_type = &typeid (T);
    handler     = &cast;
  }

  static T cast (T value) { throw bad_any_cast (bad_any_cast::bad_cast, typeid (T), typeid (T)); }
};

template <class T> struct type_converter_impl
{
  public:
    type_converter_impl ()
       : items (singleton_default<type_converter_table, false>::instance ().items)
       , count (type_converter_table::table_size)
    {
      add (typeid (T), &identity_converter);
    }

    void add (const std::type_info& type, type_converter_item* new_item)
    {
      size_t bucket_id = get_bucket_id (type);

      type_converter_item* item = items [bucket_id], *prev_item = 0;

      static const std::type_info* source_type = &typeid (T);

      for (; item && !(item->source_type == source_type && item->target_type == &type); prev_item=item, item=item->next);

      if (item)
        return; //double cast handlers

      if (prev_item)
      {
        new_item->next  = prev_item->next;
        prev_item->next = new_item;
      }
      else
      {
        new_item->next    = items [bucket_id];
        items [bucket_id] = new_item;
      }
    }
  
    const type_converter_item* find (const std::type_info& type) const
    {
      size_t bucket_id = get_bucket_id (type);

      const type_converter_item* item = items [bucket_id];

      static const std::type_info* source_type = &typeid (T);

      for (; item && !(item->source_type == source_type && item->target_type == &type); item=item->next);

      return item;
    }

  private:
    size_t get_bucket_id (const std::type_info& type) const
    {
      static const std::type_info* source_type = &typeid (T);

      return get_hash (source_type, &type) % count;
    }

  private:
    type_converter_item**      items;
    const unsigned int         count;
    identity_converter_impl<T> identity_converter;
};

template <class Ret, class Arg>
const type_converter_item* find_converter ()
{
  if (type_traits::is_const<Arg>::value && !type_traits::is_const<Ret>::value)
    return 0;
    
  if (type_traits::is_volatile<Arg>::value && !type_traits::is_volatile<Ret>::value)
    return 0;

  typedef typename type_traits::remove_cv<Ret>::type To;
  typedef typename type_traits::remove_cv<Arg>::type From;

  printf ("check %s -> %s\n", typeid (Arg).name (), typeid (Ret).name ());

  type_converter_impl<From>& impl = singleton_default<type_converter_impl<From>, false>::instance ();
  
  const type_converter_item* item = impl.find (typeid (To));
  
  if (!item)
    return 0;
    
  return item;
}

}

/*
    Преобразователь типов
*/

template <class To, class From>
inline To custom_cast (From value)
{
  const detail::type_converter_item* item = detail::find_converter<To, From> ();

  if (!item)
    throw bad_any_cast (bad_any_cast::bad_cast, typeid (From), typeid (To));

  if (item->source_type == item->target_type)
    return const_cast<typename type_traits::remove_cv<From>::type> (value);

  typedef To (*ConverterFn)(From); //From всегда по ссылке, To - всегда по значению
    
  return reinterpret_cast<ConverterFn> (item->handler)(value);
}

template <class From, class To>
inline bool has_custom_cast ()
{
  return detail::find_converter<To, From> () != 0;
}

/*
    declcast
*/

namespace detail
{

template <class From, class To, template <class, class> class CastTag>
struct declcast_impl: public type_converter_item
{
  declcast_impl ()
  {
    source_type = &typeid (From);
    target_type = &typeid (To);
    handler     = &cast;

    type_converter_impl<From>& converter_impl = singleton_default<type_converter_impl<From>, false>::instance ();

    converter_impl.add (*target_type, this);
  }

  static To cast (From value) { return CastTag<From, To> ()(value); }
};

}

template <class FromT, class ToT, template <class, class> class CastTag>
class declcast
{
  public:
    declcast ()
    {
      typedef typename type_traits::remove_cv<ToT>::type   To;
      typedef typename type_traits::remove_cv<FromT>::type From;

      singleton_default<detail::declcast_impl<From, To, CastTag>, false>::instance ();
    }
};
