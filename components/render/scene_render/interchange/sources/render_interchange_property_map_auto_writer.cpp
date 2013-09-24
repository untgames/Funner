#include "shared.h"

using namespace render::scene::interchange;

/*
    Описание реализации автоматического синхронизатора карты свойств
*/

namespace
{

struct MapDesc;

/// Список обновления
struct MapDescList
{
  MapDesc* first;
  MapDesc* last; 

  MapDescList () : first (), last () {}
};

/// Дескриптор карты свойств
struct MapDesc: public xtl::reference_counter
{
  const common::PropertyMap& properties;
  bool                       dirty;              //нужна ли синхронизация
  xtl::auto_connection       update_connection;  //соединение с сигналом оповещения об обновлении карты свойств
  MapDescList&               list;               //список обновления
  MapDesc*                   next;               //следующий элемент в списке обновления

/// Конструктор
  MapDesc (const common::PropertyMap& in_properties, MapDescList& in_list)
    : properties (in_properties)
    , dirty (true)
    , update_connection (properties.RegisterEventHandler (common::PropertyMapEvent_OnUpdate, xtl::bind (&MapDesc::OnUpdate, this)))
    , list (in_list)
    , next ()
  {
    AddToList ();
  }

/// Оповещение об обновлении
  void OnUpdate ()
  {
    if (dirty)
      return;

    dirty = true;

    AddToList ();
  }

/// Снятие флага обновления
  void ClearDirtyFlag ()
  {
    dirty = false;
  }

/// Добавление в список
  void AddToList ()
  {
    next = 0;

    if (list.last)
    {
      list.last->next = this;
      return;
    }

    list.last = list.first = this;
  }
};

typedef xtl::intrusive_ptr<MapDesc>       MapDescPtr;
typedef stl::hash_map<size_t, MapDescPtr> MapDescMap;

}

struct PropertyMapAutoWriter::Impl: public IPropertyMapWriterListener
{
  IPropertyMapWriterListener* listener;    //слушатель событий синхронизатора
  PropertyMapWriter           writer;      //синхронизатор
  MapDescMap                  descs;       //дескрипторы карт свойств
  MapDescList                 update_list; //список обновлений

/// Конструктор
  Impl (IPropertyMapWriterListener* in_listener)
    : listener (in_listener)
    , writer (this)
  {
  }

/// Удаление карты свойств
  void Detach (size_t id)
  {
    descs.erase (id);
  }

/// Обработчики событий
  void OnPropertyMapRemoved (size_t id)
  {
    Detach (id);

    if (listener)
      listener->OnPropertyMapRemoved (id);
  }

  void OnPropertyLayoutRemoved (size_t id)
  {
    if (listener)
      listener->OnPropertyLayoutRemoved (id);
  }
};

/*
    Конструктор / деструктор
*/

PropertyMapAutoWriter::PropertyMapAutoWriter (IPropertyMapWriterListener* listener)
  : impl (new Impl (listener))
{
}

PropertyMapAutoWriter::~PropertyMapAutoWriter ()
{
}

/*
    Слежение за картой свойств
*/

void PropertyMapAutoWriter::Attach (const common::PropertyMap& properties)
{
  size_t id = properties.Id ();

  MapDescMap::iterator iter = impl->descs.find (id);

  if (iter != impl->descs.end ())
    return;

  MapDescPtr desc (new MapDesc (properties, impl->update_list), false);

  impl->descs.insert_pair (id, desc);
}

void PropertyMapAutoWriter::Detach (const common::PropertyMap& properties)
{
  impl->Detach (properties.Id ());
}

/*
    Запись карты свойств (в ручную)
*/

void PropertyMapAutoWriter::Write (OutputStream& stream, const common::PropertyMap& properties)
{
  try
  {
    impl->writer.Write (stream, properties);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::interchange::PropertyMapAutoWriter::Write(OutputStream&,const common::PropertyMap&)");
    throw;
  }
}

/*
    Синхронизация
*/

void PropertyMapAutoWriter::Write (OutputStream& stream)
{
  try
  {
      //обработка списка обновлений

    for (MapDesc* desc = impl->update_list.first; desc; desc = desc->next)
    {
      try
      {
          //запись обновлений

        impl->writer.Write (stream, desc->properties);

          //очистка флага обновлений

        desc->ClearDirtyFlag ();
      }
      catch (...)
      {
        impl->update_list.first = desc->next;

        desc->ClearDirtyFlag ();

        throw;
      }
    }

      //очистка списка обновления

    impl->update_list.first = impl->update_list.last = 0;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::interchange::PropertyMapAutoWriter::Write(OutputStream&)");
    throw;
  }  
}
