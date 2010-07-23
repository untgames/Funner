#include <cstdio>

#include <stl/map>
#include <stl/hash_map>
#include <stl/hash_set>
#include <stl/string>
#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/iterator.h>

#include <math/vector.h>

#include <common/file.h>
#include <common/strlib.h>
#include <common/xml_writer.h>

#include <media/adobe/xfl.h>
#include <media/collection.h>
#include <media/image.h>

using namespace math;
using namespace common;
using namespace media;
using namespace media::adobe::xfl;

const char* APPLICATION_NAME        = "xfl-converter";
const char* DEFAULT_TEXTURES_FORMAT = "image%04u.png";
const char* XFL_MOUNT_POINT         = "/mount_points/animation.xfl";

const size_t HELP_STRING_PREFIX_LENGTH  = 30;

const float EPSILON = 0.001f;

struct Params;

typedef void (*ProcessOption)(const char* arg, Params& params);

//опция
struct Option
{
  ProcessOption process;       //функция-обработчик опции
  const char*   name;          //имя команды
  char          short_name;    //короткое имя
  const char*   argument_name; //имя аргумента
  const char*   tip;           //подсказка
};

//параметры запуска
struct Params
{
  const Option* options;                           //массив опций
  size_t        options_count;                     //количество опций
  stl::string   xfl_name;                          //имя исходного файла или папки
  stl::string   output_textures_dir_name;          //имя каталога с сохранёнными текстурами
  stl::string   output_material_textures_dir_name; //имя каталога с текстурами, используемое при генерации материала
  stl::string   output_textures_format;            //формат имён файлов в папке с текстурами
  stl::string   output_materials_file_name;        //имя файла с материалами
  stl::string   output_scene_file_name;            //имя файла с анимациями
  stl::string   material_name_pattern;             //строка шаблона поиска имени материала
  stl::string   material_name_replacement;         //строка преобразования имени материала
  stl::string   sprite_name_pattern;               //строка шаблона поиска имени спрайта
  stl::string   sprite_name_replacement;           //строка преобразования имени спрайта
  stl::string   crop_exclude;                      //необрезаемые слои
  stl::string   layers_exclude;                    //неэкспортируемые слои
  size_t        crop_alpha;                        //коэффициент обрезания по прозрачности
  bool          silent;                            //минимальное число сообщений
  bool          print_help;                        //нужно ли печатать сообщение помощи
  bool          need_pot_extent;                   //нужно ли расширять изображения до ближайшей степени двойки
  bool          need_crop_alpha;                   //нужно ли обрезать картинку по нулевой прозрачности
  bool          need_inverse_x;                    //нужно ли инвертировать знак оси X
  bool          need_inverse_y;                    //нужно ли инвертировать знак оси Y
  bool          need_relative;                     //нужно генерировать данные в относительной системе координат
};

//прямоугольная область
struct Rect
{
  size_t x, y;
  size_t width, height;
};

//дескриптор изображения
struct ImageDesc
{
  size_t      x, y;
  size_t      width, height;
  size_t      full_width, full_height;
  stl::string image_path;
  stl::string source_resource_name;
};

//событие
struct Event
{
  float       time;
  stl::string action;
};

typedef stl::hash_set<stl::hash_key<const char*> > UsedResourcesSet;
typedef stl::hash_map<stl::string, ImageDesc>      ImageMap;
typedef stl::list<Event>                           EventList;

//параметры конвертации
struct ConvertData
{
  Document                 document;
  UsedResourcesSet         used_symbols;
  ImageMap                 images;
  stl::auto_ptr<XmlWriter> scene_writer;
};

enum LayerType
{
  LayerType_Undefined,
  LayerType_Sprite,
  LayerType_SpriteGroup,
  LayerType_Instance,
};

//сравнение дробных чисел
bool float_compare (float v1, float v2)
{
  if ((v1 > v2 + EPSILON) || (v1 < v2 - EPSILON))
    return false;

  return true;
}

//получение ближайшей сверху степени двойки
size_t get_next_higher_power_of_two (size_t k)
{
  if (!k)
    return 1;

  if (!(k & (k-1)))
    return k;

  k--;

  for (size_t i=1; i < sizeof (size_t) * 8; i *= 2)
          k |= k >> i;

  return k + 1;
}

//печать ошибки с выходом из программы
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

