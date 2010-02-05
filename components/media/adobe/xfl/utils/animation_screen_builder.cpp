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

const char* APPLICATION_NAME            = "animation_screen_builder";
const char* MOTION_X_ANIMATION_PROPERTY = "headContainer.Basic_Motion.Motion_X";
const char* MOTION_Y_ANIMATION_PROPERTY = "headContainer.Basic_Motion.Motion_Y";
const char* XFL_MOUNT_POINT             = "/mount_points/animation.xfl";

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
  const Option* options;                  //массив опций
  size_t        options_count;            //количество опций
  stl::string   xfl_name;                 //имя исходного файла или папки
  stl::string   output_textures_dir_name; //имя каталога с сохранёнными текстурами
  stl::string   textures_format;          //формат текстур
  stl::string   materials_file_name;      //файл материалов
  stl::string   animation_dir_name;       //папка файлов анимаций
  stl::string   crop_exclude;             //необрезаемые слои
  stl::string   layers_exclude;           //неэкспортируемые слои
  size_t        crop_alpha;               //коэффициент обрезания по прозрачности
  size_t        resize_width;             //целевой размер для пережатия текстур
  size_t        resize_height;            //целевой размер для пережатия текстур
  float         left_x;                   //целевая координата левого края
  float         right_x;                  //целевая координата правого края
  float         top_y;                    //целевая координата верхнего края
  float         bottom_y;                 //целевая координата нижнего края
  float         target_width;             //целевая ширина
  float         target_height;            //целевая высота
  bool          silent;                   //минимальное число сообщений
  bool          print_help;               //нужно ли печатать сообщение помощи
  bool          need_pot_extent;          //нужно ли расширять изображения до ближайшей степени двойки
  bool          need_crop_alpha;          //нужно ли обрезать картинку по нулевой прозрачности
};

//прямоугольная область
struct Rect
{
  size_t x, y;
  size_t width, height;
};

struct Vec2fKeyframe
{
  float time;
  vec2f value;
};

struct MaterialInstance
{
  stl::string material_id;
  stl::string source_material;

  MaterialInstance (const char* in_material_id, const char* in_source_material)
    : material_id (in_material_id), source_material (in_source_material)
    {}
};

typedef stl::hash_set<stl::hash_key<const char*> >                  UsedResourcesSet;
typedef stl::hash_set<stl::hash_key<const char*> >                  FullscreenSymbolsSet;
typedef stl::hash_map<stl::hash_key<const char*>, vec2ui>           ResourceTilingMap;
typedef stl::hash_map<stl::hash_key<const char*>, vec2ui>           ResourceDimensionsMap;
typedef stl::hash_map<stl::hash_key<const char*>, vec2ui>           ResourceCropMap;
typedef stl::hash_map<size_t, stl::string>                          ImagesHashesMap;
typedef stl::multimap<float, stl::string, stl::less<float> >        ActivateSpritesMap;
typedef stl::hash_map<stl::hash_key<const char*>, MaterialInstance> MaterialInstancesMap;
typedef CollectionImpl<Vec2fKeyframe, ICollection<Vec2fKeyframe> >  Vec2fKeyframes;

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

//установка формата сохранения текстур
void command_line_result_textures_format (const char* value, Params& params)
{
  params.textures_format = value;
}

//установка файла материалов
void command_line_materials_file_name (const char* file_name, Params& params)
{
  params.materials_file_name = file_name;
}

