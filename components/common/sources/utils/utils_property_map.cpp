#include <stl/auto_ptr.h>
#include <stl/string>
#include <stl/vector>

#include <xtl/common_exceptions.h>
#include <xtl/lexical_cast.h>
#include <xtl/reference_counter.h>
#include <xtl/token_iterator.h>
#include <xtl/token_parser.h>

#include <math/vector.h>
#include <math/matrix.h>

#include <common/property_map.h>
#include <common/string.h>
#include <common/strlib.h>

using namespace common;

namespace
{

/*
    Константы
*/

const size_t PROPERTY_ARRAY_RESERVE_SIZE = 8; //резервируемое количество свойств

/*
    Свойство узла
*/

struct BasicProperty
{
  stl::string      name;      //имя свойства
  size_t           name_hash; //хэш имени
  PropertyType     type;      //тип свойства
  
///Конструктор
  BasicProperty (const char* in_name, PropertyType in_type)
    : name (in_name)
    , name_hash (common::strhash (in_name))
    , type (in_type)
  {
  }
  
  virtual ~BasicProperty () {}
  
///Копирование
  virtual BasicProperty* Clone () = 0;
  
///Хэш данных
  virtual size_t GetHash (size_t current_hash) = 0;
  
///Установка значения в список свойств узла
  virtual void CopyValueTo (PropertyMap& map, size_t property_index) = 0;
};

/*
    Реализация свойств для различных типов
*/

template <PropertyType type>
struct PropertyTypeMap;

template <> struct PropertyTypeMap<PropertyType_String>  { typedef stl::string type; };
template <> struct PropertyTypeMap<PropertyType_Integer> { typedef int         type; };
template <> struct PropertyTypeMap<PropertyType_Float>   { typedef float       type; };
template <> struct PropertyTypeMap<PropertyType_Vector>  { typedef math::vec4f type; };
template <> struct PropertyTypeMap<PropertyType_Matrix>  { typedef math::mat4f type; };

template <class T>
size_t get_value_hash (const T& value, size_t current_hash)
{
  return common::crc32 (&value, sizeof (T), current_hash);
}

size_t get_value_hash (const stl::string& value, size_t current_hash)
{
  return common::strhash (value.c_str (), current_hash);
}

template <PropertyType property_type> struct PropertyImpl: public BasicProperty
{
  typedef typename PropertyTypeMap<property_type>::type value_type;

  value_type value; //значение

///Конструктор
  PropertyImpl (const char* name)
    : BasicProperty (name, property_type)
    , value ()
  {    
  }
  
  PropertyImpl (const PropertyImpl& property)
    : BasicProperty (property)
    , value (property.value)
  {
  }
  
///Копирование
  BasicProperty* Clone ()
  {    
    return new PropertyImpl (*this);    
  }
  
///Хэш данных
  size_t GetHash (size_t current_hash)
  {
    return get_value_hash (value, current_hash);
  }
  
///Установка значения в список свойств узла
  void CopyValueTo (PropertyMap& map, size_t property_index)
  {  
    map.SetProperty (property_index, value);    
  }
};

typedef stl::vector<BasicProperty*> PropertyArray;

///чтение значения из строки
template <PropertyType type>
void read_value_to_property (const char* string, BasicProperty& property)
{
  typedef typename PropertyTypeMap<type>::type result_type;   

  if (!xtl::io::read_and_cast<result_type> (string, static_cast<PropertyImpl<type>&> (property).value))    
    throw xtl::format_operation_exception ("common::read_value_to_property", "Could not convert property '%s' from %s='%s' to %s",
      property.name.c_str (), get_name (PropertyType_String), string, get_name (type));  
}

template <> void read_value_to_property<PropertyType_String> (const char* string, BasicProperty& property)
{
  static_cast<PropertyImpl<PropertyType_String>&> (property).value = string;
}

///чтение значения из строки
template <PropertyType dst_type>
void read_value_from_property (const PropertyImpl<PropertyType_String>& property, typename PropertyTypeMap<dst_type>::type& result)
{
  if (!xtl::io::read_and_cast<typename PropertyTypeMap<dst_type>::type> (property.value.c_str (), result))
    throw xtl::format_operation_exception ("common::read_value_from_property", "Could not convert property '%s' from %s='%s' to %s",
      property.name.c_str (), get_name (property.type), property.value.c_str (), get_name (dst_type));
}

/*
    Связывание
*/

const size_t UNASSIGNED_PROPERTY_INDEX = ~0u;

struct Binding: public xtl::reference_counter
{
  stl::auto_ptr<IPropertySetter> setter; //функция установки свойства
  stl::auto_ptr<IPropertyGetter> getter; //функция получения свойства
  stl::string                    name;   //имя свойства
  size_t                         index;  //индекс свойства

  Binding (const char* in_name)
    : name (in_name)
    , index (UNASSIGNED_PROPERTY_INDEX)
  {
  }
};

typedef xtl::intrusive_ptr<Binding> BindingPtr;
typedef stl::vector<BindingPtr>     BindingArray;

}

/*
    Внутренние утилиты для преобразования строковых значений
*/

