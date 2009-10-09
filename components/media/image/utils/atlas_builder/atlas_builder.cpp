#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <exception>

#include <stl/vector>

#include <common/strlib.h>

#include <media/image.h>
#include <media/atlas_builder.h>

/*
    Константы
*/

const size_t HELP_STRING_PREFIX_LENGTH  = 30;
const char*  DEFAULT_ATLAS_FILE_NAME    = "images.xatlas";
const char*  DEFAULT_LAYOUT_FILE_SUFFIX = ".xatlas";

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

typedef stl::vector<const char*> StringArray;

//параметры запуска
struct Params
{
  const Option* options;          //массив опций  
  size_t        options_count;    //количество опций
  StringArray   sources;          //имя исходных изображений
  stl::string   atlas_file_name;  //имя результирующего изображения
  stl::string   layout_file_name; //имя файла разметки
  bool          silent;           //минимальное число сообщений  
  bool          print_help;       //нужно ли печатать сообщение помощи  
  bool          need_layout;      //нужно генерировать файл разметки
  bool          need_pot_rescale; //нужно ли масштабировать изображение к размерам кратным степени двойки
  bool          invert_x;         //инвертирование координаты X тайлов
  bool          invert_y;         //инвертирование координаты Y тайлов
  bool          swap_axises;      //обмен осей местами
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

//установка имени результирующего файла атласа
void command_line_result_atlas (const char* file_name, Params& params)
{
  params.atlas_file_name = file_name;  
}

//установка имени результирующего файла разметки
void command_line_result_layout (const char* file_name, Params& params)
{
  params.layout_file_name = file_name;
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

//разбор командной строки
void command_line_parse (int argc, const char* argv [], Params& params)
{
  static Option options [] = {
    {command_line_help,          "help",       '?',      0, "print help message"},
    {command_line_silent,        "silent",     's',      0, "quiet mode"},    
    {command_line_result_atlas,  "atlas",      'o', "file", "set output atlas file"},    
    {command_line_result_layout, "layout",     'l', "file", "set output layout file"},
    {command_line_no_layout,     "no-layout",   0,       0, "don't generatoe layout file"},    
    {command_line_pot,           "pot",         0,       0, "resize atlas texture to nearest greater power of two sizes"},
    {command_line_invert_x,      "invert-x",    0,       0, "invert X coordinate in layout of tiles"},
    {command_line_invert_y,      "invert-y",    0,       0, "invert Y coordinate in layout of tiles"},
    {command_line_swap_axises,   "swap-axises", 0,       0, "swap axises at layout tiles"},
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
      params.sources.push_back (arg);
      
      continue;
    }

    stl::string   option_name;
    stl::string   option_argument;      
    const Option* option = 0;
    
      //разбор длинных опций
    
    if (long_option)
    {
      arg += LONG_OPTION_NAME_PREFIX_LENGTH;
      
      const char* option_argument = 0;      
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
  if (params.atlas_file_name.empty ())
  {
    error ("no result atlas file");
    return;
  }
  
  if (params.layout_file_name.empty ())
  {
    params.layout_file_name = common::basename (params.atlas_file_name) + DEFAULT_LAYOUT_FILE_SUFFIX;
  }  

  if (params.sources.empty ())
  {
    printf ("atlas-builder [<OPTIONS>] <SOURCE> ...\n  use: atlas-builder --help\n");
    error ("no input files");
    return;
  }  
}

//построение атласа
void build (Params& params)
{
  try
  {
      //конфигурирование построителя атласа      
      
    media::AtlasBuilder builder;
    media::Image        atlas_image;
    media::Atlas        atlas;

    builder.SetAtlasImageName (params.atlas_file_name.c_str ());    
    
    for (size_t i=0, count=params.sources.size (); i<count; i++)
    {
      const char* source_name = params.sources [i];
      
      if (!params.silent)
        printf ("Load image '%s'", source_name);

      try
      {
        builder.Insert (source_name);
      }
      catch (...)
      {
        if (!params.silent)
          printf (" - Fail!\n");

        throw;
      }

      if (!params.silent)
        printf ("\n");
    }
    
      //построение атласа
      
    if (!params.silent)
      printf ("Build atlas");

    size_t pack_flags = 0;

    if (params.need_pot_rescale) pack_flags |= media::AtlasPackFlag_PowerOfTwoEdges;
    if (params.invert_x)         pack_flags |= media::AtlasPackFlag_InvertTilesX;
    if (params.invert_y)         pack_flags |= media::AtlasPackFlag_InvertTilesY;
    if (params.swap_axises)      pack_flags |= media::AtlasPackFlag_SwapAxises;

    try
    {
      builder.Build (atlas, atlas_image, pack_flags);
    }
    catch (...)
    {
      if (!params.silent)
        printf (" - Fail!\n");

      throw;
    }
    
    if (!params.silent)
      printf (" %ux%u\n", atlas_image.Width (), atlas_image.Height ());
    
      //сохранение атласа
      
    if (!params.silent)
    {
      if (params.need_layout) printf ("Save atlas '%s' and layout '%s'", params.atlas_file_name.c_str (), params.layout_file_name.c_str ());
      else                    printf ("Save atlas '%s'", params.atlas_file_name.c_str ());
    }

    try
    {
      atlas_image.Save (params.atlas_file_name.c_str ());
      
      if (params.need_layout)
        atlas.Save (params.layout_file_name.c_str ());
    }
    catch (...)
    {
      if (!params.silent)
        printf (" - Fail!\n");

      throw;
    }

    if (!params.silent)
      printf ("\nBuild successfull!\n");
  }
  catch (std::exception& e)
  {
    error ("%s", e.what ());
  }
}

int main (int argc, const char* argv [])
{  
    //инициализация

  Params params;
  
  params.options          = 0;
  params.options_count    = 0;
  params.atlas_file_name  = DEFAULT_ATLAS_FILE_NAME;
  params.print_help       = false;
  params.silent           = false;
  params.need_layout      = true;
  params.need_pot_rescale = false;
  params.invert_x         = false;
  params.invert_y         = false;
  params.swap_axises      = false;

    //разбор командной строки

  command_line_parse (argc, argv, params);
  
    // --help только печатает сообщение помощи
  
  if (params.print_help)
    return 0;
    
    //проверка корректности ввода

  validate (params);
  
    //построение атласа

  build (params);

  return 0;
}
