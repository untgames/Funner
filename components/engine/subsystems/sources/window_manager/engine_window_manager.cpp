#include <stl/string>
#include <stl/list>
#include <stl/hash_map>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/iterator.h>
#include <xtl/ref.h>
#include <xtl/reference_counter.h>
#include <xtl/trackable.h>

#include <common/component.h>
#include <common/log.h>
#include <common/parser.h>
#include <common/strlib.h>

#include <media/rms/manager.h>
#include <media/rms/server.h>

#include <input/cursor.h>

#include <syslib/application.h>
#include <syslib/screen.h>
#include <syslib/window.h>

#include <engine/attachments.h>
#include <engine/subsystem_manager.h>

using namespace common;
using namespace engine;

namespace components
{

namespace window_manager_subsytem
{

/*
    Константы
*/

const char* SUBSYSTEM_NAME = "WindowManager";                   //имя подсистемы
const char* COMPONENT_NAME = "engine.subsystems.WindowManager"; //имя компонента
const char* LOG_NAME       = COMPONENT_NAME;                    //имя потока протоколирования

//forwards
class Window;
class WindowManagerSubsystem;

Window*     create_window (ParseNode& node, WindowManagerSubsystem& manager);
const char* get_cursor_image_name (Window&);
void        reset_cursor (Window&);

/*
    Реализация подсистемы инициализации окна
*/

class WindowManagerSubsystem: public ISubsystem, private media::rms::ICustomServer, public xtl::reference_counter
{
  public:
///Конструктор
    WindowManagerSubsystem (ParseNode& node)
      : log (LOG_NAME)
      , need_restore_screen_saver (false)
      , screen_saver_saved_state (true)
    {
      try
      {
        resource_server = new media::rms::ServerGroupAttachment (get<const char*> (node, "ResourceServer", SUBSYSTEM_NAME), *this);

        for (Parser::NamesakeIterator iter=node.First ("Window"); iter; ++iter)
        {
          WindowPtr window (create_window (*iter, *this), false);

          windows.push_back (window);
        }
        
        if (ParseNode screen_saver_node = node.First ("ScreenSaver"))
        {
          screen_saver_saved_state  = syslib::Application::ScreenSaverState ();          
          need_restore_screen_saver = true;
          
          bool screen_saver_state = get<int> (screen_saver_node, "", 1) != 0;                    
          
          syslib::Application::SetScreenSaverState (screen_saver_state);
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::subsystems::WindowManagerSubsystem::WindowManagerSubsystem");
        throw;
      }
    }
    
///Деструктор
    ~WindowManagerSubsystem ()
    {
      try
      {
        resource_server.reset ();
        
        if (need_restore_screen_saver)
          syslib::Application::SetScreenSaverState (screen_saver_saved_state);
      }
      catch (...)
      {
      }
    }
    
///Поиск курсора
    syslib::WindowCursor GetCursor (const char* image_name)    
    {
      CursorMap::iterator cursor_iter = cursors.find (image_name);

      if (cursor_iter != cursors.end ())
        return cursor_iter->second;
        
      throw xtl::make_argument_exception ("engine::WindowManagerSubsystem::GetCursor", "image_name", image_name, "Cursor not found");
    }

///Подсчёт ссылок
    void AddRef () { addref (this); }
    void Release () { release (this); }
    
  private:
///Управление ресурсами
    void PrefetchResource (const char* resource_name)
    {
    }

    void LoadResource (const char* resource_name)
    {
      static const char* METHOD_NAME = "engine::subsystems::WindowManagerSubsystem::LoadResource";

      if (!resource_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "resource_name");

      try
      {
        stl::string suffix = common::suffix (resource_name);        

        if (!xtl::xstricmp (suffix.c_str (), ".xhotspot"))
        {
          LoadHotspots (resource_name);          
          
          log.Printf ("Cursors hotspots '%s' loaded", resource_name);          
        }
        else if (wcmatch (resource_name, "cursor(*)"))
        {
          common::StringArray tokens = common::parse (resource_name, "cursor\\( *'(.*)' *, *'(.*)' *\\).*");          

          if (tokens.Size () != 3)
            throw xtl::make_argument_exception ("engine::WindowManagerSubsystem::LoadResource", "resource_name", resource_name, "Bad cursor name");

          LoadCursor (tokens [1], tokens [2]);
          
          log.Printf ("Cursor '%s' loaded (path='%s')", tokens [1], tokens [2]);
        }
      }
      catch (xtl::exception& exception)
      {
        log.Printf ("Can't load resource '%s', exception: %s", resource_name, exception.what ());

        exception.touch (METHOD_NAME);

        throw;
      }
    }

    void UnloadResource (const char* resource_name)
    {
      static const char* METHOD_NAME = "engine::subsystems::WindowManagerSubsystem::UnloadResource";

      if (!resource_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "resource_name");

      if (!xtl::xstricmp (common::suffix (resource_name).c_str (), "xhotspot"))
      {
        hotspots.clear ();
        
        log.Printf ("Cursor hotspots '%s' unloaded", resource_name);
      }
      else if (wcmatch (resource_name, "cursor(*)"))
      {
        common::StringArray tokens = common::parse (resource_name, "cursor\\( *'(.*)' *, *'(.*)' *\\).*");

        if (tokens.Size () != 3)
          return;

        cursors.erase (tokens [1]);
        
        stl::string cursor_name = tokens [1];
        
        for (WindowList::iterator iter=windows.begin (), end=windows.end (); iter!=end; ++iter)
        {
          if (get_cursor_image_name (**iter) == cursor_name)
            reset_cursor (**iter);
        }
        
        log.Printf ("Cursor '%s' unloaded (path='%s')", tokens [1], tokens [2]);
      }
    }  
  
///Загрузка точек курсора
    void LoadHotspots (const char* file_name)
    {
      try
      {
        common::Parser parser (file_name);

        for (size_t i=0; i<parser.Log ().MessagesCount (); ++i)
           log.Printf ("%s", parser.Log ().Message (i));

        if (common::ParseNode hotspots_node = parser.Root ().First ("hotspots"))
        {
          for (common::Parser::NamesakeIterator iter=hotspots_node.First ("cursor"); iter; ++iter)
          {
            const char* name = get<const char*> (*iter, "name");
            int         x    = get<int> (*iter, "x"),
                        y    = get<int> (*iter, "y");

            hotspots [name] = math::vec2i (x, y);
          }
        }
        else
        {
          throw xtl::format_operation_exception ("", "<hotspots> tag not found in resource '%s'", file_name);
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::subsystems::WindowManagerSubsystem::LoadHotspots");
        throw;
      }      
    }
    
///Загрузка курсора
    syslib::WindowCursor LoadCursor (const char* name, const char* file_name)
    {
      try
      {
        CursorMap::iterator cursor_iter = cursors.find (name);
        
        if (cursor_iter != cursors.end ())
          return cursor_iter->second;        

        math::vec2i hotspot (-1, -1);
        
        HotspotMap::iterator iter = hotspots.find (file_name);
        
        if (iter != hotspots.end ())
          hotspot = iter->second;
        
        syslib::WindowCursor cursor (file_name, hotspot.x, hotspot.y);
        
        cursors [name] = cursor;
        
        return cursor;
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::subsystems::WindowManagerSubsystem::LoadCursor");
        throw;
      }
    }

  private:
    typedef xtl::intrusive_ptr<Window>                                      WindowPtr;
    typedef stl::list<WindowPtr>                                            WindowList;
    typedef stl::hash_map<stl::hash_key<const char*>, math::vec2i>          HotspotMap;
    typedef stl::hash_map<stl::hash_key<const char*>, syslib::WindowCursor> CursorMap;

  private:
    Log                                              log;  
    WindowList                                       windows;    
    stl::auto_ptr<media::rms::ServerGroupAttachment> resource_server;
    HotspotMap                                       hotspots;
    CursorMap                                        cursors;
    bool                                             need_restore_screen_saver;
    bool                                             screen_saver_saved_state;
};

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
              private input::ICursorListener, public xtl::reference_counter, private xtl::trackable
{
  public:
///Конструктор
    Window (ParseNode& node, WindowManagerSubsystem& in_manager)
      : log (LOG_NAME),
        manager (in_manager),
        window (get_window_style (node)),
        cursor (0),
        aspect_ratio (0)
    {
      static const char* METHOD_NAME = "engine::Window::Window";

        //создание окна

      const char* title = get<const char*> (node, "Title", (const char*)0);

      if (title)
        window.SetTitle (title);

      window.SetMultitouchEnabled (get<bool> (node, "MultitouchEnabled", window.IsMultitouchEnabled ()));

        //сохранение точки привязки родительского окна

      parent_window_name = get<const char*> (node, "Parent", "");
      
      syslib::Window* parent_window = 0;
      
      if (!parent_window_name.empty ())
      {
        parent_window = &AttachmentRegistry::Get<syslib::Window> (parent_window_name.c_str ());
        
        window.SetParentHandle (parent_window->Handle ());
      }
        
        //цвет заднего фона
        
      const char* background_color_string = get<const char*> (node, "BackgroundColor", "");
      
      common::StringArray background_color_tokens = common::split (background_color_string, " ");
      
      if (background_color_tokens.Size () >= 3)
      {
        syslib::Color color;
        
        color.red   = static_cast<unsigned char> (xtl::io::get<unsigned int> (background_color_tokens [0]));
        color.green = static_cast<unsigned char> (xtl::io::get<unsigned int> (background_color_tokens [1]));
        color.blue  = static_cast<unsigned char> (xtl::io::get<unsigned int> (background_color_tokens [2]));
        
        window.EnableBackground ();
        window.SetBackgroundColor (color);
      }
      
        //соотношение сторон
        
      const char* aspect_ratio_string = get<const char*> (node, "ViewportAspectRatio", "");
      
      if (*aspect_ratio_string)
      {
        common::StringArray aspect_ratio_tokens = common::split (aspect_ratio_string, ":");
        
        if (aspect_ratio_tokens.Size () == 2)
        {
          double kx = xtl::io::get<double> (aspect_ratio_tokens [0]),
                 ky = xtl::io::get<double> (aspect_ratio_tokens [1]);
                 
          aspect_ratio = kx / ky;
          
          window.SetViewportHandler (xtl::bind (&Window::UpdateViewport, this, _2));
        }
      }      

        //размеры и положение окна
        
      bool maximized = get<bool> (node, "Maximize", false),
           minimized = get<bool> (node, "Minimize", false);

      if (!maximized && !minimized)
      {
        size_t width       = get<size_t> (node, "Width"),
               height      = get<size_t> (node, "Height");
        bool   client_size = get<bool> (node, "ClientSize", true);

        if (client_size)
          window.SetClientSize (width, height);
        else
          window.SetSize (width, height);        
        
        syslib::Point position = window.Position ();

        position.x = get<size_t> (node, "XPosition", position.x);
        position.y = get<size_t> (node, "YPosition", position.y);

        window.SetPosition (position);
        
        bool is_window_visible = get<bool> (node, "Visible", window.IsVisible ());

        window.SetVisible (is_window_visible);
        window.SetActive  (is_window_visible);
      }
      else if (maximized && minimized)
      {
        throw xtl::format_operation_exception (METHOD_NAME, "Window can't be maximized and minimized at one time");
      }
      else if (maximized)
      {
        window.Maximize ();          
        window.SetActive (true);        
      }
      else if (minimized)
      {
        window.Minimize ();
        window.SetActive (false);
      }

      if (node.First ("MinDesktopWidth") || node.First ("MinDesktopHeight"))
      {
        size_t min_desktop_width  = get<size_t> (node, "MinDesktopWidth", 0),
               min_desktop_height = get<size_t> (node, "MinDesktopHeight", 0);

        stl::auto_ptr<syslib::Screen> window_screen;        

        try
        {
          window_screen.reset (new syslib::Screen (syslib::Screen::ContainingScreen (window.Handle ())));
        }
        catch (std::exception& e)
        {
          log.Printf ("Can't find containing screen for window, using screen 0, exception: '%s'", e.what ());
          
          try
          {
            window_screen.reset (new syslib::Screen ((size_t)0));
          }
          catch (std::exception& e)
          {
            log.Printf ("Can't find screen 0, exception: '%s'", e.what ());
          }
          catch (...)
          {
            log.Printf ("Can't find screen 0");
          }
        }

        if (window_screen && (window_screen->Width () < min_desktop_width || window_screen->Height () < min_desktop_height))
        {
          syslib::ScreenModeDesc best_mode;
          size_t                 best_mode_resolution = 0;

          memset (&best_mode, 0, sizeof (best_mode));

          for (size_t i = 0, count = window_screen->ModesCount (); i < count; i++)
          {
            syslib::ScreenModeDesc current_mode;

            window_screen->GetMode (i, current_mode);

            size_t current_mode_resolution = current_mode.width * current_mode.height;

              //поиск режима с максимальным разрешением, частотой развертки и цветностью
            if (current_mode_resolution < best_mode_resolution)
              continue;

            if (current_mode.width >= min_desktop_width && current_mode.height >= min_desktop_height)
            {
              if (current_mode_resolution > best_mode_resolution)
              {
                best_mode            = current_mode;
                best_mode_resolution = current_mode_resolution;
              }
              else
              {
                if (current_mode.refresh_rate > best_mode.refresh_rate)
                  best_mode = current_mode;
                else if (current_mode.refresh_rate == best_mode.refresh_rate && current_mode.color_bits > best_mode.color_bits)
                  best_mode = current_mode;
              }
            }
          }

          if (!best_mode.width)
            log.Printf ("Can't find screen mode with minimum size %ux%u", min_desktop_width, min_desktop_height);
          else
          {
            window_screen->SetCurrentMode (best_mode);
            
            saved_window_screen = window_screen;

            if (maximized)
              window.Maximize ();
          }
        }
      }
      
        //видимость курсора
      
      bool is_cursor_visible = get<bool> (node, "CursorVisible", window.IsCursorVisible ());

      window.SetCursorVisible (is_cursor_visible);
      
        //установка протокола

      const char* log_name = get<const char*> (node, "Log", "");

      if (*log_name)
        Log (log_name).Swap (log);

      window.SetDebugLog (xtl::bind (&Log::Print, &log, _1));

        //подписка на события окна

      connect_tracker (window.RegisterEventHandler (syslib::WindowEvent_OnMouseMove, xtl::bind (&Window::OnMouseMove, this, _1, _3)));
      
        //обновление области вывода
        
      window.InvalidateViewport ();

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
        
        if (!attachment_name.empty ())
          AttachmentRegistry::Unregister (attachment_name.c_str (), window);

        throw;
      }
    }

///Деструктор
    ~Window ()
    {
      try
      {
        if (saved_window_screen)
          saved_window_screen->RestoreDefaultMode ();
        
        BindCursor (0);
        
        AttachmentRegistry::Unregister (attachment_name.c_str (), window);
        AttachmentRegistry::Detach (static_cast<IAttachmentRegistryListener<syslib::Window>*> (this));
        AttachmentRegistry::Detach (static_cast<IAttachmentRegistryListener<input::Cursor>*> (this));
      }
      catch (...)
      {
      }
    }
    
///Имя курсора
    const char* CursorImageName ()
    {
      return cursor ? cursor->Image () : "";
    }
    
///Сброс курсора
    void ResetCursor ()
    {
      window.SetCursor (syslib::WindowCursor ());
    }

///Обработчик события регистрации окна
    void OnRegisterAttachment (const char* attachment_name, syslib::Window& in_window)
    {
      if (parent_window_name.empty () || attachment_name != parent_window_name || in_window.Handle () == window.ParentHandle ())
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
    
    void OnChangeImage (const char* image_name)
    {            
      window.SetCursor (manager.GetCursor (image_name));
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
        OnChangeImage (cursor->Image ());
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
    
///Обновление области вывода окна
    void UpdateViewport (syslib::Rect& viewport)
    {
      size_t window_width  = window.ClientWidth (),
             window_height = window.ClientHeight ();             
                   
      double window_aspect_ratio = double (window_width) / double (window_height);

      size_t viewport_width = 0, viewport_height = 0;

      if (window_aspect_ratio / aspect_ratio < 1.0)
      {
          //полосы по вертикали

        viewport_width  = window_width;
        viewport_height = size_t (viewport_width / aspect_ratio);
      }
      else
      {
          //полосы по горизонтали
          
        viewport_height = window_height;
        viewport_width  = size_t (viewport_height * aspect_ratio);
      }

      viewport.left   = (window_width - viewport_width) / 2;
      viewport.top    = (window_height - viewport_height) / 2;      
      viewport.right  = viewport.left + viewport_width;
      viewport.bottom = viewport.top + viewport_height;      
    }

  private:
    Log                           log;
    WindowManagerSubsystem&       manager;
    syslib::Window                window;
    stl::auto_ptr<syslib::Screen> saved_window_screen;
    input::Cursor*                cursor;
    stl::string                   attachment_name;
    stl::string                   parent_window_name;
    stl::string                   cursor_attachment_name;
    math::vec2f                   cached_cursor_position;
    double                        aspect_ratio;
};

Window* create_window (ParseNode& node, WindowManagerSubsystem& manager)
{
  return new Window (node, manager);
}

const char* get_cursor_image_name (Window& window)
{
  return window.CursorImageName ();
}

void reset_cursor (Window& window)
{
  window.ResetCursor ();
}

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

}
