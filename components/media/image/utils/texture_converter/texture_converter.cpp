#include <cmath>
#include <cstdio>

#include <stl/string>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/ref.h>

#include <common/command_line.h>
#include <common/file.h>
#include <common/strlib.h>

#include <media/compressed_image.h>
#include <media/image.h>

#if defined (_MSC_VER) || defined (__APPLE__) && !defined (IPHONE)
  #include <TextureConverter.h>
#endif

/*
    Константы
*/

const char*  APPLICATION_NAME                = "texture-converter";
const char*  ATC_RGB_AMD                     = "atitc_rgb";
const char*  ATC_RGBA_EXPLICIT_ALPHA_AMD     = "atitc_rgba_explicit_alpha";
const char*  ATC_RGBA_INTERPOLATED_ALPHA_AMD = "atitc_rgba_interpolated_alpha";
const size_t HELP_STRING_PREFIX_LENGTH       = 30;

const char DDS_MAGIC_ID [4] = {'D', 'D', 'S', ' '};

// surface description flags
const unsigned long DDSD_CAPS           = 0x00000001l;
const unsigned long DDSD_HEIGHT         = 0x00000002l;
const unsigned long DDSD_WIDTH          = 0x00000004l;
const unsigned long DDSD_PIXELFORMAT    = 0x00001000l;
const unsigned long DDSD_MIPMAPCOUNT    = 0x00020000l;

// pixel format flags
const unsigned long DDPF_FOURCC         = 0x00000004l;

// dwCaps1 flags
const unsigned long DDSCAPS_COMPLEX         = 0x00000008l;
const unsigned long DDSCAPS_TEXTURE         = 0x00001000l;
const unsigned long DDSCAPS_MIPMAP          = 0x00400000l;

typedef unsigned int  uint32;
typedef unsigned char uint8;

#if !defined(MAKEFOURCC)
# define MAKEFOURCC(ch0, ch1, ch2, ch3) \
    (uint32(uint8(ch0)) | (uint32(uint8(ch1)) << 8) | \
        (uint32(uint8(ch2)) << 16) | (uint32(uint8(ch3)) << 24 ))
#endif

const uint32 FOURCC_ATC_RGB_AMD                     = MAKEFOURCC ('A', 'T', 'C', ' ');
const uint32 FOURCC_ATC_RGBA_EXPLICIT_ALPHA_AMD     = MAKEFOURCC ('A', 'T', 'C', 'I');
const uint32 FOURCC_ATC_RGBA_INTERPOLATED_ALPHA_AMD = MAKEFOURCC ('A', 'T', 'C', 'A');

///Формат пикселей DDS файла
struct DdsPixelFormat
{
  uint32 dwSize;        //Structure size; set to 32 (bytes)
  uint32 dwFlags;       //Values which indicate what type of data is in the surface
  uint32 dwFourCC;      //Four-character codes for specifying compressed or custom formats. Possible values include: DXT1, DXT2, DXT3, DXT4, or DXT5. A FourCC of DX10 indicates the prescense of the DDS_HEADER_DXT10 extended header, and the dxgiFormat member of that structure indicates the true format. When using a four-character code, dwFlags must include DDPF_FOURCC
  uint32 dwRGBBitCount; //Number of bits in an RGB (possibly including alpha) format. Valid when dwFlags includes DDPF_RGB, DDPF_LUMINANCE, or DDPF_YUV
  uint32 dwRBitMask;    //Red (or lumiannce or Y) mask for reading color data. For instance, given the A8R8G8B8 format, the red mask would be 0x00ff0000
  uint32 dwGBitMask;    //Green (or U) mask for reading color data. For instance, given the A8R8G8B8 format, the green mask would be 0x0000ff00
  uint32 dwBBitMask;    //Blue (or V) mask for reading color data. For instance, given the A8R8G8B8 format, the blue mask would be 0x000000ff
  uint32 dwABitMask;    //Alpha mask for reading alpha data. dwFlags must include DDPF_ALPHAPIXELS or DDPF_ALPHA. For instance, given the A8R8G8B8 format, the alpha mask would be 0xff000000
};

