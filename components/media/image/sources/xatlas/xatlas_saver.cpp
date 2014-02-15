#include "shared.h"

using namespace media;
using namespace common;

namespace components
{

namespace xatlas_saver
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

      for (size_t image_index=0, images_count=atlas.ImagesCount (); image_index<images_count; image_index++)
      {
        XmlWriter::Scope scope (writer, "Image");
        
        const math::vec2ui& size = atlas.ImageSize (image_index);

        writer.WriteAttribute ("Name",   atlas.ImageName (image_index));        
        writer.WriteAttribute ("Width",  size.x);
        writer.WriteAttribute ("Height", size.y);

        for (size_t tile_index=0, tiles_count=atlas.ImageTilesCount (image_index); tile_index<tiles_count; tile_index++)
          SaveTile (atlas.ImageTile (image_index, tile_index));
      }
    }

    /*
        Сохранение тайлов
    */

    void SaveTile (const Tile& tile)
    {
      XmlWriter::Scope scope (writer, "Tile");

      writer.WriteAttribute ("Name",   tile.name);
      writer.WriteAttribute ("Left",   tile.origin.x);
      writer.WriteAttribute ("Bottom", tile.origin.y);
      writer.WriteAttribute ("Width",  tile.size.x);
      writer.WriteAttribute ("Height", tile.size.y);

      if (tile.tag)
        writer.WriteAttribute ("Tag", tile.tag);
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

}
