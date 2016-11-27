#ifndef SYSLIB_MACOSX_PLATFORM_HEADER
#define SYSLIB_MACOSX_PLATFORM_HEADER

#include <shared/cocoa.h>
#include <shared/unistd.h>

namespace syslib
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Оконный менеджер Carbon
///////////////////////////////////////////////////////////////////////////////////////////////////
class CarbonWindowManager: public DefaultWindowManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание/закрытие/уничтожение окна
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef void (*WindowMessageHandler)(window_t, WindowEvent, const WindowEventContext&, void* user_data);

    static window_t CreateWindow  (WindowStyle, WindowMessageHandler, const void* parent_handle, const char* init_string, void* user_data);
    static void     CloseWindow   (window_t);
    static void     DestroyWindow (window_t);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Попытка изменения стиля окна (может быть проигнорирована)
///////////////////////////////////////////////////////////////////////////////////////////////////
    static bool ChangeWindowStyle (window_t, WindowStyle);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение платформо-зависимого дескриптора окна и дисплея
///////////////////////////////////////////////////////////////////////////////////////////////////
    static const void* GetNativeWindowHandle  (window_t);
    static const void* GetNativeDisplayHandle (window_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Заголовок окна
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void SetWindowTitle (window_t, const wchar_t* title);
    static void GetWindowTitle (window_t, size_t buffer_size_in_chars, wchar_t* buffer);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Область окна / клиентская область
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void SetWindowRect (window_t, const Rect&);
    static void SetClientRect (window_t, const Rect&);
    static void GetWindowRect (window_t, Rect&);
    static void GetClientRect (window_t, Rect&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка флагов окна
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void SetWindowFlag (window_t, WindowFlag flag, bool state);
    static bool GetWindowFlag (window_t, WindowFlag flag);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка родительского окна
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void        SetParentWindowHandle (window_t child, const void* parent_handle);
    static const void* GetParentWindowHandle (window_t child);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обновление окна
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void InvalidateWindow (window_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Положение курсора
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void  SetCursorPosition (const Point& position);
    static Point GetCursorPosition ();
    static void  SetCursorPosition (window_t, const Point& client_position); //в клиентских координатах окна
    static Point GetCursorPosition (window_t);                               //в клиентских координатах окна

///////////////////////////////////////////////////////////////////////////////////////////////////
///Видимость курсора
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void SetCursorVisible (window_t, bool state);
    static bool GetCursorVisible (window_t);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Изображение курсора
///////////////////////////////////////////////////////////////////////////////////////////////////
    static cursor_t CreateCursor  (const char* name, int hotspot_x, int hotspot_y); //hotspot_x/hotspot_y = -1 - default value
    static void     DestroyCursor (cursor_t);
    static void     SetCursor     (window_t window, cursor_t cursor);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка/получение multitouch режима
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void SetMultitouchEnabled (window_t window, bool state);
    static bool IsMultitouchEnabled  (window_t window);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Цвет фона
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void  SetBackgroundColor (window_t window, const Color& color);
    static void  SetBackgroundState (window_t window, bool state);
    static Color GetBackgroundColor (window_t window);
    static bool  GetBackgroundState (window_t window);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание/уничтожение web-view
///////////////////////////////////////////////////////////////////////////////////////////////////
    static web_view_t CreateWebView  (IWebViewListener*);
    static void       DestroyWebView (web_view_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Присоединенное окно
///////////////////////////////////////////////////////////////////////////////////////////////////
    static window_t GetWindow (web_view_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Загрузка данных
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void LoadRequest (web_view_t, const char* uri);
    static void LoadData    (web_view_t, const char* data, size_t data_size, const char* mime_type, const char* encoding, const char* base_url);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Перезагрузка страницы / остановка загрузки / проверка наличия загрузки
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void Reload      (web_view_t);
    static void StopLoading (web_view_t);
    static bool IsLoading   (web_view_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Средства навигации
///////////////////////////////////////////////////////////////////////////////////////////////////
    static bool CanGoBack    (web_view_t);
    static bool CanGoForward (web_view_t);
    static void GoBack       (web_view_t);
    static void GoForward    (web_view_t);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер таймеров Carbon
///////////////////////////////////////////////////////////////////////////////////////////////////
class CarbonTimerManager: public DefaultTimerManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание / уничтожение таймера
///////////////////////////////////////////////////////////////////////////////////////////////////
    static timer_t CreateTimer (size_t period_in_milliseconds, TimerHandler, void* user_data);
    static void    KillTimer   (timer_t handle);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер нитей MacOSX
///////////////////////////////////////////////////////////////////////////////////////////////////
class MacOsThreadManager: public PThreadManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с семафорами
///////////////////////////////////////////////////////////////////////////////////////////////////
    static semaphore_t CreateSemaphore   (size_t initial_count);
    static void        DestroySemaphore  (semaphore_t);
    static void        WaitSemaphore     (semaphore_t);
    static bool        WaitSemaphore     (semaphore_t, size_t wait_in_milliseconds);
    static bool        TryWaitSemaphore  (semaphore_t);
    static void        PostSemaphore     (semaphore_t);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер приложения IPhone
///////////////////////////////////////////////////////////////////////////////////////////////////
class CarbonApplicationManager: public UnistdApplicationManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Открытие URL во внешнем браузере
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void OpenUrl (const char* url);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с очередью сообщений
///////////////////////////////////////////////////////////////////////////////////////////////////
    static IApplicationDelegate* CreateDefaultApplicationDelegate ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление энергосбережением
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void SetScreenSaverState (bool state);
    static bool GetScreenSaverState ();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Экранный менеджер по умолчанию
///////////////////////////////////////////////////////////////////////////////////////////////////
class MacOsScreenManager: public DefaultScreenManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Перечисление экранов
///////////////////////////////////////////////////////////////////////////////////////////////////
    static size_t GetScreensCount ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание / удаление экрана
///////////////////////////////////////////////////////////////////////////////////////////////////
    static screen_t CreateScreen  (size_t screen_index);
    static void     DestroyScreen (screen_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Имя экрана
///////////////////////////////////////////////////////////////////////////////////////////////////
    static const char* GetScreenName (screen_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение списка видео-режимов экрана
///////////////////////////////////////////////////////////////////////////////////////////////////
    static size_t GetScreenModesCount (screen_t);
    static void   GetScreenMode       (screen_t, size_t mode_index, ScreenModeDesc& mode_desc);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка текущего видео-режима экрана
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void SetScreenCurrentMode (screen_t, const ScreenModeDesc&);
    static void GetScreenCurrentMode (screen_t, ScreenModeDesc&);
    static void GetScreenDefaultMode (screen_t, ScreenModeDesc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление гамма-коррекцией экрана
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void SetScreenGammaRamp (screen_t, const Color3f table [256]);
    static void GetScreenGammaRamp (screen_t, Color3f table [256]);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск экрана вмещающего окно
///////////////////////////////////////////////////////////////////////////////////////////////////
    static screen_t FindContainingScreen (const void* native_window_handle);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение платформо-зависимого дескриптора экрана
///////////////////////////////////////////////////////////////////////////////////////////////////
    static const void* GetNativeScreenHandle (screen_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение платформо-зависимых свойств экрана
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void GetScreenProperties (screen_t, common::PropertyMap& properties);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Платформа
///////////////////////////////////////////////////////////////////////////////////////////////////
class MacOsPlatform
 : public CarbonWindowManager
 , public CarbonTimerManager 
 , public CarbonApplicationManager 
 , public MacOsThreadManager 
 , public UnistdLibraryManager
 , public MacOsScreenManager
 , public DefaultSensorManager 
 , public CocoaCookieManager
 , public DefaultScreenKeyboardManager
{
};

}

#endif
