#include "shared.h"

using namespace syslib;
using namespace syslib::macosx;

namespace
{

void display_reconfiguration_callback (CGDirectDisplayID display, CGDisplayChangeSummaryFlags flags, void *userInfo);

//Класс для работы с экранами
class ScreenManagerImpl
{
  public:
    //Конструктор/деструктор
    ScreenManagerImpl ()
    {
      try
      {
        check_quartz_error (CGDisplayRegisterReconfigurationCallback (display_reconfiguration_callback, 0), "::CGDisplayRegisterReconfigurationCallback", "Can't register display reconfiguration callback");

        CGDisplayCount displays_count;

        check_quartz_error (CGGetActiveDisplayList (0, 0, &displays_count), "::CGGetActiveDisplayList", "Can't get displays count");

        xtl::uninitialized_storage<CGDirectDisplayID> display_list (displays_count);

        check_quartz_error (CGGetActiveDisplayList (displays_count, display_list.data (), &displays_count), "::CGGetActiveDisplayList",
                            "Can't get active display list");

        CGDirectDisplayID *current_display = display_list.data ();

        for (size_t i = 0; i < displays_count; i++, current_display++)
          screens.insert_pair (*current_display, CarbonScreen (*current_display));
      }
      catch (xtl::exception& e)
      {
        e.touch ("ScreenManagerImpl::ScreenManagerImpl");
        throw;
      }
    }

    ~ScreenManagerImpl ()
    {
      CGDisplayRemoveReconfigurationCallback (display_reconfiguration_callback, 0);
    }

