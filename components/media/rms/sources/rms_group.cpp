#include "shared.h"

using namespace media::rms;
using namespace common;

/*
    Описание реализации группы ресурсов
*/

typedef stl::hash_set<stl::hash_key<const char*> > ResourceSet;
typedef xtl::signal<void (const char*)>            EventSignal;

struct ResourceGroup::Impl: public xtl::reference_counter
{
  ResourceSet references;                       //карта ресурсов
  StringArray resources;                        //имена ресурсов
  EventSignal signals [ResourceGroupEvent_Num]; //сигналы
  
  ~Impl ()
  {
    Clear ();
  }
  
  void Clear ()
  {
      //оповещение об удалении всех ресурсов
      
    for (size_t i=0, count=resources.Size (); i<count; i++)
    {
      try
      {
        signals [ResourceGroupEvent_OnRemove] (resources [i]);
      }
      catch (...)
      {
        //подавление всех исключений
      }
    }
    
      //очистка
      
    references.clear ();
    resources.Clear ();    
  }  
};

/*
    Конструкторы / деструктор / присваивание
*/

ResourceGroup::ResourceGroup ()
  : impl (new Impl)
{
}

ResourceGroup::ResourceGroup (const ResourceGroup& group)
  : impl (group.impl)
{
  addref (impl);
}

ResourceGroup::~ResourceGroup ()
{
  release (impl);
}

ResourceGroup& ResourceGroup::operator = (const ResourceGroup& group)
{
  ResourceGroup (group).Swap (*this);

  return *this;  
}

/*
    Информация о группе
*/

//пуста ли группа
bool ResourceGroup::IsEmpty () const
{
  return impl->resources.IsEmpty ();
}

//количество элементов в группе
size_t ResourceGroup::Size () const
{
  return impl->resources.Size ();
}

/*
    Перебор ресурсов
*/

const char* ResourceGroup::Item (size_t index) const
{
  try
  {
    return impl->resources [index];
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::rms::ResourceGroup::Item");
    throw;
  }
}

/*
    Добавление / удаление ресурсов
*/

void ResourceGroup::Add (const char* name)
{
  try
  {
      //проверка корректности аргументов
    
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");

      //проверка: добавлен ли уже ресурс

    if (impl->references.find (name) != impl->references.end ())
      return;

    size_t resource_index = impl->resources.Add (name);

    try
    {
      impl->references.insert (name);
    }
    catch (...)
    {
      impl->resources.Remove (resource_index);
      throw;
    }

    try
    {
      impl->signals [ResourceGroupEvent_OnAdd] (name);
    }
    catch (...)
    {
      //подавление всех исключений
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::rfx::ResourceGroup::Add(const char*)");
    throw;
  }
}

void ResourceGroup::Add (const ResourceGroup& group)
{
  try
  {
    if (impl == group.impl) //защита от добавления объекта к самому себе
      return;
      
    const StringArray& array = group.impl->resources;

    for (size_t i=0, count=array.Size (); i<count; i++)    
      Add (array [i]);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::rfx::ResourceGroup::Add(const ResourceGroup&)");
    throw;
  }
}

void ResourceGroup::Remove (const char* name)
{
    //проверка корректности аргументов

  if (!name)
    return;
    
  ResourceSet::iterator iter = impl->references.find (name);

  if (iter == impl->references.end ())
    return;

  impl->references.erase (iter);
  
  StringArray& array = impl->resources;

  for (size_t i=0, count=array.Size (); i<count; i++)
    if (!strcmp (array [i], name))
    {
      array.Remove (i);
      break;
    }
    
  try
  {
    impl->signals [ResourceGroupEvent_OnRemove] (name);
  }
  catch (...)
  {
    //подавление всех исключений
  }
}

void ResourceGroup::Remove (const ResourceGroup& group)
{
  try
  {
    if (impl == group.impl) //защита от добавления объекта к самому себе
    {
      Clear ();
      return;
    }

    const StringArray& array = group.impl->resources;

    for (size_t i=0, count=array.Size (); i<count; i++)    
      Remove (array [i]);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::rfx::ResourceGroup::Remove(const ResourceGroup&)");
    throw;
  }
}

void ResourceGroup::Clear ()
{
  impl->Clear ();
}

/*
    Подписка на события
*/

xtl::connection ResourceGroup::RegisterEventHandler (ResourceGroupEvent event, const EventHandler& handler)
{
  if (event < 0 || event >= ResourceGroupEvent_Num)
    throw xtl::make_argument_exception ("media::rms::ResourceGroup::RegisterEventHandler", "event", event);

  return impl->signals [event].connect (handler);
}

/*
    Обмен
*/

void ResourceGroup::Swap (ResourceGroup& group)
{
  stl::swap (impl, group.impl);
}

namespace media
{

namespace rms
{

void swap (ResourceGroup& group1, ResourceGroup& group2)
{
  group1.Swap (group2);
}

}

}
