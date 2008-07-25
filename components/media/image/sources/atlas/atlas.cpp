#include "shared.h"

using namespace media;
using namespace common;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация карты картинок
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Atlas::Impl
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    Impl ()
    {
    }

    Impl (const Impl& source)
    {
      *this = source;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    Impl& operator = (const Impl& source)
    {
      if (this == &source)
        return *this;

      image_name = source.image_name;

      tiles.resize (source.tiles.size ());

      for (size_t i = 0; i < source.tiles.size (); i++)
        tiles[i] = source.tiles[i];

      return *this; 
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка/получение имени картинки
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetImage (const char* image)
    {
      if (!image)
        throw xtl::make_null_argument_exception ("media::Atlas::SetImage", "image");

      image_name = image;
    }

    const char* Image () const
    {
      return image_name.c_str ();
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Количество
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t TilesCount () const
    {
      return tiles.size ();
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск
///////////////////////////////////////////////////////////////////////////////////////////////////
    const media::Tile* Find (const char* name)
    {
      if (!name)
        return 0;

      TilesArray::iterator iter = FindTile (name);

      if (iter == tiles.end ())
        return 0;

      iter->tile.name = iter->name.c_str ();

      return &(iter->tile);
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение тайла
///////////////////////////////////////////////////////////////////////////////////////////////////
    const media::Tile& Tile (size_t index)
    {
      if (index >= TilesCount ())
        throw xtl::make_range_exception ("media::Atlas::Tile", "index", index, 0u, TilesCount ());

      tiles[index].tile.name = tiles[index].name.c_str ();

      return tiles[index].tile;
    }

    const media::Tile& Tile (const char* name)
    {
      static const char* METHOD_NAME = "media::Atlas::Tile";

      if (!name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "name");

      TilesArray::iterator iter = FindTile (name);

      if (iter == tiles.end ())
        throw xtl::make_argument_exception (METHOD_NAME, "name", name, "No tile with this name");

      iter->tile.name = iter->name.c_str ();

      return iter->tile;
    }
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Добавление тайла
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Insert (const media::Tile& new_tile)
    {
      tiles.push_back (TileEntry (new_tile, new_tile.name));
      return tiles.size ();
    }

    size_t Insert (const char* name, const math::vec2ui& origin, const math::vec2ui& size)
    {
      if (!name)
        throw xtl::make_null_argument_exception ("media::Atlas::Insert", "name");

      media::Tile new_tile;

      new_tile.origin = origin;
      new_tile.size   = size;
      new_tile.name   = name;

      return Insert (new_tile);
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Изменение тайла
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Set (size_t tile_index, const media::Tile& new_tile)
    {
      if (tile_index >= TilesCount ())
        throw xtl::make_range_exception ("media::Atlas::Set", "tile_index", tile_index, 0u, TilesCount ());

      tiles[tile_index].tile = new_tile;
      tiles[tile_index].name = new_tile.name;
    }

    void Set (size_t tile_index, const char* name, const math::vec2ui& origin, const math::vec2ui& size)
    {
      if (tile_index >= TilesCount ())
        throw xtl::make_range_exception ("media::Atlas::Set", "tile_index", tile_index, 0u, TilesCount ());

      if (!name)
        throw xtl::make_null_argument_exception ("media::Atlas::Set", "name");

      tiles[tile_index].tile.origin = origin;
      tiles[tile_index].tile.size   = size;
      tiles[tile_index].name        = name;
    }
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Удаление тайла
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Remove (size_t tile_index)
    {
      if (tile_index >= TilesCount ())
        return;

      tiles.erase (tiles.begin () + tile_index);
    }

    void Remove (const char* name)
    {
      if (!name)
        return;

      TilesArray::iterator remove_iter = FindTile (name);

      if (remove_iter != tiles.end ())
        tiles.erase (remove_iter);
    }

    void RemoveAllTiles ()
    {
      tiles.clear ();
    }

  private:
    struct TileEntry
    {
      media::Tile tile;
      stl::string name;
    
      TileEntry () {}
      TileEntry (const media::Tile& in_tile, const char* in_name) : tile (in_tile), name (in_name) {}
    };

    typedef stl::vector<TileEntry> TilesArray;

  private:
    TilesArray::iterator FindTile (const char* name)
    {
      for (TilesArray::iterator iter = tiles.begin (), end = tiles.end (); iter != end; ++iter)
      {
        if (!xtl::xstrcmp (name, iter->name.c_str ()))
          return iter;
      }

      return tiles.end ();
    }

  private:
    stl::string image_name;
    TilesArray  tiles;
};


/*
   Конструкторы / деструктор
*/

Atlas::Atlas ()
  : impl (new Impl)
{
}

Atlas::Atlas (const char* file_name)
  : impl (new Impl)
{
  static const char* METHOD_NAME = "media::Atlas::Atlas";

  if (!file_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "file_name");

  try
  {
    static ComponentLoader loader ("media.image.atlas.loaders.*");

    AtlasManager::GetLoader (file_name, SerializerFindMode_ByName) (file_name, *this);
  }
  catch (xtl::exception& exception)
  {
    exception.touch (METHOD_NAME);
    throw;
  }
}

Atlas::Atlas (const Atlas& source)
  : impl (new Impl (*(source.impl)))
{
}

Atlas::~Atlas ()
{
}

/*
   Присваивание
*/

Atlas& Atlas::operator = (const Atlas& source)
{
  if (this == &source)
    return *this;

  Atlas(source).Swap (*this);

  return *this; 
}

/*
   Установка/получение имени картинки
*/

void Atlas::SetImage (const char* image)
{
  impl->SetImage (image);
}

const char* Atlas::Image () const
{
  return impl->Image ();
}

/*
   Количество
*/

size_t Atlas::TilesCount () const
{
  return impl->TilesCount ();
}

/*
   Поиск
*/

const media::Tile* Atlas::Find (const char* name) const
{
  return impl->Find (name);
}

/*
   Получение тайла
*/

const media::Tile& Atlas::Tile (size_t index) const
{
  return impl->Tile (index);
}

const media::Tile& Atlas::Tile (const char* name) const
{
  return impl->Tile (name);
}
    
/*
   Добавление тайла
*/

size_t Atlas::Insert (const media::Tile& new_tile)
{
  return impl->Insert (new_tile);
}

size_t Atlas::Insert (const char* name, const math::vec2ui& origin, const math::vec2ui& size)
{
  return impl->Insert (name, origin, size);
}

/*
   Изменение тайла
*/

void Atlas::Set (size_t tile_index, const media::Tile& new_tile)
{
  impl->Set (tile_index, new_tile);
}

void Atlas::Set (size_t tile_index, const char* name, const math::vec2ui& origin, const math::vec2ui& size)
{
  impl->Set (tile_index, name, origin, size);
}
    
/*
   Удаление тайла
*/

void Atlas::Remove (size_t tile_index)
{
  impl->Remove (tile_index);
}

void Atlas::Remove (const char* name)
{
  impl->Remove (name);
}

void Atlas::RemoveAllTiles ()
{
  impl->RemoveAllTiles ();
}

/*
   Загрузка / сохранение
*/

void Atlas::Load (const char* file_name)
{
  try
  {
    Atlas (file_name).Swap (*this);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::Atlas::Load");
    throw;
  }
}

void Atlas::Save (const char* file_name)
{
  static const char* METHOD_NAME = "media::Atlas::Save";

  if (!file_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "file_name");
    
  try
  {
    static ComponentLoader loader ("media.image.atlas.savers.*");

    AtlasManager::GetSaver (file_name, SerializerFindMode_ByName) (file_name, *this);
  }
  catch (xtl::exception& exception)
  {
    exception.touch (METHOD_NAME);
    throw;
  }
}

/*
   Обмен
*/

void Atlas::Swap (Atlas& atlas)
{
  swap (impl, atlas.impl);
}

namespace media
{

/*
   Обмен
*/

void swap (Atlas& atlas1, Atlas& atlas2)
{
  atlas1.Swap (atlas2);
}

}