    //Обновление экрана
    void UpdateScreen (CGDirectDisplayID display)
    {
      try
      {
        CGDisplayCount displays_count;

        check_quartz_error (CGGetActiveDisplayList (0, 0, &displays_count), "::CGGetActiveDisplayList", "Can't get displays count");

        xtl::uninitialized_storage<CGDirectDisplayID> display_list (displays_count);

        check_quartz_error (CGGetActiveDisplayList (displays_count, display_list.data (), &displays_count), "::CGGetActiveDisplayList",
                            "Can't get active display list");

        CGDirectDisplayID *current_display = display_list.data ();

        for (size_t i = 0; i < displays_count; i++, current_display++)
        {
          if (*current_display == display)
          {
            ScreenMap::iterator iter = screens.find (display);

            if (iter == screens.end ())
              screens.insert_pair (display, CarbonScreen (display));

            break;
          }
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::MacOsScreenManager::GetScreensCount");
        throw;
      }
    }

    //Получение экранов
    CarbonScreen GetScreen (size_t screen_index)
    {
      try
      {
        CGDisplayCount displays_count;

        check_quartz_error (CGGetActiveDisplayList (0, 0, &displays_count), "::CGGetActiveDisplayList", "Can't get displays count");

        xtl::uninitialized_storage<CGDirectDisplayID> display_list (displays_count);

        check_quartz_error (CGGetActiveDisplayList (displays_count, display_list.data (), &displays_count), "::CGGetActiveDisplayList",
                            "Can't get active display list");

        if (screen_index >= displays_count)
          throw xtl::make_range_exception ("", "screen_index", screen_index, 0u, displays_count);

        ScreenMap::iterator iter = screens.find (display_list.data () [screen_index]);

        if (iter == screens.end ())
          throw xtl::format_operation_exception ("", "Display %u is not active", screen_index);

        return iter->second;
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::ScreenManagerImpl::GetScreen (size_t)");
        throw;
      }
    }

    CarbonScreen GetScreen (screen_t screen)
    {
      try
      {
        ScreenMap::iterator iter = screens.find ((CGDirectDisplayID)screen);

        if (iter == screens.end ())
          throw xtl::format_operation_exception ("", "Can't find screen %u", screen);

        return iter->second;
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::ScreenManagerImpl::GetScreen (screen_t)");
        throw;
      }
    }

    size_t ScreensCount ()
    {
      return screens.size ();
    }

    ///Поиск экрана вмещающего окно
    screen_t FindContainingScreen (const void* window)
    {
        //получение границ окна
      ::Rect window_rect;

      if (GetWindowBounds ((WindowRef)window, kWindowStructureRgn, &window_rect) != noErr)
        return 0;

        //поиск максимального перекрытия
      size_t            max_intersection_size = 0;
      CGDirectDisplayID best_output           = 0;

      for (ScreenMap::iterator iter = screens.begin (), end = screens.end (); iter != end; ++iter)
      {
        CGDirectDisplayID display = iter->second.GetDisplayID ();

        CGRect output_bounds = CGDisplayBounds (display);

        short output_left   = (short)output_bounds.origin.x,
              output_top    = (short)output_bounds.origin.y,
              output_right  = (short)(output_bounds.origin.x + output_bounds.size.width),
              output_bottom = (short)(output_bounds.origin.y + output_bounds.size.height);

        ::Rect intersection_rect;

        intersection_rect.left   = stl::max (output_left,   window_rect.left);
        intersection_rect.top    = stl::max (output_top,    window_rect.top);
        intersection_rect.right  = stl::min (output_right,  window_rect.right);
        intersection_rect.bottom = stl::min (output_bottom, window_rect.bottom);

        if (intersection_rect.left >= intersection_rect.right)
          continue;

        if (intersection_rect.top >= intersection_rect.bottom)
          continue;

        size_t intersection_size = (intersection_rect.right - intersection_rect.left) * (intersection_rect.bottom - intersection_rect.top);

        if (intersection_size > max_intersection_size)
        {
          max_intersection_size = intersection_size;
          best_output           = display;
        }
      }

      return (screen_t)best_output;
    }

  private:
    ScreenManagerImpl (const ScreenManagerImpl&);             //no impl
    ScreenManagerImpl& operator = (const ScreenManagerImpl&); //no impl

  private:
    typedef stl::hash_map<CGDirectDisplayID, CarbonScreen> ScreenMap;

  private:
    ScreenMap screens;
};

typedef common::Singleton<ScreenManagerImpl> ScreenManagerSingleton;

void display_reconfiguration_callback (CGDirectDisplayID display, CGDisplayChangeSummaryFlags flags, void*)
{
  if (flags & kCGDisplayBeginConfigurationFlag)
    return;

  switch (flags)
  {
    case kCGDisplayMovedFlag:
    case kCGDisplayDesktopShapeChangedFlag:
      return;
  }

  try
  {
    ScreenManagerSingleton::Instance ()->UpdateScreen (display);
  }
  catch (xtl::exception& e)
  {
    printf ("Exception at processing event in ::display_reconfiguration_callback with flags %08x : '%s'\n", flags, e.what ());
  }
  catch (...)
  {
    printf ("Unknown exception at processing event in ::display_reconfiguration_callback with flags %08x\n", flags);
  }
}

}

/*
    Создание / удаление экрана
*/

screen_t MacOsScreenManager::CreateScreen  (size_t screen_index)
{
  try
  {
    return (screen_t)ScreenManagerSingleton::Instance ()->GetScreen (screen_index).GetDisplayID ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::MacOsScreenManager::CreateScreen");
    throw;
  }
}

void MacOsScreenManager::DestroyScreen (screen_t)
{
  //do nothing
}

/*
    Перечисление экранов
*/

size_t MacOsScreenManager::GetScreensCount ()
{
  try
  {
    return ScreenManagerSingleton::Instance ()->ScreensCount ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::MacOsScreenManager::GetScreensCount");
    throw;
  }
}

/*
    Имя экрана
*/

const char* MacOsScreenManager::GetScreenName (screen_t screen)
{
  try
  {
    return ScreenManagerSingleton::Instance ()->GetScreen (screen).GetName ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::MacOsScreenManager::GetScreenName");
    throw;
  }
}

/*
    Получение списка видео-режимов экрана
*/

size_t MacOsScreenManager::GetScreenModesCount (screen_t screen)
{
  try
  {
    return ScreenManagerSingleton::Instance ()->GetScreen (screen).GetModesCount ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::MacOsScreenManager::GetScreenModesCount");
    throw;
  }
}

void MacOsScreenManager::GetScreenMode (screen_t screen, size_t mode_index, ScreenModeDesc& mode_desc)
{
  try
  {
    ScreenManagerSingleton::Instance ()->GetScreen (screen).GetMode (mode_index, mode_desc);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::MacOsScreenManager::GetScreenMode");
    throw;
  }
}

/*
    Установка текущего видео-режима экрана
*/

void MacOsScreenManager::SetScreenCurrentMode (screen_t screen, const ScreenModeDesc& mode_desc)
{
  try
  {
    ScreenManagerSingleton::Instance ()->GetScreen (screen).SetCurrentMode (mode_desc);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::MacOsScreenManager::SetScreenCurrentMode");
    throw;
  }
}

void MacOsScreenManager::GetScreenCurrentMode (screen_t screen, ScreenModeDesc& mode_desc)
{
  try
  {
    ScreenManagerSingleton::Instance ()->GetScreen (screen).GetCurrentMode (mode_desc);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::MacOsScreenManager::GetScreenCurrentMode");
    throw;
  }
}

void MacOsScreenManager::GetScreenDefaultMode (screen_t screen, ScreenModeDesc& mode_desc)
{
  try
  {
    ScreenManagerSingleton::Instance ()->GetScreen (screen).GetDefaultMode (mode_desc); //Проверить придет ли нотификация при изменении в настройках
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::MacOsScreenManager::GetScreenDefaultMode");
    throw;
  }
}

/*
    Управление гамма-коррекцией экрана
*/

void MacOsScreenManager::SetScreenGammaRamp (screen_t screen, const Color3f table [256])
{
  CGGammaValue red_table[256];
  CGGammaValue green_table[256];
  CGGammaValue blue_table[256];

  for (size_t i = 0; i < 256; i++)
  {
    red_table[i]   = table[i].red;
    green_table[i] = table[i].green;
    blue_table[i]  = table[i].blue;
  }

  check_quartz_error (CGSetDisplayTransferByTable ((CGDirectDisplayID)screen, 256, red_table, green_table, blue_table),
                      "syslib::MacOsScreenManager::SetScreenGammaRamp", "Can't set gamma ramp, ::CGSetDisplayTransferByTable error");
}

void MacOsScreenManager::GetScreenGammaRamp (screen_t screen, Color3f table [256])
{
  static const char* METHOD_NAME = "syslib::MacOsScreenManager::GetScreenGammaRamp";

  CGGammaValue red_table[256];
  CGGammaValue green_table[256];
  CGGammaValue blue_table[256];

  uint32_t copied_count;

  check_quartz_error (CGGetDisplayTransferByTable ((CGDirectDisplayID)screen, 256, red_table, green_table, blue_table, &copied_count),
                      METHOD_NAME, "Can't get gamma ramp, ::CGGetDisplayTransferByTable error");

  if (copied_count != 256)
    throw xtl::format_operation_exception (METHOD_NAME, "Can't get 256 table size gamma ramp");

  for (size_t i = 0; i < 256; i++)
  {
    table[i].red   = red_table[i];
    table[i].green = green_table[i];
    table[i].blue  = blue_table[i];
  }
}

/*
    Поиск экрана вмещающего окно
*/

screen_t MacOsScreenManager::FindContainingScreen (const void* window)
{
  try
  {
    return ScreenManagerSingleton::Instance ()->FindContainingScreen (window);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::MacOsScreenManager::FindContaining");
    throw;
  }
}

/*
    Получение платформо-зависимого дескриптора экрана
*/

const void* MacOsScreenManager::GetNativeScreenHandle (screen_t screen)
{
  try
  {
    return (const void*)ScreenManagerSingleton::Instance ()->GetScreen (screen).GetDisplayID ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::MacOsScreenManager::GetNativeScreenHandle");
    throw;
  }
}

/*
    Получение платформо-зависимых свойств экрана
*/

void MacOsScreenManager::GetScreenProperties (screen_t screen, common::PropertyMap& properties)
{
  try
  {
    ScreenManagerSingleton::Instance ()->GetScreen (screen);  //Проверка что экран существует
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::MacOsScreenManager::GetScreenProperties");
    throw;
  }
}
