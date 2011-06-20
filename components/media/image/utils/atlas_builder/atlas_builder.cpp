#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <exception>

#include <stl/hash_map>
#include <stl/hash_set>
#include <stl/list>
#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/ref.h>

#include <common/command_line.h>
#include <common/strlib.h>

#include <media/image.h>
#include <media/atlas_builder.h>

using namespace common;

/*
    Константы
*/

const size_t HELP_STRING_PREFIX_LENGTH  = 30;
const char*  DEFAULT_ATLAS_FILE_NAME    = "images.png";
const char*  DEFAULT_LAYOUT_FILE_NAME   = "layout.xatlas";

/*
    Утилиты
*/

void error (const char* format, ...)
{
  va_list args;
  
  va_start (args, format);

  printf  ("error: ");
  vprintf (format, args);
  printf  ("\n");
  fflush  (stdout);

  exit (1);
}

/*
    Обработка командной строки
*/

struct Params;

//опция
struct Option
{
  CommandLine::SwitchHandler handler;       //обработчик ключа
  const char*                name;          //имя команды
  char                       short_name;    //короткое имя
  const char*                argument_name; //имя аргумента
  const char*                tip;           //подсказка
};

//параметры запуска
struct Params
{
  const Option* options;                  //массив опций
  size_t        options_count;            //количество опций
  StringArray   sources;                  //имя исходных изображений
  stl::string   atlas_file_format;        //имя результирующего изображения
  stl::string   layout_atlas_file_format; //имя результирующего изображения в атласе
  stl::string   layout_file_name;         //имя файла разметки
  stl::string   isolated_images_wildcard; //маска имён изолированных изображений не входящих в общий атлас
  size_t        max_image_size;           //максимальный размер одного изображения
  size_t        margin;                   //отступ между картинками в атласе
  bool          silent;                   //минимальное число сообщений
  bool          print_help;               //нужно ли печатать сообщение помощи
  bool          need_layout;              //нужно генерировать файл разметки
  bool          need_pot_rescale;         //нужно ли масштабировать изображение к размерам кратным степени двойки
  bool          invert_x;                 //инвертирование координаты X тайлов
  bool          invert_y;                 //инвертирование координаты Y тайлов
  bool          swap_axises;              //обмен осей местами
  bool          square_axises;            //квадратные текстуры
  bool          dont_store_images;        //не хранить все картинки в памяти
};

//получение подсказки по программе
void command_line_help (const char*, Params& params)
{
  printf ("atlas-builder [<OPTIONS>] <SOURCE> ...\n");
  printf ("  OPTIONS:\n");  
  
  for (size_t i=0; i<params.options_count; i++)
  {
    const Option& option = params.options [i];
    
    stl::string prefix = common::format (option.argument_name ? "    --%s=<%s>" : "    --%s", option.name, option.argument_name);

    if (option.short_name)
      prefix += common::format (option.argument_name ? ", -%c <%s>" : ", -%c", option.short_name, option.argument_name);

    printf ("%s ", prefix.c_str ());

    if (prefix.size () < HELP_STRING_PREFIX_LENGTH)
      for (size_t i=0, count=HELP_STRING_PREFIX_LENGTH-prefix.size (); i<count; i++)
        printf (" ");

    printf ("%s\n", option.tip);   
  }
  
  params.print_help = true;
}

//установка формата имени результирующего файла атласа
void command_line_result_atlas (const char* file_name, Params& params)
{
  params.atlas_file_format = file_name;
}

//установка формата имени результирующего файла атласа при генерации файла разметки
void command_line_result_layout_atlas (const char* file_name, Params& params)
{
  params.layout_atlas_file_format = file_name;
}

//установка имени результирующего файла разметки
void command_line_result_layout (const char* file_name, Params& params)
{
  params.layout_file_name = file_name;
}

//установка максимального размера одной картинки
void command_line_max_image_size (const char* size, Params& params)
{
  params.max_image_size = atoi (size);
}

//установка отступа между картинками
void command_line_margin (const char* size, Params& params)
{
  params.margin = atoi (size);
}

