#include "shared.h"

using namespace media;
using namespace media::ani_cursors;

namespace
{

/*
     онстанты    
*/

const size_t ICON_HEADERS_RESERVED_SIZE = 1024; //резервируемый размер буфера дл€ хранени€ ICO-заголовков

/*
     ласс сохранени€ ANI курсоров
*/

class AniSaver
{
  public:
/// онструктор
    AniSaver (const char* file_name, const Image& image, const char* format_specific_flags)
      : file (file_name)
    {
      try
      {
        file.Resize (0u);
        
          //разбор параметров сохранени€

        common::PropertyMap properties = common::parse_init_string (format_specific_flags);

          //пропуск заголовка

        RiffHeader riff_header;

        memset (&riff_header, 0, sizeof riff_header);

        Write (riff_header);

          //заголовок анимированного курсора                

        char tag [4];

        memcpy (tag, "ACON", 4);
        
        Write (tag);

        AniHeader ani_header;

        memset (&ani_header, 0, sizeof ani_header);
        
        ani_header.header_size    = sizeof ani_header;
        ani_header.images_count   = image.Depth ();
        ani_header.frames_count   = ani_header.images_count;
        ani_header.cx             = 0;
        ani_header.cy             = 0;
        ani_header.bits_per_pixel = get_bits_per_pixel (image.Format ());
        ani_header.planes_count   = 1;
        ani_header.fps            = properties.IndexOf ("fps") == -1 ? 1 : static_cast<size_t> (60.0f / properties.GetFloat ("fps"));
//        ani_header.flags          = 3;

        Write (ani_header);
        
          //формирование списка изображений
          
        memcpy (tag, "LIST", 4);
        
        Write (tag);
        
        common::filepos_t list_start_pos = file.Tell () + sizeof (size_t); 
        
        Write (0u);
        
        memcpy (tag, "fram", 4);
        
        Write (tag);

        size_t icon_reserved_size = image.Width () * image.Height () * get_bytes_per_pixel (image.Format ()) + ICON_HEADERS_RESERVED_SIZE;

        common::MemFile icon_mem_file (icon_reserved_size);
        
        common::FileSystem::Rename (icon_mem_file.Path (), common::format ("%s.ico", icon_mem_file.Path ()).c_str ());

        for (size_t i=0; i<image.Depth (); i++)
        {
          icon_mem_file.Resize (0);
          
          media::Image icon (image.Width (), image.Height (), 1, image.Format (), image.Bitmap (i));

          icon.Save (icon_mem_file.Path ());
          
          memcpy (tag, "icon", 4);

          Write (tag);
          Write (icon_mem_file.Size ());                    

          size_t write_size = file.Write (icon_mem_file.Buffer (), icon_mem_file.Size ());
          
          if (write_size != icon_mem_file.Size ())
            throw xtl::format_operation_exception ("", "Can't write %u bytes", icon_mem_file.Size ());
        }
        
        common::filepos_t list_end_pos = file.Tell ();
        
        file.Seek (list_start_pos);
        
        Write (list_end_pos - list_start_pos);
        
          //запись заголовка
          
        memcpy (riff_header.tag, "RIFF", 4);
        
        riff_header.length = file.Size () - sizeof (riff_header);

        file.Rewind ();

        Write (riff_header);
      }
      catch (xtl::exception& e)
      {
        e.touch ("media::ani_cursors::AniSaver::AniSaver('%s')", file_name);
        throw;
      }
    }
    
  private:    
///„тение данных
    template <class T>
    void Write (const T& data)
    {
      try
      {
        size_t size = file.Write (&data, sizeof T);

        if (size != sizeof T)
          throw xtl::format_operation_exception ("", "Can't write %u bytes from input file", sizeof (T));
      }
      catch (xtl::exception& e)
      {
        e.touch ("media::ani_cursors::AniSaver::Write");
        throw;
      }
    }

  private:
    common::OutputFile file;
};

/*
     омпонент
*/

class Component
{
  public:
    Component ()
    {
      ImageManager::RegisterSaver ("ani", &Component::SaveImage);
    }

  private:
    static void SaveImage (const char* file_name, const Image& image, const char* format_specific_flags)
    {
      AniSaver (file_name, image, format_specific_flags);
    }
};

extern "C"
{

common::ComponentRegistrator<Component> AniCursorSaver ("media.image.savers.ani_cursor");

}

}
