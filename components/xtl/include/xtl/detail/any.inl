/*
===================================================================================================
    Исключения
===================================================================================================    
*/

/*
    Конструкторы
*/

inline bad_any_cast::bad_any_cast (cast_error in_error_id)
  : error_id (in_error_id), source (&typeid (void)), target (&typeid (void))
  {}

inline bad_any_cast::bad_any_cast (cast_error in_error_id, const std::type_info& in_source_type, const std::type_info& in_target_type)
  : error_id (in_error_id), source (&in_source_type), target (&in_target_type)
  {}

inline const char* bad_any_cast::what () const throw ()
{
  switch (error_id)
  {
    case bad_direct_cast:       return "xtl::bad_any_cast; reason: types aren't equal";
    case bad_const_cast:        return "xtl::bad_any_cast; reason: lost cv-qualifiers";
    case bad_dynamic_cast:      return "xtl::bad_any_cast; reason: error at dynamic_cast";
    case bad_to_string_cast:    return "xtl::bad_any_cast; reason: function 'to_string' not found";
    case bad_to_value_cast:     return "xtl::bad_any_cast; reason: function 'to_value' not found";
    case bad_to_reference_cast: return "xtl::bad_any_cast; reason: no way for convert return value to reference after lexical cast";
    default:                    return "xtl::bad_any_cast";
  }
}

/*
    Вид ошибки
*/

inline bad_any_cast::cast_error bad_any_cast::error () const
{
  return error_id;
}

/*
    Исходный и целевой типы
*/

inline const std::type_info& bad_any_cast::source_type () const
{
  return *source;
}

inline const std::type_info& bad_any_cast::target_type () const
{
  return *target;
}

/*
===================================================================================================
    Описание реализации any
===================================================================================================    
*/
 
namespace detail
{

/*
    Внутреннее исключение, возникающее при невозможности приведения
*/

struct bad_any_cast_internal: public std::bad_cast
{
  bad_any_cast_internal (bad_any_cast::cast_error in_error_id) : error_id (in_error_id) {}

  const char* what () const throw () { return "xtl::detail::bad_any_cast_internal"; }

