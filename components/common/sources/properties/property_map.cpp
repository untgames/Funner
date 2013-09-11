#include "shared.h"

using namespace common;

namespace
{

/*
    Константы
*/

const size_t DEFAULT_RESERVED_BUFFER_SIZE = 64;   //резервируемый объём буфера свойств
const char*  DEFAULT_VALUES_SEPARATOR     =  " "; //разделитель значений

/*
    Отображения типов и идентификаторов типов
*/

template <class T>
struct PropertyTypeSelector {};

template <> struct PropertyTypeSelector<const char*> { enum { value = PropertyType_String  }; };
template <> struct PropertyTypeSelector<stl::string> { enum { value = PropertyType_String  }; };
template <> struct PropertyTypeSelector<int>         { enum { value = PropertyType_Integer }; };
template <> struct PropertyTypeSelector<float>       { enum { value = PropertyType_Float   }; };
template <> struct PropertyTypeSelector<math::vec4f> { enum { value = PropertyType_Vector  }; };
template <> struct PropertyTypeSelector<math::mat4f> { enum { value = PropertyType_Matrix  }; };

template <PropertyType type> struct PropertyTypeMap {};

template <> struct PropertyTypeMap<PropertyType_Integer>
{
  typedef int type;
  
  static int cast (int x)   { return x; }
  static int cast (float x) { return static_cast<int> (x); }
};

template <> struct PropertyTypeMap<PropertyType_Float>
{
  typedef float type;
  
  static float cast (float x) { return x; }
  static float cast (int x)   { return static_cast<float> (x); }  
};

template <> struct PropertyTypeMap<PropertyType_Vector>
{
  typedef math::vec4f type;
  
  static math::vec4f        cast (float x)              { return x; }
  static math::vec4f        cast (int x)                { return static_cast<float> (x); }
  static const math::vec4f& cast (const math::vec4f& x) { return x; }
};

template <> struct PropertyTypeMap<PropertyType_Matrix>
{
  typedef math::mat4f type;
  
  static math::mat4f        cast (float x)              { return x; }
  static math::mat4f        cast (int x)                { return static_cast<float> (x); }
  static const math::mat4f& cast (const math::mat4f& x) { return x; }
};

}

/*
    Описание реализации карты свойств
*/

typedef xtl::uninitialized_storage<char>           Buffer;
typedef xtl::signal<void (PropertyMapEvent event)> Signal;

struct PropertyMap::Impl: public xtl::reference_counter, public xtl::trackable
{
  PropertyLayout layout;                          //расположение свойств
  Buffer         buffer;                          //буфер с данными карты свойств
  StringArray    strings;                         //массив строк карты свойств
  size_t         hash;                            //хэш карты
  bool           need_update;                     //необходимо обновление вспомогательных данных
  Signal         signals [PropertyMapEvent_Num];  //сигналы карты свойств  

///Конструктор по умолчанию
  Impl ()
    : hash (0)
    , need_update (true)
  {
    layout.Capture ();
    
    buffer.reserve (DEFAULT_RESERVED_BUFFER_SIZE);
    
    Update ();
  }
  
///Конструктор для совместно используемого расположения свойств
  Impl (const PropertyLayout& in_layout)
    : layout (in_layout)
    , buffer (layout.BufferSize ())
    , hash (0)
    , need_update (true)
  {
    layout.Capture ();
    
    memset (buffer.data (), 0, buffer.size ());
    
    const PropertyDesc* desc = layout.Properties ();
    
    for (size_t i=0, count=layout.Size (); i<count; i++, desc++)
    {
      if (desc->type == PropertyType_String)
        *reinterpret_cast<unsigned int*> (buffer.data () + desc->offset) = strings.Add ("");
    }
    
    Update ();
  }
  
///Конструктор копирования
  Impl (const Impl& impl)
    : layout (impl.layout)
    , buffer (impl.buffer)
    , strings (impl.strings.Clone ())
    , hash (0)
    , need_update (true)
  {
    Update ();
  }
  
///Оповещение об изменении данных
  void UpdateNotify ()
  {
    need_update = true;      
    
    if (!signals [PropertyMapEvent_OnUpdate].empty ())
    {
      try
      {      
        signals [PropertyMapEvent_OnUpdate] (PropertyMapEvent_OnUpdate);
      }
      catch (...)
      {
        //подавление всех исключений
      }
    }    
  }

///Обновление данных
  void Update ()
  {
    hash = crc32 (buffer.data (), buffer.size (), layout.Hash ());

    if (strings.BufferSize ())
      hash = crc32 (strings.Buffer (), strings.BufferSize (), hash);

    need_update = false;
  }
  
///Захват объекта расположения свойств
  void CaptureLayout ()
  {
    if (layout.UseCount () == 1)
      return;

    layout = layout.Clone ();
    
    layout.Capture ();    
  }
  
///Вставка нового свойства
  size_t InsertProperty (const char* name, common::PropertyType type, size_t elements_count)
  {
    try
    {
      if (type == PropertyType_String && elements_count > 1)
        throw xtl::format_operation_exception ("", "Can't insert string arrays");
        
      if (!elements_count)
        throw xtl::make_null_argument_exception ("", "elements_count");
      
      CaptureLayout ();
      
      need_update = true;
      
      size_t index           = layout.AddProperty (name, type, elements_count),
             old_buffer_size = buffer.size ();

      try
      {
        buffer.resize (layout.BufferSize ());
        
        if (type == PropertyType_String)
        {
          unsigned int& string_index = *reinterpret_cast<unsigned int*> (buffer.data () + old_buffer_size);
          
          string_index = strings.Add ("");
        }
        else
        {
          memset (buffer.data () + old_buffer_size, 0, buffer.size () - old_buffer_size);
        }
        
        return index;
      }
      catch (...)
      {
        buffer.resize (old_buffer_size);
        layout.RemoveProperty (index);
        throw;
      }
    }
    catch (xtl::exception& e)
    {
      e.touch ("common::PropertyMap::Impl::InsertProperty");
      throw;
    }
  }
  
///Получение либо создание свойства и возвращение его индекса
  size_t GetIndexOrCreate (const char* name, common::PropertyType type, size_t elements_count, bool& new_property_inserted)
  {
    static const char* METHOD_NAME = "common::PropertyMap::Impl::GetIndexOrCreate";

    new_property_inserted = false;

    if (!name)
      throw xtl::make_null_argument_exception (METHOD_NAME, "name");

      //поиск уже созданного свойства

    int index = layout.IndexOf (name);

    if (index != -1)
      return static_cast<size_t> (index);

      //создание нового свойства

    index = static_cast<int> (InsertProperty (name, type, elements_count));

    new_property_inserted = true;

    return static_cast<size_t> (index);
  }
  
///Получение индекса свойства
  size_t GetIndex (const char* name)
  {
    static const char* METHOD_NAME = "common::PropertyMap::Impl::GetIndex";

    if (!name)
      throw xtl::make_null_argument_exception (METHOD_NAME, "name");

    int index = layout.IndexOf (name);

    if (index != -1)
      return static_cast<size_t> (index);

    throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Property has not registered");
  }
  
///Установка свойства по имени
  template <class T>
  void SetProperty (const char* method_name, const char* name, size_t elements_count, const T* values)
  {
    bool new_property_inserted = false;    
    
    size_t property_index = 0;
    
    try
    {
      property_index = GetIndexOrCreate (name, static_cast<common::PropertyType> (PropertyTypeSelector<T>::value), elements_count, new_property_inserted);

      SetProperty (property_index, elements_count, values);
    }
    catch (xtl::exception& e)
    {
      e.touch (method_name);

      if (new_property_inserted)
        RemoveProperty (property_index);

      throw;
    }
    catch (...)
    {
      if (new_property_inserted)
        RemoveProperty (property_index);

      throw;
    }
  }
  
///Установка свойства по индексу
  template <class T>
  void SetProperty (size_t property_index, size_t elements_count, const T* values)
  {
      //проверка корректности аргументов

    if (property_index >= layout.Size ())
      throw xtl::make_range_exception ("", "property_index", property_index, layout.Size ());

    if (!values)
      throw xtl::make_null_argument_exception ("", "values");      
      
    if (!elements_count)
      throw xtl::make_null_argument_exception ("", "elements_count");

    const PropertyDesc& desc = layout.Properties ()[property_index];    
    
    switch (desc.type)
    {
      case PropertyType_String:
      case PropertyType_Integer:
      case PropertyType_Float:
      case PropertyType_Vector:      
      case PropertyType_Matrix:
        break;
      default:
        throw xtl::format_operation_exception ("", "Internal error: wrong property '%s' type %d", desc.name, desc.type);
    }
    
    static const common::PropertyType source_type = static_cast<common::PropertyType> (PropertyTypeSelector<T>::value);
    
    need_update = true;
    
    ReadValueToPropertyDispatch (elements_count, values, desc, PropertyTypeMap<source_type> ());
    
    UpdateNotify ();    
  }
  
