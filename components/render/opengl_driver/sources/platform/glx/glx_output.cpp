#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::glx;

namespace
{

/*
    Константы
*/

}

/*
    Описание реализации устройства вывода
*/

typedef xtl::com_ptr<Adapter> AdapterPtr;
typedef stl::vector<OutputModeDesc> OutputModeArray;

struct Output::Impl
{
  Log               log;                         //протокол графического драйвера
  OutputModeArray   modes;                       //режимы работы устройства
  Display*          display;                     //соединение с дисплеем
  int               screen_number;               //номер экрана дисплея
  stl::string       name;                        //имя

///Конструктор
  Impl (Display* in_display, size_t in_screen_number)
    : display (in_display)
    , screen_number (in_screen_number)
    , name(common::format ("screen%u", in_screen_number))
  {
    DisplayLock lock (display);
    
    int event_base = 0, error_base = 0;

    if (!XRRQueryExtension (display, &event_base, &error_base))
      throw xtl::format_operation_exception ("render::low_level::opengl::glx::Output::Impl::Impl", "RandR extension missing");      

    int sizes_count = 0, depths_count = 0;
    
    XRRScreenSize  *sizes  = XRRSizes (display, screen_number, &sizes_count);
    int            *depths = XListDepths (display, screen_number, &depths_count);

    if (!depths_count)
    {
      static int default_depths = 0;
    
      depths       = &default_depths;
      depths_count = 1;
    }
    
    for (int size_id=0; size_id<sizes_count; size_id++)
    {
      int    rates_count = 0;
      short *rates       = XRRRates (display, screen_number, size_id, &rates_count);

      if (!rates_count)
      {
        static short default_rate = 0;      
      
        rates_count = 1;
        rates       = &default_rate;
      }
      
      for (int rate_id=0; rate_id<rates_count; rate_id++)
      {
        for (int depth_id=0; depth_id<depths_count; depth_id++)
        {
          OutputModeDesc mode_desc;
          
          mode_desc.width        = sizes [size_id].width;
          mode_desc.height       = sizes [size_id].height;
          mode_desc.color_bits   = depths [depth_id];
          mode_desc.refresh_rate = rates [rate_id];
          
          modes.push_back (mode_desc);
        }
      }
    }
  }
  
///Деструктор
  ~Impl ()
  {
    try
    {

    }
    catch (...)
    {
    }
  }
};

/*
    Конструктор / деструктор
*/

Output::Output (Display* display, size_t screen_number)
{
  try
  {
    if (!display)
      throw xtl::make_null_argument_exception ("", "display");
      
    if (screen_number < 0)
      throw xtl::make_null_argument_exception ("", "screen_number");

    impl = new Impl (display, screen_number);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::glx::Output::Output");
    throw;
  }
}

Output::~Output ()
{

}

/*
    Получение имени
*/

const char* Output::GetName ()
{
  return impl->name.c_str ();
}

/*
    Получение списка видео-режимов
*/

size_t Output::GetModesCount ()
{
  return impl->modes.size ();
}

void Output::GetModeDesc (size_t mode_index, OutputModeDesc& mode_desc)
{
  if (mode_index >= impl->modes.size ())
    throw xtl::make_range_exception ("render::low_level::opengl::glx::Output::GetModeDesc", "mode_index", mode_index, impl->modes.size ());
    
  mode_desc = impl->modes [mode_index];
}

/*
    Установка текущего видео-режима
*/

namespace
{

void raise_format_not_supported_exception (const OutputModeDesc& mode_desc)
{
  throw xtl::format_not_supported_exception ("render::low_level::opengl::glx::Output::SetModeDesc", 
    "Screen mode '%dx%dx%d@%d' not supported", mode_desc.width, mode_desc.height, mode_desc.color_bits, mode_desc.refresh_rate);
}

}

