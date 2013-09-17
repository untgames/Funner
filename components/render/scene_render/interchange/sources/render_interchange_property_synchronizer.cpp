#include "shared.h"

using namespace render::scene::interchange;

//TODO: output/input stream rollback
//TODO: exception safe

namespace
{

xtl::compile_time_assert<sizeof (int) == sizeof (int32)>     assert1;
xtl::compile_time_assert<sizeof (float) == sizeof (float32)> assert2;

typedef stl::vector<size_t> IndexArray;

#pragma pack(1)

struct PropertyMapHeader
{
  uint64 id;
  bool   has_layout;
  bool   has_layout_id;
};

}

/*
===================================================================================================
    PropertyMapWriter
===================================================================================================
*/

/*
    Описание реализации синхронизатора карты свойств
*/

struct PropertyMapWriter::Impl: public xtl::trackable
{
  struct LayoutDesc: public xtl::reference_counter
  {
    IndexArray string_indices; //индексы строк в лэйауте
    size_t     id;             //индентификатор лэйаута
    size_t     hash;           //хэш лэйаута

    LayoutDesc (size_t in_id) : id (in_id), hash () {}
  };

  typedef xtl::intrusive_ptr<LayoutDesc>       LayoutDescPtr;
  typedef stl::hash_map<size_t, LayoutDescPtr> LayoutMap;

  struct MapDesc: public xtl::reference_counter
  {
    LayoutDescPtr layout; //лэйаут
    size_t        hash;   //хэш карты

    MapDesc () : hash () {}
  };

  typedef xtl::intrusive_ptr<MapDesc>       MapDescPtr;
  typedef stl::hash_map<size_t, MapDescPtr> MapDescMap;

  IPropertyMapWriterListener* listener;      //слушатель событий
  LayoutMap                   layouts;       //синхронизированные лэйауты
  MapDescMap                  property_maps; //синхронизированные карты свойств

/// Конструктор
  Impl (IPropertyMapWriterListener* in_listener)
    : listener (in_listener)
  {
  }

/// Запись лэйаута
/// (exception safe instead of stream writing)
  void WriteLayout (OutputStream& stream, const common::PropertyLayout& layout, LayoutDescPtr& out_desc)
  {
    try
    {
        //создание лэйаута

      LayoutDescPtr desc = out_desc;

      if (!desc)
      {
        layout.Trackable ().connect_tracker (xtl::bind (&Impl::RemoveLayout, this, layout.Id ()), *this);

        desc = LayoutDescPtr (new LayoutDesc (layout.Id ()), false);
      }

        //синхронизация идентификатора лэйаута

      write (stream, static_cast<uint64> (layout.Id ()));

        //синхронизация описаний свойств

      const common::PropertyDesc* property = layout.Properties ();

      size_t strings_count = 0;

      write (stream, static_cast<uint32> (layout.Size ()));

      for (size_t i=0, count=layout.Size (); i<count; i++, property++)
      {
        write (stream, property->name);
        write (stream, static_cast<uint32> (property->type));
        write (stream, static_cast<uint32> (property->elements_count));
        write (stream, static_cast<uint32> (property->offset));

        if (property->type == common::PropertyType_String)
          strings_count++;
      }

        //синхронизация строк

      desc->string_indices.reserve (strings_count); //clear after reserve for exception safeness
      desc->string_indices.clear ();

      property = layout.Properties ();

      for (size_t i=0, count=layout.Size (); i<count; i++, property++)
      {
        if (property->type == common::PropertyType_String)
          desc->string_indices.push_back (i);
      }

        //обновление хэша

      desc->hash = layout.Hash ();

        //обновление таблицы лэйаутов

      if (!out_desc)
        layouts.insert_pair (layout.Id (), desc);

      out_desc = desc;
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::interchange::PropertyMapWriter::Impl::WriteLayout");
      throw;
    }
  }

/// Удаление лэйаута
  void RemoveLayout (size_t id)
  {
    layouts.erase (id);

    try
    {
      if (listener)
        listener->OnPropertyLayoutRemoved (id);
    }
    catch (...)
    {
    }
  }

/// Удаление карты свойств
  void RemovePropertyMap (size_t id)
  {
    property_maps.erase (id);

    try
    {
      if (listener)
        listener->OnPropertyMapRemoved (id);
    }
    catch (...)
    {
    }    
  }
};

/*
    Конструктор / деструктор
*/

PropertyMapWriter::PropertyMapWriter (IPropertyMapWriterListener* listener)
  : impl (new Impl (listener))
{
}

PropertyMapWriter::~PropertyMapWriter ()
{
}

/*
    Синхронизация карты свойств
*/