//установка флага генерации файла разметки
void command_line_no_layout (const char* file_name, Params& params)
{
  params.need_layout = false;
}

//установка параметра масштабирования к степени двойки
void command_line_pot (const char*, Params& params)
{
  params.need_pot_rescale = true;  
}

//установка параметра вывода детальной информации
void command_line_silent (const char*, Params& params)
{
  params.silent = true;
}

//установка параметра инвертирования тайлов по оси X
void command_line_invert_x (const char*, Params& params)
{
  params.invert_x = true;
}

//установка параметра инвертирования тайлов по оси Y
void command_line_invert_y (const char*, Params& params)
{
  params.invert_y = true;
}

//установка параметра обмена осей местами
void command_line_swap_axises (const char*, Params& params)
{
  params.swap_axises = true;
}

//установка параметра квадратных текстур
void command_line_square_axises (const char*, Params& params)
{
  params.square_axises = true;
}

//установка параметра хранения картинок в памяти
void command_line_dont_store_images (const char*, Params& params)
{
  params.dont_store_images = true;
}

//установка маски изолированных изображений
void command_line_isolated_images (const char* string, Params& params)
{
  params.isolated_images_wildcard = string;
}

//проверка корректности ввода
void validate (Params& params)
{
  if (params.atlas_file_format.empty ())
  {
    error ("no result atlas file");
    return;
  }
  
  if (params.layout_file_name.empty ())
    params.layout_file_name = DEFAULT_LAYOUT_FILE_NAME;

  if (params.sources.IsEmpty ())
  {
    printf ("atlas-builder [<OPTIONS>] <SOURCE> ...\n  use: atlas-builder --help\n");
    error ("no input files");
    return;
  }  
  
  if (params.layout_atlas_file_format.empty ())
  {
    params.layout_atlas_file_format = params.atlas_file_format;
  }
}

