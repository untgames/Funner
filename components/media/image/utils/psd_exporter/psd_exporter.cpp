#include <cstdio>
#include <cstdlib>
#include <exception>

#include <libpsd.h>

#include <stl/string>

#include <common/file.h>
#include <common/strlib.h>
#include <common/xml_writer.h>

#include <media/image.h>

#pragma pack (1)

/*
    Константы
*/

const size_t HELP_STRING_PREFIX_LENGTH  = 30;

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
  const Option* options;          //массив опций
  size_t        options_count;    //количество опций
  stl::string   source_file_name; //имя исходного файла
  stl::string   layout_file_name; //имя файла разметки
  stl::string   layers_dir_name;  //имя каталога с сохранёнными слоями
  stl::string   layers_format;    //строка форматирования имён слоёв
  bool          silent;           //минимальное число сообщений
  bool          print_help;       //нужно ли печатать сообщение помощи
  bool          need_layout;      //нужно генерировать файл разметки
  bool          need_layers;      //нужно сохранять слои
};

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

//разбор командной строки
void command_line_parse (int argc, const char* argv [], Params& params)
{
  static Option options [] = {
    {command_line_help,                 "help",         '?',        0, "print help message"},
    {command_line_silent,               "silent",       's',        0, "quiet mode"},    
    {command_line_result_layout,        "layout",       'o',   "file", "set output layout file"},
    {command_line_result_layers_dir,    "layers-dir",   'O',    "dir", "set output layers dir"},    
    {command_line_result_layers_format, "layers-format", 0,  "string", "set output layers format string"},
    {command_line_no_layout,            "no-layout",     0,         0, "don't generate layout file"},
    {command_line_no_layers,            "no-layers",     0,         0, "don't generate layers"},    
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
      if (!params.source_file_name.empty ())
      {
        error ("source file msut be on");
        return;
      }
    
      params.source_file_name = arg;           
      
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

//проверка корректности ввода
void validate (Params& params)
{ 
  if (params.source_file_name.empty ())
  {
    printf ("psd-exporter [<OPTIONS>] <SOURCE>\n  use: psd-exporter --help\n");
    error ("no input file");
    return;
  }

  if (params.layout_file_name.empty ())
  {
    params.layout_file_name = common::notdir (common::basename (params.source_file_name)) + ".xlayermap";
  }
  
  if (params.layers_dir_name.empty ())
  {
    params.layers_dir_name = common::notdir (common::basename (params.source_file_name));
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

void convert_image_data (size_t width, size_t height, const psd_argb_color* src_image, void* dst_image)
{
  for (size_t i=0; i<height; i++)
  {
    const bgra_t* src = (const bgra_t*)src_image + i * width;
    rgba_t*       dst = (rgba_t*)dst_image + width * (height - 1) - i * width;

    for (size_t j=0; j<width; j++, src++, dst++)
    {
      dst->red   = src->red;
      dst->green = src->green;
      dst->blue  = src->blue;
      dst->alpha = src->alpha;
    }    
  }
}

//экспорт
void export_data (Params& params)
{
  psd_context* context = 0;
  
  check_status (psd_image_load (&context, (psd_char*)params.source_file_name.c_str ()), "::psd_image_load");
  
  stl::string format = common::format ("%s/%s", params.layers_dir_name.c_str (), params.layers_format.c_str ());
  
    //сохранение разметки     
    
  if (params.need_layout)
  {
    if (!params.silent)
      printf ("Save layout '%s'...\n", params.layout_file_name.c_str ());
  
    common::XmlWriter xml_writer (params.layout_file_name.c_str ());

    common::XmlWriter::Scope image_list (xml_writer, "LayerMap");

    xml_writer.WriteAttribute ("TotalWidth",  context->width);
    xml_writer.WriteAttribute ("TotalHeight", context->height);
    
    size_t image_index = 1;       

    for (int i=0; i<context->layer_count; i++)
    {
      psd_layer_record& layer = context->layer_records [i];
      stl::string       name ((char*)layer.layer_name);      

      if (!strncmp (name.c_str (), "</", 2))
        continue;
      
      switch (layer.layer_type)
      {
        case psd_layer_type_normal:
          break;
        default:
          continue;
      }

      common::XmlWriter::Scope layer_scope (xml_writer, "Layer");

      xml_writer.WriteAttribute ("Name", name.c_str ());
      
      if (params.need_layers)
      {      
        stl::string dst_image_name = common::format (format.c_str (), image_index);      
      
        xml_writer.WriteAttribute ("Image",  dst_image_name.c_str ());
      }
      
      xml_writer.WriteAttribute ("Left",   layer.left);
      xml_writer.WriteAttribute ("Top",    layer.top);
      xml_writer.WriteAttribute ("Width",  layer.width);
      xml_writer.WriteAttribute ("Height", layer.height);      
      
      image_index++;      
    }
  }
  
    //сохранение содержимого слоёв
  
  if (params.need_layers)
  {
    if (!params.silent)
      printf ("Save layers to dir '%s'...\n", params.layers_dir_name.c_str ());
  
    if (!common::FileSystem::IsFileExist (params.layers_dir_name.c_str ()))
      common::FileSystem::Mkdir (params.layers_dir_name.c_str ());
      
    size_t image_index = 1;       

    for (int i=0; i<context->layer_count; i++)
    {
      psd_layer_record& layer = context->layer_records [i];
      stl::string       name ((char*)layer.layer_name);      

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

      media::Image image (layer.width, layer.height, 1, media::PixelFormat_RGBA8, 0);

      convert_image_data (layer.width, layer.height, layer.image_data, image.Bitmap ());

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

    Params params;
      
    params.options       = 0;
    params.options_count = 0;
    params.layers_format = "image%03u.png";
    params.print_help    = false;
    params.silent        = false;
    params.need_layout   = true;
    params.need_layers   = true;    

      //разбор командной строки

    command_line_parse (argc, argv, params);

      // --help только печатает сообщение помощи

    if (params.print_help)
      return 0;
        
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
