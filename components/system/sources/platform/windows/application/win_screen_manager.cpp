#include "../shared.h"

using namespace syslib;

namespace
{

/*
    Константы
*/

const size_t DEFAULT_SCREEN_ARRAY_RESERVE = 8;                //резервируемое количество экранов
const size_t MODE_ARRAY_RESERVE           = 32;               //резервируемое количество видео-режимов
const char*  LOG_NAME                     = "system.windows"; //имя протокола вывода сообщений

/*
    Получение информации о видео-режиме
*/

bool get_mode_desc (const char* device_name, int mode_index, size_t xdpi, size_t ydpi, ScreenModeDesc& mode_desc, Rect* screen_rect=0)
{
  DEVMODE dev_mode_desc;
  
  memset (&dev_mode_desc, 0, sizeof (dev_mode_desc));

  dev_mode_desc.dmSize  = sizeof (dev_mode_desc);

#ifdef WINCE
  if (!EnumDisplaySettings (common::to_wstring_from_utf8(device_name).c_str(), mode_index, &dev_mode_desc))
    return false;
#else
  if (!EnumDisplaySettings (device_name, mode_index, &dev_mode_desc))
    return false;
#endif

  mode_desc.width        = dev_mode_desc.dmPelsWidth;
  mode_desc.height       = dev_mode_desc.dmPelsHeight;
  mode_desc.color_bits   = dev_mode_desc.dmBitsPerPel;
  mode_desc.refresh_rate = dev_mode_desc.dmDisplayFrequency;  
  mode_desc.xdpi         = xdpi;
  mode_desc.ydpi         = ydpi;

#ifdef WINCE  
  if (screen_rect)  
  {
    screen_rect->left   = 0;
    screen_rect->top    = 0;
    screen_rect->right  = dev_mode_desc.dmPelsWidth;
    screen_rect->bottom = dev_mode_desc.dmPelsHeight;
  }
#else
  if (screen_rect)  
  {
    screen_rect->left   = dev_mode_desc.dmPosition.x;
    screen_rect->top    = dev_mode_desc.dmPosition.y;
    screen_rect->right  = dev_mode_desc.dmPosition.x + dev_mode_desc.dmPelsWidth;
    screen_rect->bottom = dev_mode_desc.dmPosition.y + dev_mode_desc.dmPelsHeight;
  }
#endif  

  return true;
}

/*
    Описатель экрана
*/

typedef stl::vector<ScreenModeDesc> ScreenModeArray;

struct ScreenDesc: public xtl::reference_counter
{
  HDC             device_context; //контекст устройства
  stl::string     name;           //имя устройства
  stl::string     win_name;       //имя устройства в Windows
  size_t          xdpi;           //плотность пикселей по горизонтали
  size_t          ydpi;           //плотность пикселей по вертикали
  ScreenModeArray modes;          //режимы экрана
  
///Конструктор
  ScreenDesc (const char* in_name, const char* in_win_name, size_t in_xdpi, size_t in_ydpi)
    : device_context ()
    , name (in_name)
    , win_name (in_win_name)
    , xdpi (in_xdpi)
    , ydpi (in_ydpi)
  {
      //отсечение завершающих пробелов в имени устройства

    stl::string::reverse_iterator iter;

    for (iter=name.rbegin (); iter!=name.rend () && *iter == ' '; ++iter);

    name.erase (iter.base (), name.end ());    
    
      //построение списка видео-режимов
      
    modes.reserve (MODE_ARRAY_RESERVE);

    ScreenModeDesc mode_desc;

    for (int mode_index=0; get_mode_desc (win_name.c_str (), mode_index, xdpi, ydpi, mode_desc); mode_index++)
      modes.push_back (mode_desc);
  }
  
///Деструктор
  ~ScreenDesc ()
  {
    if (device_context)
      DeleteDC (device_context);
  }

///Получение дескприптора контекста устройства
  HDC GetDC ()
  {
    if (device_context)
      return device_context;

    device_context = CreateDCW (common::to_wstring_from_utf8(win_name).c_str (), 0, 0, 0);

    if (!device_context)
      raise_error (common::format ("syslib::windows::ScreenDesc::GetDC(device-name='%s')", name.c_str ()).c_str ());

    return device_context; 
  }  
};

typedef xtl::intrusive_ptr<ScreenDesc> ScreenDescPtr;
typedef stl::vector<ScreenDescPtr>     ScreenDescArray;

/*
    Построитель списка экранов
*/

struct ScreenArrayBuilder
{
  common::Log      log;     //протокол
  ScreenDescArray& screens; //массив экранов