///Заголовок DDS файла
struct DdsHeader
{
  uint32         dwSize;            //Size of structure. This member must be set to 124
  uint32         dwFlags;           //Flags to indicate which members contain valid data
  uint32         dwHeight;          //Surface height (in pixels)
  uint32         dwWidth;           //Surface width (in pixels)
  uint32         dwLinearSize;      //The number of bytes per scan line in an uncompressed texture; the total number of bytes in the top level texture for a compressed texture. The pitch must be DWORD aligned
  uint32         dwDepth;           //Depth of a volume texture (in pixels), otherwise unused
  uint32         dwMipMapCount;     //Number of mipmap levels, otherwise unused
  uint32         dwReserved1 [11];  //Unused
  DdsPixelFormat ddspf;             //The pixel format
  uint32         dwCaps;            //Specifies the complexity of the surfaces stored
  uint32         dwCaps2;           //Additional detail about the surfaces stored
  uint32         dwCaps3;           //Unused
  uint32         dwCaps4;           //Unused
  uint32         dwReserved2;       //Unused
};

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

unsigned int get_mips_count (unsigned int size)
{
  return (unsigned int)(log ((double)size) / log (2.0)) + 1;
}

unsigned int get_mips_count (unsigned int width, unsigned int height)
{
  return get_mips_count (stl::max (width, height));
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

//сохранение данных в DDS-файл
void save_compressed_dds (const char* file_name, unsigned int width, unsigned int height, uint32 fourcc, unsigned int mips_count, const unsigned char** mips_data, const unsigned int* mips_data_sizes)
{
  common::OutputFile file (file_name);

  file.Write (DDS_MAGIC_ID, sizeof (DDS_MAGIC_ID));

  DdsHeader header;

  memset (&header, 0, sizeof (header));

  header.dwSize   = sizeof (header);
  header.dwFlags  = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
  header.dwHeight = height;
  header.dwWidth  = width;

  header.ddspf.dwSize   = sizeof (header.ddspf);
  header.ddspf.dwFlags  = DDPF_FOURCC;
  header.ddspf.dwFourCC = fourcc;

  header.dwCaps = DDSCAPS_TEXTURE;

  if (mips_count > 0)
  {
    header.dwFlags      |= DDSD_MIPMAPCOUNT;
    header.dwCaps       |= DDSCAPS_COMPLEX | DDSCAPS_MIPMAP;
    header.dwMipMapCount = mips_count;
  }

  file.Write (&header, sizeof (header));

  for (size_t i = 0; i < mips_count; i++)
    file.Write (mips_data [i], mips_data_sizes [i]);
}

#if defined (_MSC_VER) || defined (__APPLE__) && !defined (IPHONE)

const char* get_qualcomm_error_name (unsigned int error)
{
  switch (error)
  {
    case Q_ERROR_UNSUPPORTED_DIMENSIONS:      return "Q_ERROR_UNSUPPORTED_DIMENSIONS";
    case Q_ERROR_UNSUPPORTED_SRC_FORMAT:      return "Q_ERROR_UNSUPPORTED_SRC_FORMAT";
    case Q_ERROR_UNSUPPORTED_DST_FORMAT:      return "Q_ERROR_UNSUPPORTED_DST_FORMAT";
    case Q_ERROR_UNSUPPORTED_SRC_FORMAT_FLAG: return "Q_ERROR_UNSUPPORTED_SRC_FORMAT_FLAG";
    case Q_ERROR_UNSUPPORTED_DST_FORMAT_FLAG: return "Q_ERROR_UNSUPPORTED_DST_FORMAT_FLAG";
    case Q_ERROR_INCORRECT_SRC_PARAMETER:     return "Q_ERROR_INCORRECT_SRC_PARAMETER";
    case Q_ERROR_INCORRECT_DST_PARAMETER:     return "Q_ERROR_INCORRECT_DST_PARAMETER";
    case Q_ERROR_INCORRECT_DATASIZE:          return "Q_ERROR_INCORRECT_DATASIZE";
    case Q_ERROR_OTHER:                       return "Q_ERROR_OTHER";
    default:                                  return "Unknown";
  }
}

void check_qualcomm_error (const char* source, unsigned int error)
{
  if (error == Q_SUCCESS)
    return;

  throw xtl::format_operation_exception (source, get_qualcomm_error_name (error));
}

void qualcomm_texture_compress (const Params& params)
{
  static const char* METHOD_NAME = "media::qualcomm_texture_compress";

  media::Image source_image (params.source.c_str ());

  unsigned int source_format,
               destination_format;

  switch (source_image.Format ())
  {
    case media::PixelFormat_RGB8:
      source_format = Q_FORMAT_RGB_8UI;
      break;
    case media::PixelFormat_BGR8:
      source_image.Convert (media::PixelFormat_RGB8);
      source_format = Q_FORMAT_RGB_8UI;
      break;
    case media::PixelFormat_RGBA8:
      source_format = Q_FORMAT_RGBA_8UI;
      break;
    case media::PixelFormat_BGRA8:
      source_format = Q_FORMAT_BGRA_8888;
      break;
    case media::PixelFormat_L8:
      source_format = Q_FORMAT_LUMINANCE_8;
      break;
    case media::PixelFormat_A8:
      source_format = Q_FORMAT_ALPHA_8;
      break;
    case media::PixelFormat_LA8:
      source_format = Q_FORMAT_LUMINANCE_ALPHA_88;
      break;
    default:
      throw xtl::format_operation_exception (METHOD_NAME, "Unsupported source format '%s'", media::get_format_name (source_image.Format ()));
  }

  uint32 fourcc;

  if (params.target_format == ATC_RGB_AMD)
  {
    destination_format = Q_FORMAT_ATITC_RGB;
    fourcc             = FOURCC_ATC_RGB_AMD;
  }
  else if (params.target_format == ATC_RGBA_EXPLICIT_ALPHA_AMD)
  {
    destination_format = Q_FORMAT_ATC_RGBA_EXPLICIT_ALPHA;
    fourcc             = FOURCC_ATC_RGBA_EXPLICIT_ALPHA_AMD;
  }
  else if (params.target_format == ATC_RGBA_INTERPOLATED_ALPHA_AMD)
  {
    destination_format = Q_FORMAT_ATC_RGBA_INTERPOLATED_ALPHA;
    fourcc             = FOURCC_ATC_RGBA_INTERPOLATED_ALPHA_AMD;
  }
  else
    throw xtl::format_operation_exception (METHOD_NAME, "Unsupported target format '%s'", params.target_format.c_str ());

  TQonvertImage source;

  memset (&source, 0, sizeof (source));

  source.nFormat   = source_format;
  source.nWidth    = source_image.Width ();
  source.nHeight   = source_image.Height ();
  source.nDataSize = source.nWidth * source.nHeight * media::get_bytes_per_pixel (source_image.Format ());
  source.pData     = (unsigned char*)source_image.Bitmap ();

  unsigned int mips_count = get_mips_count (source.nWidth, source.nHeight);

  xtl::uninitialized_storage<TQonvertImage>  mips_images (mips_count);
  xtl::uninitialized_storage<unsigned char*> mips_data (mips_count);
  xtl::uninitialized_storage<unsigned int>   mips_sizes (mips_count);

  memset (mips_images.data (), 0, sizeof (TQonvertImage) * mips_count);

  TFormatFlags compression_flags;

  memset (&compression_flags, 0, sizeof (compression_flags));

  compression_flags.nFlipY = 1;

  size_t total_size = 0;

  for (size_t i = 0; i < mips_count; i++)
  {
    TQonvertImage& destination = mips_images.data () [i];

    destination.nFormat      = destination_format;
    destination.nWidth       = source.nWidth >> i;
    destination.nHeight      = source.nHeight >> i;
    destination.pFormatFlags = &compression_flags;

    if (!destination.nWidth)  destination.nWidth  = 1;
    if (!destination.nHeight) destination.nHeight = 1;

    check_qualcomm_error (METHOD_NAME, Qonvert (&source, &destination));

    mips_sizes.data () [i] = destination.nDataSize;

    total_size += destination.nDataSize;
  }

  xtl::uninitialized_storage<unsigned char> destination_data (total_size);

  size_t current_mip_data_offset = 0;

  for (size_t i = 0; i < mips_count; i++)
  {
    TQonvertImage& destination = mips_images.data () [i];

    destination.pData = destination_data.data () + current_mip_data_offset;

    check_qualcomm_error (METHOD_NAME, Qonvert (&source, &destination));

    mips_data.data () [i] = destination.pData;

    current_mip_data_offset += destination.nDataSize;
  }

  if (common::suffix (params.target) == ".dds")
    save_compressed_dds (params.target.c_str (), source.nWidth, source.nHeight, fourcc, mips_count, (const unsigned char**)mips_data.data (), mips_sizes.data ());
  else
    throw xtl::format_operation_exception (METHOD_NAME, "Unsupported container for target image '%s'", params.target.c_str ());
}

void qualcomm_texture_decompress (const Params& params)
{
  static const char* METHOD_NAME = "media::qualcomm_texture_decompress";

  media::CompressedImage source_image (params.source.c_str ());

  if ((params.source_format == ATC_RGB_AMD && xtl::xstrcmp (source_image.Format (), "atc")) ||
      (params.source_format == ATC_RGBA_EXPLICIT_ALPHA_AMD && xtl::xstrcmp (source_image.Format (), "atci")) ||
      (params.source_format == ATC_RGBA_INTERPOLATED_ALPHA_AMD && xtl::xstrcmp (source_image.Format (), "atca")))
    throw xtl::format_operation_exception (METHOD_NAME, "Source format '%s' not equal to compressed image format '%s'", params.source_format.c_str (), source_image.Format ());
  else if (params.source_format != ATC_RGB_AMD &&
           params.source_format != ATC_RGBA_EXPLICIT_ALPHA_AMD &&
           params.source_format != ATC_RGBA_INTERPOLATED_ALPHA_AMD &&
           !params.source_format.empty ())
    throw xtl::format_operation_exception (METHOD_NAME, "Unsupported source format '%s'", params.source_format.c_str ());

  unsigned int       source_format,
                     destination_format,
                     destination_size = source_image.Width () * source_image.Height ();
  media::PixelFormat result_pixel_format;

  if (!xtl::xstrcmp (source_image.Format (), "atc"))
  {
    source_format       = Q_FORMAT_ATITC_RGB;
    destination_format  = Q_FORMAT_RGB_8UI;
    destination_size   *= 3;
    result_pixel_format = media::PixelFormat_RGB8;
  }
  else if (!xtl::xstrcmp (source_image.Format (), "atci"))
  {
    source_format       = Q_FORMAT_ATC_RGBA_EXPLICIT_ALPHA;
    destination_format  = Q_FORMAT_RGBA_8UI;
    destination_size   *= 4;
    result_pixel_format = media::PixelFormat_RGBA8;
  }
  else if (!xtl::xstrcmp (source_image.Format (), "atca"))
  {
    source_format       = Q_FORMAT_ATC_RGBA_INTERPOLATED_ALPHA;
    destination_format  = Q_FORMAT_RGBA_8UI;
    destination_size   *= 4;
    result_pixel_format = media::PixelFormat_RGBA8;
  }
  else
    throw xtl::format_operation_exception (METHOD_NAME, "Unsupported compressed texture format '%s'", source_image.Format ());

  TQonvertImage source, destination;

  memset (&source, 0, sizeof (source));
  memset (&destination, 0, sizeof (destination));

  source.nFormat      = source_format;
  source.nWidth       = source_image.Width ();
  source.nHeight      = source_image.Height ();
  source.nDataSize    = source_image.BitmapSize (0, 0);
  source.pData        = (unsigned char*)source_image.Bitmap (0, 0);

  xtl::uninitialized_storage<unsigned char> destination_data (destination_size);

  destination.nFormat   = destination_format;
  destination.nWidth    = source.nWidth;
  destination.nHeight   = source.nHeight;
  destination.nDataSize = destination_size;
  destination.pData     = destination_data.data ();

  check_qualcomm_error (METHOD_NAME, Qonvert (&source, &destination));

  media::Image result (destination.nWidth, destination.nHeight, 1, result_pixel_format, destination_data.data ());

  result.Save (params.target.c_str ());
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
  #if defined (_MSC_VER) || defined (__APPLE__) && !defined (IPHONE)
      { "", ATC_RGB_AMD,                     xtl::bind (&qualcomm_texture_compress, _1) },
      { "", ATC_RGBA_EXPLICIT_ALPHA_AMD,     xtl::bind (&qualcomm_texture_compress, _1) },
      { "", ATC_RGBA_INTERPOLATED_ALPHA_AMD, xtl::bind (&qualcomm_texture_compress, _1) },
      { ATC_RGB_AMD, "",                     xtl::bind (&qualcomm_texture_decompress, _1) },
      { ATC_RGBA_EXPLICIT_ALPHA_AMD, "",     xtl::bind (&qualcomm_texture_decompress, _1) },
      { ATC_RGBA_INTERPOLATED_ALPHA_AMD, "", xtl::bind (&qualcomm_texture_decompress, _1) },
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
