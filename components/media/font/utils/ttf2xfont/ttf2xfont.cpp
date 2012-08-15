#include <cstdio>

#include <stl/set>
#include <stl/string>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/ref.h>
#include <xtl/uninitialized_storage.h>

#include <common/command_line.h>
#include <common/file.h>
#include <common/log.h>
#include <common/strconv.h>
#include <common/strlib.h>

#include <media/font_converter.h>
#include <media/image.h>

using namespace media;
using namespace common;

const char*  PROGRAM_NAME               = "ttf2xfont";
const size_t DEFAULT_GLYPH_INTERVAL     = 5;
const size_t DEFAULT_FIRST_GLYPH_CODE   = 32;
const size_t DEFAULT_LAST_GLYPH_CODE    = 255;
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
  const Option* options;                //массив опций
  size_t        options_count;          //количество опций
  stl::string   source_font;            //имя исходного шрифта
  stl::string   result_image;           //имя результирующего изображения
  stl::string   result_xml_image_name;  //имя результирующего изображения в xml
  stl::string   result_font;            //имя результирующего шрифта
  stl::string   char_map_file;          //имя файла содержащего символы для генерации шрифта
  stl::string   char_map_file_encoding; //кодировка файла содержащего символы для генерации шрифта
  float         stroke_width;           //ширина обводки
  int           force_advance;          //генерировать отступ равный ширине плюс данное значение
  int           glyph_interval;         //интервал между символами в картинке
  int           glyph_size;             //максимальный размер изображения одного символа
  int           first_glyph_code;       //код первого индекса генерируемого диапазона символов
  int           last_glyph_code;        //код последнего индекса генерируемого диапазона символов
  bool          use_force_advance;      //генерировать ли значение отступа
  bool          fast_convert;           //быстрая генерация
  bool          silent;                 //минимальное число сообщений
  bool          print_help;             //нужно ли печатать сообщение помощи
};

void log_handler (const char* log, const char* message)
{
  printf ("'%s': '%s'\n", log, message);
  fflush (stdout);
}