  ScreenArrayBuilder (ScreenDescArray& in_screens)
    : log (LOG_NAME)
    , screens (in_screens)
  {
    HDC dc = GetDC (0);
    
    EnumDisplayMonitors (dc, 0, &ScreenArrayBuilder::MonitorEnumProc, reinterpret_cast<DWORD> (this));
    
    ReleaseDC (0, dc);
  }

  static BOOL CALLBACK MonitorEnumProc (HMONITOR monitor, HDC dc, LPRECT, LPARAM data)
  {
    ScreenArrayBuilder* builder = reinterpret_cast<ScreenArrayBuilder*> (data);

    if (!builder || !dc)
      return FALSE;

    MONITORINFOEX info;

    memset (&info, 0, sizeof (info));

    info.cbSize = sizeof (info);

    if (!GetMonitorInfo (monitor, &info))
      return TRUE;

    DISPLAY_DEVICE device_info;

    memset (&device_info, 0, sizeof device_info);

    device_info.cb = sizeof device_info;

    if (!EnumDisplayDevices (info.szDevice, 0, &device_info, 0))
      return TRUE;

    int xdpi = GetDeviceCaps (dc, LOGPIXELSX),
        ydpi = GetDeviceCaps (dc, LOGPIXELSY);

    try
    {
      ScreenDescPtr screen (new ScreenDesc ((char*)device_info.DeviceString, (char*)info.szDevice, xdpi, ydpi), false);

      builder->screens.push_back (screen);
    }
    catch (std::exception& exception)
    {
      builder->log.Printf ("%s\n    at syslib::windows::ScreenArrayBuilderScreenArrayBuilder::MonitorEnumProc", exception.what ());
    }
    catch (...)
    {
      builder->log.Printf ("Unknown exception\n    at syslib::windows::ScreenArrayBuilderScreenArrayBuilder::MonitorEnumProc");
    }    

    return TRUE;
  }
};

/*
    Менеджер экранов
*/

struct ScreenManagerImpl
{
  ScreenDescArray screens; //экраны
  
///Конструктор
  ScreenManagerImpl ()
  {
    screens.reserve (DEFAULT_SCREEN_ARRAY_RESERVE);  

      //построение массива экранов

    ScreenArrayBuilder builder (screens);
  }
  
///Получение экрана
  static ScreenDescPtr GetScreen (screen_t handle)
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("syslib::windows::ScreenManagerImpl::GetScreen", "handle");
    
    return (ScreenDesc*)handle;
  }
};

typedef common::Singleton<ScreenManagerImpl> ScreenManagerSingleton;

}

/*
    Перечисление экранов
*/

size_t WindowsScreenManager::GetScreensCount ()
{
  return ScreenManagerSingleton::Instance ()->screens.size ();
}

/*
    Создание / удаление экрана
*/

screen_t WindowsScreenManager::CreateScreen (size_t screen_index)
{  
  ScreenManagerSingleton::Instance instance;

  size_t screens_count = instance->screens.size ();
  
  if (screen_index >= screens_count)
    throw xtl::make_range_exception ("syslib::WindowsScreenManager::CreateScreen", "screen_index", screen_index, screens_count);
    
  ScreenDescPtr screen = instance->screens [screen_index];
  
  addref (&*screen);

  return (screen_t)screen.get ();
}

void WindowsScreenManager::DestroyScreen (screen_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::WindowsScreenManager::DestroyScreen", "handle");
    
  ScreenDescPtr screen ((ScreenDesc*)handle);
  
  release (&*screen);
}

/*
    Имя экрана
*/

const char* WindowsScreenManager::GetScreenName (screen_t handle)
{
  try
  {
    ScreenDescPtr screen = ScreenManagerImpl::GetScreen (handle);
    
    return screen->name.c_str ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsScreenManager::GetScreenName");
    throw;
  }
}

/*
    Получение списка видео-режимов экрана
*/

size_t WindowsScreenManager::GetScreenModesCount (screen_t handle)
{
  try
  {
    ScreenDescPtr screen = ScreenManagerImpl::GetScreen (handle);
    
    return screen->modes.size ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsScreenManager::GetScreenModesCount");
    throw;
  }
}

void WindowsScreenManager::GetScreenMode (screen_t handle, size_t mode_index, ScreenModeDesc& mode_desc)
{
  try
  {
    ScreenDescPtr screen = ScreenManagerImpl::GetScreen (handle);
    
    if (mode_index >= screen->modes.size ())
      throw xtl::make_range_exception ("", "mode_index", mode_index, screen->modes.size ());
      
    mode_desc = screen->modes [mode_index];
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsScreenManager::GetScreenMode");
    throw;
  }
}