  bad_any_cast::cast_error error_id;    
};

namespace adl_defaults
{

/*
    Свободные функции по умолчанию, для игнорирования отсутствия соответствующих псевдонимов при ADL
*/

using xtl::to_string;
using xtl::to_value;
using xtl::get_root;

//по умолчанию типы не преобразуются к строкам
inline void to_string (stl::string&, default_cast_type)
{
  throw bad_any_cast_internal (bad_any_cast::bad_to_string_cast);
}

//по умолчанию типы не приводимы из строки
inline void to_value (const stl::string& buffer, default_cast_type)
{
  throw bad_any_cast_internal (bad_any_cast::bad_to_value_cast);  
}

//по умолчанию, тип не является потомком dynamic_cast_root
inline dynamic_cast_root* get_root (default_cast_type)
{
  return 0;
}

}

/*
    Определение маски квалификаторов типа
*/

enum {
  any_qualifier_const_bit        = 1,
  any_qualifier_volatile_bit     = 2,
  any_qualifier_ptr_const_bit    = 4,
  any_qualifier_ptr_volatile_bit = 8
};

template <class T> struct any_qualifier_mask                    { enum { value = 0 }; };
template <class T> struct any_qualifier_mask<const T>           { enum { value = any_qualifier_const_bit | any_qualifier_mask<T>::value }; };
template <class T> struct any_qualifier_mask<volatile T>        { enum { value = any_qualifier_volatile_bit | any_qualifier_mask<T>::value }; };
template <class T> struct any_qualifier_mask<const volatile T>  { enum { value = any_qualifier_volatile_bit | any_qualifier_const_bit | any_qualifier_mask<T>::value }; };
template <class T> struct any_qualifier_mask<T&>                { enum { value = any_qualifier_mask<T>::value }; };
template <class T> struct any_qualifier_mask<T*>                { enum { value = any_qualifier_mask<T>::value }; };
template <class T> struct any_qualifier_mask<T* const>          { enum { value = any_qualifier_mask<T>::value | any_qualifier_ptr_const_bit }; };
template <class T> struct any_qualifier_mask<T* volatile>       { enum { value = any_qualifier_mask<T>::value | any_qualifier_ptr_volatile_bit }; };
template <class T> struct any_qualifier_mask<T* const volatile> { enum { value = any_qualifier_mask<T>::value | any_qualifier_ptr_volatile_bit | any_qualifier_ptr_const_bit }; };

template <class T> struct any_qualifier_mask<reference_wrapper<T> >          { enum { value = any_qualifier_mask<T>::value }; };
template <class T> struct any_qualifier_mask<const reference_wrapper<T> >    { enum { value = any_qualifier_mask<T>::value | any_qualifier_const_bit }; };
template <class T> struct any_qualifier_mask<volatile reference_wrapper<T> > { enum { value = any_qualifier_mask<T>::value | any_qualifier_volatile_bit }; };
template <class T> struct any_qualifier_mask<const volatile reference_wrapper<T> > { enum { value = any_qualifier_mask<T>::value | any_qualifier_volatile_bit | any_qualifier_const_bit }; };

/*
    Определение хранимого типа
*/

template <class T> struct any_stored_type                    { typedef T type; };
template <class T> struct any_stored_type<const T>           { typedef T type; };
template <class T> struct any_stored_type<volatile T>        { typedef T type; };
template <class T> struct any_stored_type<const volatile T>  { typedef T type; };
template <class T> struct any_stored_type<T&>                { typedef typename any_stored_type<T>::type& type; };
template <class T> struct any_stored_type<T*>                { typedef typename any_stored_type<T>::type* type; };
template <class T> struct any_stored_type<T* const>          { typedef typename any_stored_type<T>::type* type; };
template <class T> struct any_stored_type<T* volatile>       { typedef typename any_stored_type<T>::type* type; };
template <class T> struct any_stored_type<T* const volatile> { typedef typename any_stored_type<T>::type* type; };

template <class T> struct any_stored_type<reference_wrapper<T> >
{
  typedef reference_wrapper<typename any_stored_type<T>::type> type;
};

template <class T> struct any_stored_type<const reference_wrapper<T> >:          public any_stored_type<reference_wrapper<T> > {};
template <class T> struct any_stored_type<volatile reference_wrapper<T> >:       public any_stored_type<reference_wrapper<T> > {};
template <class T> struct any_stored_type<const volatile reference_wrapper<T> >: public any_stored_type<reference_wrapper<T> > {};

/*
    Снятие константности
*/

template <class T>
inline typename any_stored_type<T>::type& get_unqualified_value (T& value)
{
  return const_cast<typename any_stored_type<T>::type&> (value);
}

template <class T>
inline typename any_stored_type<T>::type& get_unqualified_value (const T& value)
{
  return const_cast<typename any_stored_type<T>::type&> (value);  
}

template <class T>
inline typename any_stored_type<T>::type& get_unqualified_value (volatile T& value)
{
  return const_cast<typename any_stored_type<T>::type&> (value);  
}

template <class T>
inline typename any_stored_type<T>::type& get_unqualified_value (const volatile T& value)
{
  return const_cast<typename any_stored_type<T>::type&> (value);    
}

template <class T>
inline typename any_stored_type<T>::type& get_unqualified_value (reference_wrapper<T>& value)
{
  return get_unqualified_value (value.get ());
}

template <class T>
inline typename any_stored_type<T>::type& get_unqualified_value (const reference_wrapper<T>& value)
{
  return get_unqualified_value (value.get ());
}

template <class T>
inline typename any_stored_type<T>::type& get_unqualified_value (volatile reference_wrapper<T>& value)
{
  return get_unqualified_value (value.get ());
}

template <class T>
inline typename any_stored_type<T>::type& get_unqualified_value (const volatile reference_wrapper<T>& value)
{
  return get_unqualified_value (value.get ());
}

/*
    Определение RTTI-типа переменной
*/

template <class T>
inline const std::type_info& get_typeid (T& value)
{
  return &value ? typeid (value) : typeid (T);
}

template <class T>
inline const std::type_info& get_typeid (T* value)
{
  return &value ? typeid (value) : typeid (T*);
}

/*
    Проверка на 0
*/

template <class T>
inline bool is_null (const T& value)
{
  return &value == 0;
}

template <class T>
inline bool is_null (const T* value)
{
  return value == 0;
}

/*
    Интерфейс хранилища вариантных данных 
*/

struct any_holder
{
  virtual ~any_holder () {}