namespace math
{

inline bool read (const char* string, math::vec4f& result)
{
  common::StringArray tokens = common::split (string);
  
  switch (tokens.Size ())
  {    
    case 1:
    {
      float value = 0.0f;

      if (!xtl::io::read (tokens [0], value))
        return false;

      result = math::vec4f (value);

      return true;
    }
    case 2:
    case 3:
    case 4:
    {
      math::vec4f v (0.0f);
      
      for (size_t i=0, count=tokens.Size (); i<count; i++)
        if (!xtl::io::read (tokens [i], v [i]))
          return false;
      
      result = v;

      return true;
    }
    default:
      return false;
  }
}

inline bool read (const char* string, math::mat4f& result)
{
  typedef xtl::io::token_iterator<const char*> token_iterator;

  common::StringArray tokens = common::split (string);
  
  switch (tokens.Size ())
  {    
    case 1:
    {
      float value = 0.0f;

      if (!xtl::io::read (tokens [0], value))
        return false;

      result = math::mat4f (value);

      return true;
    }
    case 16:
    {
      token_iterator iter = xtl::io::make_token_iterator (tokens.Data (), tokens.Data () + tokens.Size ());
  
      return read (iter, result);
    }
    default:
      return false;
  }  
}

}

/*
    Описание реализации свойств узла
*/

struct PropertyMap::Impl: public xtl::reference_counter
{
  bool          need_hash_update;        //необходимо ли обновить хэш
  size_t        hash;                    //хэш параметров (композиция хэша данных и хэша структуры)
  size_t        data_hash;               //хэш данных
  size_t        structure_hash;          //хэш структуры
  size_t        bindings_data_hash;      //хэш отправленных параметров  
  size_t        bindings_structure_hash; //хэш структуры отправленных параметров
  PropertyArray properties;              //свойства
  BindingArray  bindings;                //связывания

///Конструкторы
  Impl ()  
    : need_hash_update (true)
    , hash (0)
    , data_hash (0)
    , structure_hash (0)
    , bindings_data_hash (0)
    , bindings_structure_hash (0)
  {
    properties.reserve (PROPERTY_ARRAY_RESERVE_SIZE);
  }
  
  Impl (const Impl& impl)
    : need_hash_update (impl.need_hash_update)
    , hash (impl.hash)
    , data_hash (impl.data_hash)
    , structure_hash (impl.structure_hash)
  {
    properties.reserve (impl.properties.size ());
    
    try
    {
      for (PropertyArray::const_iterator iter=impl.properties.begin (), end=impl.properties.end (); iter!=end; ++iter)
      {
        BasicProperty&               src_property = **iter;
        stl::auto_ptr<BasicProperty> new_property (src_property.Clone ());
        
        properties.push_back (new_property.get ());
        
        new_property.release ();
      }      
    }
    catch (...)
    {
      for (PropertyArray::iterator iter=properties.begin (), end=properties.end (); iter!=end; ++iter)
        delete *iter;

      properties.clear ();

      throw;
    }
  }

///Деструктор
  ~Impl ()
  {
    Clear ();
  }

///Получение индекса свойства
  size_t GetIndex (const char* name)
  {
    static const char* METHOD_NAME = "common::PropertyMap::Impl::GetIndex";
    
    if (!name)
      throw xtl::make_null_argument_exception (METHOD_NAME, "name");
      
    size_t hash  = common::strhash (name);
    size_t index = 0;  

    for (PropertyArray::iterator iter=properties.begin (), end=properties.end (); iter!=end; ++iter, ++index)
      if ((*iter)->name_hash == hash)
        return index;
        
    throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Property has not registered");
  }

///Создание нового свойства
  static stl::auto_ptr<BasicProperty> CreateProperty (const char* name, common::PropertyType type)
  {
    switch (type)
    {
      case PropertyType_String:  return stl::auto_ptr<BasicProperty> (new PropertyImpl<PropertyType_String> (name));
      case PropertyType_Integer: return stl::auto_ptr<BasicProperty> (new PropertyImpl<PropertyType_Integer> (name));
      case PropertyType_Float:   return stl::auto_ptr<BasicProperty> (new PropertyImpl<PropertyType_Float> (name));
      case PropertyType_Vector:  return stl::auto_ptr<BasicProperty> (new PropertyImpl<PropertyType_Vector> (name));
      case PropertyType_Matrix:  return stl::auto_ptr<BasicProperty> (new PropertyImpl<PropertyType_Matrix> (name));
      default:
        throw xtl::make_argument_exception ("common::PropertyMap::Impl::CreateProperty", "type", type);
    }
  }
  
///Получение либо создание свойства и возвращение его индекса
  size_t GetIndexOrCreate (const char* name, common::PropertyType type, bool& new_property_inserted)
  {
    static const char* METHOD_NAME = "common::PropertyMap::Impl::GetIndexOrCreate";
    
    new_property_inserted = false;
    
    if (!name)
      throw xtl::make_null_argument_exception (METHOD_NAME, "name");
      
      //поиск уже созданного свойства
      
    size_t hash  = common::strhash (name);
    size_t index = 0;  

    for (PropertyArray::iterator iter=properties.begin (), end=properties.end (); iter!=end; ++iter, ++index)
      if ((*iter)->name_hash == hash)
        return index;
        
      //создание нового свойства

    stl::auto_ptr<BasicProperty> new_property = CreateProperty (name, type);

    properties.push_back (new_property.get ());
    
    new_property_inserted = true;
    need_hash_update      = true;
    
    new_property.release ();

    return properties.size () - 1;
  }
  
///Обновление хэша
  void UpdateHashes ()
  {
    data_hash      = 0xFFFFFFFF;
    structure_hash = 0xFFFFFFFF;

    for (PropertyArray::iterator iter=properties.begin (), end=properties.end (); iter!=end; ++iter)
    {
      BasicProperty& property = **iter;

      data_hash      = property.GetHash (data_hash);      
      structure_hash = common::crc32 (&property.name_hash, sizeof (property.name_hash), structure_hash);
      structure_hash = common::crc32 (&property.type, sizeof (property.type), structure_hash);
    }
    
    hash             = common::crc32 (&data_hash, sizeof (data_hash), structure_hash);
    need_hash_update = false;
  }
  
///Получение связывания
  Binding& GetBinding (const char* name)
  {
      //попытка найти связывание
    
    for (BindingArray::iterator iter=bindings.begin (), end=bindings.end (); iter!=end; ++iter)
      if ((*iter)->name == name)
        return **iter;

      //создание связывания

    BindingPtr binding (new Binding (name), false);

    if (bindings.empty ())
      bindings.reserve (PROPERTY_ARRAY_RESERVE_SIZE);

    bindings.push_back (binding);
    
    bindings_structure_hash = 0;
    bindings_data_hash      = 0;

    return *binding;
  }
  
///Обновление структуры связываний
  void UpdateBindingsStructure ()
  {
    UpdateHashes ();
    
    for (BindingArray::iterator iter=bindings.begin (), end=bindings.end (); iter!=end; ++iter)
    {
      Binding& binding = **iter;

      size_t hash  = common::strhash (binding.name.c_str ());
      size_t index = 0;
      
      binding.index = UNASSIGNED_PROPERTY_INDEX;

      for (PropertyArray::iterator iter=properties.begin (), end=properties.end (); iter!=end; ++iter, ++index)
        if ((*iter)->name_hash == hash)
        {
          binding.index = index;
          break;
        }
    }

    bindings_structure_hash = structure_hash;
  }

///Установка свойства
  template <class T>
  static void SetProperty (PropertyMap& properties, const char* name, common::PropertyType type, const T& value)
  {
    bool new_property_inserted = false;    
    
    try
    {
      size_t property_index = properties.impl->GetIndexOrCreate (name, type, new_property_inserted);
        
      properties.SetProperty (property_index, value);
    }
    catch (xtl::exception& e)
    {
      e.touch ("common::PropertyMap::Impl::SetProperty");

      if (new_property_inserted)
        properties.RemoveProperty (properties.impl->properties.size () - 1);

      throw;
    }
    catch (...)
    {
      if (new_property_inserted)
        properties.RemoveProperty (properties.impl->properties.size () - 1);

      throw;
    }
  }
  
///Очистка
  void Clear ()
  {
    for (PropertyArray::iterator iter=properties.begin (), end=properties.end (); iter!=end; ++iter)
      delete *iter;

    properties.clear ();
    
    need_hash_update = true;
  }  
};