//построение атласа
void build (Params& params)
{
  try
  {
      //загрузка картинок
    stl::hash_map<stl::hash_key<const char*>, media::Image> images;

    if (!params.dont_store_images)
      for (size_t i = 0, count = params.sources.Size (); i < count; i++)
        images.insert_pair (params.sources [i], media::Image (params.sources [i]));

      //конфигурирование построителя атласа

    media::AtlasBuilder builder;
    media::Image        atlas_image;
    media::Atlas        atlas, result_atlas;
    size_t              pack_flags  = 0,
                        atlas_index = 0;
    StringArray         sources_to_process = params.sources;

      //обработка изолированных изображений
      
    common::StringArray isolated_images_wildcard_list = common::split (params.isolated_images_wildcard.c_str ());
      
    stl::hash_set<stl::hash_key<const char*> > is_isolated_image;      

    for (size_t i=0, count=sources_to_process.Size (); i<count; i++)
    {
      const char* source       = sources_to_process [i];      
      bool        need_isolate = false;

      for (size_t i=0, count=isolated_images_wildcard_list.Size (); i<count; i++)
      {
        const char* mask = isolated_images_wildcard_list [i];        

        if (common::wcmatch (source, mask))
        {
          need_isolate = true;
          break;
        }
      }
      
      if (!need_isolate)
        continue;        

      is_isolated_image.insert (source);
    }
    
      //построение атласов
      
    if (params.need_pot_rescale) pack_flags |= media::AtlasPackFlag_PowerOfTwoEdges;
    if (params.invert_x)         pack_flags |= media::AtlasPackFlag_InvertTilesX;
    if (params.invert_y)         pack_flags |= media::AtlasPackFlag_InvertTilesY;
    if (params.swap_axises)      pack_flags |= media::AtlasPackFlag_SwapAxises;      
    if (params.square_axises)    pack_flags |= media::AtlasPackFlag_SquareAxises;
      
    while (!sources_to_process.IsEmpty ())
    {
      if (!params.silent)
        printf ("Build atlas ");

      builder.Reset ();

      typedef stl::pair<media::Image, size_t> SourceDescPair;
      typedef stl::list<SourceDescPair>       ProcessedSourcesList;

      ProcessedSourcesList processed_sources;

      for (size_t i = 0, count = sources_to_process.Size (); i < count; i++)
      {
        const char* source      = sources_to_process [i];
        bool        is_isolated = is_isolated_image.find (source) != is_isolated_image.end ();
        
        if (is_isolated && !processed_sources.empty ()) //изолированное изображение всегда кладётся в атлас одно
          continue;

          //попытка добавить изображение в текущий атлас

        try
        {
          if (params.dont_store_images)
            builder.Insert (source);
          else
            builder.Insert (images [source], media::AtlasBuilderInsertMode_Reference);
        }
        catch (...)
        {
          if (!params.silent)
            printf ("- Fail!\n");

          throw;
        }

        size_t result_image_width = 0, result_image_height = 0;

        try
        {
          builder.GetBuildResults (result_image_width, result_image_height, params.margin, pack_flags);
        }
        catch (...)
        {
          if (!params.silent)
            printf ("- Fail!\n");

          throw;
        }        

        if (result_image_width > params.max_image_size || result_image_height > params.max_image_size)
        {         
          builder.Reset ();

          for (ProcessedSourcesList::iterator iter = processed_sources.begin (), end = processed_sources.end (); iter != end; ++iter)
            builder.Insert (iter->first, media::AtlasBuilderInsertMode_Reference);
        }
        else
        {
          if (params.dont_store_images)
            processed_sources.push_back (SourceDescPair (media::Image (sources_to_process [i]), i));
          else
            processed_sources.push_back (SourceDescPair (images [sources_to_process [i]], i));
          
          if (is_isolated)
            break; //если изолированное изображение может быть добавлено - кладём его в атлас одно
        }
      }

      if (processed_sources.empty ())
        throw xtl::format_operation_exception ("build", "Can't build atlas, image '%s' size greater than maximum atlas image size %u",
          params.sources [0], params.max_image_size);

        //сохранение атласа

      stl::string atlas_file_name, layout_atlas_file_name;

      if (atlas_index || processed_sources.size () != params.sources.Size ())
      {
        stl::string atlas_file_format, layout_atlas_file_format;
        
        if (params.atlas_file_format.find ("%") == stl::string::npos)
          atlas_file_format = common::format ("%s%%u%s", common::basename (params.atlas_file_format).c_str (), common::suffix (params.atlas_file_format).c_str ());          
        else
          atlas_file_format = params.atlas_file_format;

        atlas_file_name = common::format (atlas_file_format.c_str (), atlas_index);
        
        if (params.layout_atlas_file_format.find ("%") == stl::string::npos)
          layout_atlas_file_format = common::format ("%s%%u%s", common::basename (params.layout_atlas_file_format).c_str (), common::suffix (params.layout_atlas_file_format).c_str ());                  
        else
          layout_atlas_file_format = params.layout_atlas_file_format;
          
        layout_atlas_file_name = common::format (layout_atlas_file_format.c_str (), atlas_index);
      }
      else
      {
        atlas_file_name        = common::format (params.atlas_file_format.c_str (), atlas_index);
        layout_atlas_file_name = common::format (params.layout_atlas_file_format.c_str (), atlas_index);
      }

      builder.SetAtlasImageName (layout_atlas_file_name.c_str ());

      try
      {
        builder.Build (atlas, atlas_image, params.margin, pack_flags);

        for (size_t i = 0, count = atlas.TilesCount (); i < count; i++)
          result_atlas.Insert (atlas.Tile (i));
          
        result_atlas.SetImageSize (layout_atlas_file_name.c_str (), atlas.ImageSize (layout_atlas_file_name.c_str ()));
      }
      catch (...)
      {
        if (!params.silent)
          printf ("- Fail!\n");

        throw;
      }

      if (!params.silent)
        printf ("%ux%u\n", atlas_image.Width (), atlas_image.Height ());

      if (!params.silent)
        printf ("Save atlas '%s'\n", atlas_file_name.c_str ());

      try
      {
        atlas_image.Save (atlas_file_name.c_str ());
      }
      catch (...)
      {
        if (!params.silent)
          printf (" - Fail!\n");

        throw;
      }

      for (ProcessedSourcesList::reverse_iterator iter = processed_sources.rbegin (), end = processed_sources.rend (); iter != end; ++iter)
        sources_to_process.Remove (iter->second);

      atlas_index++;
    }

    try
    {
      if (params.need_layout)
      {
        if (!params.silent)
          printf ("Save layout '%s'\n", params.layout_file_name.c_str ());

        result_atlas.Save (params.layout_file_name.c_str ());
      }
    }
    catch (...)
    {
      if (!params.silent)
        printf (" - Fail!\n");

      throw;
    }

    if (!params.silent)
      printf ("Build successfull!\n");
  }
  catch (std::exception& e)
  {
    error ("%s", e.what ());
  }
}