  template <class T>
  void SetProperty (const char* method_name, size_t property_index, size_t elements_count, const T* values)
  {
    try
    {
      SetProperty (property_index, elements_count, values);
    }
    catch (xtl::exception& e)
    {
      e.touch (method_name);
      throw;
    }    
  }
  
///Копирование данных с приведением типа
  template <class T>
  void ReadScalarValueToProperty (size_t elements_count, const T* values, const PropertyDesc& desc)
  {
    switch (desc.type)
    {
      case PropertyType_String:
        ReadValueToProperty (elements_count, values, desc, PropertyTypeMap<PropertyType_String> ());
        return;
      case PropertyType_Integer:
        ReadValueToProperty (elements_count, values, desc, PropertyTypeMap<PropertyType_Integer> ());
        return;
      case PropertyType_Float:
        ReadValueToProperty (elements_count, values, desc, PropertyTypeMap<PropertyType_Float> ());
        return;
      case PropertyType_Vector:
        if (elements_count == desc.elements_count * 4)
        {
          ReadValueToProperty (elements_count, values, desc, PropertyTypeMap<PropertyType_Float> ());
        }
        else
        {
          ReadValueToProperty (elements_count, values, desc, PropertyTypeMap<PropertyType_Vector> ());
        }
        return;
      case PropertyType_Matrix:
        if (elements_count == desc.elements_count * 16)
        {
          ReadValueToProperty (desc.elements_count * 16, values, desc, PropertyTypeMap<PropertyType_Float> ());
        }
        else
        {
          ReadValueToProperty (elements_count, values, desc, PropertyTypeMap<PropertyType_Matrix> ());
        }
        return;
      default:
        break;
    }
    
    throw xtl::format_operation_exception ("", "Could not convert property '%s' from %s[%u] to %s[%u]", desc.name,
      get_name (static_cast<common::PropertyType> (PropertyTypeSelector<T>::value)), elements_count, get_name (desc.type), desc.elements_count);
  }

  template <class T>
  void ReadValueToPropertyDispatch (size_t elements_count, const T* values, const PropertyDesc& desc, PropertyTypeMap<PropertyType_String>)
  {
    if (elements_count > 1)
      throw xtl::format_operation_exception ("", "Internal error: attempt to set %u strings", elements_count);
    
    ReadScalarValueToProperty (elements_count, values, desc);
  }
  
  template <class T>
  void ReadValueToPropertyDispatch (size_t elements_count, const T* values, const PropertyDesc& desc, PropertyTypeMap<PropertyType_Integer>)
  {
    ReadScalarValueToProperty (elements_count, values, desc);
  }
  
  template <class T>
  void ReadValueToPropertyDispatch (size_t elements_count, const T* values, const PropertyDesc& desc, PropertyTypeMap<PropertyType_Float>)
  {
    ReadScalarValueToProperty (elements_count, values, desc);
  }
  
  template <class T>
  void ReadValueToPropertyDispatch (size_t elements_count, const T* values, const PropertyDesc& desc, PropertyTypeMap<PropertyType_Vector>)
  {
    switch (desc.type)
    {
      case PropertyType_String:
        ReadValueToProperty (elements_count, values, desc, PropertyTypeMap<PropertyType_String> ());            
        return;
      case PropertyType_Integer:
        if (elements_count * 4 == desc.elements_count)
        {
          ReadValueToProperty (elements_count * 4, &values [0][0], desc, PropertyTypeMap<PropertyType_Integer> ());
          return;
        }
        else break;        
      case PropertyType_Float:
        if (elements_count * 4 == desc.elements_count)
        {
          ReadValueToProperty (elements_count * 4, &values [0][0], desc, PropertyTypeMap<PropertyType_Float> ());
          return;
        }
        else break;
      case PropertyType_Vector:
        ReadValueToProperty (elements_count, values, desc, PropertyTypeMap<PropertyType_Vector> ());
        return;
      case PropertyType_Matrix:
        if (elements_count == desc.elements_count * 4)
        {
          ReadValueToProperty (elements_count / 4, reinterpret_cast<const math::mat4f*> (values), desc, PropertyTypeMap<PropertyType_Matrix> ());
          return;
        }
        else break;
      
        return;
      default:
        break;
    }

    throw xtl::format_operation_exception ("", "Could not convert property '%s' from %s[%u] to %s[%u]", desc.name,
      get_name (PropertyType_Vector), elements_count, get_name (desc.type), desc.elements_count);
  }
  
