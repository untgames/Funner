#include <cstdio>
#include <cstdlib>
#include <exception>

#include <libpsd.h>

#include <stl/string>
#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/ref.h>

#include <common/command_line.h>
#include <common/file.h>
#include <common/strlib.h>
#include <common/utf_converter.h>
#include <common/xml_writer.h>

#include <media/image.h>

#pragma pack (1)

/*
    Константы
*/

const size_t HELP_STRING_PREFIX_LENGTH  = 30;
const size_t DEFAULT_BLUR_PASSES_COUNT  = 4;

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

//форматы пикселя
struct bgra_t
{
  unsigned char blue;
  unsigned char green;
  unsigned char red;
  unsigned char alpha;
};

struct rgba_t
{
  unsigned char red; 
  unsigned char green;
  unsigned char blue;
  unsigned char alpha;  
};

/*
    Обработка командной строки
*/

struct Params;

//опция
struct Option
{
  common::CommandLine::SwitchHandler handler;       //обработчик ключа
  const char*                        name;          //имя команды
  char                               short_name;    //короткое имя
  const char*                        argument_name; //имя аргумента
  const char*                        tip;           //подсказка
};

//параметры запуска
struct Params
{
  const Option* options;                   //массив опций
  size_t        options_count;             //количество опций
  stl::string   source_file_name;          //имя исходного файла
  stl::string   layout_file_name;          //имя файла разметки
  stl::string   layers_dir_name;           //имя каталога с сохранёнными слоями
  stl::string   layout_layers_dir_name;    //имя каталога с сохранёнными слоями, используемое в файле разметки
  stl::string   layers_format;             //строка форматирования имён слоёв
  stl::string   crop_exclude;              //необрезаемые слои
  stl::string   dummy_materials_wildcard;  //маска имён слоев, для которых генерируются материалы-заглушки
  unsigned int  crop_alpha;                //коэффициент обрезания по прозрачности
  unsigned int  zero_alpha_fix_value;      //коэффициент определения необходимости исправления цвета прозрачного пикселя
  unsigned int  blur_passes_count;         //количество проходов, используемое при блюре
  unsigned int  max_image_size;            //максимальный размер выходного изображения
  bgra_t        zcolor_min;                //минимальный z-цвет, заменяемый на прозрачный
  bgra_t        zcolor_max;                //минимальный z-цвет, заменяемый на прозрачный
  bool          silent;                    //минимальное число сообщений
  bool          print_help;                //нужно ли печатать сообщение помощи
  bool          need_layout;               //нужно генерировать файл разметки
  bool          need_layers;               //нужно сохранять слои
  bool          need_pot_extent;           //нужно ли расширять изображения до ближайшей степени двойки
  bool          need_crop_alpha;           //нужно ли обрезать картинку по нулевой прозрачности
  bool          need_replace_zcolor;       //нужно заменять Z-цвет
  bool          need_trim_name_spaces;     //нужно ли отсекать пробелы в именах
  bool          need_fix_zero_alpha_color; //нужно исправлять ошибку с цветом нулевой альфы
  bool          has_zcolor_min;            //минимальный z-цвет установлен
  bool          has_zcolor_max;            //максимальный z-цвет установлен
};

//прямоугольная область
struct Rect
{
  int          x, y;
  unsigned int width, height;
};