  virtual size_t                qualifier_mask        () = 0;
  virtual const std::type_info& type                  () = 0;
  virtual const std::type_info& stored_type           () = 0;
  virtual const std::type_info& castable_type         () = 0;
  virtual any_holder*           clone                 () = 0;
  virtual void                  release               () = 0;
  virtual void                  dump                  (stl::string&) = 0;
  virtual void                  set_content           (const stl::string&) = 0;
  virtual dynamic_cast_root*    get_dynamic_cast_root () = 0;
  virtual bool                  null                  () = 0;
};

/*
    Содержимое вариативной переменной
*/

#ifdef _MSC_VER
  #pragma warning (push)
  #pragma warning (disable:4624) //destructor could not be generated because a base class destructor is inaccessible
#endif

template <class T> struct any_content: public any_holder
{
  typedef typename any_stored_type<T>::type base_type;

  any_content (const T& in_value) : value (get_unqualified_value (in_value)) {}

  const std::type_info& type           () { return typeid (T); }
  const std::type_info& stored_type    () { return typeid (base_type); }
  const std::type_info& castable_type  () { return get_typeid (get_castable_value (value)); }
  size_t                qualifier_mask () { return any_qualifier_mask<T>::value; }
  bool                  null           () { return is_null (get_castable_value (value)); }

  dynamic_cast_root* get_dynamic_cast_root ()
  {
    using adl_defaults::get_root;
  
    return get_unqualified_value (get_root (get_castable_value (value)));
  }
  
  void dump (stl::string& buffer)
  {
    using adl_defaults::to_string;

    to_string (buffer, get_castable_value (value));
  }
  
  void set_content (const stl::string& buffer)
  {
    using adl_defaults::to_value;
    
    to_value (buffer, get_castable_value (value));
  }

  base_type value;
};

#ifdef _MSC_VER
  #pragma warning (pop)
#endif

/*
    Реализации вариативной переменной
*/

template <class T> struct copyable_any_impl: public any_content<T>
{
  copyable_any_impl (const T& in_value) : any_content<T> (in_value) {}

  any_holder* clone   () { return new copyable_any_impl (*this); }
  void        release () { delete this; }
};

template <class T> class referenced_any_impl: public any_content<T>
{
  public:
    referenced_any_impl (const T& in_value) : any_content<T> (in_value), ref_count (1) {}

    any_holder* clone () { return ref_count++, this; }
  
    void release ()
    {
      if (!--ref_count)
        delete this;
    }

  private:
    size_t ref_count;
};

}

/*
===================================================================================================
    any
===================================================================================================    
*/

/*
    Конструкторы / деструктор
*/

inline any::any ()
  : content_ptr (0)
  {}

template <class T>
inline any::any (const T& value, bool ref_counted)
  : content_ptr (create_holder (value, ref_counted))
  {}

inline any::any (const any& in_any)
  : content_ptr (in_any.content_ptr ? in_any.content_ptr->clone () : 0)
  {}

inline any::~any ()
{
  if (content_ptr)
    content_ptr->release ();
}

/*
    Создание хранилища
*/

template <class T>
inline detail::any_holder* any::create_holder (const T& value, bool ref_counted)
{
  return ref_counted ? static_cast<detail::any_holder*> (new detail::referenced_any_impl<T> (value)) :
                       static_cast<detail::any_holder*> (new detail::copyable_any_impl<T> (value));
}

/*
    Присваивание
*/

inline any& any::operator = (const any& in_any)
{
  any (in_any).swap (*this);
  return *this;
}

template <class T>
inline any& any::operator = (const T& value)
{
  any (value).swap (*this);
  return *this;
}

/*
    Проверка на пустоту / проверка на 0
*/

inline bool any::empty () const
{
  return content_ptr == 0;
}

inline bool any::null () const
{
  return !content_ptr || content_ptr->null ();
}

/*
    Тип и данные
*/

inline const std::type_info& any::type () const
{
  return content_ptr ? content_ptr->type () : typeid (void);
}