//получение подсказки по программе
void command_line_help (const char*, Params& params)
{
  printf ("%s [<OPTIONS>] <SOURCE> ...\n", APPLICATION_NAME);
  printf ("  OPTIONS:\n");

  for (size_t i = 0; i < params.options_count; i++)
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

//установка имени пути сохранения текстур
void command_line_result_textures_dir (const char* file_name, Params& params)
{
  params.output_textures_dir_name = file_name;
}

//установка имени пути сохранения текстур
void command_line_result_material_textures_dir (const char* file_name, Params& params)
{
  params.output_material_textures_dir_name = file_name;
}

//установка формата сохранения текстур
void command_line_result_textures_format (const char* value, Params& params)
{
  params.output_textures_format = value;
}

//установка файла материалов
void command_line_materials_file_name (const char* file_name, Params& params)
{
  params.output_materials_file_name = file_name;
}

//установка папки анимаций
void command_line_scene_file_name (const char* file_name, Params& params)
{
  params.output_scene_file_name = file_name;
}

//установка необрезаемых слоёв
void command_line_crop_exclude (const char* string, Params& params)
{
  params.crop_exclude = string;
}

//установка неэкспортируемых слоёв
void command_line_layers_exclude (const char* string, Params& params)
{
  params.layers_exclude = string;
}

//установка параметра вывода детальной информации
void command_line_silent (const char*, Params& params)
{
  params.silent = true;
}

//установка параметра генерации слоёв размерами кратными степени двойки
void command_line_pot (const char*, Params& params)
{
  params.need_pot_extent = true;
}

//установка параметра обрезания по прозрачности
void command_line_crop_alpha (const char* value_string, Params& params)
{
  size_t value = (size_t)atoi (value_string);

  params.crop_alpha      = value < 0 ? 0 : value > 256 ? 256 : value;
  params.need_crop_alpha = true;
}

//установка строки замены имени материала
void command_line_material_replacement (const char* string, Params& params)
{
  common::StringArray tokens = common::split (string, ",", "");
  
  if (tokens.Size () != 2)
    error ("bad material replacement string '%s'", string);
    
  params.material_name_pattern     = tokens [0];
  params.material_name_replacement = tokens [1];  
}

//установка строки замены имени спрайта
void command_line_sprite_name_replacement (const char* string, Params& params)
{
  common::StringArray tokens = common::split (string, ",", "");
  
  if (tokens.Size () != 2)
    error ("bad sprite name replacement string '%s'", string);

  params.sprite_name_pattern     = tokens [0];
  params.sprite_name_replacement = tokens [1];
}

//установка флага необходимости инвертирования координат по оси X
void command_line_inverse_x (const char*, Params& params)
{
  params.need_inverse_x = true;
}

//установка флага необходимости инвертирования координат по оси Y
void command_line_inverse_y (const char*, Params& params)
{
  params.need_inverse_y = true;
}

//установка флага необходимости генерации в относительной системе координат
void command_line_relative (const char*, Params& params)
{
  params.need_relative = true;
}

//разбор командной строки
void command_line_parse (int argc, const char* argv [], Params& params)
{
  static Option options [] = {
    {command_line_result_textures_dir,          "out-textures-dir",          'o',        "dir",  "set output textures directory"},
    {command_line_result_textures_format,       "out-textures-format",         0,     "string",  "set output textures format string"},
    {command_line_result_material_textures_dir, "out-material-textures-dir", 'o',        "dir",  "set output textures directory in material file"},    
    {command_line_materials_file_name,          "out-materials",               0,       "file",  "set output materials file"},
    {command_line_scene_file_name,              "out-scene",                   0,       "file",  "set output scene file"},
    {command_line_crop_alpha,                   "crop-alpha",                  0,      "value",  "crop layers by alpha that less than value"},    
    {command_line_crop_exclude,                 "crop-exclude",                0,  "wildcards",  "exclude selected layers from crop"},    
    {command_line_layers_exclude,               "layers-exclude",              0,  "wildcards",  "exclude selected layers from export"},
    {command_line_material_replacement,         "material-replacement",        0,     "string",  "set material name replacement string separated by ','"},
    {command_line_sprite_name_replacement,      "sprite-replacement",          0,     "string",  "set sprite name replacement string separated by ','"},
    {command_line_pot,                          "pot",                         0,            0,  "extent textures size to nearest greater power of two"},
    {command_line_inverse_x,                    "inverse-x",                   0,            0,  "inverse X ort"},
    {command_line_inverse_y,                    "inverse-y",                   0,            0,  "inverse Y ort"},
    {command_line_relative,                     "relative",                    0,            0,  "relative coordinates"},
    {command_line_silent,                       "silent",                    's',            0,  "quiet mode"},
    {command_line_help,                         "help",                      '?',            0,  "print help message"},    
  };

  static const size_t options_count = sizeof (options) / sizeof (*options);

  params.options       = options;
  params.options_count = options_count;

    //разбор командной строки

  for (int i=1; i<argc; i++)
  {
    const char* arg = argv [i];

    static const char*  LONG_OPTION_NAME_PREFIX        = "--";
    static const size_t LONG_OPTION_NAME_PREFIX_LENGTH = strlen (LONG_OPTION_NAME_PREFIX);

    bool long_option  = !strncmp (LONG_OPTION_NAME_PREFIX, arg, LONG_OPTION_NAME_PREFIX_LENGTH);
    bool short_option = !long_option && *arg == '-' && arg [1];

    if (!long_option && !short_option)
    {
      if (!params.xfl_name.empty ())
      {
        error ("source file must be one");
        return;
      }

      params.xfl_name = arg;

      continue;
    }

    stl::string   option_name;
    stl::string   option_argument;
    const Option* option = 0;

      //разбор длинных опций

    if (long_option)
    {
      arg += LONG_OPTION_NAME_PREFIX_LENGTH;

      const char* end_option_name = strchr (arg, '=');

      if (end_option_name)
      {
        option_name.assign (arg, end_option_name);

        arg = end_option_name + 1;
      }
      else
      {
        option_name  = arg;
        arg         += strlen (arg);
      }

      for (size_t j=0; j<options_count; j++)
      {
        const Option& test_option        = options [j];
        size_t        test_option_length = strlen (test_option.name);

        if (!strncmp (test_option.name, option_name.c_str (), test_option_length))
        {
          option = &test_option;
          break;
        }
      }

      if (!option)
      {
        error ("wrong option '--%s'", option_name.c_str ());

        return;
      }

      option_name = LONG_OPTION_NAME_PREFIX + option_name;
    }

      //разбор коротких опций

    if (short_option)
    {
      arg++;

      if (arg [1])
      {
        error ("wrong option '-%c'", *arg);

        return;
      }

      for (size_t j=0; j<options_count; j++)
      {
        if (*arg == options [j].short_name)
        {
          option = &options [j];

          break;
        }
      }

      if (!option)
      {
        error ("wrong option '-%c'", *arg);

        return;
      }

      option_name = common::format ("-%c", *arg);

      if (option->argument_name)
      {
        i++;

        if (i >= argc)
        {
          error ("option '-%c' require argument", *arg);

          return;
        }

        arg = argv [i];
      }
      else
        arg = "";
    }

      //получение аргумента

    for (;*arg; arg++)
    {
      switch (*arg)
      {
        case '\'':
        case '"':
        {
          const char* end = strchr (arg + 1, *arg);

          if (end)
          {
            option_argument.append (arg + 1, end);

            arg = end;
          }
          else
          {
            error ("unquoted string at parse option '%s'", option_name.c_str ());

            return;
          }

          break;
        }
        default:
          option_argument.push_back (*arg);
          break;
      }
    }

    if (option->argument_name && option_argument.empty ())
    {
      error ("option '%s' require argument", option_name.c_str ());

      return;
    }

    if (!option->argument_name && !option_argument.empty ())
    {
      error ("option '%s' has no arguments", option_name.c_str ());

      return;
    }

    if (option->process)
      option->process (option_argument.c_str (), params);
  }
}

///проверка попадания имени в список строк
bool is_present (const char* name, const StringArray& strings)
{
  for (size_t i = 0, count = strings.Size (); i < count; i++)
  {
    const char* mask = strings [i];

    if (common::wcmatch (name, mask))
      return true;
  }

  return false;
}

//получение полного имени материала
stl::string get_full_material_name (const Params& params, const char* name)
{
  if (params.material_name_pattern.empty ())
    return name;

  return common::replace (name, params.material_name_pattern.c_str (), params.material_name_replacement.c_str ());
}

//получение имени спрайта
stl::string get_sprite_name (const Params& params, const char* name)
{
  if (params.sprite_name_pattern.empty ())
    return name;

  return common::replace (name, params.sprite_name_pattern.c_str (), params.sprite_name_replacement.c_str ());
}

//сохранение материалов
void process_materials (Params& params, ConvertData& data)
{
  if (params.output_materials_file_name.empty ())
    return; //если имя файла материалов не указано - экспорт не производится
    
  if (params.output_material_textures_dir_name.empty ())
    params.output_material_textures_dir_name = params.output_textures_dir_name;

  XmlWriter writer (params.output_materials_file_name.c_str ());

  XmlWriter::Scope scope (writer, "material_library");
  
  for (ImageMap::iterator iter=data.images.begin (); iter!=data.images.end (); ++iter)
  {
    ImageDesc&  desc = iter->second;
    const char* id   = iter->first.c_str ();
    
    if (desc.source_resource_name == id)
    {
      XmlWriter::Scope material_scope (writer, "material");

      writer.WriteAttribute ("id", get_full_material_name (params, id).c_str ());

      XmlWriter::Scope sprite_profile (writer, "sprite_profile");

      writer.WriteAttribute ("image", common::format ("%s/%s", params.output_material_textures_dir_name.c_str (), common::notdir (desc.image_path.c_str ())).c_str ());
      writer.WriteAttribute ("blend_mode", "translucent");
      writer.WriteAttribute ("tiling", "1");
      writer.WriteAttribute ("tile_width", desc.width);
      writer.WriteAttribute ("tile_height", desc.height);      
    }
    else
    {
      XmlWriter::Scope material_scope (writer, "instance_material");

      writer.WriteAttribute ("id",     id);
      writer.WriteAttribute ("source", desc.source_resource_name.c_str ());
    }
  }
}

///обработка текстур
void process_textures (Params& params, ConvertData& data)
{
    //в случае, если имя каталога с текстурами и формата текстур не задано - сохранение текстур не производится

  if (!params.silent)
    printf ("Processing textures...\n");

  if (!params.output_textures_dir_name.empty () || !params.output_textures_format.empty ())
  {
    if (params.output_textures_dir_name.empty ())
      params.output_textures_dir_name = params.xfl_name;
    
    if (params.output_textures_format.empty ())
      params.output_textures_format = DEFAULT_TEXTURES_FORMAT;
      
      //создание каталога для хранения текстур

    if (!FileSystem::IsDir (params.output_textures_dir_name.c_str ()))
      FileSystem::Mkdir (params.output_textures_dir_name.c_str ());
  }

    //построение списка текстур
    
  common::StringArray crop_exclude_list = common::split (params.crop_exclude.c_str ());
  common::StringArray textures_list;
  
  typedef stl::hash_set<stl::hash_key<const char*> > TexturesSet;
  typedef stl::hash_map<stl::hash_key<const char*>, stl::string> TexturesSymbolMap;
  
  TexturesSet       textures_set;
  TexturesSymbolMap textures_symbol;
  
    //обход символов

  for (size_t symbol_index = 0; symbol_index < data.document.Symbols ().Size (); symbol_index++)
  {
    const Symbol& symbol = data.document.Symbols () [symbol_index];

      //исключение из обработки неиспользуемых символов
    
    if (data.used_symbols.find (symbol.Name ()) == data.used_symbols.end ())
      continue;
      
      //обработка таймлайна данного символа

    const Timeline& symbol_timeline = symbol.Timeline ();

    for (Timeline::LayerList::ConstIterator layer_iter=symbol_timeline.Layers ().CreateIterator (); layer_iter; ++layer_iter)
    {
      const Layer& symbol_layer = *layer_iter;
      
      for (Layer::FrameList::ConstIterator symbol_frame_iter = symbol_layer.Frames ().CreateIterator (); symbol_frame_iter; ++symbol_frame_iter)
        for (Frame::FrameElementList::ConstIterator symbol_element_iter = symbol_frame_iter->Elements ().CreateIterator (); symbol_element_iter; ++symbol_element_iter)
        {
          const char* resource_name = symbol_element_iter->Name ();
          
          if (symbol_element_iter->Type () == FrameElementType_ResourceInstance && !textures_set.count (resource_name))
          {
            textures_set.insert (resource_name);
            
            textures_list += resource_name;
            
            textures_symbol [resource_name] = symbol.Name ();
          }
        }
    }
  }
  
    //кэш текстур
  
  typedef stl::hash_map<size_t, stl::string> ImageCache;
  
  ImageCache image_cache;
  
    //счётчик имён текстур
    
  size_t texture_name_index = 0;
    
    //обход используемых текстур
    
  for (size_t i=0; i<textures_list.Size (); i++)
  {
      //формирование имени исходной и целевой текстуры
    
    const char* resource_name = textures_list [i];
    stl::string source_path   = data.document.Resources ()[resource_name].Path ();
    stl::string target_path   = common::format (common::format ("%s/%s", params.output_textures_dir_name.c_str (), params.output_textures_format.c_str ()).c_str (), texture_name_index);
    
      //загрузка исходной текстуры
    
    Image source_image (source_path.c_str ());

        //отсечение по альфа
        
    bool need_crop_alpha = params.need_crop_alpha && !is_present (textures_symbol [resource_name].c_str (), crop_exclude_list);
    
    size_t offset_x = 0, offset_y = 0, full_width = source_image.Width (), full_height = source_image.Height ();

    if (need_crop_alpha)
    {
      Rect crop_rect;

      memset (&crop_rect, 0, sizeof (crop_rect));      
      
      crop_by_alpha (source_image, params.crop_alpha, crop_rect.x, crop_rect.y, crop_rect.width, crop_rect.height);

      if (!crop_rect.width || !crop_rect.height)
      {
        crop_rect.x      = 0;
        crop_rect.y      = 0;
        crop_rect.width  = 1;
        crop_rect.height = 1;
      }
      
      xtl::uninitialized_storage<char> image_copy_buffer (get_bytes_per_pixel (source_image.Format ()) * crop_rect.width * crop_rect.height);

      source_image.GetImage (crop_rect.x, crop_rect.y, 0, crop_rect.width, crop_rect.height, 1, source_image.Format (), image_copy_buffer.data ());
      
      media::Image (crop_rect.width, crop_rect.height, 1, source_image.Format (), image_copy_buffer.data ()).Swap (source_image);
      
        //сохранение смещения после отсечения
     
      offset_x = crop_rect.x;
      offset_y = crop_rect.y;

      if (!params.silent)
        printf ("  crop layer '%s' %ux%u: (%u, %u)-(%u, %u)\n", source_path.c_str (), full_width, full_height, crop_rect.x, crop_rect.y, 
          crop_rect.x + crop_rect.width, crop_rect.y + crop_rect.height);
    }
    
      //поиск изображения в кэше
    
    size_t               image_hash = common::crc32 (source_image.Bitmap (), get_bytes_per_pixel (source_image.Format ()) * source_image.Width () * source_image.Height ());
    ImageCache::iterator image_iter = image_cache.find (image_hash);
    
    ImageDesc desc;
    
    desc.x           = offset_x;
    desc.y           = offset_y;
    desc.width       = source_image.Width ();
    desc.height      = source_image.Height ();
    desc.full_width  = full_width;
    desc.full_height = full_height;
    
    if (image_iter != image_cache.end ())
    {
        //сохранять изображения не нужно - достаточно сделать ссылку
        
      desc.image_path           = data.images [image_iter->second].image_path;
      desc.source_resource_name = image_iter->second;
    
      data.images [resource_name] = desc;
        
      continue;
    }
    
      //сохранение изображения
      
    desc.image_path           = target_path;
    desc.source_resource_name = resource_name;
    
    data.images.insert_pair (resource_name, desc);
    
      //обновление кэша
    
    image_cache [image_hash] = resource_name;
    
      //увеличение до степени двойки
      
    if (params.need_pot_extent)
    {
      media::Image pot_image (get_next_higher_power_of_two (source_image.Width ()), get_next_higher_power_of_two (source_image.Height ()),
        1, source_image.Format ());
        
      pot_image.PutImage (0, 0, 0, source_image.Width (), source_image.Height (), 1, source_image.Format (), source_image.Bitmap ());
      
      pot_image.Swap (source_image);
    }

    if (!params.output_textures_dir_name.empty () || !params.output_textures_format.empty ())
      source_image.Save (target_path.c_str ());
    
    texture_name_index++;
  }
}

///определение типа слоя
LayerType get_layer_type (const Layer& layer)
{
  size_t resources_count = 0, symbols_count = 0;
  
  for (Layer::FrameList::ConstIterator frame_iter=layer.Frames ().CreateIterator (); frame_iter; ++frame_iter)
  {
    const Frame& frame = *frame_iter;
    
    if (frame.Elements ().Size () != 1)
      return LayerType_Undefined;

    for (Frame::FrameElementList::ConstIterator element_iter=frame.Elements ().CreateIterator (); element_iter; ++element_iter)
      switch (element_iter->Type ())
      {
        case FrameElementType_SymbolInstance:
          symbols_count++;
          break;
        case FrameElementType_ResourceInstance:
          resources_count++;
          break;
        default:
          break;
      }
  }
    
  if (!resources_count && symbols_count)
    return LayerType_Instance;
    
  if (!symbols_count && resources_count == 1)
    return LayerType_Sprite;
    
  if (!symbols_count && resources_count > 1)
    return LayerType_SpriteGroup;
    
  return LayerType_Undefined;
}

///компоновка трека
void write_track
 (ConvertData&             data,
  const PropertyAnimation& x_track,
  const PropertyAnimation& y_track,
  const char*              name_prefix,
  const math::vec2f&       offset,
  const math::vec2f&       scale = math::vec2f (1.0f, 1.0f))
{
  if (!x_track.Keyframes ().Size () || !y_track.Keyframes ().Size ())
    error ("Empty animation tracks for X/Y channels");

  float       prev_time = -1.0f, max_time = stl::max (x_track.Keyframes ()[x_track.Keyframes ().Size ()-1].time, y_track.Keyframes ()[y_track.Keyframes ().Size ()-1].time);
  math::vec2f prev_value;

  for (size_t xi=0, yi=0, xcount=x_track.Keyframes ().Size (), ycount=y_track.Keyframes ().Size (); prev_time < max_time;)
  {
    const PropertyAnimationKeyframe &x_frame = x_track.Keyframes ()[xi], &y_frame = y_track.Keyframes ()[yi];        
    
    if (!xi && !yi)
    {
      prev_time  = stl::min (x_frame.time, y_frame.time);
      prev_value = math::vec2f (x_frame.value, y_frame.value);
    }
    
    XmlWriter::Scope scope (*data.scene_writer, "Key");

    math::vec2f value;
    float       time;

    if (float_compare (x_frame.time, y_frame.time))
    {
      value = math::vec2f (x_frame.value, y_frame.value);
      time  = x_frame.time;

      if (yi != ycount - 1) yi++;
      if (xi != xcount - 1) xi++;
    }
    else
    {
      if (y_frame.time < x_frame.time)
      {
        time    = y_frame.time;
        value.x = float_compare (time, prev_time) ? x_frame.value : prev_value.x + (x_frame.value - prev_value.x) / (x_frame.time - prev_time) * (time - prev_time);
        value.y = y_frame.value;
        
        if (yi != ycount - 1) yi++;
      }
      else
      {
        time    = x_frame.time;
        value.x = x_frame.value;
        value.y = float_compare (time, prev_time) ? y_frame.value : prev_value.y + (y_frame.value - prev_value.y) / (y_frame.time - prev_time) * (time - prev_time);

        if (xi != xcount - 1) xi++;
      }            
    }      

    prev_value = value;
    prev_time  = time;
    value      = (value + offset) * scale;

    data.scene_writer->WriteAttribute ("Time", time);
    data.scene_writer->WriteAttribute ("Value", common::format ("%.3f; %.3f", value.x, value.y).c_str ());
  }
}

void write_track (ConvertData& data, const PropertyAnimation& track, float scale = 1.0f)
{
  for (size_t i=0, count=track.Keyframes ().Size (); i<count; i++)
  {
    const PropertyAnimationKeyframe &frame = track.Keyframes ()[i];

    XmlWriter::Scope scope (*data.scene_writer, "Key");
    
    data.scene_writer->WriteAttribute ("Time", frame.time);
    data.scene_writer->WriteAttribute ("Value", frame.value * scale);
  }
}

//обработка событий
void write_events_track (ConvertData& data, const EventList& events)
{
  XmlWriter::Scope track_scope (*data.scene_writer, "Track");
  
  data.scene_writer->WriteAttribute ("Name", "events");

  for (EventList::const_iterator iter=events.begin (), end=events.end (); iter!=end; ++iter)
  {
    const Event& event = *iter;
    
    XmlWriter::Scope key_scope (*data.scene_writer, "Key");
    
    data.scene_writer->WriteAttribute ("Time", event.time);
    data.scene_writer->WriteAttribute ("Event", event.action.c_str ());
  }
}

//запись треков таймлайн-спрйта
void write_timeline_sprite_tracks (ConvertData& data, const EventList& events)
{
  {
    XmlWriter::Scope size_scope (*data.scene_writer, "Track");
    
    data.scene_writer->WriteAttribute ("Name", "size");
    
    XmlWriter::Scope key_scope (*data.scene_writer, "Key");
    
    data.scene_writer->WriteAttribute ("Time", 0.0f);
    data.scene_writer->WriteAttribute ("Value", "1; 1");
  }
  
  write_events_track (data, events);
}

//запись таймлайн-спрайта
void write_timeline_sprite_data (ConvertData& data, const char* name)
{
  data.scene_writer->WriteAttribute ("Name", name);
  data.scene_writer->WriteAttribute ("Material", "transparent");
  data.scene_writer->WriteAttribute ("Active", "false");
  data.scene_writer->WriteAttribute ("Z", 0);  
}

///общая часть обработки спрайтов
void process_sprite_common
 (Params&            params,
  ConvertData&       data,
  const Frame&       frame,
  const char*        name,
  const math::vec2f& position = math::vec2f (0.0f),
  const math::vec2f& scale = math::vec2f (1.0f))
{
    //получение объекта анимаций
    
  const AnimationCore& animation = frame.Animation ();

    //сохранение положений
  
  const PropertyAnimation *x_track = animation.Properties ().Find ("headContainer.Basic_Motion.Motion_X"),
                          *y_track = animation.Properties ().Find ("headContainer.Basic_Motion.Motion_Y");
                          
  if (x_track && y_track)
  {
    XmlWriter::Scope scope (*data.scene_writer, "Track");
    
    data.scene_writer->WriteAttribute ("Name", params.need_relative ? "offset" : "position");
    
    write_track (data, *x_track, *y_track, name, params.need_relative ? math::vec3f (0.0f) : position,
      math::vec2f (params.need_inverse_x ? -1.0f : 1.0f, params.need_inverse_y ? -1.0f : 1.0f) * scale);
  }
  else if (!equal (position, math::vec2f (.0f), EPSILON) && !params.need_relative)
  {
    XmlWriter::Scope track_scope (*data.scene_writer, "Track");
    
    data.scene_writer->WriteAttribute ("Name", "position");

    XmlWriter::Scope key_scope (*data.scene_writer, "Key");

    data.scene_writer->WriteAttribute ("Time", 0.0f);
    data.scene_writer->WriteAttribute ("Value", common::format ("%.3f; %.3f", params.need_inverse_x ? -position.x : position.x,
      params.need_inverse_y ? -position.y : position.y).c_str ());    
  }
  
    //сохранение масштаба
    
  const PropertyAnimation *x_scale_track = animation.Properties ().Find ("headContainer.Transformation.Scale_X"),
                          *y_scale_track = animation.Properties ().Find ("headContainer.Transformation.Scale_Y");
  
  if (x_scale_track && y_scale_track)
  {
    XmlWriter::Scope scope (*data.scene_writer, "Track");
    
    data.scene_writer->WriteAttribute ("Name", "scale");
    
    write_track (data, *x_scale_track, *y_scale_track, name, math::vec2f (0.0f), math::vec2f (0.01f));
  }
  
    //сохранение угла
    
  const PropertyAnimation *angle_track = animation.Properties ().Find ("headContainer.Basic_Motion.Rotation_Z");
  
  if (angle_track)
  {
    XmlWriter::Scope scope (*data.scene_writer, "Track");

    data.scene_writer->WriteAttribute ("Name", "angle");

    write_track (data, *angle_track, -1.0f);
  }
  
    //сохранение альфа
    
  const PropertyAnimation* alpha_track = animation.Properties ().Find ("headContainer.Colors.Alpha_Color.Alpha_Amount");
  
  if (alpha_track)
  {
    XmlWriter::Scope scope (*data.scene_writer, "Track");
    
    data.scene_writer->WriteAttribute ("Name", "alpha");
    
    write_track (data, *alpha_track, 0.01f);
  }
}

///обработка спрайта
void process_sprite (Params& params, ConvertData& data, const Frame& frame, const char* name, const char* parent)
{
  const FrameElement& element = frame.Elements ()[0u];

  XmlWriter::Scope scope (*data.scene_writer, "Sprite");
  
    //запись имени спрайта
  
  data.scene_writer->WriteAttribute ("Name", name);
  
    //запись материала спрайта
  
  const char* resource_name = element.Name ();
  
  ImageMap::iterator mtl_iter = data.images.find (resource_name);
  
  if (mtl_iter == data.images.end ())
  {
    error ("Bad resource '%s' for sprite '%s'", resource_name, name);
    return;
  }
  
  const ImageDesc& image_desc = mtl_iter->second;
  
  data.scene_writer->WriteAttribute ("Material", get_full_material_name (params, resource_name).c_str ());
  
  if (*parent)
  {
    data.scene_writer->WriteAttribute ("Parent", parent);
    data.scene_writer->WriteAttribute ("ScaleInherit", "true");
    data.scene_writer->WriteAttribute ("OrientationInherit", "true");
    data.scene_writer->WriteAttribute ("PositionSpace", "local");
    data.scene_writer->WriteAttribute ("ScaleSpace", "local");
    data.scene_writer->WriteAttribute ("OrientationSpace", "local");
    data.scene_writer->WriteAttribute ("BindSpace", "parent");
    data.scene_writer->WriteAttribute ("ScalePivotEnabled", "false");
    data.scene_writer->WriteAttribute ("OrientationPivotEnabled", "true");
  }
  
//  if (params.need_relative)
  {
    stl::string sprite_name = get_sprite_name (params, resource_name);
    
    data.scene_writer->WriteAttribute ("Layout", sprite_name.c_str ());
  }

  data.scene_writer->WriteAttribute ("Active", "false");
  
    //сохранение центра вращений
    
  math::vec2f transformation_point = (element.TransformationPoint () - math::vec2f ((float)image_desc.x, (float)image_desc.y)) /
    math::vec2f ((float)image_desc.width, (float)image_desc.height);

  if (params.need_inverse_x)
    transformation_point.x *= -1.0f;

  if (params.need_inverse_y)
    transformation_point.y *= -1.0f;

  stl::string pivot_value_string;

  pivot_value_string = common::format ("%.3f;%.3f", transformation_point.x, transformation_point.y);

  data.scene_writer->WriteAttribute ("PivotPosition", pivot_value_string.c_str ());
  
    //сохранение размера
  
  if (!params.need_relative)
  {
    XmlWriter::Scope track_scope (*data.scene_writer, "Track");
    
    data.scene_writer->WriteAttribute ("Name",  "size");
    
    XmlWriter::Scope key_scope (*data.scene_writer, "Key");    
    
    data.scene_writer->WriteAttribute ("Time",  0.0f);
    data.scene_writer->WriteAttribute ("Value", common::format ("%u; %u", image_desc.width, image_desc.height).c_str ());
  }
  
    //сохранение общей части
    
  math::vec2f position = element.Translation () + math::vec2f (image_desc.width / 2.0f, image_desc.height / 2.0f) +
    math::vec2f (float (image_desc.x), float (image_desc.y));
  
  process_sprite_common (params, data, frame, name, position);
}

///обработка группы спрайтов
void process_sprite_group (Params& params, ConvertData& data, const Layer::FrameList& frames, const char* name_prefix)
{
  size_t frame_index = 1;
  
  for (Layer::FrameList::ConstIterator frame_iter=frames.CreateIterator (); frame_iter; ++frame_iter, ++frame_index)
  {
    const Frame& frame = *frame_iter;

    process_sprite (params, data, frame, common::format ("%s.frame%u", name_prefix, frame_index).c_str (), "");
  }
  
    //сохранение группы
    
  XmlWriter::Scope group_scope (*data.scene_writer, "SpriteGroup");

  data.scene_writer->WriteAttribute ("Name", name_prefix);
  data.scene_writer->WriteAttribute ("Sprites", common::format ("%s.frame%%u; 1; %u", name_prefix, frame_index-1).c_str ());
  data.scene_writer->WriteAttribute ("Fps", data.document.FrameRate ());
  data.scene_writer->WriteAttribute ("Visible", "false");
  
//  process_sprite_common (data, frame, name_prefix);
}

float process_timeline (Params& params, ConvertData& data, const Timeline& timeline, const char* name_prefix, EventList& events);

///обработка вложенного символа (возвращает время окончания анимации включая все вложения)
float process_symbol_instance (Params& params, ConvertData& data, const Frame& frame, const char* name_prefix)
{
  const FrameElement& element     = frame.Elements ()[0u];
  const char*         symbol_name = element.Name ();
  
  if (!data.document.Symbols ().Find (symbol_name))
    error ("Symbol '%s' not found while processing '%s'", symbol_name, name_prefix);
    
    //обработка вложений
    
  EventList events;

  float end_time = process_timeline (params, data, data.document.Symbols ()[symbol_name].Timeline (), name_prefix, events);
  
    //обработка спрайта
    
  XmlWriter::Scope sprite_scope (*data.scene_writer, "Sprite");
  
  write_timeline_sprite_data (data, name_prefix);  
  
    //сохранение центра вращений
    
  math::vec2f transformation_point = element.TransformationPoint ();
  
  if (params.need_relative)
    transformation_point += element.Translation ();
  
  if (params.need_inverse_x)
    transformation_point.x *= -1.0f;

  if (params.need_inverse_y)
    transformation_point.y *= -1.0f;  
    
  stl::string pivot_value_string = common::format ("%.3f;%.3f", transformation_point.x, transformation_point.y);

  data.scene_writer->WriteAttribute ("PivotPosition", pivot_value_string.c_str ());
  data.scene_writer->WriteAttribute ("ScalePivotEnabled", "true");
  data.scene_writer->WriteAttribute ("OrientationPivotEnabled", "true");

  process_sprite_common (params, data, frame, name_prefix, element.Translation ());
  write_timeline_sprite_tracks (data, events);
  
  return end_time;
}

///обработка таймлайна (возвращает время окончания анимации включая все вложения)
float process_timeline (Params& params, ConvertData& data, const Timeline& timeline, const char* parent_name, EventList& events)
{
    //обход слоёв
    
  float end_time = 0.0f;
    
  for (Timeline::LayerList::ConstIterator layer_iter=timeline.Layers ().CreateIterator (); layer_iter; ++layer_iter)
  {
    const Layer& layer = *layer_iter;
    
      //определение типа слоя
    
    LayerType layer_type = get_layer_type (layer);
    
    stl::string name_prefix = *parent_name ? common::format ("%s.%s", parent_name, layer.Name ()) : stl::string (layer.Name ());
    
    switch (layer_type)
    {
      case LayerType_SpriteGroup:
      {
        process_sprite_group (params, data, layer.Frames (), name_prefix.c_str ());
        
          //регистрация события запуска
          
        Event event;          
        
        const Frame& frame = layer.Frames ()[0u];
        
        event.time   = frame.FirstFrame () / data.document.FrameRate ();
        event.action = common::format ("ShowSpriteGroup ('%s')", name_prefix.c_str ());
        
        events.push_back (event);
        
        if (frame.Duration () > 1)
        {
          event.time   += frame.Duration () / data.document.FrameRate ();
          event.action  = common::format ("HideSpriteGroup ('%s')", name_prefix.c_str ());
          
          if (event.time > end_time)
            end_time = event.time;

          events.push_back (event);
        }

        break;
      }
      case LayerType_Sprite:
      {
        process_sprite (params, data, layer.Frames ()[0u], name_prefix.c_str (), parent_name);
        
          //регистрация события запуска
        
        Event event;          
        
        const Frame& frame = layer.Frames ()[0u];
        
        event.time   = frame.FirstFrame () / data.document.FrameRate ();
        event.action = common::format ("ActivateSprite ('%s')", name_prefix.c_str ());
        
        events.push_back (event);
        
        if (frame.Duration () > 1)
        {
          event.time   += frame.Duration () / data.document.FrameRate ();
          event.action  = common::format ("DeactivateSprite ('%s')", name_prefix.c_str ());
          
          if (event.time > end_time)
            end_time = event.time;

          events.push_back (event);
        }
        
        break;
      }
      case LayerType_Instance:
      {
        float symbol_end_time = process_symbol_instance (params, data, layer.Frames ()[0u], name_prefix.c_str ());        
        
        if (symbol_end_time > end_time)
          end_time = symbol_end_time;
        
          //регистрация события запуска
          
        const Frame& frame = layer.Frames ()[0u];          
        
        Event event;          
        
        event.time   = frame.FirstFrame () / data.document.FrameRate ();
        event.action = common::format ("ActivateSprite ('%s')", name_prefix.c_str ());
        
        events.push_back (event);
        
        if (frame.Duration () > 1)
        {
          event.time   += frame.Duration () / data.document.FrameRate ();
          event.action  = common::format ("DeactivateSprite ('%s')", name_prefix.c_str ());
          
          if (event.time > end_time)
            end_time = event.time;

          events.push_back (event);
        }        
        
        break;
      }
      default:
        error ("Undefined layer '%s' type", layer.Name ());
        break;
    }    
  }
  
    //автоматическая деактивация
    
  if (!float_compare (end_time, 0.0f))
  {    
    Event event;

    event.time   = end_time;
    event.action = "DeactivateSprite (sprite)";

    events.push_back (event);
  }
  
  return end_time;
}

void process_timeline (Params& params, ConvertData& data)
{
  if (params.output_scene_file_name.empty ())
    return; //в случае отсутствия файла сцены - экспорт не производится
    
  if (!params.silent)
    printf ("Processing scene...\n");

  data.scene_writer = stl::auto_ptr<XmlWriter> (new XmlWriter (params.output_scene_file_name.c_str ()));
  
  XmlWriter::Scope scope (*data.scene_writer, "AnimationScreenPart");
  
  EventList events;

  process_timeline (params, data, data.document.Timelines ()[0u], data.document.Timelines ()[0u].Name (), events);
  
  XmlWriter::Scope sprite_scope (*data.scene_writer, "Sprite");

  write_timeline_sprite_data (data, data.document.Timelines ()[0u].Name ());  
  write_timeline_sprite_tracks (data, events);
}

//построение списка используемых символов
void build_used_symbols (const Params& params, const Timeline& timeline, UsedResourcesSet& used_symbols)
{
    //построение списка исключенных их конвертации слоёв

  common::StringArray layers_exclude_list = common::split (params.layers_exclude.c_str ());
  
    //обход слоёв

  for (Timeline::LayerList::ConstIterator layer_iter = timeline.Layers ().CreateIterator (); layer_iter; ++layer_iter)
  {
    const Layer& layer = *layer_iter;
    
    if (is_present (layer.Name (), layers_exclude_list))
      continue; //пропускаем исключенные слои
      
      //обход фреймов данного слоя

    for (Layer::FrameList::ConstIterator frame_iter = layer.Frames ().CreateIterator (); frame_iter; ++frame_iter)
    {
        //обход элементов фрейма 
      
      for (Frame::FrameElementList::ConstIterator element_iter = frame_iter->Elements ().CreateIterator (); element_iter; ++element_iter)
      {
        if (element_iter->Type () != FrameElementType_SymbolInstance)
          continue;

        used_symbols.insert (element_iter->Name ());
      }
    }
  }
}

//экспорт
void export_data (Params& params)
{
  if (!common::FileSystem::IsDir (params.xfl_name.c_str ()))
  {
    common::FileSystem::Mount (XFL_MOUNT_POINT, params.xfl_name.c_str (), "zip");
    params.xfl_name = XFL_MOUNT_POINT;
  }

  ConvertData data;
  
    //открытие документа
    
  {  
    Document document (params.xfl_name.c_str ());
    
    if (!document.Timelines ().Size ())
      error ("No timelines in XFL file");
  
    if (document.Timelines ().Size () > 1)
      error ("More than one timeline in XFL file");
    
    data.document.Swap (document);
  }
  
    //построение списка используемых символов

  for (Document::TimelineList::ConstIterator iter = data.document.Timelines ().CreateIterator (); iter; ++iter)
    build_used_symbols (params, *iter, data.used_symbols);
    
    //обработка текстур
    
  process_textures (params, data);
  
    //обработка таймлайна
    
  process_timeline (params, data);
    
    //обработка материалов
    
  process_materials (params, data);
}

//проверка корректности ввода
void validate (Params& params)
{
  if (params.xfl_name.empty ())
  {
    printf ("%s [<OPTIONS>] <SOURCE>\n  use: %s --help\n", APPLICATION_NAME, APPLICATION_NAME);
    error ("no input file");
    return;
  }
}

int main (int argc, const char *argv[])
{
  try
  {
      //инициализация
    Params params;

    params.options           = 0;
    params.options_count     = 0;
    params.crop_alpha        = 0;
    params.silent            = false;
    params.print_help        = false;
    params.need_pot_extent   = false;
    params.need_crop_alpha   = false;
    params.need_inverse_x    = false;
    params.need_inverse_y    = false;
    params.need_relative     = false;

      //разбор командной строки
    command_line_parse (argc, argv, params);

      // --help только печатает сообщение помощи
    if (params.print_help)
      return 0;

      //проверка корректности ввода
    validate (params);

      //экспорт
    export_data (params);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n",exception.what ()); 
  }

  return 0;
}