int main (int argc, const char* argv [])
{  
  Params params;

  static Option options [] = {
    {xtl::bind (&command_line_help,                _1, xtl::ref (params)), "help",              '?',        0, "print help message"},
    {xtl::bind (&command_line_silent,              _1, xtl::ref (params)), "silent",            's',        0, "quiet mode"},
    {xtl::bind (&command_line_result_atlas,        _1, xtl::ref (params)), "atlas",             'o',   "file", "set output atlas file format"},
    {xtl::bind (&command_line_result_layout_atlas, _1, xtl::ref (params)), "layout-atlas",      0,     "file", "set output atlas file format that used in layout"},
    {xtl::bind (&command_line_result_layout,       _1, xtl::ref (params)), "layout",            'l',   "file", "set output layout file"},
    {xtl::bind (&command_line_max_image_size,      _1, xtl::ref (params)), "max-image-size",    0,     "size", "set maximum atlas image side size"},
    {xtl::bind (&command_line_margin,              _1, xtl::ref (params)), "margin",            0,     "size", "set margin beetween images in atlas"},
    {xtl::bind (&command_line_no_layout,           _1, xtl::ref (params)), "no-layout",         0,          0, "don't generate layout file"},
    {xtl::bind (&command_line_pot,                 _1, xtl::ref (params)), "pot",               0,          0, "resize atlas texture to nearest greater power of two sizes"},
    {xtl::bind (&command_line_invert_x,            _1, xtl::ref (params)), "invert-x",          0,          0, "invert X coordinate in layout of tiles"},
    {xtl::bind (&command_line_invert_y,            _1, xtl::ref (params)), "invert-y",          0,          0, "invert Y coordinate in layout of tiles"},
    {xtl::bind (&command_line_swap_axises,         _1, xtl::ref (params)), "swap-axises",       0,          0, "swap axises at layout tiles"},
    {xtl::bind (&command_line_isolated_images,     _1, xtl::ref (params)), "isolated-images",   0, "wildcard", "set wildcard for standalone images which will be isolate from general atlases"},
    {xtl::bind (&command_line_square_axises,       _1, xtl::ref (params)), "square",            0,          0, "square axises"},
    {xtl::bind (&command_line_dont_store_images,   _1, xtl::ref (params)), "dont-store-images", 0,          0, "dont store all images in memory"},
  };

  static const size_t options_count = sizeof (options) / sizeof (*options);

    //инициализация

  params.options           = options;
  params.options_count     = options_count;
  params.atlas_file_format = DEFAULT_ATLAS_FILE_NAME;
  params.max_image_size    = -1;
  params.margin            = 0;
  params.print_help        = false;
  params.silent            = false;
  params.need_layout       = true;
  params.need_pot_rescale  = false;
  params.invert_x          = false;
  params.invert_y          = false;
  params.swap_axises       = false;
  params.square_axises     = false;
  params.dont_store_images = false;

  CommandLine command_line;

  for (size_t i = 0; i < params.options_count; i++)
    command_line.SetSwitchHandler (options [i].name, options [i].short_name, options [i].argument_name, options [i].handler);

    //разбор командной строки
  command_line.Process (argc, argv);
  
    // --help только печатает сообщение помощи
  
  if (params.print_help)
    return 0;

  for (size_t i = 0, count = command_line.ParamsCount (); i < count; i++)
    params.sources.Add (command_line.Param (i));

    //проверка корректности ввода

  validate (params);
  
    //построение атласа

  build (params);

  return 0;
}