//получение подсказки по программе
void command_line_help (const char*, Params& params)
{
  printf ("%s [<OPTIONS>] <SOURCE> ...\n", PROGRAM_NAME);
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

//установка параметра вывода детальной информации
void command_line_silent (const char*, Params& params)
{
  params.silent = true;
}

//установка параметра быстрого конвертирования
void command_line_fast_convert (const char*, Params& params)
{
  params.fast_convert = true;
}

//установка имени результирующего файла картинки
void command_line_result_image_name (const char* file_name, Params& params)
{
  params.result_image = file_name;
}

//установка имени результирующего файла картинки в xml
void command_line_result_xml_image_name (const char* file_name, Params& params)
{
  params.result_xml_image_name = file_name;
}

//установка имени результирующего файла шрифта
void command_line_result_font (const char* file_name, Params& params)
{
  params.result_font = file_name;
}

//установка имени файла содержащего символы для генерации
void command_line_char_map_file (const char* file_name, Params& params)
{
  params.char_map_file = file_name;
}

//установка кодировки файла содержащего символы для генерации
void command_line_char_map_file_encoding (const char* encoding, Params& params)
{
  params.char_map_file_encoding = encoding;
}

//установка максимального размера изображения одного символа
void command_line_glyph_size (const char* size, Params& params)
{
  params.glyph_size = atoi (size);
}

//установка ширины обводки символов
void command_line_stroke_width (const char* size, Params& params)
{
  params.stroke_width = (float)atof (size);
}

//установка кода первого индекса генерируемого диапазона символов
void command_line_first_glyph_code (const char* index, Params& params)
{
  params.first_glyph_code = atoi (index);
}

//установка кода последнего индекса генерируемого диапазона символов
void command_line_last_glyph_code (const char* index, Params& params)
{
  params.last_glyph_code = atoi (index);
}

//установка интервала между изображениями символов в картинке
void command_line_glyph_interval (const char* size, Params& params)
{
  params.glyph_interval = atoi (size);
}

//установка генерационного отступа между символами
void command_line_force_advance (const char* size, Params& params)
{
  params.force_advance     = atoi (size);
  params.use_force_advance = true;
}

//проверка корректности ввода
void validate (Params& params)
{
  if (params.source_font.empty ())
    error ("no source font name");

  if (params.result_image.empty ())
    error ("no result image name");

  if (params.result_xml_image_name.empty ())
    params.result_xml_image_name = params.result_image;

  if (params.result_font.empty ())
    error ("no result font name");

  if (params.glyph_size < 1)
    error ("no glyph size");

  if (!params.first_glyph_code && !params.last_glyph_code)
  {
    if (params.char_map_file.empty ())
    {
      params.first_glyph_code = DEFAULT_FIRST_GLYPH_CODE;
      params.last_glyph_code  = DEFAULT_LAST_GLYPH_CODE;
    }
    else if (params.char_map_file_encoding.empty ())
      error ("char map file defined, but encoding not defined");
  }
  else if (params.last_glyph_code < params.first_glyph_code)
    error ("last glyph code %d is less then first glyph code %d", params.last_glyph_code, params.first_glyph_code);
  else if (params.first_glyph_code < 1 || params.last_glyph_code < 1)
    error ("invalid glyph code range %d - %d", params.first_glyph_code, params.last_glyph_code);

  if (params.glyph_interval < 0)
    params.glyph_interval = DEFAULT_GLYPH_INTERVAL;
}

void build (Params& params)
{
  try
  {
    LogFilter log_filter ("", LogFilter::LogHandler ());

    if (!params.silent)
      LogFilter ("*", &log_handler).Swap (log_filter);

    Font     font;
    Image    image (1, 1, 1, PixelFormat_RGBA8);
    FontDesc font_desc;

    xtl::uninitialized_storage<size_t> utf32_buffer;

    if (params.char_map_file.empty ())
    {
      utf32_buffer.resize (params.last_glyph_code - params.first_glyph_code + 1);

      for (int i = params.first_glyph_code; i <= params.last_glyph_code; i++)
        utf32_buffer.data () [i - params.first_glyph_code] = i;

      font_desc.char_codes       = utf32_buffer.data ();
      font_desc.char_codes_count = utf32_buffer.size ();
      font_desc.first_glyph_code = params.first_glyph_code;
    }
    else
    {
      stl::string file_text = common::FileSystem::LoadTextFile (params.char_map_file.c_str ());

      utf32_buffer.resize (file_text.size ());

      StringConverter string_converter (params.char_map_file_encoding.c_str (), "UTF-32LE");

      const void* source_buffer           = file_text.c_str ();
      void*       destination_buffer      = utf32_buffer.data ();
      size_t      source_buffer_size      = file_text.size (),
                  destination_buffer_size = utf32_buffer.size () * sizeof (size_t);

      string_converter.Convert (source_buffer, source_buffer_size, destination_buffer, destination_buffer_size);

      if (source_buffer_size)
        throw xtl::format_operation_exception ("", "Can't convert buffer from '%s' to UTF-32LE (source_buffer_size=%u, destination_buffer_size=%u)\n",
                                               params.char_map_file_encoding.c_str (), source_buffer_size, destination_buffer_size);

      size_t total_symbols_count = utf32_buffer.size () - destination_buffer_size / sizeof (size_t);

      if (!total_symbols_count)
        throw xtl::format_operation_exception ("", "Can't get any symbols from file '%s'", params.char_map_file.c_str ());

      typedef stl::set<size_t> IndexSet;

      IndexSet char_set;
      size_t   *current_char = utf32_buffer.data ();

      for (size_t i = 0; i < total_symbols_count; i++, current_char++)
        char_set.insert (*current_char);

      size_t first_code     = *char_set.begin (),
             last_code      = *char_set.rbegin (),
             last_char_code = first_code;

      utf32_buffer.resize (last_code - first_code + 1, false);

      current_char = utf32_buffer.data ();

      for (IndexSet::iterator iter = char_set.begin (), end = char_set.end (); iter != end; ++iter, current_char++)
      {
          //заполнение ненужных символов
        for (size_t i = 1, count = *iter - last_char_code; i < count; i++, current_char++)
          *current_char = '?';

        *current_char  = *iter;
        last_char_code = *iter;
      }

      font_desc.char_codes       = utf32_buffer.data ();
      font_desc.char_codes_count = utf32_buffer.size ();
      font_desc.first_glyph_code = first_code;
    }

    font_desc.file_name      = params.source_font.c_str ();
    font_desc.glyph_size     = params.glyph_size;
    font_desc.stroke_width   = params.stroke_width;
    font_desc.glyph_interval = params.glyph_interval;
    font_desc.fast_convert   = params.fast_convert;

    convert (font_desc, font, image);

    image.Rename (params.result_image.c_str ());

    font.Rename       (params.result_font.c_str ());
    font.SetImageName (params.result_xml_image_name.c_str ());

    if (params.use_force_advance)
    {
      GlyphInfo *current_glyph = font.Glyphs ();

      for (size_t i = 0, count = font.GlyphsTableSize (); i < count; i++, current_glyph++)
        current_glyph->advance_x = current_glyph->width + params.force_advance;
    }

    font.Save  (params.result_font.c_str ());
    image.Save (params.result_image.c_str ());
  }
  catch (std::exception& e)
  {
    error ("%s", e.what ());
  }
  catch (...)
  {
    error ("unknown exception");
  }
}

int main (int argc, const char** argv)
{
  Params params;

  static Option options [] = {
    {xtl::bind (&command_line_help,                   _1, xtl::ref (params)), "help",                  '?',          0, "print help message"},
    {xtl::bind (&command_line_silent,                 _1, xtl::ref (params)), "silent",                's',          0, "quiet mode"},
    {xtl::bind (&command_line_result_image_name,      _1, xtl::ref (params)), "image",                 'i',     "file", "set output image file"},
    {xtl::bind (&command_line_result_xml_image_name,  _1, xtl::ref (params)), "xml-image-name",         0,      "file", "set output image name in xml file"},
    {xtl::bind (&command_line_result_font,            _1, xtl::ref (params)), "out-font",              'o',     "file", "set output font name"},
    {xtl::bind (&command_line_char_map_file,          _1, xtl::ref (params)), "char-map-file",          0,      "file", "set file with characters to include in font"},
    {xtl::bind (&command_line_char_map_file_encoding, _1, xtl::ref (params)), "char-map-file-encoding", 0,  "encoding", "set encoding of file with characters to include in font"},
    {xtl::bind (&command_line_glyph_size,             _1, xtl::ref (params)), "glyph-size",             0,      "size", "set glyph size"},
    {xtl::bind (&command_line_stroke_width,           _1, xtl::ref (params)), "stroke-width",           0,      "size", "set stroke width"},
    {xtl::bind (&command_line_first_glyph_code,       _1, xtl::ref (params)), "first-glyph-code",       0,     "index", "set first index of generated glyph range"},
    {xtl::bind (&command_line_last_glyph_code,        _1, xtl::ref (params)), "last-glyph-code",        0,     "index", "set last index of generated glyph range"},
    {xtl::bind (&command_line_glyph_interval,         _1, xtl::ref (params)), "glyph-interval",         0,      "size", "set glyph interval"},
    {xtl::bind (&command_line_force_advance,          _1, xtl::ref (params)), "force-advance",          0,      "size", "set force generated advance x"},
    {xtl::bind (&command_line_fast_convert,           _1, xtl::ref (params)), "fast",                   0,           0, "fast mode"},
  };

  static const size_t options_count = sizeof (options) / sizeof (*options);

    //инициализация

  params.options           = options;
  params.options_count     = options_count;
  params.glyph_size        = 0;
  params.stroke_width      = 0;
  params.first_glyph_code  = 0;
  params.last_glyph_code   = 0;
  params.glyph_interval    = -1;
  params.force_advance     = 0;
  params.fast_convert      = false;
  params.print_help        = false;
  params.use_force_advance = false;
  params.silent            = false;

  CommandLine command_line;

  for (size_t i = 0; i < params.options_count; i++)
    command_line.SetSwitchHandler (options [i].name, options [i].short_name, options [i].argument_name, options [i].handler);

  try
  {
      //разбор командной строки
    command_line.Process (argc, argv);

      // --help только печатает сообщение помощи

    if (params.print_help)
      return 0;

    if (command_line.ParamsCount () != 1)
    {
      printf ("Sources count is %d, only one source file allowed\n", command_line.ParamsCount ());
      return 1;
    }

    params.source_font = command_line.Param (0);

      //проверка корректности ввода

    validate (params);

      //построение шрифта

    build (params);
  }
  catch (std::exception& e)
  {
    printf ("exception '%s'\n", e.what ());
    return 1;
  }

  return 0;
}