/*
    Установка текущего видео-режима экрана
*/

namespace
{

void set_mode_desc (const char* device_name, DEVMODE* mode)
{
#ifdef WINCE
  LONG result = ChangeDisplaySettingsEx (common::to_wstring_from_utf8(device_name).c_str(), mode, 0, 0, 0);
#else
  LONG result = ChangeDisplaySettingsEx (device_name, mode, 0, 0, 0);
#endif

  if (result == DISP_CHANGE_SUCCESSFUL)
    return;

  const char* msg = "";
  
  switch (result)
  {
#ifdef DISP_CHANGE_BADDUALVIEW
    case DISP_CHANGE_BADDUALVIEW: msg = "System is DualView capable"; break;
#endif
    case DISP_CHANGE_BADFLAGS:    msg = "Invalid set of flags"; break;
    case DISP_CHANGE_BADMODE:     msg = "The graphics mode is not supported"; break;
    case DISP_CHANGE_BADPARAM:    msg = "Invalid parameter"; break;
    case DISP_CHANGE_FAILED:      msg = "The display driver failed the specified graphics mode"; break;
    case DISP_CHANGE_NOTUPDATED:  msg = "Unable to write settings to the registry"; break;
    case DISP_CHANGE_RESTART:     msg = "The computer must be restarted in order for the graphics mode to work"; break;
    default:
      raise_error ("ChangeDisplaySettingsEx");
      break;
  }

  throw xtl::format_operation_exception ("ChangeDisplaySettingsEx", msg);
}

}

void WindowsScreenManager::SetScreenCurrentMode (screen_t handle, const ScreenModeDesc& mode_desc)
{
  const char* device_name = "null";

  try
  {    
    ScreenDescPtr screen = ScreenManagerImpl::GetScreen (handle);
    
    device_name = screen->name.c_str ();
    
    DEVMODE dev_mode_desc;

    memset (&dev_mode_desc, 0, sizeof (dev_mode_desc));
    
    dev_mode_desc.dmSize             = sizeof (DEVMODE);
    dev_mode_desc.dmPelsWidth        = mode_desc.width;
    dev_mode_desc.dmPelsHeight       = mode_desc.height;
    dev_mode_desc.dmBitsPerPel       = mode_desc.color_bits;
    dev_mode_desc.dmDisplayFrequency = mode_desc.refresh_rate;
    dev_mode_desc.dmFields           = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | (mode_desc.refresh_rate ? DM_DISPLAYFREQUENCY : 0);
    
    set_mode_desc (screen->win_name.c_str (), &dev_mode_desc);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsScreenManager::SetCurrentMode(device-name='%s', mode='%ux%ux%u @ %uHz')", device_name, mode_desc.width, mode_desc.height, mode_desc.color_bits, mode_desc.refresh_rate);
    throw;
  }
}

void WindowsScreenManager::GetScreenCurrentMode (screen_t handle, ScreenModeDesc& mode_desc)
{
  try
  {
    ScreenDescPtr screen = ScreenManagerImpl::GetScreen (handle);    
    
    get_mode_desc (screen->win_name.c_str (), ENUM_CURRENT_SETTINGS, screen->xdpi, screen->ydpi, mode_desc);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsScreenManager::GetScreenCurrentMode");
    throw;
  }
}

void WindowsScreenManager::GetScreenDefaultMode (screen_t handle, ScreenModeDesc& mode_desc)
{
  try
  {
    ScreenDescPtr screen = ScreenManagerImpl::GetScreen (handle);
    
    get_mode_desc (screen->win_name.c_str (), ENUM_REGISTRY_SETTINGS, screen->xdpi, screen->ydpi, mode_desc);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsScreenManager::GetScreenDefaultMode");
    throw;
  }
}

/*
    Управление гамма-коррекцией экрана
*/

