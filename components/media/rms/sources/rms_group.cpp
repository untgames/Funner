#include "shared.h"

using namespace media::rms;
using namespace common;

/*
    Описание реализации группы ресурсов
*/

typedef stl::hash_set<stl::hash_key<const char*> > ResourceSet;
typedef xtl::signal<void (const char*)>            EventSignal;

struct Group::Impl: public xtl::reference_counter
{
  ResourceSet references;                       //карта ресурсов
  StringArray resources;                        //имена ресурсов
  EventSignal signals [GroupEvent_Num]; //сигналы

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
        signals [GroupEvent_OnRemove] (resources [i]);
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

Group::Group ()
  : impl (new Impl)
{
}

Group::Group (const Group& group)
  : impl (group.impl)
{
  addref (impl);
}

Group::~Group ()
{
  release (impl);
}

Group& Group::operator = (const Group& group)
{
  Group (group).Swap (*this);

  return *this;  
}

/*
    Информация о группе
*/

//пуста ли группа
bool Group::IsEmpty () const
{
  return impl->resources.IsEmpty ();
}

//количество элементов в группе
size_t Group::Size () const
{
  return impl->resources.Size ();
}

/*
    Перебор ресурсов
*/

const char* Group::Item (size_t index) const
{
  try
  {
    return impl->resources [index];
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::rms::Group::Item");
    throw;
  }
}

/*
    Добавление / удаление ресурсов
*/

void Group::Add (const char* name)
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
      impl->signals [GroupEvent_OnAdd] (name);
    }
    catch (...)
    {
      //подавление всех исключений
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::rfx::Group::Add(const char*)");
    throw;
  }
}

void Group::Add (const Group& group)
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
    exception.touch ("media::rfx::Group::Add(const Group&)");
    throw;
  }
}

void Group::Remove (const char* name)
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
    impl->signals [GroupEvent_OnRemove] (name);
  }
  catch (...)
  {
    //подавление всех исключений
  }
}

void Group::Remove (const Group& group)
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
    exception.touch ("media::rfx::Group::Remove(const Group&)");
    throw;
  }
}

void Group::Clear ()
{
  impl->Clear ();
}

/*
    Подписка на события
*/

xtl::connection Group::RegisterEventHandler (GroupEvent event, const EventHandler& handler) const
{
  if (event < 0 || event >= GroupEvent_Num)
    throw xtl::make_argument_exception ("media::rms::Group::RegisterEventHandler", "event", event);

  return impl->signals [event].connect (handler);
}

/*
    Обмен
*/

void Group::Swap (Group& group)
{
  stl::swap (impl, group.impl);
}

namespace media
{

namespace rms
{

void swap (Group& group1, Group& group2)
{
  group1.Swap (group2);
}

}

}
