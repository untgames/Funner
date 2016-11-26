#include "shared.h"

using namespace common;

namespace
{

/*
    Константы
*/

const size_t RESERVED_BINDINGS_COUNT = 8; //резервируемое количество связываний

}

/*
    Описание реализации PropertyBindingMap
*/

struct PropertyBindingMap::Impl: public xtl::reference_counter, public xtl::trackable
{
  struct BindingDesc: public xtl::reference_counter
  {
    stl::string             name;       //имя свойства
    size_t                  name_hash;  //хэш имени свойства
    common::PropertyBinding binding;    //связывание
  };
  
  typedef xtl::intrusive_ptr<BindingDesc>             BindingDescPtr;
  typedef stl::vector<BindingDescPtr>                 BindingDescArray;
  typedef xtl::signal<void (PropertyBindingMapEvent)> Signal;
  
  BindingDescArray bindings;                              //массив связываний
  Signal           signals [PropertyBindingMapEvent_Num]; //сигналы

///Конструктор
  Impl ()
  {
    bindings.reserve (RESERVED_BINDINGS_COUNT);
  }  
  
///Синхронизация
  struct SyncFromPropertyMap { void operator () (common::PropertyBinding& binding, const common::PropertyMap& map, size_t index) const { binding.CopyFromPropertyMap (map, index); } };
  struct SyncToPropertyMap   { void operator () (const common::PropertyBinding& binding, common::PropertyMap& map, size_t index) const { binding.CopyToPropertyMap (map, index); } };  

