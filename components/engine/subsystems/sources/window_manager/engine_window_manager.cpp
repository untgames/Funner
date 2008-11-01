#include <stl/string>
#include <stl/list>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/iterator.h>
#include <xtl/ref.h>
#include <xtl/reference_counter.h>

#include <common/component.h>
#include <common/log.h>
#include <common/parser.h>

#include <syslib/window.h>

#include <engine/attachments.h>
#include <engine/subsystem_manager.h>

using namespace common;
using namespace engine;

namespace
{

/*
    Константы
*/

const char* SUBSYSTEM_NAME = "WindowManager";                   //имя подсистемы
const char* COMPONENT_NAME = "engine.subsystems.WindowManager"; //имя компонента
const char* LOG_NAME       = COMPONENT_NAME;                    //имя потока протоколирования

/*
    Утилиты
*/

syslib::WindowStyle get_window_style (ParseNode& node)
{
  const char* style = get<const char*> (node, "Style");

  if (!xtl::xstricmp ("PopUp", style))      return syslib::WindowStyle_PopUp;
  if (!xtl::xstricmp ("Overlapped", style)) return syslib::WindowStyle_Overlapped;

  throw xtl::make_argument_exception ("", "Style", style);
}

/*
    Реализация окна
*/

class Window: public IAttachmentRegistryListener<syslib::Window>, public xtl::reference_counter
{
  public:
///Конструктор
    Window (ParseNode& node)
      : log (LOG_NAME),
        window (get_window_style (node), get<size_t> (node, "Width"), get<size_t> (node, "Height"))    
    {
        //создание окна
      
      const char* title = get<const char*> (node, "Title", (const char*)0);

      if (title)
        window.SetTitle (title);

      syslib::Point position = window.Position ();

      position.x = get<size_t> (node, "XPosition", position.x);
      position.y = get<size_t> (node, "YPosition", position.y);

      window.SetPosition (position);

      bool is_window_visible = get<bool> (node, "Visible", window.IsVisible ()),
           is_cursor_visible = get<bool> (node, "Cursor", window.IsCursorVisible ());

      window.SetVisible (is_window_visible);
      window.SetCursorVisible (is_cursor_visible);
      
        //сохранение точки привязки родительского окна

      parent_window_name = get<const char*> (node, "Parent", "");

        //установка протокола

      const char* log_name = get<const char*> (node, "Log", "");
      
      if (*log_name)
        Log (log_name).Swap (log);

      window.SetDebugLog (xtl::bind (&Log::Print, &log, _1));

        //регистрация слушателя событий появления новых окон

      AttachmentRegistry::Attach (this, AttachmentRegistryAttachMode_ForceNotify);      
      
        //регистрация окна
        
      try
      {
        attachment_name = get<const char*> (node, "Attachment", "");        

        if (!attachment_name.empty ())
          AttachmentRegistry::Register (attachment_name.c_str (), xtl::ref (window));
      }
      catch (...)
      {
        AttachmentRegistry::Detach (this);
        throw;
      }
    }
    
///Деструктор
    ~Window ()
    {
      AttachmentRegistry::Unregister (attachment_name.c_str (), window);
      AttachmentRegistry::Detach (this);
    }    

///Обработчик события регистрации окна
    void OnRegisterAttachment (const char* attachment_name, syslib::Window& in_window)
    {
      if (!parent_window_name.empty () && attachment_name != parent_window_name)
        return;

      window.SetParentHandle (in_window.Handle ());
    }

    void OnUnregisterAttachment (const char* attachment_name, syslib::Window&)
    {
      if (!parent_window_name.empty () && attachment_name != parent_window_name)
        return;

      window.SetParentHandle (0);
    }

  private:
    Log            log;
    syslib::Window window;
    stl::string    attachment_name;
    stl::string    parent_window_name;    
};

/*
    Реализация подсистемы инициализации окна
*/

class WindowManagerSubsystem: public ISubsystem, public xtl::reference_counter
{
  public:
///Конструктор
    WindowManagerSubsystem (ParseNode& node)
    {
      for (Parser::NamesakeIterator iter=node.First ("Window"); iter; ++iter)
      {
        WindowPtr window (new Window (*iter), false);
        
        windows.push_back (window);
      }
    }

///Подсчёт ссылок
    void AddRef () { addref (this); }
    void Release () { release (this); }
    
  private:
    typedef xtl::intrusive_ptr<Window> WindowPtr;
    typedef stl::list<WindowPtr>       WindowList;

  private:
    WindowList windows;
};

/*
    Компонент подсистемы инциализации окон
*/

class WindowManagerComponent
{
  public:
    WindowManagerComponent () 
    {
      StartupManager::RegisterStartupHandler (SUBSYSTEM_NAME, &StartupHandler);
    }
    
  private:
    static void StartupHandler (ParseNode& node, SubsystemManager& manager)
    {
      try
      {        
        xtl::com_ptr<ISubsystem> subsystem (new WindowManagerSubsystem (node), false);

        manager.AddSubsystem (subsystem.get ());
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::WindowManagerComponent::StartupHandler");
        throw;
      }
    }
};

extern "C"
{

ComponentRegistrator<WindowManagerComponent> WindowManager (COMPONENT_NAME);

}

}