  template <class T>
  void ReadValueToPropertyDispatch (size_t elements_count, const T* values, const PropertyDesc& desc, PropertyTypeMap<PropertyType_Matrix>)
  {
    switch (desc.type)
    {
      case PropertyType_String:
        ReadValueToProperty (elements_count, values, desc, PropertyTypeMap<PropertyType_String> ());            
        return;
      case PropertyType_Integer:
        if (elements_count * 16 == desc.elements_count)
        {
          ReadValueToProperty (elements_count * 16, &values [0][0][0], desc, PropertyTypeMap<PropertyType_Integer> ());
          return;
        }
        else break;
      case PropertyType_Float:
        if (elements_count * 16 == desc.elements_count)
        {
          ReadValueToProperty (elements_count * 16, &values [0][0][0], desc, PropertyTypeMap<PropertyType_Float> ());
          return;
        }
        else break;
      case PropertyType_Vector:
        if (elements_count * 4 == desc.elements_count)
        {
          ReadValueToProperty (elements_count * 4, &values [0][0], desc, PropertyTypeMap<PropertyType_Vector> ());
          return;
        }
        else break;
      case PropertyType_Matrix:
        ReadValueToProperty (elements_count, values, desc, PropertyTypeMap<PropertyType_Matrix> ());
        return;
      default:
        break;
    }

    throw xtl::format_operation_exception ("", "Could not convert property '%s' from %s[%u] to %s[%u]", desc.name,
      get_name (PropertyType_Matrix), elements_count, get_name (desc.type), desc.elements_count);
  }

  template <common::PropertyType type, class T>
  void ReadValueToProperty (size_t elements_count, const T* values, const PropertyDesc& desc, PropertyTypeMap<type>)
  {
    if (elements_count != desc.elements_count && elements_count * sizeof (T) != desc.elements_count * get_size (desc.type))
      throw xtl::format_operation_exception ("common::PropertyMap::Impl::ReadValueToProperty(size_t,const T*,const PropertyDesc&)",
        "Could not convert property '%s' from %s[%u] to %s[%u]", desc.name, get_name (static_cast<common::PropertyType> (PropertyTypeSelector<T>::value)),
        elements_count, get_name (desc.type), desc.elements_count);    

    typedef typename PropertyTypeMap<type>::type result_type;

    result_type* property_values = reinterpret_cast<result_type*> (buffer.data () + desc.offset);

    for (;elements_count--; property_values++, values++)
      *property_values = PropertyTypeMap<type>::cast (*values);
  }
  
  template <class T>
  void ReadValueToProperty (size_t elements_count, const T* values, const PropertyDesc& desc, PropertyTypeMap<PropertyType_String>)
  {
    stl::string s;

    xtl::to_string (s, values, values + elements_count, DEFAULT_VALUES_SEPARATOR);

    unsigned int string_index = *reinterpret_cast<unsigned int*> (buffer.data () + desc.offset);

    strings.Set (string_index, s.c_str ());
  }

  template <common::PropertyType type>
  void ReadValueToProperty (size_t, const char* const* values, const PropertyDesc& desc, PropertyTypeMap<type>)
  {
    typedef typename PropertyTypeMap<type>::type result_type;

    common::StringArray tokens = common::split (values [0], " ;,");
    
    result_type* property_values = reinterpret_cast<result_type*> (buffer.data () + desc.offset);
    
    xtl::io::token_iterator<const char*> iter = xtl::io::make_token_iterator (tokens.Data (), tokens.Data () + tokens.Size ());

    if (ReadRange (iter, property_values, desc.elements_count) != desc.elements_count || iter.available ())
      throw xtl::format_operation_exception ("common::PropertyMap::Impl::ReadValueToProperty(const char*,PropertyDesc&)",
        desc.elements_count == 1 ? "Could not convert property '%s' from %s='%s' to %s" :
        "Could not convert property '%s' from %s='%s' to %s [%u]", desc.name, get_name (PropertyType_String), values [0], get_name (type),
        desc.elements_count);
  }

  void ReadValueToProperty (size_t, const char* const* values, const PropertyDesc& desc, PropertyTypeMap<PropertyType_String>)
  {
    unsigned int string_index = *reinterpret_cast<unsigned int*> (buffer.data () + desc.offset);
    
    strings.Set (string_index, values [0]);
  }
  
  template <class Token, class BaseIter, class T>
  size_t ReadRange (xtl::io::token_iterator<Token, BaseIter>& iter, T* first, size_t count)
  {
    size_t read_count = 0;
    
    for (;count--; ++first, ++read_count)
    {
      T value;
      
      if (!ReadValue (iter, value))
        break;

      *first = value;
    }

    return read_count;
  }
  
  template <class Iter, class T>
  bool ReadValue (Iter& iter, T& value)
  {
    return read (iter, value);
  }
  
  template <class Iter>
  bool ReadValue (Iter& iter, math::vec4f& result)
  {
    size_t available = iter.available ();
    
    if (available > 4)
      available = 4;
    
    switch (available)
    {    
      case 1:
      {
        float value = 0.0f;

        if (!read (iter, value))
          return false;

        result = math::vec4f (value);

        return true;
      }
      case 2:
      case 3:
      case 4:
      {
        math::vec4f v (0.0f);
        
        for (size_t i=0; i<available; i++)
          if (!read (iter, v [i]))
            return false;

        result = v;

        return true;
      }
      default:
        return false;
    }
  }
  
  template <class Iter>  
  bool ReadValue (Iter& iter, math::mat4f& result)
  {
    size_t available = iter.available ();
    
    if (available > 16)
      available = 16;
    
    switch (available)
    {    
      case 1:
      {
        float value = 0.0f;

        if (!read (iter, value))
          return false;

        result = math::mat4f (value);

        return true;
      }
      case 16:
        return read (iter, result);
      default:
        return false;
    }  
  }  
  
///Чтение свойства по имени
  template <class T>
  void GetProperty (const char* method_name, const char* name, size_t elements_count, T* values)
  {
    try
    {
      GetProperty (GetIndex (name), elements_count, values);
    }
    catch (xtl::exception& e)
    {
      e.touch (method_name);
      throw;
    }
  }
  
///Чтение свойства по индексу
  template <class T>
  void GetProperty (size_t property_index, size_t elements_count, T* values)
  {
    if (property_index >= layout.Size ())
      throw xtl::make_range_exception ("", "property_index", property_index, layout.Size ());

    if (!values)
      throw xtl::make_null_argument_exception ("", "values");
      
    if (!elements_count)
      throw xtl::make_null_argument_exception ("", "elements_count");

    const PropertyDesc& desc = layout.Properties ()[property_index];

    switch (desc.type)
    {
      case PropertyType_String:
      case PropertyType_Integer:
      case PropertyType_Float:
      case PropertyType_Vector:      
      case PropertyType_Matrix:
        break;
      default:
        throw xtl::format_operation_exception ("", "Internal error: wrong property '%s' type %d", desc.name, desc.type);
    }
    
    static const common::PropertyType target_type = static_cast<common::PropertyType> (PropertyTypeSelector<T>::value);

    ReadValueFromPropertyDispatch (elements_count, values, desc, PropertyTypeMap<target_type> ());
  }
  
