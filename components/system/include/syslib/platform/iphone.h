#ifndef SYSLIB_PLATFORM_IPHONE_HEADER
#define SYSLIB_PLATFORM_IPHONE_HEADER

#include <syslib/window.h>

#ifdef __OBJC__
  #import <Foundation/NSDictionary.h>
#endif

namespace syslib
{

namespace iphone
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Ориентации устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
enum InterfaceOrientation
{
  InterfaceOrientation_Portrait           = 1,      //портретная ориентация (кнопка Home снизу)
  InterfaceOrientation_PortraitUpsideDown = 1 << 1, //портретная ориентация (кнопка Home сверху)
  InterfaceOrientation_LandscapeLeft      = 1 << 2, //альбомная ориентация (кнопка Home слева)
  InterfaceOrientation_LandscapeRight     = 1 << 3, //альбомная ориентация (кнопка Home справа)

  InterfaceOrientation_Unknown            = 1 << 4
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Интерфейс слушателя событий окна
///////////////////////////////////////////////////////////////////////////////////////////////////
class IWindowListener
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///События разворота
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void OnInterfaceOrientationWillChange (InterfaceOrientation old_orientation, InterfaceOrientation new_orientation, float duration) {} //ориентация интерфейса окна начала изменяться
    virtual void OnInterfaceOrientationChanged    (InterfaceOrientation old_orientation, InterfaceOrientation new_orientation) {}                 //изменилась ориентация интерфейса окна

///////////////////////////////////////////////////////////////////////////////////////////////////
///События движения
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void OnShakeMotionBegan () {} //пользователь начал жест "тряски" телефона
    virtual void OnShakeMotionEnded () {} //пользователь закончил жест "тряски" телефона

  protected:
    virtual ~IWindowListener () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер окон
///////////////////////////////////////////////////////////////////////////////////////////////////
class WindowManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Добавление/удаление подписчиков на события окна
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void AttachWindowListener (const Window& window, IWindowListener* listener);
    static void DetachWindowListener (const Window& window, IWindowListener* listener);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка/получение разрешенных ориентаций окна
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void SetAllowedOrientations (const Window& window, int orientations);
    static int  GetAllowedOrientations (const Window& window);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Интерфейс слушателя событий приложения
///////////////////////////////////////////////////////////////////////////////////////////////////
class IApplicationListener
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///События
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void OnMemoryWarning () {}   //нехватка памяти

  protected:
    virtual ~IApplicationListener () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер приложения
///////////////////////////////////////////////////////////////////////////////////////////////////
class ApplicationManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Добавление/удаление подписчиков на события приложения
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void AttachApplicationListener (IApplicationListener* listener);
    static void DetachApplicationListener (IApplicationListener* listener);

#ifdef __OBJC__
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение параметров запуска приложения
///////////////////////////////////////////////////////////////////////////////////////////////////
    static NSDictionary* GetLaunchOptions ();
#endif

};

}

}

#endif
