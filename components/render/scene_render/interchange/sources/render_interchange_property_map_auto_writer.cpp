#include "shared.h"

using namespace render::scene::interchange;

/*
    Константы
*/

namespace
{

const char*  LOG_NAME                        = "render.scene.interchange"; //имя потока отладочного протоколирования
const size_t MAP_REMOVE_LIST_RESERVE_SIZE    = 1024;                       //резервируемое количество элементов в списке удаления
const size_t LAYOUT_REMOVE_LIST_RESERVE_SIZE = 128;                        //резервируемое количество элементов в списке удаления

}

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
typedef stl::hash_map<uint64, MapDescPtr> MapDescMap;
typedef stl::vector<uint64>               IdArray;

}

struct PropertyMapAutoWriter::Impl: public IPropertyMapWriterListener
{
  IPropertyMapWriterListener* listener;           //слушатель событий синхронизатора
  PropertyMapWriter           writer;             //синхронизатор
  MapDescMap                  descs;              //дескрипторы карт свойств
  MapDescList                 update_list;        //список обновлений
  IdArray                     map_remove_list;    //список идентификаторов на удаление
  IdArray                     layout_remove_list; //список идентификаторов на удаление
  common::Log                 log;                //поток отладочного протоколирования

/// Конструктор
  Impl (IPropertyMapWriterListener* in_listener)
    : listener (in_listener)
    , writer (this)
    , log (LOG_NAME)
  {
    map_remove_list.reserve (MAP_REMOVE_LIST_RESERVE_SIZE);
    layout_remove_list.reserve (LAYOUT_REMOVE_LIST_RESERVE_SIZE);
  }

/// Удаление карты свойств
  void Detach (uint64 id)
  {
    descs.erase (id);
  }

/// Обработчики событий
  void OnPropertyMapRemoved (uint64 id)
  {
    try
    {
      Detach (id);

      map_remove_list.push_back (id);

      if (listener)
        listener->OnPropertyMapRemoved (id);
    }
    catch (std::exception& e)
    {
      log.Printf ("%s\n    at render::scene::interchange::PropertyMapAutoWriter::Impl::OnPropertyMapRemoved", e.what ());
    }
    catch (...)
    {
      log.Printf ("unknown exception\n    at render::scene::interchange::PropertyMapAutoWriter::Impl::OnPropertyMapRemoved");
    }
  }

  void OnPropertyLayoutRemoved (uint64 id)
  {
    try
    {
      layout_remove_list.push_back (id);

      if (listener)
        listener->OnPropertyLayoutRemoved (id);
    }
    catch (std::exception& e)
    {
      log.Printf ("%s\n    at render::scene::interchange::PropertyMapAutoWriter::Impl::OnPropertyLayoutRemoved", e.what ());
    }
    catch (...)
    {
      log.Printf ("unknown exception\n    at render::scene::interchange::PropertyMapAutoWriter::Impl::OnPropertyLayoutRemoved");
    }
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
  uint64 id = properties.Id ();

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
      //обработка списов удаления

    for (IdArray::iterator iter=impl->layout_remove_list.begin (), end=impl->layout_remove_list.end (); iter!=end; ++iter)
    {
      size_t saved_position = stream.Position ();

      try
      {
        stream.BeginCommand (CommandId_RemovePropertyLayout);
        write               (stream, static_cast<uint64> (*iter));
        stream.EndCommand   ();
      }
      catch (...)
      {
        stream.SetPosition (saved_position);

        impl->layout_remove_list.erase (impl->layout_remove_list.begin (), iter);

        throw;
      }
    }

    impl->layout_remove_list.clear ();

    for (IdArray::iterator iter=impl->map_remove_list.begin (), end=impl->map_remove_list.end (); iter!=end; ++iter)
    {
      size_t saved_position = stream.Position ();

      try
      {
        stream.BeginCommand (CommandId_RemovePropertyMap);
        write               (stream, static_cast<uint64> (*iter));
        stream.EndCommand   ();
      }
      catch (...)
      {
        stream.SetPosition (saved_position);

        impl->map_remove_list.erase (impl->map_remove_list.begin (), iter);

        throw;
      }
    }

    impl->map_remove_list.clear ();

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