//установка папки анимаций
void command_line_animation_dir_name (const char* file_name, Params& params)
{
  params.animation_dir_name = file_name;
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

//установка целевого размера экрана для пережатия текстур
void command_line_resize_width (const char* value, Params& params)
{
  params.resize_width = atoi (value);
}

//установка целевого размера экрана для пережатия текстур
void command_line_resize_height (const char* value, Params& params)
{
  params.resize_height = atoi (value);
}

//установка левой координаты для перерасчета анимаций
void command_line_left_x (const char* value, Params& params)
{
  params.left_x = static_cast<float> (atof (value));
}

//установка правой координаты для перерасчета анимаций
void command_line_right_x (const char* value, Params& params)
{
  params.right_x = static_cast<float> (atof (value));
}

//установка верхней координаты для перерасчета анимаций
void command_line_top_y (const char* value, Params& params)
{
  params.top_y = static_cast<float> (atof (value));
}

//установка нижней координаты для перерасчета анимаций
void command_line_bottom_y (const char* value, Params& params)
{
  params.bottom_y = static_cast<float> (atof (value));
}

//установка ширины виртуального экрана для перерасчета анимаций
void command_line_target_width (const char* value, Params& params)
{
  params.target_width = static_cast<float> (atof (value));
}

//установка высоты виртуального экрана для перерасчета анимаций
void command_line_target_height (const char* value, Params& params)
{
  params.target_height = static_cast<float> (atof (value));
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

//разбор командной строки
void command_line_parse (int argc, const char* argv [], Params& params)
{
  static Option options [] = {
    {command_line_result_textures_dir,        "textures-dir",          'o', "dir",       "set output textures directory"},
    {command_line_result_textures_format,     "textures-format",       0,   "string",    "set output textures format string"},
    {command_line_materials_file_name,        "materials-file",        0,   "file",      "set output materials file"},
    {command_line_animation_dir_name,         "animation-dir",         0,   "dir",       "set output animation directory"},
    {command_line_crop_exclude,               "crop-exclude",          0,   "wildcards", "exclude selected layers from crop"},
    {command_line_layers_exclude,             "layers-exclude",        0,   "wildcards", "exclude selected layers from export"},
    {command_line_resize_width,               "resize-width",          0,   "value",     "set target screen width for texture resizing"},
    {command_line_resize_height,              "resize-height",         0,   "value",     "set target screen height for texture resizing"},
    {command_line_left_x,                     "left-x",                0,   "value",     "set left x coordinate for animation calculation"},
    {command_line_right_x,                    "right-x",               0,   "value",     "set right x coordinate for animation calculation"},
    {command_line_top_y,                      "top-y",                 0,   "value",     "set top y coordinate for animation calculation"},
    {command_line_bottom_y,                   "bottom-y",              0,   "value",     "set bottom y coordinate for animation calculation"},
    {command_line_target_width,               "target-width",          0,   "value",     "set target width for animation calculation"},
    {command_line_target_height,              "target-height",         0,   "value",     "set target height for animation calculation"},
    {command_line_silent,                     "silent",                's', 0,           "quiet mode"},
    {command_line_help,                       "help",                  '?', 0,           "print help message"},
    {command_line_pot,                        "pot",                   0,   0,           "extent textures size to nearest greater power of two"},
    {command_line_crop_alpha,                 "crop-alpha",            0,   "value",     "crop layers by alpha that less than value"},
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
      error ("option '%s' has not arguments", option_name.c_str ());

      return;
    }

    if (option->process)
      option->process (option_argument.c_str (), params);
  }
}

void save_materials (const Params& params, const Document& document, const ResourceTilingMap& tiling_map, const MaterialInstancesMap& material_instances)
{
  stl::string xmtl_name = params.materials_file_name;

  if (xmtl_name.empty ())
  {
    stl::string document_base_name = common::notdir (document.Name ());

    xmtl_name = common::format ("%s.xmtl", document_base_name.c_str ());
  }

  XmlWriter writer (xmtl_name.c_str ());

  XmlWriter::Scope scope (writer, "material_library");

  for (Document::ResourceList::ConstIterator iter = document.Resources ().CreateIterator (); iter; ++iter)
  {
    if (material_instances.find (iter->Name ()) != material_instances.end ())
      continue;

    XmlWriter::Scope material_scope (writer, "material");

    writer.WriteAttribute ("id", iter->Name ());

    XmlWriter::Scope sprite_profile (writer, "sprite_profile");

    writer.WriteAttribute ("image", iter->Path ());
    writer.WriteAttribute ("blend_mode", "translucent");

    ResourceTilingMap::const_iterator tiling = tiling_map.find (iter->Name ());

    if (tiling != tiling_map.end ())
    {
      writer.WriteAttribute ("tiling", "1");
      writer.WriteAttribute ("tile_width", tiling->second.x);
      writer.WriteAttribute ("tile_height", tiling->second.y);
    }
  }

  for (MaterialInstancesMap::const_iterator iter = material_instances.begin (), end = material_instances.end (); iter != end; ++iter)
  {
    XmlWriter::Scope material_scope (writer, "instance_material");

    writer.WriteAttribute ("id",     iter->second.material_id.c_str ());
    writer.WriteAttribute ("source", iter->second.source_material.c_str ());
  }
}

void write_track_key (XmlWriter& writer, float time, const char* value, const char* value_attribute = "Value")
{
  XmlWriter::Scope key_scope (writer, "Key");

  writer.WriteAttribute ("Time",  time);
  writer.WriteAttribute (value_attribute, value);
}

void write_track_key (XmlWriter& writer, float time, vec2f value)
{
  stl::string value_string = common::format ("%f;%f", value.x, value.y);

  write_track_key (writer, time, value_string.c_str ());
}

void write_track_key (XmlWriter& writer, float time, float value)
{
  stl::string value_string = common::format ("%f", value);

  write_track_key (writer, time, value_string.c_str ());
}

void compose_vec2f_keyframes (const PropertyAnimation::KeyframeList& x_keyframes, const PropertyAnimation::KeyframeList& y_keyframes, Vec2fKeyframes& keyframes)
{
  keyframes.Reserve (stl::max (x_keyframes.Size (), y_keyframes.Size ()));

  size_t x_keyframes_count        = x_keyframes.Size (),
         y_keyframes_count        = y_keyframes.Size (),
         current_x_keyframe_index = 0,
         current_y_keyframe_index = 0;

  PropertyAnimation::KeyframeList::ConstIterator current_x_keyframe = x_keyframes.CreateIterator (),
                                                 current_y_keyframe = y_keyframes.CreateIterator ();

  float last_exported_time = -1;

  while (current_x_keyframe_index < x_keyframes_count || current_y_keyframe_index < y_keyframes_count)
  {
    Vec2fKeyframe new_keyframe;

    if ((current_x_keyframe_index >= x_keyframes_count) || (current_x_keyframe->time > current_y_keyframe->time && current_y_keyframe_index < y_keyframes_count))
    {
      if (current_y_keyframe->time > last_exported_time + EPSILON)
      {
        last_exported_time = current_y_keyframe->time;

        new_keyframe.time = current_y_keyframe->time;

        new_keyframe.value.y = current_y_keyframe->value;

        if (!current_x_keyframe_index || (current_x_keyframe_index >= x_keyframes_count))
          new_keyframe.value.x = current_x_keyframe->value;
        else
        {
          const PropertyAnimationKeyframe& previous_x_keyframe = x_keyframes [current_x_keyframe_index - 1];

          new_keyframe.value.x = previous_x_keyframe.value + (current_x_keyframe->value - previous_x_keyframe.value) / (current_x_keyframe->time - previous_x_keyframe.time) * (current_y_keyframe->time - previous_x_keyframe.time);
        }

        keyframes.Add (new_keyframe);
      }

      current_y_keyframe_index++;

      if (current_y_keyframe_index < y_keyframes_count)
        ++current_y_keyframe;
    }
    else
    {
      if (current_x_keyframe->time > last_exported_time + EPSILON)
      {
        last_exported_time = current_x_keyframe->time;

        new_keyframe.time = current_x_keyframe->time;

        new_keyframe.value.x = current_x_keyframe->value;

        if (!current_y_keyframe_index || (current_y_keyframe_index >= y_keyframes_count))
          new_keyframe.value.y = current_y_keyframe->value;
        else
        {
          const PropertyAnimationKeyframe& previous_y_keyframe = y_keyframes [current_y_keyframe_index - 1];

          new_keyframe.value.y = previous_y_keyframe.value + (current_y_keyframe->value - previous_y_keyframe.value) / (current_y_keyframe->time - previous_y_keyframe.time) * (current_x_keyframe->time - previous_y_keyframe.time);
        }

        keyframes.Add (new_keyframe);
      }

      current_x_keyframe_index++;

      if (current_x_keyframe_index < x_keyframes_count)
        ++current_x_keyframe;
    }
  }
}

void preprocess_symbols (const Params& params, Document& document, const UsedResourcesSet& used_symbols,
                         ResourceTilingMap& tiling_map, MaterialInstancesMap& material_instances,
                         ResourceDimensionsMap& resources_dimensions, FullscreenSymbolsSet& fullscreen_symbols,
                         ResourceCropMap& resource_crops)
{
  static const char* METHOD_NAME = "preprocess_symbols";

  if (!params.silent)
    printf ("preprocessing symbols...\n");

  float resize_x_factor = params.resize_width ? params.resize_width / (float)document.Width () : 1.f,
        resize_y_factor = params.resize_height ? params.resize_height / (float)document.Height () : 1.f;

  UsedResourcesSet used_resources;
  ImagesHashesMap  duplicate_images;

  if (!FileSystem::IsDir (params.output_textures_dir_name.c_str ()))
    FileSystem::Mkdir (params.output_textures_dir_name.c_str ());

  common::StringArray crop_exclude_list = common::split (params.crop_exclude.c_str ());

  Document::ResourceList& resources = document.Resources ();

  for (size_t symbol_index = 0; symbol_index < document.Symbols ().Size (); symbol_index++)
  {
    const Symbol& symbol = ((ICollection<Symbol>&)document.Symbols ()) [symbol_index];

    if (used_symbols.find (symbol.Name ()) == used_symbols.end ())
      continue;

    const Timeline& symbol_timeline = symbol.Timeline ();

    if (symbol_timeline.Layers ().Size () > 1)
      xtl::format_operation_exception (METHOD_NAME, "Can't process symbol '%s', it has more than one layer", symbol.Name ());

    bool need_crop_alpha = params.need_crop_alpha;

    if (need_crop_alpha)
    {
      for (size_t i = 0, count = crop_exclude_list.Size (); i < count; i++)
      {
        const char* mask = crop_exclude_list [i];

        if (common::wcmatch (symbol.Name (), mask))
        {
          need_crop_alpha = false;
          break;
        }
      }
    }

    Layer&              symbol_layer       = ((ICollection<Layer>&)symbol_timeline.Layers ()) [0];
    Frame&              first_symbol_frame = ((ICollection<Frame>&)symbol_layer.Frames ()) [0];
    const FrameElement& resource_element   = ((ICollection<FrameElement>&)first_symbol_frame.Elements ()) [0];
    const Resource&     resource           = resources [resource_element.Name ()];

    size_t bitmaps_count = 0;

    for (Layer::FrameList::ConstIterator symbol_frame_iter = symbol_layer.Frames ().CreateIterator (); symbol_frame_iter; ++symbol_frame_iter)
      for (Frame::FrameElementList::ConstIterator symbol_element_iter = symbol_frame_iter->Elements ().CreateIterator (); symbol_element_iter; ++symbol_element_iter, bitmaps_count++)
        if (symbol_element_iter->Type () != FrameElementType_ResourceInstance)
          throw xtl::format_operation_exception (METHOD_NAME, "Can't process symbol '%s', symbol contains elements other than resource instance", symbol.Name ());

    if (!bitmaps_count)
      throw xtl::format_operation_exception (METHOD_NAME, "Can't process symbol '%s', referenced symbol elements has no resource instance", symbol.Name ());

    Image image (resource.Path ());

    size_t image_width  = image.Width (),
           image_height = image.Height ();

    if (image_width == document.Width () && image_height == document.Height ())
      fullscreen_symbols.insert (symbol.Name ());

    stl::string resource_dir = common::dir (resource.Name ()),
                save_folder_name;

    if (resource_dir == "./")
      save_folder_name = common::format ("%s/", params.output_textures_dir_name.c_str ());
    else
      save_folder_name = common::format ("%s/%s", params.output_textures_dir_name.c_str (), resource_dir.c_str ());

    size_t check_frame = 1;

    Layer::FrameList &layer_frames = symbol_layer.Frames ();

    stl::string correct_element_name;

    PropertyAnimation motion_x_animation, motion_y_animation;

    motion_x_animation.SetName (MOTION_X_ANIMATION_PROPERTY);
    motion_y_animation.SetName (MOTION_Y_ANIMATION_PROPERTY);

    PropertyAnimation::KeyframeList &motion_x_animation_keyframes = motion_x_animation.Keyframes (),
                                    &motion_y_animation_keyframes = motion_y_animation.Keyframes ();

    float last_x_motion = 0, last_y_motion = 0, first_frame_crop_x = 0, first_frame_crop_y = 0;

    for (Layer::FrameList::Iterator symbol_frame_iter = layer_frames.CreateIterator (); symbol_frame_iter; ++symbol_frame_iter)
    {
      size_t symbol_frame_first_frame = symbol_frame_iter->FirstFrame ();

      for (Frame::FrameElementList::Iterator symbol_element_iter = symbol_frame_iter->Elements ().CreateIterator (); symbol_element_iter; ++symbol_element_iter, check_frame++)
      {
        if (symbol_element_iter->Type () != FrameElementType_ResourceInstance)
          throw xtl::format_operation_exception (METHOD_NAME, "Can't process symbol '%s', symbol contains elements other than resource instance", symbol.Name ());

        if (symbol_frame_first_frame != check_frame - 1) //генерация промежуточных кадров если анимация содержит разрывы
        {
          if (check_frame == 1)
            throw xtl::format_operation_exception (METHOD_NAME, "Can't process symbol '%s', referenced frame animation starts not from 0 frame (%u)", symbol.Name (), symbol_frame_first_frame);

          for (size_t instance_index = check_frame, last_instance_index = symbol_frame_first_frame + 1; instance_index < last_instance_index; instance_index++)
          {
            stl::string source_material_name;

            MaterialInstancesMap::const_iterator instance_iter = material_instances.find (correct_element_name.c_str ());

            if (instance_iter == material_instances.end ())
              source_material_name = correct_element_name;
            else
              source_material_name = instance_iter->second.source_material;

            stl::string instance_element_name = common::format ("%s%u.%s", save_folder_name.c_str (), instance_index, params.textures_format.c_str ());

            material_instances.insert_pair (instance_element_name.c_str (), MaterialInstance (instance_element_name.c_str (), source_material_name.c_str ()));

            Resource new_resource;

            new_resource.SetName (instance_element_name.c_str ());
            new_resource.SetPath (source_material_name.c_str ());

            resources.Add (new_resource);

            PropertyAnimationKeyframe motion_x_keyframe, motion_y_keyframe;

            motion_x_keyframe.value = last_x_motion;
            motion_y_keyframe.value = last_y_motion;
            motion_x_keyframe.time  = motion_y_keyframe.time = (instance_index - 1) / document.FrameRate ();

            motion_x_animation_keyframes.Add (motion_x_keyframe);
            motion_y_animation_keyframes.Add (motion_y_keyframe);
          }

          check_frame = symbol_frame_first_frame + 1;
        }

        const Resource& frame_resource = resources [symbol_element_iter->Name ()];

        Image frame (frame_resource.Path ());

        size_t frame_width  = frame.Width (),
               frame_height = frame.Height ();

        if (frame_width != image_width || frame_height != image_height)
          throw xtl::format_operation_exception (METHOD_NAME, "Can't process symbol '%s', symbol contains animations with different frame size", symbol.Name ());

        Rect crop_rect;

        memset (&crop_rect, 0, sizeof (crop_rect));

        if (need_crop_alpha)
        {
          crop_by_alpha (frame, params.crop_alpha, crop_rect.x, crop_rect.y, crop_rect.width, crop_rect.height);

          if (!crop_rect.width || !crop_rect.height)
          {
            crop_rect.x      = 0;
            crop_rect.y      = 0;
            crop_rect.width  = 1;
            crop_rect.height = 1;
          }
        }
        else
        {
          crop_rect.width  = image_width;
          crop_rect.height = image_height;
        }

        size_t resized_crop_width, resized_crop_height;

        if (params.resize_width)
        {
          resized_crop_width  = stl::max ((size_t)ceil(crop_rect.width  * resize_x_factor), (size_t)1);
          resized_crop_height = stl::max ((size_t)ceil(crop_rect.height * resize_y_factor), (size_t)1);
        }
        else
        {
          resized_crop_width  = crop_rect.width;
          resized_crop_height = crop_rect.height;
        }

        xtl::uninitialized_storage<char> image_copy_buffer (get_bytes_per_pixel (image.Format ()) * crop_rect.width * crop_rect.height);

        frame.GetImage (crop_rect.x, crop_rect.y, 0, crop_rect.width, crop_rect.height, 1, frame.Format (), image_copy_buffer.data ());

        size_t cropped_image_hash = common::crc32 (image_copy_buffer.data (), image_copy_buffer.size (), 0) + crop_rect.width * 2 + crop_rect.height * 3;

        ImagesHashesMap::iterator duplicate_image_iter = duplicate_images.find (cropped_image_hash);

        if (bitmaps_count > 1)
          correct_element_name = common::format ("%s%u.%s", save_folder_name.c_str (), check_frame, params.textures_format.c_str ());
        else
        {
          stl::string resource_file_base_name = common::notdir (common::basename (resource.Path ()));
          correct_element_name = common::format ("%s%s.%s", save_folder_name.c_str (), resource_file_base_name.c_str (), params.textures_format.c_str ());
        }

        Resource new_resource;

        new_resource.SetName (correct_element_name.c_str ());
        new_resource.SetPath (correct_element_name.c_str ());

        if (duplicate_image_iter == duplicate_images.end ())
        {
          size_t new_image_width  = resized_crop_width,
                 new_image_height = resized_crop_height;

          if (params.need_pot_extent)
          {
            new_image_width  = get_next_higher_power_of_two (new_image_width);
            new_image_height = get_next_higher_power_of_two (new_image_height);
          }

          if (params.resize_width)
          {
            Image resizing_image (crop_rect.width, crop_rect.height, 1, frame.Format (), image_copy_buffer.data ());

            resizing_image.Resize (resized_crop_width, resized_crop_height, 1);

            resizing_image.GetImage (0, 0, 0, resized_crop_width, resized_crop_height, 1, frame.Format (), image_copy_buffer.data ());
          }

          Image cropped_frame (new_image_width, new_image_height, 1, frame.Format ());

          memset (cropped_frame.Bitmap (), 0, new_image_width * new_image_height * get_bytes_per_pixel (frame.Format ()));

          cropped_frame.PutImage (0, new_image_height - resized_crop_height, 0, resized_crop_width, resized_crop_height, 1, frame.Format (), image_copy_buffer.data ());

          if (!FileSystem::IsDir (save_folder_name.c_str ()))
            FileSystem::Mkdir (save_folder_name.c_str ());

          cropped_frame.Save (correct_element_name.c_str ());

          if (resized_crop_width != new_image_width || resized_crop_height != new_image_height)
            tiling_map.insert_pair (correct_element_name.c_str (), vec2ui (resized_crop_width, resized_crop_height));

          duplicate_images.insert_pair (cropped_image_hash, correct_element_name);

          resources_dimensions.insert_pair (correct_element_name.c_str (), math::vec2ui (resized_crop_width, resized_crop_height));
        }
        else if (correct_element_name != duplicate_image_iter->second && !resources.Find (correct_element_name.c_str ()))
        {
          material_instances.insert_pair (correct_element_name.c_str (), MaterialInstance (correct_element_name.c_str (), duplicate_image_iter->second.c_str ()));
          resources_dimensions.insert_pair (correct_element_name.c_str (), resources_dimensions [duplicate_image_iter->second.c_str ()]);
        }

        if (!resources.Find (correct_element_name.c_str ()))
          resources.Add (new_resource);

        used_resources.insert (new_resource.Name ());

        symbol_element_iter->SetName (correct_element_name.c_str ());

          //update transformation and translation
        vec2f current_transformation_point = symbol_element_iter->TransformationPoint (),
              current_translation          = symbol_element_iter->Translation ();

        if (check_frame == 1)
        {
          first_frame_crop_x = crop_rect.x;
          first_frame_crop_y = crop_rect.y;
        }

        last_x_motion = current_translation.x + crop_rect.x - first_frame_crop_x;
        last_y_motion = current_translation.y + crop_rect.y - first_frame_crop_y;

        if ((bitmaps_count > 1) && (!float_compare (current_transformation_point.x, 0) || !float_compare (current_transformation_point.y, 0)))
          throw xtl::format_not_supported_exception (METHOD_NAME, "Can't process symbol '%s', animated sprites transformation point not supported", symbol.Name ());

        PropertyAnimationKeyframe motion_x_keyframe, motion_y_keyframe;

        motion_x_keyframe.value = last_x_motion;
        motion_y_keyframe.value = last_y_motion;
        motion_x_keyframe.time  = motion_y_keyframe.time = (check_frame - 1) / document.FrameRate ();

        motion_x_animation_keyframes.Add (motion_x_keyframe);
        motion_y_animation_keyframes.Add (motion_y_keyframe);

        resource_crops.insert_pair (correct_element_name.c_str (), vec2ui (crop_rect.x, crop_rect.y));
      }
    }

    AnimationCore frame_animation (first_symbol_frame.Animation ());

    frame_animation.SetDuration (first_symbol_frame.Duration ());

    AnimationCore::PropertyAnimationList& animated_properties = frame_animation.Properties ();

    if (animated_properties.Find (MOTION_X_ANIMATION_PROPERTY) || animated_properties.Find (MOTION_Y_ANIMATION_PROPERTY))
      throw xtl::format_not_supported_exception (METHOD_NAME, "Can't process symbol '%s', symbol frame animation not supported", symbol.Name ());

    animated_properties.Add (motion_x_animation);
    animated_properties.Add (motion_y_animation);

    first_symbol_frame.SetAnimation (frame_animation);
  }

  for (size_t i = 0; i < resources.Size ();)
  {
    if (used_resources.find (((ICollection<Resource>&)resources) [i].Name ()) == used_resources.end ())
      resources.Remove (i);
    else
      i++;
  }
}

void save_timeline (const Params& params, const Document& document, const Timeline& timeline,
                    const ResourceDimensionsMap& resources_dimensions,
                    const FullscreenSymbolsSet& fullscreen_symbols,
                    const ResourceCropMap& resource_crops)
{
  if (!params.silent)
    printf ("saving timeline...\n");

  stl::string xml_name;

  if (params.animation_dir_name.empty ())
    xml_name = common::format ("%s.xml", timeline.Name ());
  else
  {
    if (!FileSystem::IsDir (params.animation_dir_name.c_str ()))
      FileSystem::Mkdir (params.animation_dir_name.c_str ());

    xml_name = common::format ("%s/%s.xml", params.animation_dir_name.c_str (), timeline.Name ());
  }

  XmlWriter writer (xml_name.c_str ());

  XmlWriter::Scope screen_scope (writer, "AnimationScreenPart");

  size_t document_width    = document.Width (),
         document_height   = document.Height ();
  int    target_width_abs  = abs (static_cast<int> (params.target_width)),
         target_height_abs = abs (static_cast<int> (params.target_height));
  float  x_scale           = static_cast<float> (params.target_width) / document_width,
         y_scale           = static_cast<float> (params.target_height) / document_height,
         resize_x_factor   = params.resize_width ? params.resize_width / static_cast<float> (document_width) : 1.f,
         resize_y_factor   = params.resize_height ? params.resize_height / static_cast<float> (document_height) : 1.f;

  ActivateSpritesMap activate_sprites_info;

  common::StringArray layers_exclude_list = common::split (params.layers_exclude.c_str ());

  for (int i = timeline.Layers ().Size () - 1; i >= 0; i--)
  {
    const Layer& layer = ((ICollection<Layer>&)timeline.Layers ()) [i];

    bool ignore_layer = false;

    for (size_t j = 0, count = layers_exclude_list.Size (); j < count; j++)
    {
      const char* mask = layers_exclude_list [j];

      if (common::wcmatch (layer.Name (), mask))
      {
        ignore_layer = true;
        break;
      }
    }

    if (ignore_layer)
      continue;

    for (Layer::FrameList::ConstIterator frame_iter = layer.Frames ().CreateIterator (); frame_iter; ++frame_iter)
      for (Frame::FrameElementList::ConstIterator element_iter = frame_iter->Elements ().CreateIterator (); element_iter; ++element_iter)
      {
        if (element_iter->Type () != FrameElementType_SymbolInstance)
          continue;

          //Поиск и верификация картинок
        const Symbol* symbol = document.Symbols ().Find (element_iter->Name ());

        if (!symbol)
        {
          if (!params.silent)
            printf ("Can't process animation for frame element '%s' of layer '%s', references symbol not found\n", element_iter->Name (), layer.Name ());
          continue;
        }

        float begin_time = frame_iter->FirstFrame () / document.FrameRate (),
              duration   = frame_iter->Duration () / document.FrameRate ();

        const Timeline&     symbol_timeline    = symbol->Timeline ();
        const Layer&        symbol_layer       = ((const ICollection<Layer>&)symbol_timeline.Layers ()) [0];
        const Frame&        first_symbol_frame = ((const ICollection<Frame>&)symbol_layer.Frames ()) [0];
        const FrameElement& resource_element   = ((const ICollection<FrameElement>&)first_symbol_frame.Elements ()) [0];
        const Resource&     resource           = document.Resources () [resource_element.Name ()];

        ResourceDimensionsMap::const_iterator dimensions_iter = resources_dimensions.find (resource.Path ());
        ResourceCropMap::const_iterator       crop_iter       = resource_crops.find (resource.Path ());

        if (dimensions_iter == resources_dimensions.end ())
        {
          if (!params.silent)
            printf ("Can't find dimensions for resource '%s'\n", resource.Path ());
          continue;
        }

        if (crop_iter == resource_crops.end ())
        {
          if (!params.silent)
            printf ("Can't find crop info for resource '%s'\n", resource.Path ());
          continue;
        }

        size_t image_width  = dimensions_iter->second.x,
               image_height = dimensions_iter->second.y;

        if (!image_width || !image_height)
        {
          if (!params.silent)
            printf ("Resource '%s' has zero dimensions, skipping\n", resource.Path ());
          continue;
        }

        XmlWriter::Scope sprite_scope (writer, "Sprite");

        writer.WriteAttribute ("Active", "false");
        writer.WriteAttribute ("Name", element_iter->Name ());

        activate_sprites_info.insert_pair (begin_time, common::format ("ActivateSprite('%s')", element_iter->Name ()));

        size_t bitmaps_count = symbol_layer.Frames () [symbol_layer.Frames ().Size () - 1].FirstFrame () + 1;

        vec2f transformation_point (element_iter->TransformationPoint ().x + resource_element.TransformationPoint ().x,
                                    element_iter->TransformationPoint ().y + resource_element.TransformationPoint ().y);

        int additional_x_offset = 0, additional_y_offset = 0;

        if (bitmaps_count == 1)
        {
          writer.WriteAttribute ("Material", resource.Name ());

          if (fullscreen_symbols.find (symbol->Name ()) != fullscreen_symbols.end () && !element_iter->TransformationPoint ().x && !element_iter->TransformationPoint ().y)
          {
            additional_x_offset -= document.Width () / 2.f;
            additional_y_offset -= document.Height () / 2.f;
          }
        }
        else
        {
          stl::string resource_dir              = common::dir (resource.Name ()),
                      resource_extension        = common::suffix (resource.Path ()),
                      material_animation_string = common::format ("%s%%u%s; 1; %u", resource_dir.c_str (), resource_extension.c_str (), bitmaps_count),
                      fps_string                = common::format ("%f", document.FrameRate ());

          writer.WriteAttribute ("MaterialAnimation", material_animation_string);
          writer.WriteAttribute ("Fps",               fps_string);
          writer.WriteAttribute ("RepeatMode",        "clamp_to_end");
        }

        stl::string pivot_value_string;

        if ((bitmaps_count > 1) || (fullscreen_symbols.find (symbol->Name ()) != fullscreen_symbols.end () && !element_iter->TransformationPoint ().x && !element_iter->TransformationPoint ().y))
          pivot_value_string = "-0.5;-0.5;0";
        else
        {
          pivot_value_string = common::format ("%f;%f;0", (transformation_point.x - crop_iter->second.x) / image_width * resize_x_factor - 0.5f,
                                                          (transformation_point.y - crop_iter->second.y) / image_height * resize_y_factor - 0.5f);
        }

        writer.WriteAttribute ("PivotPosition", pivot_value_string.c_str());

          //Сохранение треков
        {
          XmlWriter::Scope track_scope (writer, "Track");

          writer.WriteAttribute ("Name", "size");

          write_track_key (writer, 0, vec2f (image_width / resize_x_factor * target_width_abs / (float)document.Width (), image_height / resize_y_factor * target_height_abs / (float)document.Height ()));
        }

        {
          XmlWriter::Scope track_scope (writer, "Track");

          writer.WriteAttribute ("Name", "alpha");

          const PropertyAnimation&               alpha_track = frame_iter->Animation ().Properties () ["headContainer.Colors.Alpha_Color.Alpha_Amount"];
          const PropertyAnimation::KeyframeList& keyframes   = alpha_track.Keyframes ();

          if (alpha_track.Enabled ())
          {
            for (PropertyAnimation::KeyframeList::ConstIterator keyframe_iter = keyframes.CreateIterator (); keyframe_iter; ++keyframe_iter)
              write_track_key (writer, keyframe_iter->time, keyframe_iter->value / 100.f);
          }
        }

        {
          XmlWriter::Scope track_scope (writer, "Track");

          writer.WriteAttribute ("Name", "position");

          const PropertyAnimation               &motion_x_track = frame_iter->Animation ().Properties () [MOTION_X_ANIMATION_PROPERTY],
                                                &motion_y_track = frame_iter->Animation ().Properties () [MOTION_Y_ANIMATION_PROPERTY];
          const PropertyAnimation::KeyframeList &x_keyframes    = motion_x_track.Keyframes (),
                                                &y_keyframes    = motion_y_track.Keyframes ();

          vec2f base_point;

          if (bitmaps_count > 1)
            base_point = vec2f (params.left_x + (crop_iter->second.x + element_iter->Translation ().x + resource_element.Translation ().x) / document.Width () * params.target_width,
                                params.top_y - (document.Height () - crop_iter->second.y + element_iter->Translation ().y + resource_element.Translation ().y) / document.Height () * params.target_height);
          else
            base_point = vec2f (params.left_x + (transformation_point.x + element_iter->Translation ().x + resource_element.Translation ().x) / document.Width () * params.target_width,
                                params.top_y - (transformation_point.y + element_iter->Translation ().y + resource_element.Translation ().y) / document.Height () * params.target_height);

          if (!motion_x_track.Enabled () || !motion_y_track.Enabled ())
            write_track_key (writer, 0, base_point);
          else
          {
            Vec2fKeyframes keyframes;

            compose_vec2f_keyframes (x_keyframes, y_keyframes, keyframes);

            for (Vec2fKeyframes::ConstIterator iter = keyframes.CreateIterator (); iter; ++iter)
              write_track_key (writer, iter->time, base_point + vec2f (iter->value.x * x_scale, -iter->value.y * y_scale));
          }
        }

        {
          const PropertyAnimation *motion_x_track = first_symbol_frame.Animation ().Properties ().Find (MOTION_X_ANIMATION_PROPERTY),
                                  *motion_y_track = first_symbol_frame.Animation ().Properties ().Find (MOTION_Y_ANIMATION_PROPERTY);

          if (motion_x_track && motion_y_track && motion_x_track->Enabled () && motion_y_track->Enabled ())
          {
            XmlWriter::Scope track_scope (writer, "Track");

            writer.WriteAttribute ("Name", "offset");

            const PropertyAnimation::KeyframeList &x_keyframes = motion_x_track->Keyframes (),
                                                  &y_keyframes = motion_y_track->Keyframes ();

            Vec2fKeyframes keyframes;

            compose_vec2f_keyframes (x_keyframes, y_keyframes, keyframes);

            for (Vec2fKeyframes::ConstIterator iter = keyframes.CreateIterator (); iter; ++iter)
              write_track_key (writer, iter->time, vec2f ((iter->value.x + additional_x_offset) * x_scale, (iter->value.y + additional_y_offset) * y_scale));
          }
        }

        //TODO export rotation!

        {
          XmlWriter::Scope track_scope (writer, "Track");

          writer.WriteAttribute ("Name", "scale");

          const PropertyAnimation               &scale_x_track = frame_iter->Animation ().Properties () ["headContainer.Transformation.Scale_X"],
                                                &scale_y_track = frame_iter->Animation ().Properties () ["headContainer.Transformation.Scale_Y"];
          const PropertyAnimation::KeyframeList &x_keyframes   = scale_x_track.Keyframes (),
                                                &y_keyframes   = scale_y_track.Keyframes ();

          if (scale_x_track.Enabled () && scale_y_track.Enabled ())
          {
            Vec2fKeyframes keyframes;

            compose_vec2f_keyframes (x_keyframes, y_keyframes, keyframes);

            for (Vec2fKeyframes::ConstIterator iter = keyframes.CreateIterator (); iter; ++iter)
              write_track_key (writer, iter->time, vec2f (iter->value.x / 100.f, iter->value.y / 100.f));
          }
        }

        {
          XmlWriter::Scope track_scope (writer, "Track");

          writer.WriteAttribute ("Name", "events");

          stl::string event_string = common::format ("DeactivateSprite ('%s')", element_iter->Name ());

          write_track_key (writer, duration, event_string.c_str (), "Event");
        }

        {
          const PropertyAnimation               &skew_x_track = frame_iter->Animation ().Properties () ["headContainer.Transformation.Skew_X"],
                                                &skew_y_track = frame_iter->Animation ().Properties () ["headContainer.Transformation.Skew_Y"];
          const PropertyAnimation::KeyframeList &x_keyframes  = skew_x_track.Keyframes (),
                                                &y_keyframes  = skew_y_track.Keyframes ();

          if (skew_x_track.Enabled () && skew_y_track.Enabled ())
          {
            for (size_t j = 0, count = x_keyframes.Size (); j < count; j++)
            {
              const PropertyAnimationKeyframe &x_keyframe = x_keyframes [j],
                                              &y_keyframe = y_keyframes [j];

              if (!params.silent && (x_keyframe.value || y_keyframe.value))
                printf ("Unsupported animation for frame element '%s' of layer '%s', skew not supported\n", element_iter->Name (), layer.Name ());
            }
          }
        }
      }
  }

  XmlWriter::Scope sprite_scope (writer, "Sprite");

  writer.WriteAttribute ("Name", "_____SpriteManager_____");
  writer.WriteAttribute ("Material", "transparent");

  XmlWriter::Scope track_scope (writer, "Track");

  writer.WriteAttribute ("Name", "events");

  for (ActivateSpritesMap::const_iterator iter = activate_sprites_info.begin (), end = activate_sprites_info.end (); iter != end; ++iter)
    write_track_key (writer, iter->first, iter->second.c_str (), "Event");
}

//построение списка используемых символов
void build_used_symbols (const Params& params, const Timeline& timeline, UsedResourcesSet& used_symbols)
{
  common::StringArray layers_exclude_list = common::split (params.layers_exclude.c_str ());

  for (Timeline::LayerList::ConstIterator layer_iter = timeline.Layers ().CreateIterator (); layer_iter; ++layer_iter)
  {
    bool ignore_layer = false;

    for (size_t i = 0, count = layers_exclude_list.Size (); i < count; i++)
    {
      const char* mask = layers_exclude_list [i];

      if (common::wcmatch (layer_iter->Name (), mask))
      {
        ignore_layer = true;
        break;
      }
    }

    if (ignore_layer)
      continue;

    for (Layer::FrameList::ConstIterator frame_iter = layer_iter->Frames ().CreateIterator (); frame_iter; ++frame_iter)
      for (Frame::FrameElementList::ConstIterator element_iter = frame_iter->Elements ().CreateIterator (); element_iter; ++element_iter)
      {
        if (element_iter->Type () != FrameElementType_SymbolInstance)
          continue;

        used_symbols.insert (element_iter->Name ());
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

  Document document (params.xfl_name.c_str ());

  size_t document_width  = document.Width (),
         document_height = document.Height ();

  if (!params.target_width || !params.target_height)
  {
    params.target_width  = static_cast<float> (document_width);
    params.target_height = static_cast<float> (document_height);
    params.right_x       = params.left_x + params.target_width;
    params.bottom_y      = params.top_y - params.target_height;
  }

  float document_aspect_ratio = document_width / (float)document_height;

  if (!params.resize_width && params.resize_height)
  {
    params.resize_width = static_cast<size_t> (params.resize_height * document_aspect_ratio);
  }
  else if (params.resize_width && !params.resize_height)
  {
    params.resize_height = static_cast<size_t> (params.resize_width / document_aspect_ratio);
  }

  if (params.resize_width && params.resize_height)
  {
    float resize_aspect_ratio = params.resize_width / (float)params.resize_height;

    if (!float_compare (document_aspect_ratio, resize_aspect_ratio) && !params.silent)
      printf ("Requested resize %u %u will greatly change image aspect ratio %f\n", params.resize_width, params.resize_height, document_aspect_ratio);
  }

  if ((params.resize_width > document_width || params.resize_height > document_height) && !params.silent)
    printf ("Requested resize to bigger size, image quality will be reduced\n");

  ResourceTilingMap     tiling_map;
  MaterialInstancesMap  material_instances;
  UsedResourcesSet      used_symbols;
  FullscreenSymbolsSet  fullscreen_symbols;
  ResourceDimensionsMap resource_dimensions;
  ResourceCropMap       resource_crops;

  for (Document::TimelineList::ConstIterator iter = document.Timelines ().CreateIterator (); iter; ++iter)
    build_used_symbols (params, *iter, used_symbols);

  preprocess_symbols (params, document, used_symbols, tiling_map, material_instances, resource_dimensions, fullscreen_symbols, resource_crops);

  for (Document::TimelineList::ConstIterator iter = document.Timelines ().CreateIterator (); iter; ++iter)
    save_timeline (params, document, *iter, resource_dimensions, fullscreen_symbols, resource_crops);

  save_materials (params, document, tiling_map, material_instances);
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

  if (params.output_textures_dir_name.empty ())
    params.output_textures_dir_name = common::notdir (common::basename (params.xfl_name));

  if (!params.target_width || !params.target_height)
  {
    if (params.left_x == params.right_x || params.top_y == params.bottom_y)
      params.left_x = params.right_x = params.top_y = params.bottom_y = params.target_width = params.target_height = 0;
    else
    {
      params.target_width  = params.right_x - params.left_x;
      params.target_height = params.top_y - params.bottom_y;
    }
  }
  else
  {
    params.right_x  = params.left_x + params.target_width;
    params.bottom_y = params.top_y - params.target_height;
  }
}

int main (int argc, const char *argv[])
{
  try
  {
      //инициализация
    Params params;

    params.options                = 0;
    params.options_count          = 0;
    params.textures_format        = "png";
    params.crop_alpha             = 0;
    params.resize_width           = 0;
    params.resize_height          = 0;
    params.left_x                 = 0;
    params.right_x                = 0;
    params.top_y                  = 0;
    params.bottom_y               = 0;
    params.target_width           = 0;
    params.target_height          = 0;
    params.silent                 = false;
    params.print_help             = false;
    params.need_pot_extent        = false;
    params.need_crop_alpha        = false;

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
