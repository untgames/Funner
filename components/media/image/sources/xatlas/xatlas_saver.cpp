#include "shared.h"

using namespace media;
using namespace common;

namespace
{

typedef stl::list     <const Tile*>                            TilesList;
typedef stl::hash_map <stl::hash_key <const char*>, TilesList> TilesMap;

/*
    Вспомогательный класс сохранения карт картинок
*/

class XmlAtlasSaver
{
  public:
    /*
        Конструктор
    */
  
    XmlAtlasSaver (const char* file_name, const Atlas& in_atlas)
      : writer (file_name), atlas (in_atlas)
    {
      SaveAtlas ();
    }

  private:  
    /*
        Сохранение карты картинок
    */
    
    void SaveAtlas ()
    {
      XmlWriter::Scope scope (writer, "Atlas");

      TilesMap tiles_map;

      for (size_t i = 0, count = atlas.TilesCount (); i < count; i++)
      {
        const Tile& tile = atlas.Tile (i);

        TilesMap::iterator iter = tiles_map.find (tile.image);

        if (iter == tiles_map.end ())
          iter = tiles_map.insert_pair (tile.image, TilesList ()).first;

        iter->second.push_back (&tile);
      }

      for (TilesMap::iterator iter = tiles_map.begin (), end = tiles_map.end (); iter != end; ++iter)
      {
        XmlWriter::Scope scope (writer, "Image");

        writer.WriteAttribute ("Name", (*iter->second.begin ())->image);

        for (TilesList::iterator list_iter = iter->second.begin (), list_end = iter->second.end (); list_iter != list_end; ++list_iter)
          SaveTile (*list_iter);
      }
    }

    /*
        Сохранение тайлов
    */

    void SaveTile (const Tile* tile)
    {
      XmlWriter::Scope scope (writer, "Tile");

      writer.WriteAttribute ("Name",   tile->name);
      writer.WriteAttribute ("Left",   tile->origin.x);
      writer.WriteAttribute ("Bottom", tile->origin.y);
      writer.WriteAttribute ("Width",  tile->size.x);
      writer.WriteAttribute ("Height", tile->size.y);
    }
    
  private:
    XmlWriter    writer;  //сериализатор XML
    const Atlas& atlas;   //карта картинок
};

/*
    Сохранение карты картинок
*/

void xatlas_save (const char* file_name, const Atlas& atlas)
{
  XmlAtlasSaver (file_name, atlas);
}

/*
   Компонент сохранение карт картинок
*/

class XAtlasSaverComponent
{
  public:
    //загрузка компонента
    XAtlasSaverComponent () 
    {
      AtlasManager::RegisterSaver ("xatlas", &xatlas_save);
    }
};

extern "C"
{

ComponentRegistrator<XAtlasSaverComponent> XAtlasSaver ("media.image.atlas.savers.xatlas");

}

}
