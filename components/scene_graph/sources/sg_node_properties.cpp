#include "shared.h"

using namespace scene_graph;

namespace
{

/*
    Константы
*/

const size_t PROPERTY_ARRAY_RESERVE_SIZE = 8; //резервируемое количество свойств

/*
    Свойство узла
*/

struct NodeProperty
{
  stl::string      name;      //имя свойства
  size_t           name_hash; //хэш имени
  NodePropertyType type;      //тип свойства
  
///Конструктор
  NodeProperty (const char* in_name, NodePropertyType in_type)
    : name (in_name)
    , name_hash (common::strhash (in_name))
    , type (in_type)
  {
  }
  
  virtual ~NodeProperty () {}
  
///Копирование
  virtual NodeProperty* Clone () = 0;
  
///Хэш данных
  virtual size_t GetHash (size_t current_hash) = 0;
  
///Установка значения в список свойств узла
  virtual void CopyValueTo (NodeProperties& map, size_t property_index) = 0;
};

/*
    Реализация свойств для различных типов
*/

template <NodePropertyType type>
struct NodePropertyTypeMap;

template <> struct NodePropertyTypeMap<NodePropertyType_String>  { typedef stl::string type; };
template <> struct NodePropertyTypeMap<NodePropertyType_Integer> { typedef int         type; };
template <> struct NodePropertyTypeMap<NodePropertyType_Float>   { typedef float       type; };
template <> struct NodePropertyTypeMap<NodePropertyType_Vector>  { typedef math::vec4f type; };
template <> struct NodePropertyTypeMap<NodePropertyType_Matrix>  { typedef math::mat4f type; };

template <class T>
size_t get_value_hash (const T& value, size_t current_hash)
{
  return common::crc32 (&value, sizeof (T), current_hash);
}

size_t get_value_hash (const stl::string& value, size_t current_hash)
{
  return common::strhash (value.c_str (), current_hash);
}

template <NodePropertyType node_type> struct NodePropertyImpl: public NodeProperty
{
  typedef typename NodePropertyTypeMap<node_type>::type value_type;

  value_type value; //значение

///Конструктор
  NodePropertyImpl (const char* name)
    : NodeProperty (name, node_type)
    , value ()
  {    
  }
  
  NodePropertyImpl (const NodePropertyImpl& property)
    : NodeProperty (property)
    , value (property.value)
  {
  }
  
///Копирование
  NodeProperty* Clone ()
  {    
    return new NodePropertyImpl (*this);    
  }
  
///Хэш данных
  size_t GetHash (size_t current_hash)
  {
    return get_value_hash (value, current_hash);
  }
  
///Установка значения в список свойств узла
  void CopyValueTo (NodeProperties& map, size_t property_index)
  {  
    map.SetProperty (property_index, value);    
  }
};

typedef stl::vector<NodeProperty*> PropertyArray;

///чтение значения из строки
template <NodePropertyType type>
void read_value_to_property (const char* string, NodeProperty& property)
{
  typedef typename NodePropertyTypeMap<type>::type result_type;   

  if (!xtl::io::read_and_cast<result_type> (string, static_cast<NodePropertyImpl<type>&> (property).value))    
    throw xtl::format_operation_exception ("scene_graph::read_value_to_property", "Could not convert property '%s' from %s='%s' to %s",
      property.name.c_str (), get_name (NodePropertyType_String), string, get_name (type));  
}

template <> void read_value_to_property<NodePropertyType_String> (const char* string, NodeProperty& property)
{
  static_cast<NodePropertyImpl<NodePropertyType_String>&> (property).value = string;
}

///чтение значения из строки
template <NodePropertyType dst_type>
void read_value_from_property (const NodePropertyImpl<NodePropertyType_String>& property, typename NodePropertyTypeMap<dst_type>::type& result)
{
  if (!xtl::io::read_and_cast<typename NodePropertyTypeMap<dst_type>::type> (property.value.c_str (), result))
    throw xtl::format_operation_exception ("scene_graph::read_value_from_property", "Could not convert property '%s' from %s='%s' to %s",
      property.name.c_str (), get_name (property.type), property.value.c_str (), get_name (dst_type));
}

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

struct NodeProperties::Impl
{
  xtl::reference_counter ref_count;        //счётчик ссылок
  bool                   need_hash_update; //необходимо ли обновить хэш
  size_t                 hash;             //хэш параметров (композиция хэша данных и хэша структуры)
  size_t                 data_hash;        //хэш данных
  size_t                 structure_hash;   //хэш структуры
  PropertyArray          properties;       //свойства

///Конструкторы
  Impl ()  
    : need_hash_update (true)
    , hash (0)
    , data_hash (0)
    , structure_hash (0)
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
        NodeProperty&               src_property = **iter;
        stl::auto_ptr<NodeProperty> new_property (src_property.Clone ());
        
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
  
///Получение индекса свойства
  size_t GetIndex (const char* name)
  {
    static const char* METHOD_NAME = "scene_graph::NodeProperties::Impl::GetIndex";
    
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
  static stl::auto_ptr<NodeProperty> CreateProperty (const char* name, NodePropertyType type)
  {
    switch (type)
    {
      case NodePropertyType_String:  return stl::auto_ptr<NodeProperty> (new NodePropertyImpl<NodePropertyType_String> (name));
      case NodePropertyType_Integer: return stl::auto_ptr<NodeProperty> (new NodePropertyImpl<NodePropertyType_Integer> (name));
      case NodePropertyType_Float:   return stl::auto_ptr<NodeProperty> (new NodePropertyImpl<NodePropertyType_Float> (name));
      case NodePropertyType_Vector:  return stl::auto_ptr<NodeProperty> (new NodePropertyImpl<NodePropertyType_Vector> (name));
      case NodePropertyType_Matrix:  return stl::auto_ptr<NodeProperty> (new NodePropertyImpl<NodePropertyType_Matrix> (name));
      default:
        throw xtl::make_argument_exception ("scene_graph::NodeProperties::Impl::CreateProperty", "type", type);
    }
  }
  
///Получение либо создание свойства и возвращение его индекса
  size_t GetIndexOrCreate (const char* name, NodePropertyType type, bool& new_property_inserted)
  {
    static const char* METHOD_NAME = "scene_graph::NodeProperties::Impl::GetIndexOrCreate";
    
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

    stl::auto_ptr<NodeProperty> new_property = CreateProperty (name, type);

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
      NodeProperty& property = **iter;

      data_hash      = property.GetHash (data_hash);      
      structure_hash = common::crc32 (&property.name_hash, sizeof (property.name_hash), structure_hash);
      structure_hash = common::crc32 (&property.type, sizeof (property.type), structure_hash);
    }
    
    hash             = common::crc32 (&data_hash, sizeof (data_hash), structure_hash);
    need_hash_update = false;
  }
  
///Установка свойства
  template <class T>
  static void SetProperty (NodeProperties& properties, const char* name, NodePropertyType type, const T& value)
  {
    bool new_property_inserted = false;    
    
    try
    {
      size_t property_index = properties.impl->GetIndexOrCreate (name, type, new_property_inserted);
        
      properties.SetProperty (property_index, value);
    }
    catch (xtl::exception& e)
    {
      e.touch ("scene_graph::NodeProperties::Impl::SetProperty");

      if (new_property_inserted)
        properties.Remove (properties.impl->properties.size () - 1);

      throw;
    }
    catch (...)
    {
      if (new_property_inserted)
        properties.Remove (properties.impl->properties.size () - 1);

      throw;
    }
  }
};

/*
    Конструкторы / деструктор
*/

NodeProperties::NodeProperties ()
  : impl (new Impl)
{
}

NodeProperties::NodeProperties (const NodeProperties& properties)
  : impl (new Impl (*properties.impl))
{
}

NodeProperties::~NodeProperties ()
{
  Clear ();

  delete impl;
}

/*
    Создание свойств узла
*/

NodeProperties::Pointer NodeProperties::Create ()
{
  return Pointer (new NodeProperties, false);
}

/*
    Копирование
*/

NodeProperties::Pointer NodeProperties::Clone () const
{
  try
  {
    return Pointer (new NodeProperties (*this), false);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::NodeProperties::Clone");
    throw;
  }
}

/*
    Количество свойств
*/

size_t NodeProperties::Size () const
{
  return impl->properties.size ();
}

/*
    Хэш параметров
*/

size_t NodeProperties::Hash () const
{
  if (!impl->need_hash_update)
    return impl->hash;
    
  impl->UpdateHashes ();
  
  return impl->hash;
}

size_t NodeProperties::DataHash () const
{
  if (!impl->need_hash_update)
    return impl->data_hash;
    
  impl->UpdateHashes ();
  
  return impl->data_hash;
}

size_t NodeProperties::StructureHash () const
{
  if (!impl->need_hash_update)
    return impl->structure_hash;
    
  impl->UpdateHashes ();
  
  return impl->structure_hash;
}

/*
    Имя свойства
*/

const char* NodeProperties::PropertyName (size_t index) const
{
  if (index >= impl->properties.size ())
    throw xtl::make_range_exception ("scene_graph::NodeProperties::PropertyName", "index", index, impl->properties.size ());
    
  return impl->properties [index]->name.c_str ();
}

void NodeProperties::SetPropertyName (size_t index, const char* name)
{
  static const char* METHOD_NAME = "scene_graph::NodeProperties::SetPropertyName(size_t,const char*)";
  
    //проверка корректности аргументов
    
  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");
  
  if (index >= impl->properties.size ())
    throw xtl::make_range_exception (METHOD_NAME, "index", index, impl->properties.size ());
    
  NodeProperty& property  = *impl->properties [index];
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

void NodeProperties::SetPropertyName (const char* old_name, const char* new_name)
{
  try
  {
    SetPropertyName (impl->GetIndex (old_name), new_name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::NodeProperties::SetPropertyName(const char*,const char*)");
    throw;
  }
}

/*
    Тип свойства
*/

NodePropertyType NodeProperties::PropertyType (size_t index) const
{
  static const char* METHOD_NAME = "scene_graph::NodeProperties::PropertyType(size_t)";
  
  if (index >= impl->properties.size ())
    throw xtl::make_range_exception (METHOD_NAME, "index", index, impl->properties.size ());
    
  return impl->properties [index]->type;
}

NodePropertyType NodeProperties::PropertyType (const char* name) const
{
  try
  {
    return PropertyType (impl->GetIndex (name));
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::NodeProperties::PropertyType(const char*)");
    throw;
  }
}

/*
    Смена типа
*/

void NodeProperties::SetPropertyType (size_t index, NodePropertyType type)
{
  try
  {
      //проверка корректности аргументов и необходимости преобразования
    
    if (index >= impl->properties.size ())
      throw xtl::make_range_exception ("", "index", index, impl->properties.size ());
      
    NodeProperty& src_property = *impl->properties [index];
      
    if (src_property.type == type)
      return;
      
    switch (type)
    {
      case NodePropertyType_String:
      case NodePropertyType_Integer:
      case NodePropertyType_Float:
      case NodePropertyType_Vector:
      case NodePropertyType_Matrix:
        break;
      default:
        throw xtl::make_argument_exception ("", "type", type);
    }
    
      //создание нового свойства      
    
    stl::auto_ptr<NodeProperty> new_property = Impl::CreateProperty (src_property.name.c_str (), type);
    
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
    e.touch ("scene_graph::NodeProperties::SetPropertyType(size_t,NodePropertyType)");
    throw;
  }
}

void NodeProperties::SetPropertyType (const char* name, NodePropertyType type)
{
  try
  {
    SetPropertyType (impl->GetIndex (name), type);
  } 
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::NodeProperties::SetPropertyType(const char*,NodePropertyType)");
    throw;
  }
}

/*
    Поиск индекса свойства
*/

int NodeProperties::IndexOf (const char* name) const
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

bool NodeProperties::IsPresent (const char* name) const
{
  return IndexOf (name) != -1;
}

/*
    Удаление параметров
*/

void NodeProperties::Remove (const char* name)
{
  int index = IndexOf (name);

  if (index == -1)
    return;
    
  Remove ((size_t)index);
}

void NodeProperties::Remove (size_t index)
{
  if (index >= impl->properties.size ())
    return;
    
  delete impl->properties [index];
  
  impl->properties.erase (impl->properties.begin () + index);
  
  impl->need_hash_update = true;
}

void NodeProperties::Clear ()
{
  for (PropertyArray::iterator iter=impl->properties.begin (), end=impl->properties.end (); iter!=end; ++iter)
    delete *iter;

  impl->properties.clear ();
  
  impl->need_hash_update = true;
}

/*
    Подсчёт ссылок
*/

void NodeProperties::AddRef () const
{
  impl->ref_count.increment ();
}

void NodeProperties::Release () const
{
  if (impl->ref_count.decrement ())
    delete this;
}

size_t NodeProperties::UseCount () const
{
  return impl->ref_count.use_count ();
}

/*
    Установка свойства по имени
*/

void NodeProperties::SetProperty (const char* name, const char* value)
{
  try
  {
    Impl::SetProperty (*this, name, NodePropertyType_String, value);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::NodeProperties::SetProperty(const char*,const char*)");
    throw;
  }
}

void NodeProperties::SetProperty (const char* name, const stl::string& value)
{
  try
  {
    Impl::SetProperty (*this, name, NodePropertyType_String, value.c_str ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::NodeProperties::SetProperty(const char*,const stl::string&)");
    throw;
  }
}

void NodeProperties::SetProperty (const char* name, int value)
{
  try
  {
    Impl::SetProperty (*this, name, NodePropertyType_Integer, value);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::NodeProperties::SetProperty(const char*,int)");
    throw;
  }
}

void NodeProperties::SetProperty (const char* name, float value)
{
  try
  {
    Impl::SetProperty (*this, name, NodePropertyType_Float, value);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::NodeProperties::SetProperty(const char*,float)");
    throw;
  }
}

void NodeProperties::SetProperty (const char* name, const math::vec4f& value)
{
  try
  {
    Impl::SetProperty (*this, name, NodePropertyType_Vector, value);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::NodeProperties::SetProperty(const char*,const math::vec4f&)");
    throw;
  }
}

void NodeProperties::SetProperty (const char* name, const math::mat4f& value)
{
  try
  {
    Impl::SetProperty (*this, name, NodePropertyType_Matrix, value);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::NodeProperties::SetProperty(const char*,const math::mat4f&)");
    throw;
  }
}

/*
    Установка свойства по индексу
*/

void NodeProperties::SetProperty (size_t property_index, const char* value)
{
  try
  {
    if (property_index >= impl->properties.size ())
      throw xtl::make_range_exception ("", "property_index", property_index, impl->properties.size ());
      
    if (!value)
      throw xtl::make_null_argument_exception ("", "value");
      
    NodeProperty& property = *impl->properties [property_index]; 
    
    switch (property.type)
    {
      case NodePropertyType_String:
        read_value_to_property<NodePropertyType_String> (value, property);
        break;
      case NodePropertyType_Integer:
        read_value_to_property<NodePropertyType_Integer> (value, property);
        break;
      case NodePropertyType_Float:
        read_value_to_property<NodePropertyType_Float> (value, property);
        break;
      case NodePropertyType_Vector:      
        read_value_to_property<NodePropertyType_Vector> (value, property);        
        break;
      case NodePropertyType_Matrix:
        read_value_to_property<NodePropertyType_Matrix> (value, property);
        break;
      default:
        throw xtl::format_operation_exception ("", "Internal error: wrong property '%s' type %d", property.name.c_str (), property.type);        
    }
    
    impl->need_hash_update = true;
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::NodeProperties::SetProperty(size_t,const char*)");
    throw;
  }
}

void NodeProperties::SetProperty (size_t property_index, const stl::string& value)
{
  try
  {
    SetProperty (property_index, value.c_str ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::NodeProperties::SetProperty(size_t,const stl::string&)");
    throw;
  }
}

void NodeProperties::SetProperty (size_t property_index, int value)
{
  try
  {
    if (property_index >= impl->properties.size ())
      throw xtl::make_range_exception ("", "property_index", property_index, impl->properties.size ());
      
    NodeProperty& property = *impl->properties [property_index];
    
    switch (property.type)
    {
      case NodePropertyType_String:
        xtl::to_string (static_cast<NodePropertyImpl<NodePropertyType_String>&> (property).value, value);
        break;
      case NodePropertyType_Integer:
        static_cast<NodePropertyImpl<NodePropertyType_Integer>&> (property).value = value;
        break;
      case NodePropertyType_Float:
        static_cast<NodePropertyImpl<NodePropertyType_Float>&> (property).value = static_cast<float> (value);
        break;
      case NodePropertyType_Vector:
        static_cast<NodePropertyImpl<NodePropertyType_Vector>&> (property).value = static_cast<float> (value);
        break;
      case NodePropertyType_Matrix:
        static_cast<NodePropertyImpl<NodePropertyType_Matrix>&> (property).value = static_cast<float> (value);
        break;
      default:
        throw xtl::format_operation_exception ("", "Internal error: wrong property '%s' type %d",  property.name.c_str (), property.type);        
    }
    
    impl->need_hash_update = true;
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::NodeProperties::SetProperty(size_t,int)");
    throw;
  }
}

void NodeProperties::SetProperty (size_t property_index, float value)
{
  try
  {
    if (property_index >= impl->properties.size ())
      throw xtl::make_range_exception ("", "property_index", property_index, impl->properties.size ());
      
    NodeProperty& property = *impl->properties [property_index];
    
    switch (property.type)
    {
      case NodePropertyType_String:
        xtl::to_string (static_cast<NodePropertyImpl<NodePropertyType_String>&> (property).value, value);
        break;
      case NodePropertyType_Integer:
        static_cast<NodePropertyImpl<NodePropertyType_Integer>&> (property).value = static_cast<int> (value);
        break;
      case NodePropertyType_Float:
        static_cast<NodePropertyImpl<NodePropertyType_Float>&> (property).value = value;
        break;
      case NodePropertyType_Vector:
        static_cast<NodePropertyImpl<NodePropertyType_Vector>&> (property).value = value;
        break;
      case NodePropertyType_Matrix:
        static_cast<NodePropertyImpl<NodePropertyType_Matrix>&> (property).value = value;
        break;
      default:
        throw xtl::format_operation_exception ("", "Internal error: wrong property '%s' type %d",  property.name.c_str (), property.type);        
    }
    
    impl->need_hash_update = true;
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::NodeProperties::SetProperty(size_t,float)");
    throw;
  }
}

void NodeProperties::SetProperty (size_t property_index, const math::vec4f& value)
{
  try
  {
    if (property_index >= impl->properties.size ())
      throw xtl::make_range_exception ("", "property_index", property_index, impl->properties.size ());
      
    NodeProperty& property = *impl->properties [property_index];
    
    switch (property.type)
    {
      case NodePropertyType_String:
        xtl::to_string (static_cast<NodePropertyImpl<NodePropertyType_String>&> (property).value, value);
        break;
      case NodePropertyType_Vector:
        static_cast<NodePropertyImpl<NodePropertyType_Vector>&> (property).value = value;
        break;
      case NodePropertyType_Integer:
      case NodePropertyType_Float:
      case NodePropertyType_Matrix:
        throw xtl::format_operation_exception ("", "Could not convert property '%s' from %s to %s", property.name.c_str (), get_name (NodePropertyType_Vector), get_name (property.type));
      default:
        throw xtl::format_operation_exception ("", "Internal error: wrong property '%s' type %d", property.name.c_str (), property.type);        
    }
    
    impl->need_hash_update = true;
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::NodeProperties::SetProperty(size_t,const math::vec4f&)");
    throw;
  }
}

void NodeProperties::SetProperty (size_t property_index, const math::mat4f& value)
{
  try
  {
    if (property_index >= impl->properties.size ())
      throw xtl::make_range_exception ("", "property_index", property_index, impl->properties.size ());
      
    NodeProperty& property = *impl->properties [property_index];
    
    switch (property.type)
    {
      case NodePropertyType_String:
        xtl::to_string (static_cast<NodePropertyImpl<NodePropertyType_String>&> (property).value, value);
        break;
      case NodePropertyType_Matrix:
        static_cast<NodePropertyImpl<NodePropertyType_Matrix>&> (property).value = value;
        break;
      case NodePropertyType_Integer:        
      case NodePropertyType_Float:        
      case NodePropertyType_Vector:
        throw xtl::format_operation_exception ("", "Could not convert property '%s' from %s to %s", property.name.c_str (), get_name (NodePropertyType_Matrix), get_name (property.type));
      default:
        throw xtl::format_operation_exception ("", "Internal error: wrong property '%s' type %d", property.name.c_str (), property.type);        
    }
    
    impl->need_hash_update = true;
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::NodeProperties::SetProperty(size_t,const math::mat4f&)");
    throw;
  }
}

/*
    Чтение свойства по имени
*/

void NodeProperties::GetProperty (const char* name, stl::string& result) const
{
  try
  {
    GetProperty (impl->GetIndex (name), result);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::NodeProperties::GetProperty(const char*,stl::string&)");
    throw;
  }
}

void NodeProperties::GetProperty (const char* name, int& result) const
{
  try
  {
    GetProperty (impl->GetIndex (name), result);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::NodeProperties::GetProperty(const char*,int&)");
    throw;
  }
}

void NodeProperties::GetProperty (const char* name, float& result) const
{
  try
  {
    GetProperty (impl->GetIndex (name), result);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::NodeProperties::GetProperty(const char*,float&)");
    throw;
  }
}

void NodeProperties::GetProperty (const char* name, math::vec4f& result) const
{
  try
  {
    GetProperty (impl->GetIndex (name), result);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::NodeProperties::GetProperty(const char*,const math::vec4f&)");
    throw;
  }
}

void NodeProperties::GetProperty (const char* name, math::mat4f& result) const
{
  try
  {
    GetProperty (impl->GetIndex (name), result);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::NodeProperties::GetProperty(const char*,const math::mat4f&)");
    throw;
  }
}

const char* NodeProperties::GetString (const char* name) const
{  
  try
  {
    return GetString (impl->GetIndex (name));
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::NodeProperties::GetString(const char*)");
    throw;
  }
}

int NodeProperties::GetInteger (const char* name) const
{
  try
  {
    return GetInteger (impl->GetIndex (name));
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::NodeProperties::GetInteger(const char*)");
    throw;
  }
}

float NodeProperties::GetFloat (const char* name) const
{
  try
  {
    return GetFloat (impl->GetIndex (name));
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::NodeProperties::GetFloat(const char*)");
    throw;
  }
}

const math::vec4f& NodeProperties::GetVector (const char* name) const
{
  try
  {
    return GetVector (impl->GetIndex (name));
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::NodeProperties::GetVector(const char*)");
    throw;
  }
}

const math::mat4f& NodeProperties::GetMatrix (const char* name) const
{
  try
  {
    return GetMatrix (impl->GetIndex (name));
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::NodeProperties::GetMatrix(const char*)");
    throw;
  }
}

/*
    Чтение свойства по индексу
*/

void NodeProperties::GetProperty (size_t property_index, stl::string& result) const
{
  try
  {    
    if (property_index >= impl->properties.size ())
      throw xtl::make_range_exception ("", "property_index", property_index, impl->properties.size ());
      
    NodeProperty& property = *impl->properties [property_index];
    
    switch (property.type)
    {
      case NodePropertyType_String:
        result = static_cast<NodePropertyImpl<NodePropertyType_String>&> (property).value;
        break;
      case NodePropertyType_Integer:
        xtl::to_string (result, static_cast<NodePropertyImpl<NodePropertyType_Integer>&> (property).value);
        break;
      case NodePropertyType_Float:      
        xtl::to_string (result, static_cast<NodePropertyImpl<NodePropertyType_Float>&> (property).value);        
        break;
      case NodePropertyType_Vector:    
        xtl::to_string (result, static_cast<NodePropertyImpl<NodePropertyType_Vector>&> (property).value);
        break;    
      case NodePropertyType_Matrix:    
        xtl::to_string (result, static_cast<NodePropertyImpl<NodePropertyType_Matrix>&> (property).value);
        break;    
      default:
        throw xtl::format_operation_exception ("", "Internal error: wrong property '%s' type %d", property.name.c_str (), property.type);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::NodeProperties::GetProperty(size_t,stl::string&)");
    throw;
  }
}

void NodeProperties::GetProperty (size_t property_index, int& result) const
{
  try
  {
    result = GetInteger (property_index);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::NodeProperties::GetProperty(size_t,int&)");
    throw;
  }
}

void NodeProperties::GetProperty (size_t property_index, float& result) const
{
  try
  {
    result = GetFloat (property_index);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::NodeProperties::GetProperty(size_t,float&)");
    throw;
  }
}

void NodeProperties::GetProperty (size_t property_index, math::vec4f& result) const
{
  try
  {
    if (property_index >= impl->properties.size ())
      throw xtl::make_range_exception ("", "property_index", property_index, impl->properties.size ());
      
    NodeProperty& property = *impl->properties [property_index];
    
    switch (property.type)
    {
      case NodePropertyType_String:
        read_value_from_property<NodePropertyType_Vector> (static_cast<NodePropertyImpl<NodePropertyType_String>&> (property), result);
        break;
      case NodePropertyType_Integer:
        result = static_cast<float> (static_cast<NodePropertyImpl<NodePropertyType_Integer>&> (property).value);
        break;
      case NodePropertyType_Float:
        result = static_cast<NodePropertyImpl<NodePropertyType_Float>&> (property).value;
        break;
      case NodePropertyType_Vector:    
        result = static_cast<NodePropertyImpl<NodePropertyType_Vector>&> (property).value;
        break;
      case NodePropertyType_Matrix:
        throw xtl::format_operation_exception ("", "Could not convert property '%s' from %s to %s", property.name.c_str (), get_name (property.type), get_name (NodePropertyType_Vector));
      default:
        throw xtl::format_operation_exception ("", "Internal error: wrong property '%s' type %d", property.name.c_str (), property.type);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::NodeProperties::GetProperty(size_t,math::vec4f&)");
    throw;
  }
}

void NodeProperties::GetProperty (size_t property_index, math::mat4f& result) const
{
  try
  {
    if (property_index >= impl->properties.size ())
      throw xtl::make_range_exception ("", "property_index", property_index, impl->properties.size ());
      
    NodeProperty& property = *impl->properties [property_index];
    
    switch (property.type)
    {
      case NodePropertyType_String:
        read_value_from_property<NodePropertyType_Matrix> (static_cast<NodePropertyImpl<NodePropertyType_String>&> (property), result);
        break;
      case NodePropertyType_Integer:
        result = static_cast<float> (static_cast<NodePropertyImpl<NodePropertyType_Integer>&> (property).value);
        break;
      case NodePropertyType_Float:
        result = static_cast<NodePropertyImpl<NodePropertyType_Float>&> (property).value;
        break;
      case NodePropertyType_Matrix:
        result = static_cast<NodePropertyImpl<NodePropertyType_Matrix>&> (property).value;
        break;
      case NodePropertyType_Vector:
        throw xtl::format_operation_exception ("", "Could not convert property '%s' from '%s' to %s", property.name.c_str (), get_name (property.type), get_name (NodePropertyType_Matrix));
      default:
        throw xtl::format_operation_exception ("", "Internal error: wrong property '%s' type %d", property.name.c_str (), property.type);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::NodeProperties::GetProperty(size_t,math::mat4f&)");
    throw;
  }
}

const char* NodeProperties::GetString (size_t property_index) const
{
  static const char* METHOD_NAME = "scene_graph::NodeProperties::GetString(size_t)";

  if (property_index >= impl->properties.size ())
    throw xtl::make_range_exception (METHOD_NAME, "property_index", property_index, impl->properties.size ());
    
  NodeProperty& property = *impl->properties [property_index];
  
  switch (property.type)
  {
    case NodePropertyType_String:
      return static_cast<NodePropertyImpl<NodePropertyType_String>&> (property).value.c_str ();
    case NodePropertyType_Integer:
    case NodePropertyType_Float:
    case NodePropertyType_Vector:
    case NodePropertyType_Matrix:
      throw xtl::format_operation_exception (METHOD_NAME, "Could not convert property '%s' from %s to %s", property.name.c_str (), get_name (property.type), get_name (NodePropertyType_String));
    default:
      throw xtl::format_operation_exception (METHOD_NAME, "Internal error: wrong property '%s' type %d", property.name.c_str (), property.type);
  }
}

int NodeProperties::GetInteger (size_t property_index) const
{
  try
  {
    if (property_index >= impl->properties.size ())
      throw xtl::make_range_exception ("", "property_index", property_index, impl->properties.size ());

    NodeProperty& property = *impl->properties [property_index];

    switch (property.type)
    {
      case NodePropertyType_String:
      {
        int result = 0;

        read_value_from_property<NodePropertyType_Integer> (static_cast<NodePropertyImpl<NodePropertyType_String>&> (property), result);

        return result;
      }
      case NodePropertyType_Integer:
        return static_cast<NodePropertyImpl<NodePropertyType_Integer>&> (property).value;
      case NodePropertyType_Float:
        return static_cast<int> (static_cast<NodePropertyImpl<NodePropertyType_Float>&> (property).value);
      case NodePropertyType_Vector:
      case NodePropertyType_Matrix:
        throw xtl::format_operation_exception ("", "Could not convert property '%s' from %s to %s", property.name.c_str (), get_name (property.type), get_name (NodePropertyType_Integer));
      default:
        throw xtl::format_operation_exception ("", "Internal error: wrong property '%s' type %d", property.name.c_str (), property.type);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::NodeProperties::GetInteger(size_t)");
    throw;
  }
}

float NodeProperties::GetFloat (size_t property_index) const
{
  try
  {
    if (property_index >= impl->properties.size ())
      throw xtl::make_range_exception ("", "property_index", property_index, impl->properties.size ());

    NodeProperty& property = *impl->properties [property_index];

    switch (property.type)
    {
      case NodePropertyType_String:
      {
        float result = 0.0f;

        read_value_from_property<NodePropertyType_Float> (static_cast<NodePropertyImpl<NodePropertyType_String>&> (property), result);

        return result;
      }
      case NodePropertyType_Integer:
        return static_cast<float> (static_cast<NodePropertyImpl<NodePropertyType_Integer>&> (property).value);
      case NodePropertyType_Float:
        return static_cast<NodePropertyImpl<NodePropertyType_Float>&> (property).value;
      case NodePropertyType_Vector:
      case NodePropertyType_Matrix:
        throw xtl::format_operation_exception ("", "Could not convert property '%s' from %s to %s", property.name.c_str (), get_name (property.type), get_name (NodePropertyType_Float));
      default:
        throw xtl::format_operation_exception ("", "Internal error: wrong property '%s' type %d", property.name.c_str (), property.type);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::NodeProperties::GetFloat(size_t)");
    throw;
  }
}

const math::vec4f& NodeProperties::GetVector (size_t property_index) const
{
  static const char* METHOD_NAME = "scene_graph::NodeProperties::GetVector(size_t)";

  if (property_index >= impl->properties.size ())
    throw xtl::make_range_exception (METHOD_NAME, "property_index", property_index, impl->properties.size ());

  NodeProperty& property = *impl->properties [property_index];

  switch (property.type)
  {
    case NodePropertyType_Vector:  
      return static_cast<NodePropertyImpl<NodePropertyType_Vector>&> (property).value;
    case NodePropertyType_String:      
    case NodePropertyType_Integer:      
    case NodePropertyType_Float:          
    case NodePropertyType_Matrix:
      throw xtl::format_operation_exception (METHOD_NAME, "Could not convert property '%s' from %s to %s", property.name.c_str (), get_name (property.type), get_name (NodePropertyType_Vector));
    default:
      throw xtl::format_operation_exception (METHOD_NAME, "Internal error: wrong property '%s' type %d", property.name.c_str (), property.type);
  }
}

const math::mat4f& NodeProperties::GetMatrix (size_t property_index) const
{
  static const char* METHOD_NAME = "scene_graph::NodeProperties::GetMatrix(size_t)";

  if (property_index >= impl->properties.size ())
    throw xtl::make_range_exception (METHOD_NAME, "property_index", property_index, impl->properties.size ());

  NodeProperty& property = *impl->properties [property_index];

  switch (property.type)
  {
    case NodePropertyType_Matrix:  
      return static_cast<NodePropertyImpl<NodePropertyType_Matrix>&> (property).value;
    case NodePropertyType_String:      
    case NodePropertyType_Integer:      
    case NodePropertyType_Float:          
    case NodePropertyType_Vector:
      throw xtl::format_operation_exception (METHOD_NAME, "Could not convert property '%s' from %s to %s", property.name.c_str (), get_name (property.type), get_name (NodePropertyType_Matrix));
    default:
      throw xtl::format_operation_exception (METHOD_NAME, "Internal error: wrong property '%s' type %d", property.name.c_str (), property.type);
  }
}

/*
    Получение имени типа
*/

namespace scene_graph
{

const char* get_name (NodePropertyType type)
{
  switch (type)
  {
    case NodePropertyType_String:  return "string";
    case NodePropertyType_Integer: return "integer";
    case NodePropertyType_Float:   return "float";
    case NodePropertyType_Vector:  return "vector";
    case NodePropertyType_Matrix:  return "matrix";
    default:
      throw xtl::make_argument_exception ("scene_graph::get_name(NodePropertyType)", "type", type);
  }
}

}