void PropertyMapWriter::Write (OutputStream& stream, const common::PropertyMap& properties)
{
  try
  {
    bool need_send_layout_id = false;

      //поиск карты и лэйаута

    const common::PropertyLayout& layout = properties.Layout ();

    Impl::LayoutDescPtr        layout_desc;
    Impl::MapDescPtr           map_desc;
    Impl::MapDescMap::iterator map_iter = impl->property_maps.find (properties.Id ());

    if (map_iter != impl->property_maps.end ())
    {
      map_desc    = map_iter->second;
      layout_desc = map_desc->layout;

      if (properties.Hash () == map_desc->hash)
        return;

      if (layout_desc->id != layout.Id ())
      {
        layout_desc = Impl::LayoutDescPtr ();

        Impl::LayoutMap::iterator layout_iter = impl->layouts.find (layout.Id ());

        if (layout_iter != impl->layouts.end ())
          layout_desc = layout_iter->second;
      }
    }
    else
    {
      Impl::LayoutMap::iterator layout_iter = impl->layouts.find (layout.Id ());

      if (layout_iter != impl->layouts.end ())
        layout_desc = layout_iter->second;
    }

      //синхронизация идентификатора карты

    PropertyMapHeader header = {static_cast<uint64> (properties.Id ()), false, false};

    if (!layout_desc || layout_desc->hash != layout.Hash ())
    {
      header.has_layout = true;
    }
    else if (!map_desc || map_desc->layout != layout_desc)
    {
      header.has_layout_id = true;
    }

    stream.BeginCommand (CommandId_UpdatePropertyMap);

    try
    {
      stream.WriteData (&header, sizeof (header));

        //синхронизация лэйаута

      if (header.has_layout)
      {
        impl->WriteLayout (stream, layout, layout_desc);

        if (map_desc)
          map_desc->layout = layout_desc;
      }
      
      if (header.has_layout_id)
      {
        write (stream, static_cast<uint64> (layout.Id ()));
      }

        //создание объекта слежения за картой свойств

      if (!map_desc)
      {
        map_desc = Impl::MapDescPtr (new Impl::MapDesc, false);

        map_desc->layout = layout_desc;

        properties.Trackable ().connect_tracker (xtl::bind (&Impl::RemovePropertyMap, &*impl, properties.Id ()), *impl);

        impl->property_maps.insert_pair (properties.Id (), map_desc);
      }

        //синхронизация буфера карты свойств

      write (stream, static_cast<uint32> (properties.BufferSize ()));

      stream.WriteData (properties.BufferData (), properties.BufferSize ());

        //синхронизация строк

      for (IndexArray::iterator iter=layout_desc->string_indices.begin (), end=layout_desc->string_indices.end (); iter!=end; ++iter)
        write (stream, properties.GetString (*iter));

        //обновление хэша

      map_desc->hash = properties.Hash ();

        //закрытие команды

      stream.EndCommand ();
    }
    catch (...)
    {
      stream.EndCommand (); //FIX
      throw;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::interchange::PropertyMapWriter::Write");
    throw;
  }
}

/*
===================================================================================================
    PropertyMapReader
===================================================================================================
*/

/*
    Описание реализации парсера карты свойств
*/

struct PropertyMapReader::Impl
{
  struct LayoutDesc: public xtl::reference_counter
  {
    common::PropertyLayout layout;         //расположение свойств
    IndexArray             string_indices; //индексы строк в лэйауте
  };

  typedef xtl::intrusive_ptr<LayoutDesc>             LayoutDescPtr;
  typedef stl::hash_map<size_t, LayoutDescPtr>       LayoutMap;

  struct MapDesc: public xtl::reference_counter
  {
    common::PropertyMap properties;   //карта свойств
    LayoutDescPtr       layout;       //расположение свойств
    size_t              layout_hash;  //хэш лэйаута

    MapDesc (const LayoutDescPtr& desc) : properties (desc->layout), layout (desc), layout_hash (layout->layout.Hash ()) {}
  };

  typedef xtl::intrusive_ptr<MapDesc>       MapDescPtr;
  typedef stl::hash_map<size_t, MapDescPtr> MapDescMap;

  LayoutMap  layouts;       //расположения свойств
  MapDescMap property_maps; //карты свойств

/// Чтение лэйаута
  LayoutDescPtr ReadLayout (InputStream& stream)
  {
    try
    {
        //чтение идентификатора лэйаута

      size_t layout_id = static_cast<size_t> (read (stream, xtl::type<uint64> ()));

        //получение лэйаута

      Impl::LayoutMap::iterator iter = layouts.find (layout_id);

      Impl::LayoutDescPtr desc;

      if (iter == layouts.end ())
      {
        desc = LayoutDescPtr (new LayoutDesc, false);
      }
      else
      {
        desc = iter->second;
      }

        //чтение лэйаута

      common::PropertyLayout& layout = desc->layout;

      layout.Clear ();
      desc->string_indices.clear ();

      size_t strings_count = 0, properties_count = read (stream, xtl::type<uint32> ());

      for (size_t i=0; i<properties_count; i++)
      {
        const char*          name           = read (stream, xtl::type<const char*> ());
        common::PropertyType type           = static_cast<common::PropertyType> (read (stream, xtl::type<uint32> ()));
        size_t               elements_count = read (stream, xtl::type<uint32> ());

        size_t property_index = layout.AddProperty (name, type, elements_count);

        const common::PropertyDesc& property = layout.Properties ()[property_index];

        size_t required_offset = read (stream, xtl::type<uint32> ());

        if (required_offset != property.offset)
          throw xtl::format_operation_exception ("", "Internal error: required offset for layout %u is %u, but actual is %u", layout_id, required_offset, property.offset);

        if (property.type == common::PropertyType_String)
          strings_count++;
      }

        //синхронизация строк

      desc->string_indices.reserve (strings_count);

      const common::PropertyDesc* property = layout.Properties ();

      for (size_t i=0, count=layout.Size (); i<count; i++, property++)
      {
        if (property->type == common::PropertyType_String)
          desc->string_indices.push_back (i);
      }      

        //добавление лэйаута в карту лэйаутов

      if (iter == layouts.end ())
        layouts.insert_pair (layout_id, desc);

      return desc;
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::interchange::PropertyMapReader::Impl::ReadLayout");
      throw;
    }
  }
};

/*
    Конструктор / деструктор
*/

PropertyMapReader::PropertyMapReader ()
  : impl (new Impl)
{
}

PropertyMapReader::~PropertyMapReader ()
{
}

/*
    Получение карты свойств
*/

common::PropertyMap PropertyMapReader::GetProperties (size_t id) const
{
  try
  {
    Impl::MapDescMap::iterator iter = impl->property_maps.find (id);

    if (iter == impl->property_maps.end ())
      throw xtl::make_argument_exception ("", "id", id, "PropertyMap with such id has not been registered");

    return iter->second->properties;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::interchange::PropertyMapReader::GetProperties");
    throw;
  }
}

/*
    Обновление карты
*/

void PropertyMapReader::Read (InputStream& stream)
{
  try
  {
    const PropertyMapHeader& header = stream.Read<PropertyMapHeader> ();

      //синхронизация лэйаута

    Impl::LayoutDescPtr layout_desc;

    if (header.has_layout)
    {      
      layout_desc = impl->ReadLayout (stream);
    }
    else if (header.has_layout_id)
    {
      size_t layout_id = static_cast<size_t> (read (stream, xtl::type<uint64> ()));

      Impl::LayoutMap::iterator iter = impl->layouts.find (layout_id);

      if (iter == impl->layouts.end ())
        throw xtl::format_operation_exception ("", "Internal error: can't read property map %u because corresponding layout %u has not been registered", (size_t)header.id, layout_id);

      layout_desc = iter->second;
    }

      //синхронизация карты свойств

    Impl::MapDescMap::iterator map_iter = impl->property_maps.find (static_cast<size_t> (header.id));

    Impl::MapDescPtr map_desc;

    if (map_iter == impl->property_maps.end ())
    {
      if (!layout_desc)
        throw xtl::format_operation_exception ("", "Internal error: no layout for new property map %u", size_t (header.id));

        //новая карта свойств

      Impl::MapDescPtr desc (new Impl::MapDesc (layout_desc), false);

      impl->property_maps.insert_pair (static_cast<size_t> (header.id), desc);

      map_desc = desc;
    }
    else
    {
      map_desc = map_iter->second;

      if (layout_desc)
      {
          //обновление лэйаута существующей карты
         
        const common::PropertyLayout& layout = layout_desc->layout;

        if (map_desc->layout != layout_desc || map_desc->layout_hash != layout.Hash ())
        {
          map_desc->properties.Reset (layout);

          map_desc->layout      = layout_desc;
          map_desc->layout_hash = layout.Hash ();
        }
      }
      else
      {
        layout_desc = map_desc->layout;
      }
    }

      //синхронизация карты свойств

    common::PropertyMap& properties = map_desc->properties;

      //синхронизация буфера карты свойств

    size_t      src_buffer_size = stream.Read<uint32> ();
    const void* src_buffer      = stream.ReadData (src_buffer_size);

    if (src_buffer_size != properties.BufferSize ())
      throw xtl::format_operation_exception ("", "Internal error: can't sync property map %u. Incoming buffer data size is %u but actual is %u", size_t (header.id), src_buffer_size, properties.BufferSize ());

    properties.SetBufferData (src_buffer);

      //синхронизация строк

    for (IndexArray::iterator iter=layout_desc->string_indices.begin (), end=layout_desc->string_indices.end (); iter!=end; ++iter)
    {
      const char* string = read (stream, xtl::type<const char*> ());

      properties.SetProperty (*iter, string);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::interchange::PropertyMapReader::Read");
    throw;
  }
}

/*
    Удаление карты свойств и лэйаута
*/

void PropertyMapReader::RemoveProperties (size_t id)
{
  impl->layouts.erase (id);
}

void PropertyMapReader::RemoveLayout (size_t id)
{
  impl->property_maps.erase (id);
}