  template <class Fn, class Map> void Synchronize (Map& map)
  {
    Fn fn;
    
    stl::auto_ptr<stl::string> error_string;
    size_t                     errors_count = 0;
    
    for (Impl::BindingDescArray::iterator iter=bindings.begin (), end=bindings.end (); iter!=end; ++iter)
    {
      Impl::BindingDesc& desc = **iter;
      
      try
      {
        int property_index = map.IndexOf (desc.name.c_str ());
        
        if (property_index == -1)
          continue;
          
        fn (desc.binding, map, size_t (property_index));
      }
      catch (std::exception& e)
      {
        if (!error_string)
          error_string.reset (new stl::string);
          
        *error_string += common::format ("\n  %u) exception at set property '%s': %s", ++errors_count, desc.name.c_str (), e.what ());
      }
      catch (...)
      {
        if (!error_string)
          error_string.reset (new stl::string);        

        *error_string += common::format ("\n  %u) unknown exception at set property '%s'\n", ++errors_count, desc.name.c_str ());
      }
    }    
    
    if (error_string)
      throw xtl::format_operation_exception ("", "Property bindings synchronization exception. Errors:%s", error_string->c_str ());
  }
  
///Оповещение о возникновении события
  void Notify (PropertyBindingMapEvent event)
  {
    if (signals [event].empty ())
      return;
      
    try
    {
      signals [event] (event);
    }
    catch (...)
    {
      //подавление всех исключений
    }
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

PropertyBindingMap::PropertyBindingMap ()
  : impl (new Impl)
{
}

PropertyBindingMap::PropertyBindingMap (const PropertyBindingMap& map)
  : impl (map.impl)
{
  addref (impl);
}

PropertyBindingMap::~PropertyBindingMap ()
{
  release (impl);
}

PropertyBindingMap& PropertyBindingMap::operator = (const PropertyBindingMap& map)
{
  PropertyBindingMap (map).Swap (*this);
  
  return *this;
}

/*
    Количество свойств в карте
*/

size_t PropertyBindingMap::Size () const
{
  return impl->bindings.size ();
}

/*
    Поиск индекса свойства
*/

int PropertyBindingMap::IndexOf (const char* name) const
{
  if (!name)
    return -1;
    
  size_t name_hash = strhash (name);
  
  for (Impl::BindingDescArray::const_iterator iter=impl->bindings.begin (), end=impl->bindings.end (); iter!=end; ++iter)
    if ((*iter)->name_hash == name_hash)
      return (int)(iter - impl->bindings.begin ());
      
  return -1;
}

bool PropertyBindingMap::IsPresent (const char* name) const
{
  return IndexOf (name) != -1;
}

/*
    Имя свойства
*/

const char* PropertyBindingMap::PropertyName (size_t index) const
{
  if (index >= impl->bindings.size ())
    throw xtl::make_range_exception ("common::PropertyBindingMap::PropertyName", "index", index, impl->bindings.size ());
    
  return impl->bindings [index]->name.c_str ();
}

size_t PropertyBindingMap::PropertyNameHash (size_t index) const
{
  if (index >= impl->bindings.size ())
    throw xtl::make_range_exception ("common::PropertyBindingMap::PropertyNameHash", "index", index, impl->bindings.size ());
    
  return impl->bindings [index]->name_hash;
}

void PropertyBindingMap::SetPropertyName (size_t index, const char* name)
{
  static const char* METHOD_NAME = "common::PropertyBindingMap::SetPropertyName";

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");

  if (index >= impl->bindings.size ())
    throw xtl::make_range_exception (METHOD_NAME, "index", index, impl->bindings.size ());
    
  Impl::BindingDesc& desc = *impl->bindings [index];
    
  desc.name      = name;
  desc.name_hash = strhash (name);
  
  impl->Notify (PropertyBindingMapEvent_OnLayoutUpdate);
}

/*
    Связывание свойства
*/

common::PropertyBinding& PropertyBindingMap::PropertyBinding (size_t index)
{
  if (index >= impl->bindings.size ())
    throw xtl::make_range_exception ("common::PropertyBindingMap::PropertyBinding", "index", index, impl->bindings.size ());
    
  return impl->bindings [index]->binding;
}

const common::PropertyBinding& PropertyBindingMap::PropertyBinding (size_t index) const
{
  return const_cast<PropertyBindingMap&> (*this).PropertyBinding (index);
}

void PropertyBindingMap::SetPropertyBinding (size_t index, const common::PropertyBinding& binding)
{
  if (index >= impl->bindings.size ())
    throw xtl::make_range_exception ("common::PropertyBindingMap::SetPropertyBinding", "index", index, impl->bindings.size ());

  impl->bindings [index]->binding = binding;  
}

/*
    Добавление нового свойства
*/

size_t PropertyBindingMap::AddProperty (const char* name, const common::PropertyBinding& binding)
{
  //TODO check max properties count

  if (!name)
    throw xtl::make_null_argument_exception ("common::PropertyBindingMap::AddProperty", "name");
    
  Impl::BindingDescPtr desc (new Impl::BindingDesc, false);
  
  desc->name      = name;
  desc->name_hash = strhash (name);
  desc->binding   = binding;
  
  impl->bindings.push_back (desc);
  
  impl->Notify (PropertyBindingMapEvent_OnLayoutUpdate);  
  
  return impl->bindings.size () - 1;
}

/*
    Удаление свойств
*/

void PropertyBindingMap::RemoveProperty (size_t index)
{
  if (index >= impl->bindings.size ())
    return;
    
  impl->bindings.erase (impl->bindings.begin () + index);
  
  impl->Notify (PropertyBindingMapEvent_OnLayoutUpdate);  
}

void PropertyBindingMap::RemoveProperty (const char* name)
{
  RemoveProperty (size_t (IndexOf (name)));
}

void PropertyBindingMap::Clear ()
{
  impl->bindings.clear ();
  
  impl->Notify (PropertyBindingMapEvent_OnLayoutUpdate);  
}

/*
    Синхронизация с картой свойств
*/

void PropertyBindingMap::CopyFromPropertyMap (const PropertyMap& map)
{
  try
  {
    impl->Synchronize<Impl::SyncFromPropertyMap> (map);
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyBindingMap::CopyToPropertyMap");
    throw;
  }
}

void PropertyBindingMap::CopyToPropertyMap (PropertyMap& map) const
{
  try
  {
    impl->Synchronize<Impl::SyncToPropertyMap> (map);
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyBindingMap::CopyToPropertyMap");
    throw;
  }  
}

/*
    Создание объекта синхронизации
*/

PropertyBindingMapSynchronizer PropertyBindingMap::CreateSynchronizer (PropertyMap& map)
{
  try
  {
    return PropertyBindingMapSynchronizer (*this, map);
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyBindingMap::CreateSynchronizer");
    throw;
  }
}

/*
    Создание карты свойств
*/

PropertyMap PropertyBindingMap::CreatePropertyMap () const
{
  try
  {
    PropertyMap map;
    
    for (Impl::BindingDescArray::const_iterator i=impl->bindings.begin (), end=impl->bindings.end (); i!=end; ++i)
    {
      const Impl::BindingDesc& desc = **i;
      
      desc.binding.CopyToPropertyMap (map, desc.name.c_str ());
    }

    return map;
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyBindingMap::CreatePropertyMap");
    throw;
  }
}

/*
    Подписка на события карты связывания свойств
*/

xtl::connection PropertyBindingMap::RegisterEventHandler (PropertyBindingMapEvent event, const EventHandler& handler) const
{
  if (event < 0 || event >= PropertyBindingMapEvent_Num)
    throw xtl::make_argument_exception ("common::PropertyBindingMap::RegisterEventHandler", "event", event);

  return impl->signals [event].connect (handler);
}

/*
    Объект оповещения об удалении
*/

xtl::trackable& PropertyBindingMap::Trackable () const
{
  return *impl;
}

namespace common
{

xtl::trackable& get_trackable (PropertyBindingMap& map)
{
  return map.Trackable ();
}

}

/*
    Обмен
*/

void PropertyBindingMap::Swap (PropertyBindingMap& map)
{
  stl::swap (impl, map.impl);
}

namespace common
{

void swap (PropertyBindingMap& map1, PropertyBindingMap& map2)
{
  map1.Swap (map2);
}

/*
    Копирование из строки
*/

void copy_to (const stl::string& value, PropertyMap& map, const PropertySelector& property_selector)
{
  set_property (map, property_selector, value.c_str ());
}

}
