#include <cstdio>

#include <stl/string>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/ref.h>

#include <common/command_line.h>
#include <common/strlib.h>

#include <media/image.h>

#if defined (_MSC_VER) || defined (__MACH__)
  #include <TextureConverter.h>
#endif

/*
    Константы
*/

const char*  APPLICATION_NAME          = "texture-converter";
const size_t HELP_STRING_PREFIX_LENGTH = 30;

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
  common::CommandLine::SwitchHandler handler;       //обработчик ключа
  const char*                        name;          //имя команды
  char                               short_name;    //короткое имя
  const char*                        argument_name; //имя аргумента
  const char*                        tip;           //подсказка
};

typedef xtl::function<void (const Params&)> ConvertFunction;

struct ConversionDesc
{
  const char*     source_format;
  const char*     target_format;
  ConvertFunction function;
};

//параметры запуска
struct Params
{
  const Option*         options;                  //массив опций
  size_t                options_count;            //количество опций
  const ConversionDesc* converters;               //функции конвертирования
  size_t                converters_count;         //количество функций конвертирования
  stl::string           source;                   //имя исходного изображения
  stl::string           source_format;            //формат исходного изображения
  stl::string           target;                   //имя результирующего изображения
  stl::string           target_format;            //формат результирующего изображения
  bool                  silent;                   //минимальное число сообщений
  bool                  print_help;               //нужно ли печатать сообщение помощи
};

//получение подсказки по программе
void command_line_help (const char*, Params& params)
{
  printf ("%s [<OPTIONS>] <SOURCE> ...\n", APPLICATION_NAME);
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

  printf ("  Supported converions:\n");

  for (size_t i = 0; i < params.converters_count; i++)
  {
    const ConversionDesc& conversion_desc = params.converters [i];

    printf ("    '%s' -> '%s'\n", conversion_desc.source_format, conversion_desc.target_format);
  }

  params.print_help = true;
}

//установка формата исходного изображения
void command_line_source_format (const char* format, Params& params)
{
  params.source_format = format;
}

//установка имени результирующего изображения
void command_line_result_image (const char* file_name, Params& params)
{
  params.target = file_name;
}

//установка формата результирующего изображения
void command_line_result_format (const char* format, Params& params)
{
  params.target_format = format;
}

//установка параметра вывода детальной информации
void command_line_silent (const char*, Params& params)
{
  params.silent = true;
}

//проверка корректности ввода
void validate (Params& params)
{
  if (params.target.empty ())
  {
    error ("no result image");
    return;
  }
}

#if defined (_MSC_VER) || defined (__MACH__)

void qualcomm_texture_compress (const Params& params)
{

}

void qualcomm_texture_decompress (const Params& params)
{

}

#endif

void common_convert (const Params& params)
{
  media::Image image (params.source.c_str ());

  image.Save (params.target.c_str ());
}

void convert (const Params& params)
{
  bool converted = false;

  for (size_t i = 0; i < params.converters_count; i++)
  {
    const ConversionDesc& conversion_desc = params.converters [i];

    if (params.source_format == conversion_desc.source_format && params.target_format == conversion_desc.target_format)
    {
      conversion_desc.function (params);
      converted = true;
      break;
    }
  }

  if (!converted)
    error ("Conversion from '%s' to '%s' not supported", params.source_format.c_str (), params.target_format.c_str ());
}

int main (int argc, const char* argv [])
{  
  try
  {
    Params params;

    static Option options [] = {
      {xtl::bind (&command_line_help,          _1, xtl::ref (params)), "help",          '?',  0,        "print help message"},
      {xtl::bind (&command_line_silent,        _1, xtl::ref (params)), "silent",        's',  0,        "quiet mode"},
      {xtl::bind (&command_line_source_format, _1, xtl::ref (params)), "source-format",  0,   "format", "set input image format"},
      {xtl::bind (&command_line_result_image,  _1, xtl::ref (params)), "target",         'o', "file",   "set output image name"},
      {xtl::bind (&command_line_result_format, _1, xtl::ref (params)), "target-format",  0,   "format", "set output image format"},
    };

    static const size_t options_count = sizeof (options) / sizeof (*options);

    static ConversionDesc converters [] = {
  #if defined (_MSC_VER) || defined (__MACH__)
      { "", "atitc_rgb",                     xtl::bind (&qualcomm_texture_compress, _1) },
      { "", "atitc_rgba_explicit_alpha",     xtl::bind (&qualcomm_texture_compress, _1) },
      { "", "atitc_rgba_interpolated_alpha", xtl::bind (&qualcomm_texture_compress, _1) },
      { "atitc_rgb", "",                     xtl::bind (&qualcomm_texture_decompress, _1) },
      { "atitc_rgba_explicit_alpha", "",     xtl::bind (&qualcomm_texture_decompress, _1) },
      { "atitc_rgba_interpolated_alpha", "", xtl::bind (&qualcomm_texture_decompress, _1) },
  #endif
      { "", "",                              xtl::bind (&common_convert, _1) }
    };

    static const size_t converters_count = sizeof (converters) / sizeof (*converters);

      //инициализация

    params.options           = options;
    params.options_count     = options_count;
    params.converters        = converters;
    params.converters_count  = converters_count;
    params.print_help        = false;
    params.silent            = false;

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
      printf ("%s [<OPTIONS>] <SOURCE> ...\n  use: %s --help\n", APPLICATION_NAME, APPLICATION_NAME);
      error ("no input file");
      return 1;
    }

    params.source = command_line.Param (0);

      //проверка корректности ввода
  
    validate (params);

      //конвертирование изображения
  
    convert (params);
  }
  catch (std::exception& e)
  {
    error ("%s", e.what ());
  }
  
  return 0;
}