void Output::SetCurrentMode (const OutputModeDesc& mode_desc)
{
  // блокировка дисплея

  DisplayLock lock (impl->display);

  // получение корневого окна
  
  Window root = RootWindow (impl->display, impl->screen_number);
  
  // получение конфигурации экрана
  
  XRRScreenConfiguration *conf = XRRGetScreenInfo (impl->display, root);
  
  // получение текущего разрешения и ориентации экрана
  
  Rotation original_rotation = 0;
  SizeID   original_size_id  = XRRConfigCurrentConfiguration (conf, &original_rotation);
  short    original_rate     = XRRConfigCurrentRate (conf);
  
  // получение всех доступных расрешений

  int            sizes_count = 0;
  SizeID         size_id     = 0;
  XRRScreenSize* sizes       = XRRSizes (impl->display, impl->screen_number, &sizes_count);

  // поиск запрашиваемого разрешения в списке доступных
  
  for (size_id=0; size_id<sizes_count; size_id++)
  {
    if (sizes [size_id].width == (int)mode_desc.width && sizes [size_id].height == (int)mode_desc.height)
      break;
  }

  // если не нашли, то выбрасываем исключение
  
  if (size_id == sizes_count)
    raise_format_not_supported_exception (mode_desc);
    
  // получение всех доступных частот для запрашиваемого разрешения
  
  int    rates_count = 0, rate_id = 0;
  short *rates       = XRRRates (impl->display, impl->screen_number, size_id, &rates_count);

  // если доступных частот нет, то выбрасываем исключение
  
  if (!rates_count)
    raise_format_not_supported_exception (mode_desc);
 
  // поиск запрашиваемой частоты

  for (rate_id=0; rate_id<rates_count; rate_id++)
  {
    if (rates [rate_id] == (int)mode_desc.refresh_rate)
      break;
  }
  
  // если частота не найдена, то выбрасываем исключение
  
  if (rate_id == rates_count)
    raise_format_not_supported_exception (mode_desc);
    
  // глубина цвета окна указывается при его создании и не может быть изменена
  // возможный вариант установки глубины цвета - пересоздание корневого или клиентского окна
  
  // если текущие разрешение и частота совпадают с запрашиваемыми, то ничего не делаем
  
  if (original_size_id == size_id && original_rate == rates [rate_id])
    return;
    
  // установка конфигурации экрана

  Status status = XRRSetScreenConfigAndRate (impl->display, conf, root, size_id, original_rotation, mode_desc.refresh_rate, CurrentTime);
  
  // если установка завершилась с ошибкой, то выбрасываем исключение

  if (status < Success)
    throw xtl::format_operation_exception ("render::low_level::opengl::glx::Output::SetModeDesc", "XRRSetScreenConfigAndRate failed");
}

void Output::GetCurrentMode (OutputModeDesc& mode_desc)
{
  // блокировка дисплея

  DisplayLock lock (impl->display);                                               

  // получение корневого окна
    
  Window root = RootWindow (impl->display, impl->screen_number);
  
  // получение конфигурации экрана
  
  XRRScreenConfiguration *conf = XRRGetScreenInfo (impl->display, root);
  
  // заполнение декриптора
  
  mode_desc.width        = DisplayWidth  (impl->display, impl->screen_number);
  mode_desc.height       = DisplayHeight (impl->display, impl->screen_number);
  mode_desc.color_bits   = DefaultDepth  (impl->display, impl->screen_number);
  mode_desc.refresh_rate = XRRConfigCurrentRate (conf);
}

/*
    Управление гамма-коррекцией
*/

void Output::SetGammaRamp (const Color3f table [256])
{
//  throw xtl::format_not_supported_exception ("render::low_level::opengl::glx::Output::GetGammaRamp", "Gamma ramp not supported in EGL");
}

void Output::GetGammaRamp (Color3f table [256])
{
  // блокировка дисплея

  DisplayLock lock (impl->display);                                               

  int size = 0;
  int event_base;
  int error_base;
  
  // запрос расширения XF86VidMode
  
  if (XF86VidModeQueryExtension (impl->display, &event_base, &error_base) == 0)
    throw xtl::format_operation_exception ("render::low_level::opengl::glx::Output::GetGammaRamp",
      "XF86VidModeQueryExtension missing");
      
  // получение размера гаммы
    
  if (!XF86VidModeGetGammaRampSize(impl->display, impl->screen_number, &size))
    throw xtl::format_operation_exception ("render::low_level::opengl::glx::Output::GetGammaRamp",
      "failed to get XF86VidModeGetGammaRampSize");
      
  // проверка корректности размера гаммы
      
  if (size != 256)
    throw xtl::format_operation_exception ("render::low_level::opengl::glx::Output::GetGammaRamp",
      "bad gamma ramp size: %d", size);

  // получение гаммы
  
  stl::vector<unsigned short> red   (size);
  stl::vector<unsigned short> green (size);
  stl::vector<unsigned short> blue  (size);

  XF86VidModeGetGammaRamp (impl->display, impl->screen_number, size, &red[0], &green[0], &blue[0]);
  
  // преобразование гаммы
  
  for (size_t i=0; i<size; i++)
  {
    table [i].red   = (float) red   [i] / 65535.f;
    table [i].green = (float) green [i] / 65535.f;
    table [i].blue  = (float) blue  [i] / 65535.f;
  }
}