/*
    Конструкторы / деструктор
*/

PropertyMap::PropertyMap ()
  : impl (new Impl)
{
}

PropertyMap::PropertyMap (Impl* in_impl)
  : impl (in_impl)
{
}

PropertyMap::PropertyMap (const PropertyMap& properties)
  : impl (properties.impl)
{
  addref (impl);
}

PropertyMap::~PropertyMap ()
{
  release (impl);
}

PropertyMap& PropertyMap::operator = (const PropertyMap& properties)
{
  PropertyMap (properties).Swap (*this);
  return *this;
}

/*
    Копирование
*/

PropertyMap PropertyMap::Clone () const
{
  try
  {
    return PropertyMap (new Impl (*impl));
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::Clone");
    throw;
  }
}

/*
    Количество свойств
*/

size_t PropertyMap::Size () const
{
  return impl->properties.size ();
}

/*
    Хэш параметров
*/

size_t PropertyMap::Hash () const
{
  if (!impl->need_hash_update)
    return impl->hash;
    
  impl->UpdateHashes ();
  
  return impl->hash;
}

size_t PropertyMap::DataHash () const
{
  if (!impl->need_hash_update)
    return impl->data_hash;
    
  impl->UpdateHashes ();
  
  return impl->data_hash;
}

size_t PropertyMap::StructureHash () const
{
  if (!impl->need_hash_update)
    return impl->structure_hash;
    
  impl->UpdateHashes ();
  
  return impl->structure_hash;
}

/*
    Имя свойства
*/

const char* PropertyMap::PropertyName (size_t index) const
{
  if (index >= impl->properties.size ())
    throw xtl::make_range_exception ("common::PropertyMap::PropertyName", "index", index, impl->properties.size ());
    
  return impl->properties [index]->name.c_str ();
}

