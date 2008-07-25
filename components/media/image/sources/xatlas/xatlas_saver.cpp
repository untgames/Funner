#include "shared.h"

using namespace media;
using namespace common;

namespace
{

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
      
      writer.WriteAttribute ("Image", atlas.Image ());

      SaveTiles ();
    }

    /*
        Сохранение тайлов
    */

    void SaveTiles ()
    {
      for (size_t i = 0; i < atlas.TilesCount (); i++)
        SaveTile (i);
    }

    void SaveTile (size_t index)
    {
      XmlWriter::Scope scope (writer, "Tile");
      const Tile& tile = atlas.Tile (index);

      writer.WriteAttribute ("Id",        tile.name);
      writer.WriteAttribute ("XPosition", tile.origin.x);
      writer.WriteAttribute ("YPosition", tile.origin.y);
      writer.WriteAttribute ("Width",     tile.size.x);
      writer.WriteAttribute ("Height",    tile.size.y);
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
