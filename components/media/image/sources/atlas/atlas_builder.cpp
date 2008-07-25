#include "shared.h"

using namespace media;

namespace
{

//получение ближайшей сверху степени двойки
size_t get_next_higher_power_of_two (size_t k) 
{
  if (!k)
    return 1;

  k--;

  for (int i=1; i < sizeof (size_t) * 8; i *= 2)
          k |= k >> i;

  return k + 1;
}

}

/*
   Реализация построителя атласа изображений
*/

struct AtlasBuilder::Impl
{
  public:
    typedef AtlasBuilder::PackHandler PackHandler;
  
/*
   Конструкторы / деструктор
*/
    Impl (const PackHandler& in_pack_handler)
      : pack_handler (in_pack_handler)
      {}

    ~Impl ()
    {
    }

/*
   Установка имени атласной картинки (полной)
*/
    void SetAtlasImageName (const char* name)
    {
      if (!name)
        xtl::make_null_argument_exception ("media::AtlasBuilder::SetAtlasImageName", "name");

      atlas_image_name = name;
    }

    const char* AtlasImageName () const
    {
      return atlas_image_name.c_str ();
    }

/*
   Добавление изображений
*/
    void Insert (Image& image, AtlasBuilderInsertMode mode)
    {
      switch (mode)
      {
        case AtlasBuilderInsertMode_Copy:
        {
          try
          {
            images.push_back (ImagePtr (new Image (image)));
          }
          catch (xtl::exception& exception)
          {
            exception.touch ("media::AtlasBuilder::Insert");
            throw;
          }
          
          break;
        }
        case AtlasBuilderInsertMode_Capture:
        {
          try
          {
            ImagePtr new_image (new Image ());

            new_image->Swap (image);

            images.push_back (new_image);
          }
          catch (xtl::exception& exception)
          {
            exception.touch ("media::AtlasBuilder::Insert");
            throw;
          }

          break;
        }
        default:
          throw xtl::make_argument_exception ("media::AtlasBuilder::Insert", "mode", mode, "Unknown insert mode");
      }
    }

    void Insert (const char* image_name)
    {
      try
      {
        images.push_back (ImagePtr (new Image (image_name)));
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("media::AtlasBuilder::Insert");
        throw;
      }
    }

/*
   Построение карты
*/
    void Build (Atlas& out_atlas, Image& out_atlas_image, size_t pack_flags)
    {
      if (images.empty ())
      {
        Atlas ().Swap (out_atlas);
        Image ().Swap (out_atlas_image);

        return;
      }

      try
      {
        xtl::uninitialized_storage<math::vec2ui> sizes (images.size ());
        xtl::uninitialized_storage<math::vec2ui> origins (images.size ());

        math::vec2ui *current_size = sizes.data ();

        for (size_t i = 0; i < images.size (); i++, current_size++)
        {
          current_size->x = images[i]->Width ();
          current_size->y = images[i]->Height ();
        }

        pack_handler (sizes.size (), sizes.data (), origins.data (), pack_flags);

        Atlas  result_atlas;
        size_t result_image_width = 0;
        size_t result_image_height = 0;

        result_atlas.SetImage (atlas_image_name.c_str ());

        for (size_t i = 0; i < origins.size (); i++)
        {
          Tile new_tile;

          new_tile.name   = images[i]->Name ();
          new_tile.origin = origins.data ()[i];
          new_tile.size   = sizes.data ()[i];

          result_atlas.Insert (new_tile);

          if (result_image_width < (origins.data ()[i].x + sizes.data ()[i].x))
            result_image_width = origins.data ()[i].x + sizes.data ()[i].x;
          if (result_image_height < (origins.data ()[i].y + sizes.data ()[i].y))
            result_image_height = origins.data ()[i].y + sizes.data ()[i].y;
        }

        if (pack_flags & AtlasPackFlag_PowerOfTwoEdges)
        {
          result_image_width  = get_next_higher_power_of_two (result_image_width);
          result_image_height = get_next_higher_power_of_two (result_image_height);
        }

        Image result_image (result_image_width, result_image_height, 1, (*images.begin ())->Format ());

        memset (result_image.Bitmap (), 0, result_image_width * result_image_height * get_bytes_per_pixel (result_image.Format ()));

        for (size_t i = 0; i < origins.size (); i++)
          result_image.PutImage (origins.data ()[i].x, origins.data ()[i].y, 0, sizes.data ()[i].x, sizes.data ()[i].y, 1, images[i]->Format (), images[i]->Bitmap ());

        result_atlas.Swap (out_atlas);
        result_image.Swap (out_atlas_image);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("media::AtlasBuilder::Build");
        throw;
      }
    }

  private:
    typedef xtl::shared_ptr<Image> ImagePtr;
    typedef stl::vector<ImagePtr>  ImagesArray;

  private:
    PackHandler pack_handler;
    stl::string atlas_image_name;
    ImagesArray images;
};

/*
   Построитель атласа изображений
*/
  
/*
   Конструкторы / деструктор
*/

AtlasBuilder::AtlasBuilder (const char* pack_handler_name)
{
  try
  {
    const PackHandler& pack_handler = AtlasBuilderManager::GetPacker (pack_handler_name);

    impl = new Impl (pack_handler);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::AtlasBuilder::AtlasBuilder");
    throw;
  }
}

AtlasBuilder::AtlasBuilder (const PackHandler& pack_handler)
  : impl (new Impl (pack_handler))
{
}

AtlasBuilder::~AtlasBuilder ()
{
}

/*
   Установка имени атласной картинки (полной)
*/

void AtlasBuilder::SetAtlasImageName (const char* name)
{
  impl->SetAtlasImageName (name);
}

const char* AtlasBuilder::AtlasImageName () const
{
  return impl->AtlasImageName ();
}

/*
   Добавление изображений
*/

void AtlasBuilder::Insert (Image& image, AtlasBuilderInsertMode mode)
{
  impl->Insert (image, mode);
}

void AtlasBuilder::Insert (const char* image_name)
{
  impl->Insert (image_name);
}

/*
   Сброс
*/

void AtlasBuilder::Reset ()
{
  AtlasBuilder ().Swap (*this);
}

/*
   Построение карты
*/

void AtlasBuilder::Build (Atlas& out_atlas, Image& out_atlas_image, size_t pack_flags)
{
  impl->Build (out_atlas, out_atlas_image, pack_flags);
}

/*
   Обмен
*/

void AtlasBuilder::Swap (AtlasBuilder& atlas_builder)
{
  stl::swap (impl, atlas_builder.impl);
}

namespace media
{

/*
   Обмен
*/

void swap (AtlasBuilder& atlas_builder1, AtlasBuilder& atlas_builder2)
{
  atlas_builder1.Swap (atlas_builder2);
}

}