inline const std::type_info& any::castable_type () const
{
  return content_ptr ? content_ptr->castable_type () : typeid (void);
}

template <class T>
inline T* any::content ()
{
  return const_cast<T*> (const_cast<const any&> (*this).content<T> ());
}

template <class T>
inline const T* any::content () const
{
  if (!content_ptr)
    return 0;
     
  if (&typeid (T) != &content_ptr->type ())
    return 0; //преобразование невозможно, из-за неэквивалентности базовых типов
    
  return &static_cast<const detail::any_content<T>*> (content_ptr)->value;
}

/*
    Приведение
*/

namespace detail
{

//обёртки для приведения типов через dynamic_cast
template <class DstT, bool has_vtbl=type_traits::is_polymorphic<DstT>::value>
struct dynamic_caster_helper
{
  static DstT* cast (dynamic_cast_root* source)
  {
    DstT* result = dynamic_cast<DstT*> (source);
    
    if (!result)
      throw bad_any_cast_internal (bad_any_cast::bad_dynamic_cast);
      
    return result;
  }
};

template <class DstT> struct dynamic_caster_helper<DstT, false>
{
  static DstT* cast (dynamic_cast_root*) { throw bad_any_cast_internal (bad_any_cast::bad_dynamic_cast); }
};

template <class DstT> struct dynamic_caster
{
  static DstT& cast (dynamic_cast_root* source) { return *dynamic_caster_helper<DstT>::cast (source); }
};

template <class DstT> struct dynamic_caster<DstT*>
{
  static DstT* cast (dynamic_cast_root* source) { return dynamic_caster_helper<DstT>::cast (source); }
};

//попытка приведения строки в значение
template <class T>
inline T try_lexical_cast (const stl::string& buffer, type<T>)
{
  using adl_defaults::to_value;

  T value;

  to_value (buffer, value);

  return value;
}

template <class T>
inline T& try_lexical_cast (const stl::string&, type<T&>)
{
    //невозможно лексикографически приводить к ссылочным типам данных
  throw bad_any_cast_internal (bad_any_cast::bad_to_reference_cast);
}

}

template <class T>
inline const T any::cast () const
{
  if (!content_ptr)
    throw bad_any_cast (bad_any_cast::bad_direct_cast, type (), typeid (T));
    
  try
  {    
    typedef typename type_traits::remove_reference<T>::type nonref;    
    typedef typename detail::any_stored_type<nonref>::type  stored_type;  
    
      //проверка возможности const_cast приведения
    
    static const size_t target_qualifier_mask = detail::any_qualifier_mask<nonref>::value;  
    
    bool is_lost_qualifiers = (content_ptr->qualifier_mask () & ~target_qualifier_mask) != 0;

       //попытка прямого преобразования

    const std::type_info* content_type = &content_ptr->stored_type ();
      
    if (&typeid (stored_type) == content_type)
    {
      if (is_lost_qualifiers)
      {
          //преобразование невозможно, из-за понижения уровня cv-квалификаторов

        throw bad_any_cast (bad_any_cast::bad_const_cast, type (), typeid (T));      
      }
    
      return static_cast<detail::any_content<stored_type>*> (content_ptr)->value;
    }
      
      //попытка приведения из reference_wrapper
      
    if (&typeid (reference_wrapper<stored_type>) == content_type)
    {
      if (is_lost_qualifiers)
      {
          //преобразование невозможно, из-за понижения уровня cv-квалификаторов

        throw bad_any_cast (bad_any_cast::bad_const_cast, type (), typeid (T));      
      }
    
      return static_cast<detail::any_content<reference_wrapper<stored_type> >*> (content_ptr)->value.get ();
    }
      
      //попытка приведения через dynamic_cast_root    
      
    if (dynamic_cast_root* dc_root = content_ptr->get_dynamic_cast_root ())
    {
      if (is_lost_qualifiers)
      {
          //преобразование невозможно, из-за понижения уровня cv-квалификаторов

        throw bad_any_cast (bad_any_cast::bad_const_cast, type (), typeid (T));      
      }
    
      return detail::dynamic_caster<nonref>::cast (dc_root);      
    }

      //попытка лексикографического приведения

    stl::string buffer;

    content_ptr->dump (buffer);
    
    return detail::try_lexical_cast (buffer, xtl::type<typename detail::any_stored_type<T>::type> ());
  }
  catch (detail::bad_any_cast_internal& exception)
  {
    throw bad_any_cast (exception.error_id, type (), typeid (T));
  }
}

