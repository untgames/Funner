#include <stl/string>
#include <stl/list>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/iterator.h>
#include <xtl/ref.h>
#include <xtl/reference_counter.h>
#include <xtl/connection.h>

#include <common/component.h>
#include <common/log.h>
#include <common/parser.h>

#include <input/cursor.h>

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

class Window: public IAttachmentRegistryListener<syslib::Window>, public IAttachmentRegistryListener<input::Cursor>,
              private input::ICursorListener, public xtl::reference_counter
{
  public:
///Конструктор
    Window (ParseNode& node)
      : log (LOG_NAME),
        window (get_window_style (node)),
        cursor (0)
    {
        //создание окна

      size_t width       = get<size_t> (node, "Width"),
             height      = get<size_t> (node, "Height");
      bool   client_size = get<bool> (node, "ClientSize", true);

      if (client_size)
        window.SetClientSize (width, height);
      else
        window.SetSize (width, height);

      const char* title = get<const char*> (node, "Title", (const char*)0);

      if (title)
        window.SetTitle (title);

      syslib::Point position = window.Position ();

      position.x = get<size_t> (node, "XPosition", position.x);
      position.y = get<size_t> (node, "YPosition", position.y);

      window.SetPosition (position);

      bool is_window_visible = get<bool> (node, "Visible", window.IsVisible ()),
           is_cursor_visible = get<bool> (node, "CursorVisible", window.IsCursorVisible ());

      window.SetVisible (is_window_visible);
      window.SetCursorVisible (is_cursor_visible);

        //сохранение точки привязки родительского окна

      parent_window_name = get<const char*> (node, "Parent", "");

        //установка протокола

      const char* log_name = get<const char*> (node, "Log", "");

      if (*log_name)
        Log (log_name).Swap (log);

      window.SetDebugLog (xtl::bind (&Log::Print, &log, _1));

        //подписка на события окна

      on_mouse_move_connection = window.RegisterEventHandler (syslib::WindowEvent_OnMouseMove, xtl::bind (&Window::OnMouseMove, this, _1, _3));

        //регистрация окна

      try
      {
        attachment_name        = get<const char*> (node, "Id", "");
        cursor_attachment_name = get<const char*> (node, "Cursor", "");

        if (!attachment_name.empty ())
          AttachmentRegistry::Register (attachment_name.c_str (), xtl::make_const_ref (xtl::ref (window)));

          //регистрация слушателя событий появления новых окон

        AttachmentRegistry::Attach (static_cast<IAttachmentRegistryListener<syslib::Window>*> (this), AttachmentRegistryAttachMode_ForceNotify);
        AttachmentRegistry::Attach (static_cast<IAttachmentRegistryListener<input::Cursor>*> (this), AttachmentRegistryAttachMode_ForceNotify);
      }
      catch (...)
      {
        AttachmentRegistry::Detach (static_cast<IAttachmentRegistryListener<syslib::Window>*> (this));
        AttachmentRegistry::Detach (static_cast<IAttachmentRegistryListener<input::Cursor>*> (this));
        throw;
      }
    }

///Деструктор
    ~Window ()
    {
      BindCursor (0);
      AttachmentRegistry::Unregister (attachment_name.c_str (), window);
      AttachmentRegistry::Detach (static_cast<IAttachmentRegistryListener<syslib::Window>*> (this));
      AttachmentRegistry::Detach (static_cast<IAttachmentRegistryListener<input::Cursor>*> (this));
    }

///Обработчик события регистрации окна
    void OnRegisterAttachment (const char* attachment_name, syslib::Window& in_window)
    {
      if (parent_window_name.empty () || attachment_name != parent_window_name)
        return;

      window.SetParentHandle (in_window.Handle ());
    }

    void OnUnregisterAttachment (const char* attachment_name, syslib::Window&)
    {
      if (parent_window_name.empty () || attachment_name != parent_window_name)
        return;

      window.SetParentHandle (0);
    }

///Обработчик события регистрации курсора
    void OnRegisterAttachment (const char* attachment_name, input::Cursor& in_cursor)
    {
      if (cursor_attachment_name.empty () || cursor_attachment_name != attachment_name)
        return;

      BindCursor (&in_cursor);
    }

    void OnUnregisterAttachment (const char* attachment_name, input::Cursor&)
    {
      if (cursor_attachment_name.empty () || cursor_attachment_name != attachment_name)
        return;

      BindCursor (0);
    }

///Обработчики событий курсора
    void OnChangePosition (const math::vec2f& position)
    {
      if (position == cached_cursor_position)
        return;

      syslib::Rect client_rect = window.ClientRect ();

      window.SetCursorPosition (size_t (client_rect.left + position.x * (client_rect.right - client_rect.left)),
        size_t (client_rect.top + position.y * (client_rect.bottom - client_rect.top)));

      cached_cursor_position = position;
    }

    void OnChangeVisible (bool state)
    {
      window.SetCursorVisible (state);
    }

    void OnDestroy ()
    {
      cursor = 0;
    }

  private:
    void BindCursor (input::Cursor* new_cursor)
    {
      if (cursor)
      {
        cursor->Detach (this);
        cursor = 0;
      }

      cursor = new_cursor;

      if (cursor)
      {
        cursor->Attach (this);

        SetCursorPosition (window.CursorPosition (), window.ClientRect ());
        OnChangeVisible (cursor->IsVisible ());
      }
    }

    void SetCursorPosition (const syslib::Point& position, const syslib::Rect& client_rect)
    {
      if (!cursor)
        return;

      cached_cursor_position = math::vec2f (position.x / float (client_rect.right - client_rect.left),
        position.y / float (client_rect.bottom - client_rect.top));

      cursor->SetPosition (cached_cursor_position);
    }

    void OnMouseMove (syslib::Window& window, const syslib::WindowEventContext& context)
    {
      SetCursorPosition (context.cursor_position, window.ClientRect ());
    }

  private:
    Log                  log;
    syslib::Window       window;
    input::Cursor*       cursor;
    stl::string          attachment_name;
    stl::string          parent_window_name;
    stl::string          cursor_attachment_name;
    math::vec2f          cached_cursor_position;
    xtl::auto_connection on_mouse_move_connection;
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

        manager.Add (SUBSYSTEM_NAME, subsystem.get ());
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

ComponentRegistrator<WindowManagerComponent> WindowManagerSubsystem (COMPONENT_NAME);

}

}