  template <class T>
  void GetProperty (const char* method_name, size_t property_index, size_t elements_count, T* values)
  {
    try
    {
      GetProperty (property_index, elements_count, values);
    }
    catch (xtl::exception& e)
    {
      e.touch (method_name);
      throw;
    }
  }  
  
///Чтение значения из свойства
  template <class T>
  void ReadScalarValueFromProperty (size_t elements_count, T* result, const PropertyDesc& desc)
  {
    switch (desc.type)
    {
      case PropertyType_String:
        ReadValueFromProperty (elements_count, result, desc, PropertyTypeMap<PropertyType_String> ());
        return;
      case PropertyType_Integer:
        ReadValueFromProperty (elements_count, result, desc, PropertyTypeMap<PropertyType_Integer> ());
        return;
      case PropertyType_Float:
        ReadValueFromProperty (elements_count, result, desc, PropertyTypeMap<PropertyType_Float> ());
        return;
      case PropertyType_Vector:
        if (elements_count == desc.elements_count * 4)
        {
          ReadValueFromProperty (elements_count, result, desc, PropertyTypeMap<PropertyType_Float> ());
          return;
        }
        else break;
      case PropertyType_Matrix:
        if (elements_count == desc.elements_count * 16)
        {
          ReadValueFromProperty (elements_count, result, desc, PropertyTypeMap<PropertyType_Float> ());
          return;
        }
        else break;
      default:
        break;
    }
    
    throw xtl::format_operation_exception ("", "Could not convert property '%s' from %s[%u] to %s[%u]", desc.name,
      get_name (desc.type), desc.elements_count, get_name (static_cast<common::PropertyType> (PropertyTypeSelector<T>::value)), elements_count);
  }

  template <class T>
  void ReadValueFromPropertyDispatch (size_t elements_count, T* result, const PropertyDesc& desc, PropertyTypeMap<PropertyType_String>)
  {
    if (elements_count > 1)
      throw xtl::format_operation_exception ("", "Internal error: attempt to set %u strings", elements_count);
      
    switch (desc.type)
    {
      case PropertyType_String:
        ReadValueFromProperty (elements_count, result, desc, PropertyTypeMap<PropertyType_String> ());
        break;
      case PropertyType_Integer:
        ReadValueFromProperty (elements_count, result, desc, PropertyTypeMap<PropertyType_Integer> ());
        break;
      case PropertyType_Float:
        ReadValueFromProperty (elements_count, result, desc, PropertyTypeMap<PropertyType_Float> ());
        break;
      case PropertyType_Vector:
        ReadValueFromProperty (elements_count, result, desc, PropertyTypeMap<PropertyType_Vector> ());
        break;    
      case PropertyType_Matrix:
        ReadValueFromProperty (elements_count, result, desc, PropertyTypeMap<PropertyType_Matrix> ());
        break;
     default:
        throw xtl::format_operation_exception ("", "Could not convert property '%s' from %s[%u] to %s[%u]", desc.name,
          get_name (desc.type), desc.elements_count, get_name (static_cast<common::PropertyType> (PropertyTypeSelector<T>::value)), elements_count);
    }
  }
  
  template <class T>
  void ReadValueFromPropertyDispatch (size_t elements_count, T* result, const PropertyDesc& desc, PropertyTypeMap<PropertyType_Integer>)
  {
    ReadScalarValueFromProperty (elements_count, result, desc);
  }
  
  template <class T>
  void ReadValueFromPropertyDispatch (size_t elements_count, T* result, const PropertyDesc& desc, PropertyTypeMap<PropertyType_Float>)
  {
    ReadScalarValueFromProperty (elements_count, result, desc);
  }
  
  template <class T>
  void ReadValueFromPropertyDispatch (size_t elements_count, T* result, const PropertyDesc& desc, PropertyTypeMap<PropertyType_Vector>)
  {
    switch (desc.type)
    {
      case PropertyType_String:
        ReadValueFromProperty (elements_count, result, desc, PropertyTypeMap<PropertyType_String> ());
        return;
      case PropertyType_Integer:
        ReadValueFromProperty (elements_count, result, desc, PropertyTypeMap<PropertyType_Integer> ());
        return;
      case PropertyType_Float:
        ReadValueFromProperty (elements_count, result, desc, PropertyTypeMap<PropertyType_Float> ());
        return;
      case PropertyType_Vector:
        ReadValueFromProperty (elements_count, result, desc, PropertyTypeMap<PropertyType_Vector> ());
        return;
      case PropertyType_Matrix:
        if (elements_count == desc.elements_count * 4)
        {
          ReadValueFromProperty (elements_count, result, desc, PropertyTypeMap<PropertyType_Vector> ());
          return;
        }
        else break;        
      default:
        break;
    }
    
    throw xtl::format_operation_exception ("", "Could not convert property '%s' from %s[%u] to %s[%u]", desc.name,
      get_name (desc.type), desc.elements_count, get_name (PropertyType_Vector), elements_count);
  }
  
  template <class T>
  void ReadValueFromPropertyDispatch (size_t elements_count, T* result, const PropertyDesc& desc, PropertyTypeMap<PropertyType_Matrix>)
  {
    switch (desc.type)
    {
      case PropertyType_String:
        ReadValueFromProperty (elements_count, result, desc, PropertyTypeMap<PropertyType_String> ());
        return;
      case PropertyType_Integer:
        ReadValueFromProperty (elements_count, result, desc, PropertyTypeMap<PropertyType_Integer> ());
        return;
      case PropertyType_Float:
        ReadValueFromProperty (elements_count, result, desc, PropertyTypeMap<PropertyType_Float> ());
        return;
      case PropertyType_Vector:
        if (elements_count * 4 == desc.elements_count)
        {
          ReadValueFromProperty (desc.elements_count, &result [0][0], desc, PropertyTypeMap<PropertyType_Vector> ());
          return;
        }
        else break;
      case PropertyType_Matrix:
        ReadValueFromProperty (elements_count, result, desc, PropertyTypeMap<PropertyType_Matrix> ());
        return;
      default:
        break;
    }

    throw xtl::format_operation_exception ("", "Could not convert property '%s' from %s[%u] to %s[%u]", desc.name,
      get_name (desc.type), desc.elements_count, get_name (PropertyType_Matrix), elements_count);
  }

  template <common::PropertyType type, class T>
  void ReadValueFromProperty (size_t elements_count, T* values, const PropertyDesc& desc, PropertyTypeMap<type>)
  {
    if (elements_count != desc.elements_count && elements_count * sizeof (T) != desc.elements_count * get_size (desc.type))
      throw xtl::format_operation_exception ("common::PropertyMap::Impl::ReadValueFromProperty(size_t,const T*,const PropertyDesc&)",
        "Could not convert property '%s' from %s[%u] to %s[%u]", desc.name, get_name (desc.type), desc.elements_count,
        get_name (static_cast<common::PropertyType> (PropertyTypeSelector<T>::value)), elements_count);

    typedef typename PropertyTypeMap<type>::type source_type;

    source_type* property_values = reinterpret_cast<source_type*> (buffer.data () + desc.offset);

    for (;elements_count--; property_values++, values++)
      *values = PropertyTypeMap<static_cast<common::PropertyType> (PropertyTypeSelector<T>::value)>::cast (*property_values);
  }
  