void WindowsScreenManager::SetScreenGammaRamp (screen_t handle, const Color3f table [256])
{
#ifdef WINCE
  raise_error("::GetScreenGammaRamp not supported for Windows Mobile");
#else
  try
  {
    ScreenDescPtr screen = ScreenManagerImpl::GetScreen (handle);
    
    WORD gamma_ramp_table[3][256];

    for (size_t i=0; i<256; i++)
    {
      gamma_ramp_table [0][i] = (WORD)(table [i].red * 65535.f);
      gamma_ramp_table [1][i] = (WORD)(table [i].green * 65535.f);
      gamma_ramp_table [2][i] = (WORD)(table [i].blue * 65535.f);
    }

    if (!SetDeviceGammaRamp (screen->GetDC (), gamma_ramp_table))
      raise_error (common::format ("::SetDeviceGammaRamp(device-name='%s')", screen->name.c_str ()).c_str ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsScreenManager::SetScreenGammaRamp");
    throw;
  }
#endif
}

void WindowsScreenManager::GetScreenGammaRamp (screen_t handle, Color3f table [256])
{
#ifdef WINCE
  raise_error("::GetScreenGammaRamp not supported for Windows Mobile");
#else
  try
  {
    ScreenDescPtr screen = ScreenManagerImpl::GetScreen (handle);

    WORD gamma_ramp_table [3][256];

    if (!GetDeviceGammaRamp (screen->GetDC (), gamma_ramp_table))
      raise_error (common::format ("::GetDeviceGammaRamp(device-name='%s')", screen->name.c_str ()).c_str ());

    for (size_t i=0; i<256; i++)
    {
      table [i].red   = (float)gamma_ramp_table[0][i] / 65535.f;
      table [i].green = (float)gamma_ramp_table[1][i] / 65535.f;
      table [i].blue  = (float)gamma_ramp_table[2][i] / 65535.f;
    }    
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsScreenManager::GetScreenGammaRamp");
    throw;
  }
#endif
}

/*
    Поиск экрана вмещающего окно
*/

namespace
{

struct ScreenSearcher
{
  ScreenDescPtr    containing_screen;        //устройств вывода с максимальной областью перекрытия
  size_t           intersection_region_size; //размер области перекрытия
  ScreenDescArray& screens;
  
  ScreenSearcher (HDC dc, ScreenDescArray& in_screens) : intersection_region_size (0), screens (in_screens)
  {
    EnumDisplayMonitors (dc, 0, &ScreenSearcher::MonitorEnumProc, reinterpret_cast<DWORD> (this));
  }

  static BOOL CALLBACK MonitorEnumProc (HMONITOR monitor, HDC dc, LPRECT intersection_region, LPARAM data)
  {
    ScreenSearcher* search_data = reinterpret_cast<ScreenSearcher*> (data);

    if (!search_data || !dc)
      return FALSE;

    MONITORINFOEX info;

    memset (&info, 0, sizeof (info));

    info.cbSize = sizeof (info);

    if (!GetMonitorInfo (monitor, &info))
      return FALSE;

    size_t intersection_region_size = (intersection_region->right - intersection_region->left) *
                                      (intersection_region->bottom - intersection_region->top);                                      
                                      
    if (intersection_region_size > search_data->intersection_region_size)
    {
      for (ScreenDescArray::iterator iter=search_data->screens.begin (), end=search_data->screens.end (); iter != end; ++iter)
      {
        ScreenDescPtr screen = *iter;      
        
        if (!strcmp (screen->win_name.c_str (), (char*)info.szDevice))
        {
          search_data->intersection_region_size = intersection_region_size;
          search_data->containing_screen        = screen;
          
          break;
        }
      }
    }
    
    return TRUE;
  }  
};

}

screen_t WindowsScreenManager::FindContainingScreen (const void* native_window_handle)
{
  try
  {
    ScreenManagerSingleton::Instance instance;
    
    HWND window = (HWND)native_window_handle;
    
    if (!window)
      throw xtl::make_null_argument_exception ("", "native_window_handle");

    HDC dc = GetWindowDC (window);

    if (!dc)
      raise_error ("::GetWindowDC");

    ScreenSearcher searcher (dc, instance->screens);

    ReleaseDC (window, dc);
    
    if (searcher.containing_screen)
      addref (&*searcher.containing_screen);

    return (screen_t)searcher.containing_screen.get ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsScreenManager::FindContainingScreen");
    throw;
  }
}

/*
    Получение платформо-зависимого дескриптора экрана
*/

const void* WindowsScreenManager::GetNativeScreenHandle (screen_t handle)
{  
  try
  {
    ScreenDescPtr screen = ScreenManagerImpl::GetScreen (handle);    
    
    return screen->GetDC ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsScreenManager::GetNativeScreenHandle");
    throw;
  }
}

/*
    Получение платформо-зависимых свойств экрана
*/

void WindowsScreenManager::GetScreenProperties (screen_t handle, common::PropertyMap& properties)
{
  try
  {
    ScreenDescPtr screen = ScreenManagerImpl::GetScreen (handle);    
    
    properties.SetProperty ("win-name", screen->win_name.c_str ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsScreenManager::GetScreenProperties");
    throw;
  }
}
