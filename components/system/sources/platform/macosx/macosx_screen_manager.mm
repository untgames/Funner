#import <AppKit/NSScreen.h>
#import <AppKit/NSWindow.h>

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
        e.touch ("syslib::MacOsXScreenManager::GetScreensCount");
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
        ScreenMap::iterator iter = screens.find ((CGDirectDisplayID)(size_t)screen);

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
      NSScreen* screen = ((NSWindow*)window).screen;

      if (!screen)
        screen = NSScreen.mainScreen;  //return main screen for minimized windows

      NSNumber* display_id = screen.deviceDescription [@"NSScreenNumber"];

      if ([display_id isKindOfClass:[NSNumber class]])
        return (screen_t)display_id.unsignedLongValue;

      return 0;
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

screen_t MacOsXScreenManager::CreateScreen  (size_t screen_index)
{
  try
  {
    return (screen_t)(size_t)ScreenManagerSingleton::Instance ()->GetScreen (screen_index).GetDisplayID ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::MacOsXScreenManager::CreateScreen");
    throw;
  }
}

void MacOsXScreenManager::DestroyScreen (screen_t)
{
  //do nothing
}

/*
    Перечисление экранов
*/

size_t MacOsXScreenManager::GetScreensCount ()
{
  try
  {
    return ScreenManagerSingleton::Instance ()->ScreensCount ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::MacOsXScreenManager::GetScreensCount");
    throw;
  }
}

/*
    Имя экрана
*/

const char* MacOsXScreenManager::GetScreenName (screen_t screen)
{
  try
  {
    return ScreenManagerSingleton::Instance ()->GetScreen (screen).GetName ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::MacOsXScreenManager::GetScreenName");
    throw;
  }
}

/*
    Получение списка видео-режимов экрана
*/

size_t MacOsXScreenManager::GetScreenModesCount (screen_t screen)
{
  try
  {
    return ScreenManagerSingleton::Instance ()->GetScreen (screen).GetModesCount ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::MacOsXScreenManager::GetScreenModesCount");
    throw;
  }
}

void MacOsXScreenManager::GetScreenMode (screen_t screen, size_t mode_index, ScreenModeDesc& mode_desc)
{
  try
  {
    ScreenManagerSingleton::Instance ()->GetScreen (screen).GetMode (mode_index, mode_desc);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::MacOsXScreenManager::GetScreenMode");
    throw;
  }
}

/*
    Установка текущего видео-режима экрана
*/

void MacOsXScreenManager::SetScreenCurrentMode (screen_t screen, const ScreenModeDesc& mode_desc)
{
  try
  {
    ScreenManagerSingleton::Instance ()->GetScreen (screen).SetCurrentMode (mode_desc);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::MacOsXScreenManager::SetScreenCurrentMode");
    throw;
  }
}

void MacOsXScreenManager::GetScreenCurrentMode (screen_t screen, ScreenModeDesc& mode_desc)
{
  try
  {
    ScreenManagerSingleton::Instance ()->GetScreen (screen).GetCurrentMode (mode_desc);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::MacOsXScreenManager::GetScreenCurrentMode");
    throw;
  }
}

void MacOsXScreenManager::GetScreenDefaultMode (screen_t screen, ScreenModeDesc& mode_desc)
{
  try
  {
    ScreenManagerSingleton::Instance ()->GetScreen (screen).GetDefaultMode (mode_desc); //Проверить придет ли нотификация при изменении в настройках
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::MacOsXScreenManager::GetScreenDefaultMode");
    throw;
  }
}

/*
    Управление гамма-коррекцией экрана
*/

void MacOsXScreenManager::SetScreenGammaRamp (screen_t screen, const Color3f table [256])
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

  check_quartz_error (CGSetDisplayTransferByTable ((CGDirectDisplayID)(size_t)screen, 256, red_table, green_table, blue_table),
                      "syslib::MacOsXScreenManager::SetScreenGammaRamp", "Can't set gamma ramp, ::CGSetDisplayTransferByTable error");
}

void MacOsXScreenManager::GetScreenGammaRamp (screen_t screen, Color3f table [256])
{
  static const char* METHOD_NAME = "syslib::MacOsXScreenManager::GetScreenGammaRamp";

  CGGammaValue red_table[256];
  CGGammaValue green_table[256];
  CGGammaValue blue_table[256];

  uint32_t copied_count;

  check_quartz_error (CGGetDisplayTransferByTable ((CGDirectDisplayID)(size_t)screen, 256, red_table, green_table, blue_table, &copied_count),
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

screen_t MacOsXScreenManager::FindContainingScreen (const void* window)
{
  try
  {
    return ScreenManagerSingleton::Instance ()->FindContainingScreen (window);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::MacOsXScreenManager::FindContaining");
    throw;
  }
}

/*
    Получение платформо-зависимого дескриптора экрана
*/

const void* MacOsXScreenManager::GetNativeScreenHandle (screen_t screen)
{
  try
  {
    return (const void*)(size_t)ScreenManagerSingleton::Instance ()->GetScreen (screen).GetDisplayID ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::MacOsXScreenManager::GetNativeScreenHandle");
    throw;
  }
}

/*
    Получение платформо-зависимых свойств экрана
*/

void MacOsXScreenManager::GetScreenProperties (screen_t screen, common::PropertyMap& properties)
{
  try
  {
    ScreenManagerSingleton::Instance ()->GetScreen (screen);  //Проверка что экран существует
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::MacOsXScreenManager::GetScreenProperties");
    throw;
  }
}