  template <class T>
  void ReadValueFromProperty (size_t elements_count, T* values, const PropertyDesc& desc, PropertyTypeMap<PropertyType_String>)  
  {
    static const char* METHOD_NAME = "common::PropertyMap::Impl::ReadValueFromProperty(size_t,const T*,const PropertyDesc&)";
    
    if (elements_count != desc.elements_count)
      throw xtl::format_operation_exception (METHOD_NAME, "Property '%s' elements count %u mismatch setting elements count %u", desc.name, desc.elements_count, elements_count);

    if (elements_count > 1)
      throw xtl::format_operation_exception (METHOD_NAME, "Internal error: property '%s' is string array with %u elements", desc.name, desc.elements_count);
      
    unsigned int string_index = *reinterpret_cast<unsigned int*> (buffer.data () + desc.offset);
    
    const char* string = strings [string_index];

    common::StringArray tokens = common::split (string, " ;,");
    
    xtl::io::token_iterator<const char*> iter = xtl::io::make_token_iterator (tokens.Data (), tokens.Data () + tokens.Size ());

    if (ReadRange (iter, values, desc.elements_count) != desc.elements_count || iter.available ())
      throw xtl::format_operation_exception (METHOD_NAME,
        desc.elements_count == 1 ? "Could not convert property '%s' from %s='%s' to %s" :
        "Could not convert property '%s' from %s='%s' to %s [%u]", desc.name, get_name (PropertyType_String), string,
        get_name (static_cast<common::PropertyType> (PropertyTypeSelector<T>::value)), desc.elements_count);
  }
  
  template <common::PropertyType type>
  void ReadValueFromProperty (size_t elements_count, stl::string* values, const PropertyDesc& desc, PropertyTypeMap<type>)
  {
    typedef typename PropertyTypeMap<type>::type source_type;

    source_type* property_values = reinterpret_cast<source_type*> (buffer.data () + desc.offset);

    xtl::to_string (*values, property_values, property_values + desc.elements_count, DEFAULT_VALUES_SEPARATOR);
  }
  
  void ReadValueFromProperty (size_t elements_count, stl::string* values, const PropertyDesc& desc, PropertyTypeMap<PropertyType_String>)
  {
    unsigned int string_index = *reinterpret_cast<unsigned int*> (buffer.data () + desc.offset);

    *values = strings [string_index];
  }  
  
///Получение данных
  template <class T>
  const T* GetData (const char* method_name, size_t property_index)
  {
    try
    {
      if (property_index >= layout.Size ())
        throw xtl::make_range_exception ("", "property_index", property_index, layout.Size ());
        
      static const common::PropertyType target_type = static_cast<common::PropertyType> (PropertyTypeSelector<T>::value);

      const PropertyDesc& desc = layout.Properties ()[property_index];
      
      if (desc.type != target_type)
        throw xtl::format_operation_exception ("", "Could not convert property '%s' from %s to %s", desc.name, get_name (desc.type), get_name (target_type));
        
      return reinterpret_cast<const T*> (buffer.data () + desc.offset);
    }
    catch (xtl::exception& e)
    {
      e.touch (method_name);
      throw;
    }
  }  
  
///Удаление свойства
  void RemoveProperty (size_t index)
  {
    try
    {
      if (index >= layout.Size ())
        return;
        
      CaptureLayout ();
      
      const PropertyDesc& desc = layout.Properties ()[index];
      
      if (desc.type == PropertyType_String)
      {
        unsigned int string_index = *reinterpret_cast<unsigned int*> (buffer.data () + desc.offset);
        
          //поиск дубликатов
        
        const PropertyDesc* update_desc = layout.Properties ();        
        
        size_t string_entries_count = 0;
        
        for (size_t i=0, count=layout.Size (); i<count; i++, update_desc++)
          if (update_desc->type == PropertyType_String)
          {
            unsigned int& update_string_index = *reinterpret_cast<unsigned int*> (buffer.data () + desc.offset);
            
            if (update_string_index == string_index)
              string_entries_count++;
          }
          
        if (string_entries_count == 1) //при отсутствии дубликатов удаление строки
        {
          strings.Remove (string_index);
          
          update_desc = layout.Properties ();          

          for (size_t i=0, count=layout.Size (); i<count; i++, update_desc++)
          {
            if (update_desc->type == PropertyType_String)
            {              
              unsigned int& update_string_index = *reinterpret_cast<unsigned int*> (buffer.data () + desc.offset);
            
              if (update_string_index > string_index)
                update_string_index--;
            }
          }
        }                  
      }
      
      size_t property_size = get_size (desc.type) * desc.elements_count;
      
      memcpy (buffer.data () + desc.offset, buffer.data () + desc.offset + property_size, buffer.size () - desc.offset - property_size);

      buffer.resize (buffer.size () - property_size);

      layout.RemoveProperty (index);
      
      UpdateNotify ();
    }
    catch (xtl::exception& e)
    {
      e.touch ("common::PropertyMap::RemoveProperty(size_t)");
      throw;
    }
  }
  
///Изменение параметров свойства
  void ChangeProperty (size_t index, common::PropertyType type, size_t elements_count)
  {
      //проверка корректности аргументов
      
    if (index >= layout.Size ())
      throw xtl::make_range_exception ("", "index", index, layout.Size ());
      
    switch (type)
    {
      case PropertyType_String:
      case PropertyType_Integer:
      case PropertyType_Float:
      case PropertyType_Vector:      
      case PropertyType_Matrix:
        break;
      default:
        throw xtl::make_argument_exception ("", "type", type);
    }
    
    if (!elements_count)
      throw xtl::make_null_argument_exception ("", "elements_count");            
      
    const PropertyDesc& desc = layout.Properties ()[index];
    
    if (desc.type == type && desc.elements_count == elements_count)
      return;
      
      //получение изменения размера буфера

    size_t new_property_size = get_size (type) * elements_count,
           old_property_size = get_size (desc.type) * desc.elements_count;
          
    int size_diff = new_property_size - old_property_size;
    
      //предварительные операции    

    CaptureLayout ();

    need_update = true;
    
      //сохранение параметров
    
    common::PropertyType old_type = desc.type;

    size_t old_elements_count = desc.elements_count,
           old_offset         = desc.offset,
           old_strings_count = strings.Size ();

    Buffer old_buffer (buffer.size () + size_diff);    
    
      //замена буфера
      
    old_buffer.swap (buffer);    

    try
    {
        //копирование данных

      memcpy (buffer.data (), old_buffer.data (), desc.offset);
      memset (buffer.data () + desc.offset, 0, new_property_size);
      memcpy (buffer.data () + desc.offset + new_property_size, old_buffer.data () + desc.offset + old_property_size, old_buffer.size () - 
        desc.offset - old_property_size);
        
        //добавление строки в случае необходимости

      if (type == PropertyType_String)
        *reinterpret_cast<unsigned int*> (buffer.data () + desc.offset) = strings.Add ("");

        //изменение типа в лэйауте
        
      layout.SetPropertyType (index, type);
      layout.SetPropertyElementsCount (index, elements_count);

        //установка свойства
        
      switch (old_type)
      {
        case PropertyType_String:
        {
          unsigned int string_index = *reinterpret_cast<unsigned int*> (old_buffer.data () + old_offset);
          const char*  string       = strings [string_index];

          SetProperty (index, 1, &string);

          break;
        }
        case PropertyType_Integer:
          SetProperty (index, old_elements_count, reinterpret_cast<const int*> (old_buffer.data () + old_offset));
          break;
        case PropertyType_Float:
          SetProperty (index, old_elements_count, reinterpret_cast<const float*> (old_buffer.data () + old_offset));
          break;
        case PropertyType_Vector:
          SetProperty (index, old_elements_count, reinterpret_cast<const math::vec4f*> (old_buffer.data () + old_offset));
          break;
        case PropertyType_Matrix:
          SetProperty (index, old_elements_count, reinterpret_cast<const math::mat4f*> (old_buffer.data () + old_offset));
          break;
        default:
          throw xtl::format_operation_exception ("", "Internal error: wrong property '%s' type %d", layout.Properties ()[index].name, old_type);
      }
    }
    catch (...)
    {
      layout.SetPropertyType (index, old_type);
      layout.SetPropertyElementsCount (index, old_elements_count);
      
      buffer.swap (old_buffer);
      strings.Remove (old_strings_count, strings.Size () - old_strings_count);
      
      throw;
    }
  }