/*
    Лексикографическое приведение
*/

inline void any::to_string (stl::string& buffer) const
{
  if (!content_ptr)
  {
    buffer.clear ();
    return;
  }

  try
  {
    content_ptr->dump (buffer);
  }
  catch (detail::bad_any_cast_internal& exception)
  {
    throw bad_any_cast (exception.error_id, type (), typeid (stl::string));
  }
}

inline void any::set_content (const stl::string& buffer)
{
  if (!content_ptr)
    return;

    //проверка возможности const_cast приведения

  if (content_ptr->qualifier_mask () & (detail::any_qualifier_const_bit | detail::any_qualifier_ptr_const_bit))
  {
      //преобразование невозможно, из-за понижения уровня cv-квалификаторов

    throw bad_any_cast (bad_any_cast::bad_const_cast, typeid (const stl::string), type ());
  }
    
  try
  {
    content_ptr->set_content (buffer);
  }
  catch (detail::bad_any_cast_internal& exception)
  {
    throw bad_any_cast (exception.error_id, typeid (const stl::string), type ());
  }
}

inline void to_string (stl::string& buffer, const volatile any& value)
{
  const_cast<const any&> (value).to_string (buffer);
}

inline void to_value  (const stl::string& buffer, volatile any& value)
{
  const_cast<any&> (value).set_content (buffer);
}

/*
    Обмен
*/

inline any& any::swap (any& in_any)
{
  detail::any_holder* tmp = content_ptr;
  content_ptr             = in_any.content_ptr;
  in_any.content_ptr      = tmp;
  
  return *this;
}

inline void swap (any& a1, any& a2)
{
  a1.swap (a2);
}

/*
===================================================================================================
    Утилиты
===================================================================================================    
*/

/*
    Создание any с подсчётом ссылок
*/

template <class T>
inline any make_ref_any (T& value)
{
  return any (value, true);
}

/*
    Приведение типов
*/

template <class T>
inline T* any_cast (any* a)
{
  return a ? a->content<T> () : 0;
}

template <class T>
inline const T* any_cast (const any* a)
{
  return a ? a->content<const T> () : 0;
}

template <class T>
inline T any_cast (any& a)
{
  typedef typename type_traits::remove_reference<T>::type nonref;

  nonref* result = any_cast<nonref> (&a);

  if (!result)
    throw bad_any_cast (bad_any_cast::bad_direct_cast, a.type (), typeid (T));

  return *result;
}

template <class T>
inline const T any_cast (const any& a)
{
  typedef typename type_traits::remove_reference<T>::type nonref;

  const nonref* result = any_cast<nonref> (&a);

  if (!result)
    throw bad_any_cast (bad_any_cast::bad_direct_cast, a.type (), typeid (T));

  return *result;
}

/*
    Многоуровневое приведение типов
*/

template <class T>
inline const T any_multicast (const any& a)
{
  return a.cast<T> ();
}

/*
    Получение приводимого значения. Используется как базовое при работе any_multicast
*/

template <class T>
inline T& get_castable_value (T& value)
{
  return value;
}

template <class T>
inline T& get_castable_value (T* ptr)
{
  return *ptr;
}

inline char* get_castable_value (char* ptr)
{
  return ptr;
}

inline wchar_t* get_castable_value (wchar_t* ptr)
{
  return ptr;
}

template <class T>
inline T& get_castable_value (stl::auto_ptr<T>& ptr)
{
  return *ptr;
}

template <class T>
inline T& get_castable_value (shared_ptr<T>& ptr)
{
  return *ptr;
}

template <class T, template <class > class Strategy>
inline T& get_castable_value (intrusive_ptr<T, Strategy>& ptr)
{
  return *ptr;
}

template <class T>
inline T& get_castable_value (com_ptr<T>& ptr)
{
  return *ptr;
}

template <class T>
inline T& get_castable_value (reference_wrapper<T>& ref)
{
  return ref.get ();
}