void PropertyMap::SetPropertyName (size_t index, const char* name)
{
  static const char* METHOD_NAME = "common::PropertyMap::SetPropertyName(size_t,const char*)";
  
    //проверка корректности аргументов
    
  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");
  
  if (index >= impl->properties.size ())
    throw xtl::make_range_exception (METHOD_NAME, "index", index, impl->properties.size ());
    
  BasicProperty& property  = *impl->properties [index];
  size_t        name_hash = common::strhash (name);
    
  for (size_t i=0, count=impl->properties.size (); i<count; i++)
    if (impl->properties [i]->name_hash == name_hash)
    {
      if (i == index)
        return; //установка того же самого имени
        
      throw xtl::format_operation_exception (METHOD_NAME, "Could not change property '%s' name to '%s'. "
        "Property with this name has already registered at index %u", property.name.c_str (), name, i);
    }
    
    //обновление имени
  
  property.name      = name;
  property.name_hash = name_hash;
  
  impl->need_hash_update = true;
}

void PropertyMap::SetPropertyName (const char* old_name, const char* new_name)
{
  try
  {
    SetPropertyName (impl->GetIndex (old_name), new_name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::SetPropertyName(const char*,const char*)");
    throw;
  }
}

/*
    Тип свойства
*/

PropertyType PropertyMap::PropertyType (size_t index) const
{
  static const char* METHOD_NAME = "common::PropertyMap::PropertyType(size_t)";
  
  if (index >= impl->properties.size ())
    throw xtl::make_range_exception (METHOD_NAME, "index", index, impl->properties.size ());
    
  return impl->properties [index]->type;
}

PropertyType PropertyMap::PropertyType (const char* name) const
{
  try
  {
    return PropertyType (impl->GetIndex (name));
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::PropertyType(const char*)");
    throw;
  }
}

/*
    Смена типа
*/

void PropertyMap::SetPropertyType (size_t index, common::PropertyType type)
{
  try
  {
      //проверка корректности аргументов и необходимости преобразования
    
    if (index >= impl->properties.size ())
      throw xtl::make_range_exception ("", "index", index, impl->properties.size ());
      
    BasicProperty& src_property = *impl->properties [index];
      
    if (src_property.type == type)
      return;
      
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
    
      //создание нового свойства      
    
    stl::auto_ptr<BasicProperty> new_property = Impl::CreateProperty (src_property.name.c_str (), type);
    
      //обновление значения    

    impl->properties [index] = new_property.get ();

    try
    {
      src_property.CopyValueTo (*this, index);      

      delete &src_property;

      new_property.release ();

      impl->need_hash_update = true;      
    }
    catch (...)
    {
      impl->properties [index] = &src_property;
      throw;
    }    
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::SetPropertyType(size_t,PropertyType)");
    throw;
  }
}

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

/*
    Поиск индекса свойства
*/

int PropertyMap::IndexOf (const char* name) const
{
  if (!name)
    return -1;
    
  size_t hash  = common::strhash (name);
  int    index = 0;  

  for (PropertyArray::iterator iter=impl->properties.begin (), end=impl->properties.end (); iter!=end; ++iter, ++index)
    if ((*iter)->name_hash == hash)
      return index;
      
  return -1;
}

bool PropertyMap::IsPresent (const char* name) const
{
  return IndexOf (name) != -1;
}

/*
    Удаление параметров
*/

void PropertyMap::RemoveProperty (const char* name)
{
  int index = IndexOf (name);

  if (index == -1)
    return;
    
  RemoveProperty ((size_t)index);
}

void PropertyMap::RemoveProperty (size_t index)
{
  if (index >= impl->properties.size ())
    return;
    
  delete impl->properties [index];
  
  impl->properties.erase (impl->properties.begin () + index);
  
  impl->need_hash_update = true;
}

void PropertyMap::Clear ()
{
  impl->Clear ();
}

/*
    Установка свойства по имени
*/

void PropertyMap::SetProperty (const char* name, const char* value)
{
  try
  {
    Impl::SetProperty (*this, name, PropertyType_String, value);
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::SetProperty(const char*,const char*)");
    throw;
  }
}

void PropertyMap::SetProperty (const char* name, const stl::string& value)
{
  try
  {
    Impl::SetProperty (*this, name, PropertyType_String, value.c_str ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::SetProperty(const char*,const stl::string&)");
    throw;
  }
}

void PropertyMap::SetProperty (const char* name, int value)
{
  try
  {
    Impl::SetProperty (*this, name, PropertyType_Integer, value);
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::SetProperty(const char*,int)");
    throw;
  }
}

void PropertyMap::SetProperty (const char* name, float value)
{
  try
  {
    Impl::SetProperty (*this, name, PropertyType_Float, value);
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::SetProperty(const char*,float)");
    throw;
  }
}

void PropertyMap::SetProperty (const char* name, const math::vec4f& value)
{
  try
  {
    Impl::SetProperty (*this, name, PropertyType_Vector, value);
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::SetProperty(const char*,const math::vec4f&)");
    throw;
  }
}

void PropertyMap::SetProperty (const char* name, const math::mat4f& value)
{
  try
  {
    Impl::SetProperty (*this, name, PropertyType_Matrix, value);
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::SetProperty(const char*,const math::mat4f&)");
    throw;
  }
}

/*
    Установка свойства по индексу
*/

void PropertyMap::SetProperty (size_t property_index, const char* value)
{
  try
  {
    if (property_index >= impl->properties.size ())
      throw xtl::make_range_exception ("", "property_index", property_index, impl->properties.size ());
      
    if (!value)
      throw xtl::make_null_argument_exception ("", "value");
      
    BasicProperty& property = *impl->properties [property_index]; 
    
    switch (property.type)
    {
      case PropertyType_String:
        read_value_to_property<PropertyType_String> (value, property);
        break;
      case PropertyType_Integer:
        read_value_to_property<PropertyType_Integer> (value, property);
        break;
      case PropertyType_Float:
        read_value_to_property<PropertyType_Float> (value, property);
        break;
      case PropertyType_Vector:      
        read_value_to_property<PropertyType_Vector> (value, property);        
        break;
      case PropertyType_Matrix:
        read_value_to_property<PropertyType_Matrix> (value, property);
        break;
      default:
        throw xtl::format_operation_exception ("", "Internal error: wrong property '%s' type %d", property.name.c_str (), property.type);        
    }
    
    impl->need_hash_update = true;
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::SetProperty(size_t,const char*)");
    throw;
  }
}

void PropertyMap::SetProperty (size_t property_index, const stl::string& value)
{
  try
  {
    SetProperty (property_index, value.c_str ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::SetProperty(size_t,const stl::string&)");
    throw;
  }
}

void PropertyMap::SetProperty (size_t property_index, int value)
{
  try
  {
    if (property_index >= impl->properties.size ())
      throw xtl::make_range_exception ("", "property_index", property_index, impl->properties.size ());
      
    BasicProperty& property = *impl->properties [property_index];
    
    switch (property.type)
    {
      case PropertyType_String:
        xtl::to_string (static_cast<PropertyImpl<PropertyType_String>&> (property).value, value);
        break;
      case PropertyType_Integer:
        static_cast<PropertyImpl<PropertyType_Integer>&> (property).value = value;
        break;
      case PropertyType_Float:
        static_cast<PropertyImpl<PropertyType_Float>&> (property).value = static_cast<float> (value);
        break;
      case PropertyType_Vector:
        static_cast<PropertyImpl<PropertyType_Vector>&> (property).value = static_cast<float> (value);
        break;
      case PropertyType_Matrix:
        static_cast<PropertyImpl<PropertyType_Matrix>&> (property).value = static_cast<float> (value);
        break;
      default:
        throw xtl::format_operation_exception ("", "Internal error: wrong property '%s' type %d",  property.name.c_str (), property.type);        
    }
    
    impl->need_hash_update = true;
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::SetProperty(size_t,int)");
    throw;
  }
}

void PropertyMap::SetProperty (size_t property_index, float value)
{
  try
  {
    if (property_index >= impl->properties.size ())
      throw xtl::make_range_exception ("", "property_index", property_index, impl->properties.size ());
      
    BasicProperty& property = *impl->properties [property_index];
    
    switch (property.type)
    {
      case PropertyType_String:
        xtl::to_string (static_cast<PropertyImpl<PropertyType_String>&> (property).value, value);
        break;
      case PropertyType_Integer:
        static_cast<PropertyImpl<PropertyType_Integer>&> (property).value = static_cast<int> (value);
        break;
      case PropertyType_Float:
        static_cast<PropertyImpl<PropertyType_Float>&> (property).value = value;
        break;
      case PropertyType_Vector:
        static_cast<PropertyImpl<PropertyType_Vector>&> (property).value = value;
        break;
      case PropertyType_Matrix:
        static_cast<PropertyImpl<PropertyType_Matrix>&> (property).value = value;
        break;
      default:
        throw xtl::format_operation_exception ("", "Internal error: wrong property '%s' type %d",  property.name.c_str (), property.type);        
    }
    
    impl->need_hash_update = true;
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::SetProperty(size_t,float)");
    throw;
  }
}

void PropertyMap::SetProperty (size_t property_index, const math::vec4f& value)
{
  try
  {
    if (property_index >= impl->properties.size ())
      throw xtl::make_range_exception ("", "property_index", property_index, impl->properties.size ());
      
    BasicProperty& property = *impl->properties [property_index];
    
    switch (property.type)
    {
      case PropertyType_String:
        xtl::to_string (static_cast<PropertyImpl<PropertyType_String>&> (property).value, value);
        break;
      case PropertyType_Vector:
        static_cast<PropertyImpl<PropertyType_Vector>&> (property).value = value;
        break;
      case PropertyType_Integer:
      case PropertyType_Float:
      case PropertyType_Matrix:
        throw xtl::format_operation_exception ("", "Could not convert property '%s' from %s to %s", property.name.c_str (), get_name (PropertyType_Vector), get_name (property.type));
      default:
        throw xtl::format_operation_exception ("", "Internal error: wrong property '%s' type %d", property.name.c_str (), property.type);        
    }
    
    impl->need_hash_update = true;
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::SetProperty(size_t,const math::vec4f&)");
    throw;
  }
}

void PropertyMap::SetProperty (size_t property_index, const math::mat4f& value)
{
  try
  {
    if (property_index >= impl->properties.size ())
      throw xtl::make_range_exception ("", "property_index", property_index, impl->properties.size ());
      
    BasicProperty& property = *impl->properties [property_index];
    
    switch (property.type)
    {
      case PropertyType_String:
        xtl::to_string (static_cast<PropertyImpl<PropertyType_String>&> (property).value, value);
        break;
      case PropertyType_Matrix:
        static_cast<PropertyImpl<PropertyType_Matrix>&> (property).value = value;
        break;
      case PropertyType_Integer:        
      case PropertyType_Float:        
      case PropertyType_Vector:
        throw xtl::format_operation_exception ("", "Could not convert property '%s' from %s to %s", property.name.c_str (), get_name (PropertyType_Matrix), get_name (property.type));
      default:
        throw xtl::format_operation_exception ("", "Internal error: wrong property '%s' type %d", property.name.c_str (), property.type);        
    }
    
    impl->need_hash_update = true;
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::SetProperty(size_t,const math::mat4f&)");
    throw;
  }
}

/*
    Чтение свойства по имени
*/

void PropertyMap::GetProperty (const char* name, stl::string& result) const
{
  try
  {
    GetProperty (impl->GetIndex (name), result);
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::GetProperty(const char*,stl::string&)");
    throw;
  }
}

void PropertyMap::GetProperty (const char* name, int& result) const
{
  try
  {
    GetProperty (impl->GetIndex (name), result);
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::GetProperty(const char*,int&)");
    throw;
  }
}

void PropertyMap::GetProperty (const char* name, float& result) const
{
  try
  {
    GetProperty (impl->GetIndex (name), result);
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::GetProperty(const char*,float&)");
    throw;
  }
}

void PropertyMap::GetProperty (const char* name, math::vec4f& result) const
{
  try
  {
    GetProperty (impl->GetIndex (name), result);
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::GetProperty(const char*,const math::vec4f&)");
    throw;
  }
}

void PropertyMap::GetProperty (const char* name, math::mat4f& result) const
{
  try
  {
    GetProperty (impl->GetIndex (name), result);
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::GetProperty(const char*,const math::mat4f&)");
    throw;
  }
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

int PropertyMap::GetInteger (const char* name) const
{
  try
  {
    return GetInteger (impl->GetIndex (name));
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::GetInteger(const char*)");
    throw;
  }
}

float PropertyMap::GetFloat (const char* name) const
{
  try
  {
    return GetFloat (impl->GetIndex (name));
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::GetFloat(const char*)");
    throw;
  }
}

const math::vec4f& PropertyMap::GetVector (const char* name) const
{
  try
  {
    return GetVector (impl->GetIndex (name));
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::GetVector(const char*)");
    throw;
  }
}

const math::mat4f& PropertyMap::GetMatrix (const char* name) const
{
  try
  {
    return GetMatrix (impl->GetIndex (name));
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::GetMatrix(const char*)");
    throw;
  }
}

/*
    Чтение свойства по индексу
*/

void PropertyMap::GetProperty (size_t property_index, stl::string& result) const
{
  try
  {    
    if (property_index >= impl->properties.size ())
      throw xtl::make_range_exception ("", "property_index", property_index, impl->properties.size ());
      
    BasicProperty& property = *impl->properties [property_index];
    
    switch (property.type)
    {
      case PropertyType_String:
        result = static_cast<PropertyImpl<PropertyType_String>&> (property).value;
        break;
      case PropertyType_Integer:
        xtl::to_string (result, static_cast<PropertyImpl<PropertyType_Integer>&> (property).value);
        break;
      case PropertyType_Float:      
        xtl::to_string (result, static_cast<PropertyImpl<PropertyType_Float>&> (property).value);        
        break;
      case PropertyType_Vector:    
        xtl::to_string (result, static_cast<PropertyImpl<PropertyType_Vector>&> (property).value);
        break;    
      case PropertyType_Matrix:    
        xtl::to_string (result, static_cast<PropertyImpl<PropertyType_Matrix>&> (property).value);
        break;    
      default:
        throw xtl::format_operation_exception ("", "Internal error: wrong property '%s' type %d", property.name.c_str (), property.type);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::GetProperty(size_t,stl::string&)");
    throw;
  }
}

void PropertyMap::GetProperty (size_t property_index, int& result) const
{
  try
  {
    result = GetInteger (property_index);
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::GetProperty(size_t,int&)");
    throw;
  }
}

void PropertyMap::GetProperty (size_t property_index, float& result) const
{
  try
  {
    result = GetFloat (property_index);
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::GetProperty(size_t,float&)");
    throw;
  }
}

void PropertyMap::GetProperty (size_t property_index, math::vec4f& result) const
{
  try
  {
    if (property_index >= impl->properties.size ())
      throw xtl::make_range_exception ("", "property_index", property_index, impl->properties.size ());
      
    BasicProperty& property = *impl->properties [property_index];
    
    switch (property.type)
    {
      case PropertyType_String:
        read_value_from_property<PropertyType_Vector> (static_cast<PropertyImpl<PropertyType_String>&> (property), result);
        break;
      case PropertyType_Integer:
        result = static_cast<float> (static_cast<PropertyImpl<PropertyType_Integer>&> (property).value);
        break;
      case PropertyType_Float:
        result = static_cast<PropertyImpl<PropertyType_Float>&> (property).value;
        break;
      case PropertyType_Vector:    
        result = static_cast<PropertyImpl<PropertyType_Vector>&> (property).value;
        break;
      case PropertyType_Matrix:
        throw xtl::format_operation_exception ("", "Could not convert property '%s' from %s to %s", property.name.c_str (), get_name (property.type), get_name (PropertyType_Vector));
      default:
        throw xtl::format_operation_exception ("", "Internal error: wrong property '%s' type %d", property.name.c_str (), property.type);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::GetProperty(size_t,math::vec4f&)");
    throw;
  }
}

void PropertyMap::GetProperty (size_t property_index, math::mat4f& result) const
{
  try
  {
    if (property_index >= impl->properties.size ())
      throw xtl::make_range_exception ("", "property_index", property_index, impl->properties.size ());
      
    BasicProperty& property = *impl->properties [property_index];
    
    switch (property.type)
    {
      case PropertyType_String:
        read_value_from_property<PropertyType_Matrix> (static_cast<PropertyImpl<PropertyType_String>&> (property), result);
        break;
      case PropertyType_Integer:
        result = static_cast<float> (static_cast<PropertyImpl<PropertyType_Integer>&> (property).value);
        break;
      case PropertyType_Float:
        result = static_cast<PropertyImpl<PropertyType_Float>&> (property).value;
        break;
      case PropertyType_Matrix:
        result = static_cast<PropertyImpl<PropertyType_Matrix>&> (property).value;
        break;
      case PropertyType_Vector:
        throw xtl::format_operation_exception ("", "Could not convert property '%s' from '%s' to %s", property.name.c_str (), get_name (property.type), get_name (PropertyType_Matrix));
      default:
        throw xtl::format_operation_exception ("", "Internal error: wrong property '%s' type %d", property.name.c_str (), property.type);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::GetProperty(size_t,math::mat4f&)");
    throw;
  }
}

const char* PropertyMap::GetString (size_t property_index) const
{
  static const char* METHOD_NAME = "common::PropertyMap::GetString(size_t)";

  if (property_index >= impl->properties.size ())
    throw xtl::make_range_exception (METHOD_NAME, "property_index", property_index, impl->properties.size ());
    
  BasicProperty& property = *impl->properties [property_index];
  
  switch (property.type)
  {
    case PropertyType_String:
      return static_cast<PropertyImpl<PropertyType_String>&> (property).value.c_str ();
    case PropertyType_Integer:
    case PropertyType_Float:
    case PropertyType_Vector:
    case PropertyType_Matrix:
      throw xtl::format_operation_exception (METHOD_NAME, "Could not convert property '%s' from %s to %s", property.name.c_str (), get_name (property.type), get_name (PropertyType_String));
    default:
      throw xtl::format_operation_exception (METHOD_NAME, "Internal error: wrong property '%s' type %d", property.name.c_str (), property.type);
  }
}

int PropertyMap::GetInteger (size_t property_index) const
{
  try
  {
    if (property_index >= impl->properties.size ())
      throw xtl::make_range_exception ("", "property_index", property_index, impl->properties.size ());

    BasicProperty& property = *impl->properties [property_index];

    switch (property.type)
    {
      case PropertyType_String:
      {
        int result = 0;

        read_value_from_property<PropertyType_Integer> (static_cast<PropertyImpl<PropertyType_String>&> (property), result);

        return result;
      }
      case PropertyType_Integer:
        return static_cast<PropertyImpl<PropertyType_Integer>&> (property).value;
      case PropertyType_Float:
        return static_cast<int> (static_cast<PropertyImpl<PropertyType_Float>&> (property).value);
      case PropertyType_Vector:
      case PropertyType_Matrix:
        throw xtl::format_operation_exception ("", "Could not convert property '%s' from %s to %s", property.name.c_str (), get_name (property.type), get_name (PropertyType_Integer));
      default:
        throw xtl::format_operation_exception ("", "Internal error: wrong property '%s' type %d", property.name.c_str (), property.type);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::GetInteger(size_t)");
    throw;
  }
}

float PropertyMap::GetFloat (size_t property_index) const
{
  try
  {
    if (property_index >= impl->properties.size ())
      throw xtl::make_range_exception ("", "property_index", property_index, impl->properties.size ());

    BasicProperty& property = *impl->properties [property_index];

    switch (property.type)
    {
      case PropertyType_String:
      {
        float result = 0.0f;

        read_value_from_property<PropertyType_Float> (static_cast<PropertyImpl<PropertyType_String>&> (property), result);

        return result;
      }
      case PropertyType_Integer:
        return static_cast<float> (static_cast<PropertyImpl<PropertyType_Integer>&> (property).value);
      case PropertyType_Float:
        return static_cast<PropertyImpl<PropertyType_Float>&> (property).value;
      case PropertyType_Vector:
      case PropertyType_Matrix:
        throw xtl::format_operation_exception ("", "Could not convert property '%s' from %s to %s", property.name.c_str (), get_name (property.type), get_name (PropertyType_Float));
      default:
        throw xtl::format_operation_exception ("", "Internal error: wrong property '%s' type %d", property.name.c_str (), property.type);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::GetFloat(size_t)");
    throw;
  }
}

const math::vec4f& PropertyMap::GetVector (size_t property_index) const
{
  static const char* METHOD_NAME = "common::PropertyMap::GetVector(size_t)";

  if (property_index >= impl->properties.size ())
    throw xtl::make_range_exception (METHOD_NAME, "property_index", property_index, impl->properties.size ());

  BasicProperty& property = *impl->properties [property_index];

  switch (property.type)
  {
    case PropertyType_Vector:  
      return static_cast<PropertyImpl<PropertyType_Vector>&> (property).value;
    case PropertyType_String:      
    case PropertyType_Integer:      
    case PropertyType_Float:          
    case PropertyType_Matrix:
      throw xtl::format_operation_exception (METHOD_NAME, "Could not convert property '%s' from %s to %s", property.name.c_str (), get_name (property.type), get_name (PropertyType_Vector));
    default:
      throw xtl::format_operation_exception (METHOD_NAME, "Internal error: wrong property '%s' type %d", property.name.c_str (), property.type);
  }
}

const math::mat4f& PropertyMap::GetMatrix (size_t property_index) const
{
  static const char* METHOD_NAME = "common::PropertyMap::GetMatrix(size_t)";

  if (property_index >= impl->properties.size ())
    throw xtl::make_range_exception (METHOD_NAME, "property_index", property_index, impl->properties.size ());

  BasicProperty& property = *impl->properties [property_index];

  switch (property.type)
  {
    case PropertyType_Matrix:  
      return static_cast<PropertyImpl<PropertyType_Matrix>&> (property).value;
    case PropertyType_String:      
    case PropertyType_Integer:      
    case PropertyType_Float:          
    case PropertyType_Vector:
      throw xtl::format_operation_exception (METHOD_NAME, "Could not convert property '%s' from %s to %s", property.name.c_str (), get_name (property.type), get_name (PropertyType_Matrix));
    default:
      throw xtl::format_operation_exception (METHOD_NAME, "Internal error: wrong property '%s' type %d", property.name.c_str (), property.type);
  }
}

/*
    Связывание свойств
*/

void PropertyMap::BindProperty (const char* name, IPropertyGetter* in_getter)
{
  stl::auto_ptr<IPropertyGetter> getter (in_getter);
  
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");

    if (!getter)
      throw xtl::make_null_argument_exception ("", "getter");
      
    impl->GetBinding (name).getter = getter;
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::BindProperty(const char*, IPropertyGetter*)");
    throw;
  }
}

void PropertyMap::BindProperty (const char* name, IPropertySetter* in_setter)
{
  stl::auto_ptr<IPropertySetter> setter (in_setter);
  
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");

    if (!setter)
      throw xtl::make_null_argument_exception ("", "setter");
      
    impl->GetBinding (name).setter = setter;
    
    impl->bindings_data_hash = 0;      
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::BindProperty(const char*, IPropertySetter*)");
    throw;
  }
}

void PropertyMap::UnbindProperty (const char* name)
{
  if (!name)
    return;
    
  for (BindingArray::iterator iter=impl->bindings.begin (), end=impl->bindings.end (); iter!=end; ++iter)
  {
    impl->bindings.erase (iter);
    return;
  }
}

void PropertyMap::UnbindAllProperties ()
{
  impl->bindings.clear ();
}

/*
    Обновление связываний
*/

void PropertyMap::UpdateBindings ()
{
  try
  {
    if (impl->bindings_structure_hash != StructureHash ())
      impl->UpdateBindingsStructure ();

    for (BindingArray::iterator iter=impl->bindings.begin (), end=impl->bindings.end (); iter!=end; ++iter)
    {
      Binding& binding = **iter;
      
      if (!binding.getter)
        continue;
        
      if (binding.index != UNASSIGNED_PROPERTY_INDEX)
      {
        binding.getter->Update (*this, binding.index);
      }
      else
      {
        binding.getter->Update (*this, binding.name.c_str ());
      }
    }
    
    impl->bindings_data_hash = Hash ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::UpdateBindings");
    throw;
  }
}

void PropertyMap::CommitBindings () const
{
  try
  {
    if (impl->bindings_data_hash == Hash ())
      return;
      
    if (impl->bindings_structure_hash != StructureHash ())
      impl->UpdateBindingsStructure ();
      
    for (BindingArray::iterator iter=impl->bindings.begin (), end=impl->bindings.end (); iter!=end; ++iter)
    {
      Binding& binding = **iter;
      
      if (!binding.setter || binding.index == UNASSIGNED_PROPERTY_INDEX)
        continue;

      binding.setter->Update (*this, binding.index);
    }
      
    impl->bindings_data_hash = Hash ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyMap::CommitBindings");
    throw;
  }
}

/*
    Обмен
*/

void PropertyMap::Swap (PropertyMap& properties)
{
  stl::swap (impl, properties.impl);
}

namespace common
{

void swap (PropertyMap& properties1, PropertyMap& properties2)
{
  properties1.Swap (properties2);
}

}

/*
    Получение имени типа
*/

namespace common
{

const char* get_name (PropertyType type)
{
  switch (type)
  {
    case PropertyType_String:  return "string";
    case PropertyType_Integer: return "integer";
    case PropertyType_Float:   return "float";
    case PropertyType_Vector:  return "vector";
    case PropertyType_Matrix:  return "matrix";
    default:
      throw xtl::make_argument_exception ("common::get_name(PropertyType)", "type", type);
  }
}

}