  void ChangeProperty (const char* method_name, size_t index, common::PropertyType type)
  {
    try
    {
      ChangeProperty (index, type, layout.PropertyElementsCount (index));
    }
    catch (xtl::exception& e)
    {
      e.touch (method_name);
      throw;
    }
  }
  
  void ChangeProperty (const char* method_name, size_t index, size_t elements_count)
  {
    try
    {
      ChangeProperty (index, layout.PropertyType (index), elements_count);
    }
    catch (xtl::exception& e)
    {
      e.touch (method_name);
      throw;
    }
  }
};

namespace common
{

template <>
const char* PropertyMap::Impl::GetData (const char* method_name, size_t property_index)
{
  try
  {
    if (property_index >= layout.Size ())
      throw xtl::make_range_exception ("", "property_index", property_index, layout.Size ());        

    const PropertyDesc& desc = layout.Properties ()[property_index];

    if (desc.type != PropertyType_String)
      throw xtl::format_operation_exception ("", "Could not convert property '%s' from %s to %s", desc.name, get_name (desc.type), get_name (PropertyType_String));

    unsigned int string_index = *reinterpret_cast<unsigned int*> (buffer.data () + desc.offset);

    return strings [string_index];
  }
  catch (xtl::exception& e)
  {
    e.touch (method_name);
    throw;
  }    
}

}

/*
    Конструкторы / деструктор / присваивание
*/

PropertyMap::PropertyMap ()
  : impl (new Impl)
{
}

PropertyMap::PropertyMap (Impl* in_impl)
  : impl (in_impl)
{
}

PropertyMap::PropertyMap (const PropertyLayout& layout)
  : impl (new Impl (layout))
{
}

PropertyMap::PropertyMap (const PropertyMap& map)
  : impl (map.impl)
{
  addref (impl);
}

PropertyMap::~PropertyMap ()
{
  release (impl);
}

PropertyMap& PropertyMap::operator = (const PropertyMap& map)
{
  PropertyMap (map).Swap (*this);
  return *this;
}

/*
    Идентификатор
*/

size_t PropertyMap::Id () const
{
  return reinterpret_cast<size_t> (impl);
}

/*
    Получение объекта оповещения об удалении
*/

xtl::trackable& PropertyMap::Trackable () const
{
  return *impl;
}

namespace common
{

xtl::trackable& get_trackable (const PropertyMap& map)
{
  return map.Trackable ();
}

}

/*
    Подписка на события карты свойств
*/

xtl::connection PropertyMap::RegisterEventHandler (PropertyMapEvent event, const EventHandler& handler) const
{
  switch (event)
  {
    case PropertyMapEvent_OnUpdate:
      return impl->signals [event].connect (handler);
    default:
      throw xtl::make_argument_exception ("common::PropertyMap::RegisterEventHandler", "event", event);
  }
}

/*
    Копирование
*/

PropertyMap PropertyMap::Clone () const
{
  return PropertyMap (new Impl (*impl));
}

/*
    Количество свойств
*/

size_t PropertyMap::Size () const
{
  return impl->layout.Size ();
}

/*
    Хэш параметров
*/

//хэш данных
size_t PropertyMap::Hash () const
{
  if (impl->need_update)
    impl->Update ();
    
  return impl->hash;
}

//хэш структуры параметров
size_t PropertyMap::LayoutHash () const
{
  return impl->layout.Hash ();
}

/*
    Имя свойства
*/

const char* PropertyMap::PropertyName (size_t index) const
{
  try
  {
    return impl->layout.PropertyName (index);
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::PropertyName");
    throw;
  }
}

void PropertyMap::SetPropertyName (size_t index, const char* name)
{
  try
  {
    impl->CaptureLayout ();
    
    impl->layout.SetPropertyName (index, name);
    
    impl->UpdateNotify ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::SetPropertyName(size_t,const char*)");
    throw;
  }
}

void PropertyMap::SetPropertyName (const char* old_name, const char* new_name)
{
  try
  {
    impl->CaptureLayout ();
    
    impl->layout.SetPropertyName (old_name, new_name);

    impl->UpdateNotify ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::SetPropertyName(const char*,const char*)");
    throw;
  }
}

/*
    Тип свойства / количество элементов
*/

PropertyType PropertyMap::PropertyType (size_t index) const
{
  try
  {
    return impl->layout.PropertyType (index);
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::PropertyType(size_t)");
    throw;
  }
}