//получение подсказки по программе
void command_line_help (const char*, Params& params)
{
  printf ("psd-exporter [<OPTIONS>] <SOURCE> ...\n");
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

//установка имени результирующего файла разметки
void command_line_result_layout (const char* file_name, Params& params)
{
  params.layout_file_name = file_name;
}

//установка имени результирующей директории с содержимым слоёв
void command_line_result_layers_dir (const char* dir_name, Params& params)
{
  params.layers_dir_name = dir_name;
}

//установка имени результирующей директории с содержимым слоёв, используемой в файле разметки
void command_line_result_layout_layers_dir (const char* dir_name, Params& params)
{
  params.layout_layers_dir_name = dir_name;
}

//установка формата имени результирующих файлов слоёв
void command_line_result_layers_format (const char* format, Params& params)
{
  params.layers_format = format;
}

//установка флага генерации файла разметки
void command_line_no_layout (const char* file_name, Params& params)
{
  params.need_layout = false;
}

//установка флага генерации файлов с содержимым слоёв
void command_line_no_layers (const char* file_name, Params& params)
{
  params.need_layers = false;
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
  int value = atoi (value_string);

  params.crop_alpha      = value < 0 ? 0 : value > 256 ? 256 : value;
  params.need_crop_alpha = true;
}

//установка необрезаемых слоёв
void command_line_crop_exclude (const char* string, Params& params)
{
  params.crop_exclude = string;
}

//установка максимального размера выходного изображения
void command_line_max_image_size (const char* value_string, Params& params)
{
  params.max_image_size = (size_t)atoi (value_string);
}

//установка необходимости отсечения пробелов в именах
void command_line_trim_name_spaces (const char*, Params& params)
{
  params.need_trim_name_spaces = true;
}

//установка необходимости исправление цвета прозрачной альфы
void command_line_fix_zero_alpha_color (const char* value, Params& params)
{
  params.need_fix_zero_alpha_color = true;
  params.zero_alpha_fix_value      = (size_t)atoi (value);
}

//установка количества проходов блюра
void command_line_blur_passes_count (const char* value, Params& params)
{
  params.blur_passes_count = (size_t)atoi (value);
}

//установка значения z-цвета
void command_line_zcolor_min (const char* value, Params& params)
{
  common::StringArray tokens = common::split (value, " ");
  
  if (tokens.Size () != 3)
    error ("ZColor must contains 3 values from 0 to 255");
    
  params.zcolor_min.red      = (unsigned char)atoi (tokens [0]);
  params.zcolor_min.green    = (unsigned char)atoi (tokens [1]);
  params.zcolor_min.blue     = (unsigned char)atoi (tokens [2]);
  params.need_replace_zcolor = true;
  params.has_zcolor_min      = true;
  
  if (!params.has_zcolor_max)
    params.zcolor_max = params.zcolor_min;
}

void command_line_zcolor_max (const char* value, Params& params)
{
  common::StringArray tokens = common::split (value, " ");
  
  if (tokens.Size () != 3)
    error ("ZColor must contains 3 values from 0 to 255");
    
  params.zcolor_max.red      = (unsigned char)atoi (tokens [0]);
  params.zcolor_max.green    = (unsigned char)atoi (tokens [1]);
  params.zcolor_max.blue     = (unsigned char)atoi (tokens [2]);
  params.need_replace_zcolor = true;
  params.has_zcolor_max      = true;

  if (!params.has_zcolor_min)
    params.zcolor_min = params.zcolor_max;
}

//установка заменяемого значения прозрачности для z-цвета
void command_line_zalpha (const char* value, Params& params)
{
  params.zcolor_min.alpha    = (unsigned char)atoi (value);
  params.need_replace_zcolor = true; 
}

//установка маски слоев с минимальными материалами
void command_line_dummy_materials (const char* string, Params& params)
{
  params.dummy_materials_wildcard = string;
}

//проверка корректности ввода
void validate (Params& params)
{ 
  if (params.layout_file_name.empty ())
  {
    params.layout_file_name = common::notdir (common::basename (params.source_file_name)) + ".xlayermap";
  }
  
  if (params.layers_dir_name.empty ())
  {
    params.layers_dir_name = common::notdir (common::basename (params.source_file_name));
  }
  
  if (params.layout_layers_dir_name.empty ())
  {
    params.layout_layers_dir_name = params.layers_dir_name;
  }
}

//проверка статуса PSD
void check_status (psd_status status, const char* source)
{
  if (status == psd_status_done)
    return;
    
  fprintf (stderr, "Function %s return error status %d\n", source, status);
  exit (1);  
}

/*const char* get_layer_type_string (psd_layer_type type)
{
  switch (type)
  {
    case psd_layer_type_normal:                return "psd_layer_type_normal";
    case psd_layer_type_hidden:                return "psd_layer_type_hidden";
    case psd_layer_type_folder:                return "psd_layer_type_folder";
    case psd_layer_type_solid_color:           return "psd_layer_type_solid_color";
    case psd_layer_type_gradient_fill:         return "psd_layer_type_gradient_fill";
    case psd_layer_type_pattern_fill:          return "psd_layer_type_pattern_fill";
    case psd_layer_type_levels:                return "psd_layer_type_levels";
    case psd_layer_type_curves:                return "psd_layer_type_curves";
    case psd_layer_type_brightness_contrast:   return "psd_layer_type_brightness_contrast";
    case psd_layer_type_color_balance:         return "psd_layer_type_color_balance";
    case psd_layer_type_hue_saturation:        return "psd_layer_type_hue_saturation";
    case psd_layer_type_selective_color:       return "psd_layer_type_selective_color";
    case psd_layer_type_threshold:             return "psd_layer_type_threshold";
    case psd_layer_type_invert:                return "psd_layer_type_invert";
    case psd_layer_type_posterize:             return "psd_layer_type_posterize";
    case psd_layer_type_channel_mixer:         return "psd_layer_type_channel_mixer";
    case psd_layer_type_gradient_map:          return "psd_layer_type_gradient_map";
    case psd_layer_type_photo_filter:          return "psd_layer_type_photo_filter";
    default:                                   return "unknown";    
  }
} */

//преобразование растровой карты
void convert_image_data (size_t src_width, size_t src_height, const psd_argb_color* src_image, const Rect& src_crop, size_t dst_width, size_t dst_height, void* dst_image)
{    
  for (size_t i=0; i<src_crop.height; i++)
  {
    const bgra_t* src = (const bgra_t*)src_image + (src_crop.y + i) * src_width + src_crop.x;
    rgba_t*       dst = (rgba_t*)dst_image + dst_width * (dst_height - 1) - i * dst_width;

    for (size_t j=0; j<src_crop.width; j++, src++, dst++)
    {
      dst->red   = src->red;
      dst->green = src->green;
      dst->blue  = src->blue;
      dst->alpha = src->alpha;
    }
  }
}

//обрезание
void crop_by_alpha (size_t width, size_t height, const psd_argb_color* image, size_t crop_alpha, Rect& cropped_rect)
{
  int  min_x = 0, min_y = 0, max_x = 0, max_y = 0;
  bool first_point_found = false;

  for (int y=0; y<(int)height; y++)
  {
    const bgra_t* data = (const bgra_t*)image + y * width;

    for (int x=0; x<(int)width; x++, data++)
    {
      if (data->alpha < crop_alpha)
        continue;
        
      if (!first_point_found)
      {
        min_x             = max_x = x;
        min_y             = max_y = y;
        first_point_found = true;

        continue;        
      }

      if (x < min_x) min_x = x;
      if (y < min_y) min_y = y;
      if (x > max_x) max_x = x;
      if (y > max_y) max_y = y;
    }
  }
  
  if (!first_point_found)
  {
    cropped_rect.x      = 0;
    cropped_rect.y      = 0;
    cropped_rect.width  = 0;
    cropped_rect.height = 0;
    
    return;
  }  
  
  if (!cropped_rect.width || !cropped_rect.height)
  {
    cropped_rect.x      = min_x;
    cropped_rect.y      = min_y;
    cropped_rect.width  = max_x - min_x + 1;
    cropped_rect.height = max_y - min_y + 1;      
  }
  else
  {
    if (min_x < cropped_rect.x) cropped_rect.x = min_x;
    if (min_y < cropped_rect.y) cropped_rect.y = min_y;

    if (max_x > (int)(cropped_rect.x + cropped_rect.width))
      cropped_rect.width = max_x - cropped_rect.x + 1;
      
    if (max_y > (int)(cropped_rect.y + cropped_rect.height))
      cropped_rect.height = max_y - cropped_rect.y + 1;
  }  
}

//замена z-цвета
void zcolor_correction (const bgra_t& zcolor_min, const bgra_t& zcolor_max, size_t width, size_t height, psd_argb_color* image)
{
  for (int y=0; y<(int)height; y++)
  {
    bgra_t* data = (bgra_t*)image + y * width;

    for (int x=0; x<(int)width; x++, data++)
    {
      if (zcolor_min.red > data->red || zcolor_min.green > data->green || zcolor_min.blue > data->blue)
        continue;
        
      if (zcolor_max.red < data->red || zcolor_max.green < data->green || zcolor_max.blue < data->blue)
        continue;        
        
      data->alpha = zcolor_min.alpha;
    }
  }
}

//усреднение цвета
template <size_t source_count>
void get_average_color (rgba_t* (&source_pixels) [source_count], rgba_t& result)
{
  size_t avg_red = 0, avg_green = 0, avg_blue = 0;
                            
  for (size_t k=0; k<source_count; k++)
  {
    const rgba_t& src_pixel = *source_pixels [k];
    
    avg_red   += src_pixel.red;
    avg_green += src_pixel.green;
    avg_blue  += src_pixel.blue;
  }

  avg_red   /= source_count;
  avg_green /= source_count;
  avg_blue  /= source_count;
  
  result.red   = static_cast<unsigned char> (avg_red);
  result.green = static_cast<unsigned char> (avg_green);
  result.blue  = static_cast<unsigned char> (avg_blue);
}

//исправление ошибки с цветом пикселей с нулевой альфой
void fix_zero_alpha_color (unsigned int width, unsigned int height, rgba_t* bitmap, unsigned char fix_alpha_value, size_t passes_count)
{
  if (!width || !height)
    return;

    //первый проход - зануление цвета под полностью прозрачными пикселями (alpha = 0)

  for (unsigned int i=0; i<height; i++)
  {
    rgba_t* pixel = bitmap + width * i;

    for (unsigned int j=0; j<width; j++, pixel++)
    {
      if (pixel->alpha <= fix_alpha_value)
      {
        pixel->red   = 0;
        pixel->green = 0;
        pixel->blue  = 0;
      }
    }         
  }
  
    //блюр картинки
    
  media::Image tmp_image (width, height, 1, media::PixelFormat_RGBA8);  
  
  rgba_t* tmp_bitmap = reinterpret_cast<rgba_t*> (tmp_image.Bitmap (0));
  
  memcpy (tmp_bitmap, bitmap, width * height * sizeof (rgba_t));

  for (size_t pass=0; pass<passes_count; pass++)
  {
      //усреднение верхней линии
    
    if (height > 1)
    {
      rgba_t* pixel = tmp_bitmap;
      
      if (width > 1)
      {
        rgba_t* avg_pixels [4] = {pixel, pixel + 1, pixel + width, pixel + width + 1};
        
        get_average_color (avg_pixels, *pixel);
      }
      else
      {
        rgba_t* avg_pixels [2] = {pixel, pixel + width};
        
        get_average_color (avg_pixels, *pixel);
      }

      ++pixel;
      
      for (size_t j=1; j<width-1; j++, pixel++)
      {
        rgba_t* avg_pixels [6] = {pixel - 1, pixel, pixel + 1, pixel + width - 1, pixel + width, pixel + width + 1};
                                  
        get_average_color (avg_pixels, *pixel);
      }
      
      if (width > 1)
      {
        rgba_t* avg_pixels [4] = {pixel + width, pixel + width - 1, pixel, pixel - 1};
        
        get_average_color (avg_pixels, *pixel);
      }
    }    
    else
    {
      rgba_t* pixel = tmp_bitmap;
      
      if (width > 1)
      {
        rgba_t* avg_pixels [2] = {pixel, pixel + 1};
        
        get_average_color (avg_pixels, *pixel);
      }

      ++pixel;
      
      for (size_t j=1; j<width-1; j++, pixel++)
      {
        rgba_t* avg_pixels [3] = {pixel - 1, pixel, pixel + 1};
                                  
        get_average_color (avg_pixels, *pixel);
      }
      
      if (width > 1)
      {
        rgba_t* avg_pixels [2] = {pixel, pixel - 1};
        
        get_average_color (avg_pixels, *pixel);
      }
    }
    
      //усреднение центра
    
    for (size_t i=1; i<height-1; i++)
    {
      rgba_t* pixel = tmp_bitmap + width * i;
      
      if (width > 1)
      {
        rgba_t* avg_pixels [6] = {pixel - width, pixel - width + 1, pixel, pixel + 1, pixel + width, pixel + width + 1};
        
        get_average_color (avg_pixels, *pixel);
      }
      else
      {
        rgba_t* avg_pixels [3] = {pixel - width, pixel, pixel + width};
        
        get_average_color (avg_pixels, *pixel);
      }

      ++pixel;
      
      for (size_t j=1; j<width-1; j++, pixel++)
      {
        rgba_t* avg_pixels [9] = {pixel - width - 1, pixel - width, pixel - width + 1, pixel - 1, pixel, pixel + 1,
                                  pixel + width - 1, pixel + width, pixel + width + 1};
                                  
        get_average_color (avg_pixels, *pixel);
      }
      
      if (width > 1)
      {
        rgba_t* avg_pixels [6] = {pixel - width, pixel - width - 1, pixel, pixel - 1, pixel + width, pixel + width - 1};
        
        get_average_color (avg_pixels, *pixel);
      }
    }
    
      //усреднение нижней линии
    
    if (height > 1)
    {
      rgba_t* pixel = tmp_bitmap + (height-1) * width;
      
      if (width > 1)
      {
        rgba_t* avg_pixels [4] = {pixel, pixel + 1, pixel - width, pixel - width + 1};
        
        get_average_color (avg_pixels, *pixel);
      }
      else
      {
        rgba_t* avg_pixels [2] = {pixel, pixel - width};
        
        get_average_color (avg_pixels, *pixel);
      }

      ++pixel;
      
      for (size_t j=1; j<width-1; j++, pixel++)
      {
        rgba_t* avg_pixels [6] = {pixel - 1, pixel, pixel + 1, pixel - width - 1, pixel - width, pixel - width + 1};
                                  
        get_average_color (avg_pixels, *pixel);
      }
      
      if (width > 1)
      {
        rgba_t* avg_pixels [4] = {pixel - width, pixel - width - 1, pixel, pixel - 1};
        
        get_average_color (avg_pixels, *pixel);
      }
    }    
    else
    {
      rgba_t* pixel = tmp_bitmap;
      
      if (width > 1)
      {
        rgba_t* avg_pixels [2] = {pixel, pixel + 1};
        
        get_average_color (avg_pixels, *pixel);
      }

      ++pixel;
      
      for (size_t j=1; j<width-1; j++, pixel++)
      {
        rgba_t* avg_pixels [3] = {pixel - 1, pixel, pixel + 1};
                                  
        get_average_color (avg_pixels, *pixel);
      }
      
      if (width > 1)
      {
        rgba_t* avg_pixels [2] = {pixel, pixel - 1};
        
        get_average_color (avg_pixels, *pixel);
      }
    }    
  }
  
    //формирование окончательного изображения
    
  for (size_t i=0; i<height; i++)
  {
    rgba_t* pixel        = bitmap + width * i;
    rgba_t* blured_pixel = tmp_bitmap + width * i;

    for (size_t j=0; j<width; j++, pixel++, blured_pixel++)
    {
      if (pixel->alpha <= fix_alpha_value)
      {
        pixel->red   = blured_pixel->red;
        pixel->green = blured_pixel->green;
        pixel->blue  = blured_pixel->blue;
      }
    }         
  }
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

//получение имени слоя
stl::string get_layer_name (const Params& params, const psd_layer_record& layer)
{
  xtl::uninitialized_storage<char> ascii_layer_name (layer.unicode_name_length + 1);

  memset (ascii_layer_name.data (), 0, ascii_layer_name.size ());

  const void* layer_name_start       = layer.unicode_name;
  size_t      source_size            = layer.unicode_name_length * 2;
  void*       ascii_layer_name_start = ascii_layer_name.data ();
  size_t      destination_size       = ascii_layer_name.size () - 1;

  while (source_size && destination_size)
    convert_encoding (common::Encoding_UTF16BE, layer_name_start, source_size, common::Encoding_ASCII7, ascii_layer_name_start, destination_size);

  return params.need_trim_name_spaces ? common::trim (ascii_layer_name.data ()) : ascii_layer_name.data ();
}

//экспорт
void export_data (Params& params)
{
  psd_context* context = 0;
  
  check_status (psd_image_load (&context, (psd_char*)params.source_file_name.c_str ()), "::psd_image_load");       
  
    //обрезание по альфа
  
  typedef stl::vector<Rect> RectArray;
  
  RectArray           cropped_layers;
  common::StringArray crop_exclude_list = common::split (params.crop_exclude.c_str ());

  if (!params.silent && params.need_crop_alpha)
    printf ("Crop layers by alpha...\n");

    //построение дерева связности

  typedef stl::vector<int>         IndexArray;
  typedef stl::vector<stl::string> StringArray;

  IndexArray  parents (context->layer_count, -1), index_stack;
  StringArray layer_names (context->layer_count);

  index_stack.push_back (-1);

  for (int i=0; i<context->layer_count; i++)
  {
    int               index = context->layer_count - i - 1; //обратный порядок
    psd_layer_record& layer = context->layer_records [index];

    layer_names [index] = get_layer_name (params, layer);
    parents [index]     = index_stack.back ();

    if (!strncmp (layer_names [index].c_str (), "</", 2))
    {
      if (index_stack.empty ())
      {
        printf ("error: Invalid folders structure. Attempt to close root folder");
        exit (1);
      }

      index_stack.pop_back ();

      continue;
    }

    switch (layer.layer_type)
    {
      case psd_layer_type_folder:
        index_stack.push_back (index);
        break;
      default:
        break;
    }
  }

    //экспорт слоев

  cropped_layers.reserve (context->layer_count);

  size_t image_index = 1;       

  for (int i=0; i<context->layer_count; i++)
  {
    psd_layer_record&  layer = context->layer_records [i];
    const stl::string& name  = layer_names [i];

    if (!strncmp (name.c_str (), "</", 2))
      continue;
    
    switch (layer.layer_type)
    {
      case psd_layer_type_normal:
        break;
      default:
        continue;
    }

    //проверка корректности имени слоя
    for (const char* symbol = name.c_str (); *symbol; symbol++)
    {
      static const char* allowed_symbols = "_.- $%!@#^()[],~";

      if (!isalnum (*symbol) && !strchr (allowed_symbols, *symbol))
        throw xtl::format_operation_exception ("export_data", "Unallowed symbol '%c' in layer '%s'", *symbol, name.c_str ());
    }

    //коррекция z-цвета
    
    if (params.need_replace_zcolor)
      zcolor_correction (params.zcolor_min, params.zcolor_max, layer.width, layer.height, layer.image_data);

    Rect rect = {0, 0, 0, 0};           
    
    bool need_crop_alpha = params.need_crop_alpha;
    
    if (need_crop_alpha)
    {
      for (size_t i=0, count=crop_exclude_list.Size (); i<count; i++)
      {
        const char* mask = crop_exclude_list [i];
        
        if (common::wcmatch (name.c_str (), mask))
        {
          need_crop_alpha = false;
          break;
        }
      }
    }
  
    if (need_crop_alpha)  
    {
      crop_by_alpha (layer.width, layer.height, layer.image_data, params.crop_alpha, rect);      

      if ((rect.width != (size_t)layer.width || rect.height != (size_t)layer.height) && !params.silent)
        printf ("  crop layer '%s' %ux%u: (%u, %u)-(%u, %u)\n", name.c_str (), layer.width, layer.height, rect.x, rect.y, rect.x + rect.width, rect.y + rect.height);
    }
    else
    {
      rect.x      = 0;
      rect.y      = 0;
      rect.width  = layer.width;
      rect.height = layer.height;
    }
          
    cropped_layers.push_back (rect);

    image_index++;
  }
  
    //сохранение разметки
    
  if (params.need_layout)
  {
    stl::string format = common::format ("%s/%s", params.layout_layers_dir_name.c_str (), params.layers_format.c_str ());      
  
    if (!params.silent)
      printf ("Save layout '%s'...\n", params.layout_file_name.c_str ());
  
    common::XmlWriter xml_writer (params.layout_file_name.c_str ());

    common::XmlWriter::Scope image_list (xml_writer, "LayerMap");

    xml_writer.WriteAttribute ("TotalWidth",  context->width);
    xml_writer.WriteAttribute ("TotalHeight", context->height);
    
    size_t image_index = 1;       

    for (int i=0; i<context->layer_count; i++)
    {
      psd_layer_record&  layer = context->layer_records [i];
      const stl::string& name  = layer_names [i];

      if (!strncmp (name.c_str (), "</", 2))
        continue;

      const char* tag         = "";
      bool        need_layers = params.need_layers,
                  need_bounds = true;

      switch (layer.layer_type)
      {
        case psd_layer_type_normal:
          tag = "Layer";
          break;
        case psd_layer_type_folder:
        {
          tag         = "Folder";
          need_layers = false;
          need_bounds = false;
          break;
        }
        default:
          continue;
      }

      common::XmlWriter::Scope layer_scope (xml_writer, tag);

      xml_writer.WriteAttribute ("Name", name.c_str ());

      if (need_layers)
      {      
        stl::string dst_image_name = common::format (format.c_str (), image_index);      
      
        xml_writer.WriteAttribute ("Image",  dst_image_name.c_str ());
      }

      if (parents [i] != -1)
        xml_writer.WriteAttribute ("Parent", layer_names [parents [i]].c_str ());

      if (need_bounds)
      {
        const Rect& cropped_rect = cropped_layers [image_index - 1];

        xml_writer.WriteAttribute ("Left",    layer.left + cropped_rect.x);
        xml_writer.WriteAttribute ("Top",     layer.top + cropped_rect.y);
        xml_writer.WriteAttribute ("Width",   cropped_rect.width);
        xml_writer.WriteAttribute ("Height",  cropped_rect.height);

        image_index++;
      }

      xml_writer.WriteAttribute ("Opacity", layer.opacity);
      xml_writer.WriteAttribute ("Visible", layer.visible != 0);      
    }
  }

    //сохранение содержимого слоёв
  
  if (params.need_layers)
  {
    stl::string format = common::format ("%s/%s", params.layers_dir_name.c_str (), params.layers_format.c_str ());    
  
    if (!params.silent)
      printf ("Save layers to dir '%s'...\n", params.layers_dir_name.c_str ());
  
    if (!common::FileSystem::IsFileExist (params.layers_dir_name.c_str ()))
      common::FileSystem::Mkdir (params.layers_dir_name.c_str ());
      
    size_t image_index = 1;       

    common::StringArray dummy_materials_wildcard_list = common::split (params.dummy_materials_wildcard.c_str ());

    media::Image dummy_material (1, 1, 1, media::PixelFormat_RGB8);

    memset (dummy_material.Bitmap (), 0, get_bytes_per_pixel (dummy_material.Format ()) * dummy_material.Width () * dummy_material.Height ());

    for (int i=0; i<context->layer_count; i++)
    {
      psd_layer_record&  layer = context->layer_records [i];
      const stl::string& name  = layer_names [i];

      if (!strncmp (name.c_str (), "</", 2))
        continue;
      
      switch (layer.layer_type)
      {
        case psd_layer_type_normal:
          break;
        default:
          continue;
      }           
      
      stl::string dst_image_name = common::format (format.c_str (), image_index);
      
      if (!params.silent)
        printf ("  save '%s'...\n", dst_image_name.c_str ());

      bool need_dummy_material = false;

      for (size_t j = 0, count = dummy_materials_wildcard_list.Size (); j < count; j++)
      {
        const char* mask = dummy_materials_wildcard_list [j];

        if (common::wcmatch (name.c_str (), mask))
        {
          need_dummy_material = true;
          break;
        }
      }

      if (need_dummy_material)
      {
        dummy_material.Save (dst_image_name.c_str ());

        image_index++;
        continue;
      }

      const Rect& cropped_rect = cropped_layers [image_index - 1];

      unsigned int image_width  = cropped_rect.width,
                   image_height = cropped_rect.height;

      if (params.need_pot_extent)
      {
        image_width  = (unsigned int)get_next_higher_power_of_two (image_width);
        image_height = (unsigned int)get_next_higher_power_of_two (image_height);
      }

      media::Image image (image_width, image_height, 1, media::PixelFormat_RGBA8, 0);

      if (params.need_pot_extent)
        memset (image.Bitmap (), 0, get_bytes_per_pixel (image.Format ()) * image.Width () * image.Height ());

      convert_image_data (layer.width, layer.height, layer.image_data, cropped_rect, image_width, image_height, image.Bitmap ());

      if (params.need_fix_zero_alpha_color)
      {
        if (!params.silent)
          printf ("  fix zero alpha pixels for '%s'...\n", dst_image_name.c_str ());
        
        fix_zero_alpha_color (image.Width (), image.Height (), reinterpret_cast<rgba_t*> (image.Bitmap ()), params.zero_alpha_fix_value, params.blur_passes_count);
      }

      if (params.max_image_size)
      {
        if (image.Width () > params.max_image_size || image.Height () > params.max_image_size)
        {
          unsigned int new_image_width, new_image_height;
          
          if (image.Width () > image.Height ())
          {
            new_image_width  = params.max_image_size;
            new_image_height = (size_t)(image.Height () * (new_image_width / (float)image.Width ()));
          }
          else
          {
            new_image_height = params.max_image_size;
            new_image_width  = (size_t)(image.Width () * (new_image_height / (float)image.Height ()));
          }

          if (!params.silent)
            printf ("  rescale '%s' from %ux%u to %ux%u...\n", dst_image_name.c_str (), image.Width (), image.Height (),
              new_image_width, new_image_height);

          image.Resize (new_image_width, new_image_height, 1);
        }
      }

      image.Save (dst_image_name.c_str ());

      image_index++;
    }
  }
   
  psd_image_free (context);
}

//точка входа
int main (int argc, const char* argv [])
{
  try
  {
      //инициализация
      
    bgra_t default_zcolor = {0, 0, 0, 0};

    Params params;

    static Option options [] = {
      {xtl::bind (&command_line_help,                     _1, xtl::ref (params)), "help",             '?',          0, "print help message"},
      {xtl::bind (&command_line_silent,                   _1, xtl::ref (params)), "silent",           's',          0, "quiet mode"},
      {xtl::bind (&command_line_result_layout_layers_dir, _1, xtl::ref (params)), "layout-layers-dir", 0,       "dir", "set output layers dir that used in layout file"},
      {xtl::bind (&command_line_result_layout,            _1, xtl::ref (params)), "layout",           'o',     "file", "set output layout file"},
      {xtl::bind (&command_line_result_layers_dir,        _1, xtl::ref (params)), "layers-dir",       'O',      "dir", "set output layers dir"},
      {xtl::bind (&command_line_result_layers_format,     _1, xtl::ref (params)), "layers-format",     0,    "string", "set output layers format string"},
      {xtl::bind (&command_line_no_layout,                _1, xtl::ref (params)), "no-layout",         0,           0, "don't generate layout file"},
      {xtl::bind (&command_line_no_layers,                _1, xtl::ref (params)), "no-layers",         0,           0, "don't generate layers"},
      {xtl::bind (&command_line_pot,                      _1, xtl::ref (params)), "pot",               0,           0, "extent layers image size to nearest greater power of two"},
      {xtl::bind (&command_line_crop_alpha,               _1, xtl::ref (params)), "crop-alpha",        0,     "value", "crop layers by alpha that less than value"},
      {xtl::bind (&command_line_crop_exclude,             _1, xtl::ref (params)), "crop-exclude",      0, "wildcards", "exclude selected layers from crop"},
      {xtl::bind (&command_line_dummy_materials,          _1, xtl::ref (params)), "dummy-materials",   0, "wildcard", "set wildcard for images which will have dummy materials"},
      {xtl::bind (&command_line_fix_zero_alpha_color,     _1, xtl::ref (params)), "fix-zero-alpha",    0,     "value", "fixup color of pixels with alpha <= value"},
      {xtl::bind (&command_line_blur_passes_count,        _1, xtl::ref (params)), "blur-passes-count", 0,     "value", "number of blur passes (used in zero alpha fixup)"},
      {xtl::bind (&command_line_max_image_size,           _1, xtl::ref (params)), "max-image-size",    0,     "value", "max output image size (image with greater size will be rescaled)"},
      {xtl::bind (&command_line_trim_name_spaces,         _1, xtl::ref (params)), "trim-names",        0,           0, "trim spaces in all layers names"},
      {xtl::bind (&command_line_zcolor_min,               _1, xtl::ref (params)), "zcolor-min",        0,     "value", "z-color minimal value for alpha replacement (red green blue)"},
      {xtl::bind (&command_line_zcolor_max,               _1, xtl::ref (params)), "zcolor-max",        0,     "value", "z-color maximum value for alpha replacement (red green blue)"},
      {xtl::bind (&command_line_zalpha,                   _1, xtl::ref (params)), "zalpha",            0,     "value", "z-alpha: replacement value for z-color pixels"},
    };

    params.options                   = options;
    params.options_count             = sizeof (options) / sizeof (*options);
    params.layers_format             = "image%03u.png";
    params.crop_alpha                = 0;
    params.zero_alpha_fix_value      = 0;
    params.blur_passes_count         = DEFAULT_BLUR_PASSES_COUNT;
    params.zcolor_min                = default_zcolor;
    params.zcolor_max                = default_zcolor;    
    params.max_image_size            = 0;
    params.print_help                = false;
    params.silent                    = false;
    params.need_layout               = true;
    params.need_layers               = true;
    params.need_pot_extent           = false;
    params.need_crop_alpha           = false;
    params.need_trim_name_spaces     = false;
    params.need_fix_zero_alpha_color = false;
    params.need_replace_zcolor       = false;
    params.has_zcolor_min            = false;
    params.has_zcolor_max            = false;

    common::CommandLine command_line;

    for (size_t i = 0; i < params.options_count; i++)
      command_line.SetSwitchHandler (options [i].name, options [i].short_name, options [i].argument_name, options [i].handler);

      //разбор командной строки
    command_line.Process (argc, argv);

      // --help только печатает сообщение помощи

    if (params.print_help)
      return 0;

    if (command_line.ParamsCount () != 1)
    {
      printf ("psd-exporter [<OPTIONS>] <SOURCE>\n  use: psd-exporter --help\n");
      error ("no input file");
      return 1;
    }

    params.source_file_name = command_line.Param (0);

      //проверка корректности ввода

    validate (params);
      
      //экспорт

    export_data (params);

    return 0;
  }
  catch (std::exception& e)
  {
    fprintf (stderr, "%s\n", e.what ());
    return 1;
  }

  return 0;
}