PropertyType PropertyMap::PropertyType (const char* name) const
{
  try
  {
    return impl->layout.PropertyType (name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::PropertyType(const char*)");
    throw;
  }
}

size_t PropertyMap::PropertyElementsCount (size_t index) const
{
  try
  {
    return impl->layout.PropertyElementsCount (index);
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::PropertyElementsCount(size_t)");
    throw;
  }
}

size_t PropertyMap::PropertyElementsCount (const char* name) const
{
  try
  {
    return impl->layout.PropertyElementsCount (name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::PropertyElementsCount(const char*)");
    throw;
  }
}

/*
    Смена типа / количества элементов
*/

void PropertyMap::SetPropertyType (const char* name, common::PropertyType type)
{
  try
  {
    SetPropertyType (impl->GetIndex (name), type);
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::SetPropertyType(const char*,PropertyType)");
    throw;
  }
}

void PropertyMap::SetPropertyElementsCount (const char* name, size_t count)
{
  try
  {
    SetPropertyElementsCount (impl->GetIndex (name), count);
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::SetPropertyElementsCount(const char*,size_t)");
    throw;
  }
}

void PropertyMap::SetPropertyType (size_t index, common::PropertyType type)
{
  impl->ChangeProperty ("common::PropertyMap::SetPropertyType(size_t,PropertyType)", index, type);
}

void PropertyMap::SetPropertyElementsCount (size_t index, size_t count)
{
  impl->ChangeProperty ("common::PropertyMap::SetPropertyElementsCount(size_t,size_t)", index, count);
}

/*
    Поиск индекса свойства
*/

int PropertyMap::IndexOf (const char* name) const
{
  return impl->layout.IndexOf (name);
}

bool PropertyMap::IsPresent (const char* name) const
{
  return impl->layout.IsPresent (name);
}

/*
    Добавление свойства
*/

size_t PropertyMap::AddProperty (const char* name, common::PropertyType type, size_t elements_count)
{
  try
  {
    size_t index = impl->InsertProperty (name, type, elements_count);
    
    impl->UpdateNotify ();
    
    return index;
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::AddProperty");
    throw;
  }
}

/*
    Установка свойства по имени
*/

void PropertyMap::SetProperty (const char* name, int value)
{
  SetProperty (name, 1, &value);
}

void PropertyMap::SetProperty (const char* name, float value)
{
  SetProperty (name, 1, &value);
}

void PropertyMap::SetProperty (const char* name, const math::vec4f& value)
{
  SetProperty (name, 1, &value);
}

void PropertyMap::SetProperty (const char* name, const math::mat4f& value)
{
  SetProperty (name, 1, &value);
}

void PropertyMap::SetProperty (const char* name, const stl::string& value)
{
  SetProperty (name, value.c_str ());
}

void PropertyMap::SetProperty (const char* name, const char* value)
{
  impl->SetProperty ("common::PropertyMap::SetProperty(const char*,const char*)", name, 1, &value);
}

void PropertyMap::SetProperty (const char* name, size_t count, const int* values)
{
  impl->SetProperty ("common::PropertyMap::SetProperty(const char*,size_t,const int*)", name, count, values);
}

void PropertyMap::SetProperty (const char* name, size_t count, const float* values)
{
  impl->SetProperty ("common::PropertyMap::SetProperty(const char*,size_t,const float*)", name, count, values);
}

void PropertyMap::SetProperty (const char* name, size_t count, const math::vec4f* values)
{
  impl->SetProperty ("common::PropertyMap::SetProperty(const char*,size_t,const math::vec4f*)", name, count, values);
}

void PropertyMap::SetProperty (const char* name, size_t count, const math::mat4f* values)
{
  impl->SetProperty ("common::PropertyMap::SetProperty(const char*,size_t,const math::mat4f*)", name, count, values);
}

/*
    Установка свойства по индексу
*/

void PropertyMap::SetProperty (size_t property_index, const stl::string& value)
{
  SetProperty (property_index, value.c_str ());
}

void PropertyMap::SetProperty (size_t property_index, int value)
{
  SetProperty (property_index, 1, &value);
}

void PropertyMap::SetProperty (size_t property_index, float value)
{
  SetProperty (property_index, 1, &value);
}

void PropertyMap::SetProperty (size_t property_index, const math::vec4f& value)
{
  SetProperty (property_index, 1, &value);
}

void PropertyMap::SetProperty (size_t property_index, const math::mat4f& value)
{
  SetProperty (property_index, 1, &value);
}

void PropertyMap::SetProperty (size_t property_index, const char* value)
{
  impl->SetProperty ("common::PropertyMap::SetProperty(size_t,const char*)", property_index, 1, &value);
}

void PropertyMap::SetProperty (size_t property_index, size_t count, const int* values)
{
  impl->SetProperty ("common::PropertyMap::SetProperty(size_t,const int*)", property_index, count, values);
}

void PropertyMap::SetProperty (size_t property_index, size_t count, const float* values)
{
  impl->SetProperty ("common::PropertyMap::SetProperty(size_t,const float*)", property_index, count, values);
}

void PropertyMap::SetProperty (size_t property_index, size_t count, const math::vec4f* values)
{
  impl->SetProperty ("common::PropertyMap::SetProperty(size_t,const math::vec4f*)", property_index, count, values);
}

void PropertyMap::SetProperty (size_t property_index, size_t count, const math::mat4f* values)
{
  impl->SetProperty ("common::PropertyMap::SetProperty(size_t,const math::mat4f*)", property_index, count, values);
}

/*
    Чтение свойства по имени
*/

void PropertyMap::GetProperty (const char* name, int& result) const
{
  GetProperty (name, 1, &result);
}

void PropertyMap::GetProperty (const char* name, float& result) const
{
  GetProperty (name, 1, &result);
}

void PropertyMap::GetProperty (const char* name, math::vec4f& result) const
{
  GetProperty (name, 1, &result);
}

void PropertyMap::GetProperty (const char* name, math::mat4f& result) const
{
  GetProperty (name, 1, &result);
}

void PropertyMap::GetProperty (const char* name, stl::string& result) const
{
  impl->GetProperty ("common::PropertyMap::GetProperty(const char*,stl::string&)", name, 1, &result);
}

void PropertyMap::GetProperty (const char* name, size_t count, int* values) const
{
  impl->GetProperty ("common::PropertyMap::GetProperty(const char*,size_t,int*)", name, count, values);
}

void PropertyMap::GetProperty (const char* name, size_t count, float* values) const
{
  impl->GetProperty ("common::PropertyMap::GetProperty(const char*,size_t,float*)", name, count, values);
}

void PropertyMap::GetProperty (const char* name, size_t count, math::vec4f* values) const
{
  impl->GetProperty ("common::PropertyMap::GetProperty(const char*,size_t,math::vec4f*)", name, count, values);
}

void PropertyMap::GetProperty (const char* name, size_t count, math::mat4f* values) const
{
  impl->GetProperty ("common::PropertyMap::GetProperty(const char*,size_t,math::mat4f*)", name, count, values);
}

int PropertyMap::GetInteger (const char* name) const
{
  int result = 0;
  
  GetProperty (name, 1, &result);
  
  return result;
}

float PropertyMap::GetFloat (const char* name) const
{
  float result = 0.0f;
  
  GetProperty (name, 1, &result);
  
  return result;
}

const math::vec4f& PropertyMap::GetVector (const char* name) const
{
  return GetVectorArray (name)[0];
}

const math::mat4f& PropertyMap::GetMatrix (const char* name) const
{
  return GetMatrixArray (name)[0];
}

const char* PropertyMap::GetString (const char* name) const
{
  try
  {
    return GetString (impl->GetIndex (name));
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::GetString(const char*)");
    throw;
  }
}

const int* PropertyMap::GetIntegerArray (const char* name) const
{
  try
  {
    return GetIntegerArray (impl->GetIndex (name));
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::GetIntegerArray(const char*)");
    throw;
  }
}

const float* PropertyMap::GetFloatArray (const char* name) const
{
  try
  {
    return GetFloatArray (impl->GetIndex (name));
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::GetFloatArray(const char*)");
    throw;
  }
}

const math::vec4f* PropertyMap::GetVectorArray (const char* name) const
{
  try
  {
    return GetVectorArray (impl->GetIndex (name));
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::GetVectorArray(const char*)");
    throw;
  }
}

const math::mat4f* PropertyMap::GetMatrixArray (const char* name) const
{
  try
  {
    return GetMatrixArray (impl->GetIndex (name));
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::GetMatrixArray(const char*)");
    throw;
  }
}

/*
    Чтение свойства по индексу
*/

void PropertyMap::GetProperty (size_t property_index, int& result) const
{
  GetProperty (property_index, 1, &result);
}

void PropertyMap::GetProperty (size_t property_index, float& result) const
{
  GetProperty (property_index, 1, &result);
}

void PropertyMap::GetProperty (size_t property_index, math::vec4f& result) const
{
  GetProperty (property_index, 1, &result);
}

void PropertyMap::GetProperty (size_t property_index, math::mat4f& result) const
{
  GetProperty (property_index, 1, &result);
}

void PropertyMap::GetProperty (size_t property_index, stl::string& result) const
{
  impl->GetProperty ("common::PropertyMap::GetProperty(size_t,stl::string&)", property_index, 1, &result);
}

void PropertyMap::GetProperty (size_t property_index, size_t count, int* values) const
{
  impl->GetProperty ("common::PropertyMap::GetProperty(size_t,size_t,int*)", property_index, count, values);
}

void PropertyMap::GetProperty (size_t property_index, size_t count, float* values) const
{
  impl->GetProperty ("common::PropertyMap::GetProperty(size_t,size_t,float*)", property_index, count, values);
}

void PropertyMap::GetProperty (size_t property_index, size_t count, math::vec4f* values) const
{
  impl->GetProperty ("common::PropertyMap::GetProperty(size_t,size_t,math::vec4f*)", property_index, count, values);
}

void PropertyMap::GetProperty (size_t property_index, size_t count, math::mat4f* values) const
{
  impl->GetProperty ("common::PropertyMap::GetProperty(size_t,size_t,math::mat4f*)", property_index, count, values);
}

int PropertyMap::GetInteger (size_t property_index) const
{
  int result = 0;
  
  GetProperty (property_index, 1, &result);
  
  return result;
}

float PropertyMap::GetFloat (size_t property_index) const
{
  float result = 0.0f;

  GetProperty (property_index, 1, &result);

  return result;
}

const math::vec4f& PropertyMap::GetVector (size_t property_index) const
{
  return GetVectorArray (property_index)[0];
}

const math::mat4f& PropertyMap::GetMatrix (size_t property_index) const
{
  return GetMatrixArray (property_index)[0];
}

const char* PropertyMap::GetString (size_t property_index) const
{
  return impl->GetData<char> ("common::PropertyMap::GetString(size_t)", property_index);
}

const int* PropertyMap::GetIntegerArray (size_t property_index) const
{
  return impl->GetData<int> ("common::PropertyMap::GetIntegerArray(size_t)", property_index);
}

const float* PropertyMap::GetFloatArray (size_t property_index) const
{
  return impl->GetData<float> ("common::PropertyMap::GetFloatArray(size_t)", property_index);
}

const math::vec4f* PropertyMap::GetVectorArray (size_t property_index) const
{
  return impl->GetData<math::vec4f> ("common::PropertyMap::GetVectorArray(size_t)", property_index);
}

const math::mat4f* PropertyMap::GetMatrixArray (size_t property_index) const
{
  return impl->GetData<math::mat4f> ("common::PropertyMap::GetMatrixArray(size_t)", property_index);
}

/*
    Удаление свойств
*/

void PropertyMap::RemoveProperty (const char* name)
{
  int index = impl->layout.IndexOf (name);
  
  if (index == -1)
    return;
    
  RemoveProperty (static_cast<size_t> (index));
}

void PropertyMap::RemoveProperty (size_t index)
{
  impl->RemoveProperty (index);
}

void PropertyMap::Clear ()
{
  try
  {
    impl->layout.Clear ();

    impl->buffer.resize (0);
    impl->strings.Clear ();

    impl->UpdateNotify ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::Clear");
    throw;
  }
}

/*
    Получение расположения параметров
*/

PropertyLayout PropertyMap::Layout () const
{
  return impl->layout;
}

/*
    Сброс лэйаута
*/

void PropertyMap::Reset (const PropertyLayout& in_layout)
{
  try
  {
    PropertyLayout layout (in_layout);

    layout.Capture ();

    StringArray strings;    
    Buffer      buffer (layout.BufferSize ());

    memset (buffer.data (), 0, buffer.size ());
    
    const PropertyDesc* desc = layout.Properties ();
    
    for (size_t i=0, count=layout.Size (); i<count; i++, desc++)
    {
      if (desc->type == PropertyType_String)
        *reinterpret_cast<unsigned int*> (buffer.data () + desc->offset) = strings.Add ("");
    }    

    impl->layout      = layout;
    impl->hash        = 0;
    impl->need_update = true;
    impl->strings     = strings;
    
    impl->buffer.swap (buffer);

    impl->UpdateNotify ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::Reset");
    throw;
  }
}

/*
    Работа с буфером
*/

const void* PropertyMap::BufferData () const
{
  return impl->buffer.data ();
}

size_t PropertyMap::BufferSize () const
{
  return impl->buffer.size ();
}

/*
    Обновление буфера
*/

void PropertyMap::SetBufferData (const void* source_data)
{
  try
  {
    if (!source_data)
      throw xtl::make_null_argument_exception ("", "source_data");

    const PropertyDesc* desc = impl->layout.Properties ();

    if (impl->strings.Size ())
    {    
      for (size_t i=0, count=impl->layout.Size (); i<count; i++, desc++)
      {
        if (desc->type == PropertyType_String)
        {
          unsigned int string_index = *reinterpret_cast<const unsigned int*> (reinterpret_cast<const char*> (source_data) + desc->offset);

          if (string_index >= impl->strings.Size ())
            throw xtl::format_operation_exception ("", "Can't set buffer to the property map because property[%u] has string index %u which is large than strings count %u", i, string_index, impl->strings.Size ());
        }
      }
    }

    memcpy (impl->buffer.data (), source_data, impl->buffer.size ());

    impl->UpdateNotify ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::SetBufferData");
    throw;
  }
}

/*
    Обмен
*/

void PropertyMap::Swap (PropertyMap& map)
{
  stl::swap (impl, map.impl);
}

namespace common
{

void swap (PropertyMap& map1, PropertyMap& map2)
{
  map1.Swap (map2);
}

}
