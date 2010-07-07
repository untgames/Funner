#include "shared.h"

using namespace engine;
using namespace engine::dot_net;
using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Windows::Forms;
using namespace System::Drawing;
using namespace System::ComponentModel;

namespace Funner
{

/*
    Класс движка
*/

private ref class Engine
{
  public:    
///Получение движка
    static IEngine* GetEngine ()
    {
      if (engine)
        return engine;
        
      HMODULE library = LoadLibrary ("funner.dll");

      if (!library)
        throw gcnew InvalidOperationException ("funner.dll library not found");
      
      FunnerInitProc FunnerInit = (FunnerInitProc)GetProcAddress (library, "FunnerInit");  
      
      try
      {
        engine = FunnerInit ();

        if (!engine)
          throw gcnew InvalidOperationException ("Funner native engine has bad entries");
      }
      finally
      {
//        FreeLibrary (module);
      }

      return engine;
    }

  private:
    static IEngine* engine = 0; //указатель на объект движка
};


/*
    Обёртка над слушателем событий окна движка
*/

private interface class IFunnerViewListener
{
  void OnEnginePaint      ();
  void OnEngineResize     (EventArgs^);
  void OnEngineMouseEnter (EventArgs^);
  void OnEngineMouseLeave (EventArgs^);
  void OnEngineMouseHover (EventArgs^);
  void OnEngineMouseMove  (MouseEventArgs^);  
  void OnEngineMouseDown  (MouseEventArgs^);
  void OnEngineMouseUp    (MouseEventArgs^);
  void OnEngineKeyDown    (KeyEventArgs^);
  void OnEngineKeyUp      (KeyEventArgs^);
};

namespace
{

/*
    Слушатель событий окна движка
*/

class FunnerViewListener: public IWindowListener
{
  public:
    msclr::auto_gcroot<IFunnerViewListener^> Listener;
  
    ///Конструктор
    FunnerViewListener ()
      : keys_converter (gcnew KeysConverter)
    {
    }
    
    ///Обработчик события перерисовки окна
    void OnPaint ()
    {
      printf ("%s\n", __FUNCTION__);      
      if (Listener)
        Listener->OnEnginePaint ();
    }
    
    ///Обработчик изменения размеров окна
    void OnSize (size_t, size_t)
    {
      if (Listener)
        Listener->OnEngineResize (gcnew EventArgs ());      
    }
    
    ///Обработчики событий ввода    
    void OnMouseEnter (int, int)
    {
      if (Listener)
        Listener->OnEngineMouseEnter (gcnew EventArgs ());
    }

    void OnMouseHover (int, int)
    {
      if (Listener)
        Listener->OnEngineMouseHover (gcnew EventArgs ());
    }

    void OnMouseLeave (int, int)
    {
      if (Listener)
        Listener->OnEngineMouseLeave (gcnew EventArgs ());
    }
    
/*    void OnMouseMove (int x, int y)
    {
      if (Listener)
        Listener->OnEngineMouseMove ();
    }
    
    void OnMouseDown (MouseButton button, int x, int y)
    {
      switch (button)
      {
        case MouseButton_Left:
          if (window->EngineLeftButtonDown != nullptr)
            window->EngineLeftButtonDown (x, y);
            
          break;
        case MouseButton_Right:
          if (window->EngineRightButtonDown != nullptr)
            window->EngineRightButtonDown (x, y);

          break;
        case MouseButton_Middle:
          if (window->EngineMiddleButtonDown != nullptr)
            window->EngineMiddleButtonDown (x, y);

          break;
        default:
          break;
      }
    }

    void OnMouseUp (MouseButton button, int x, int y)
    {
      switch (button)
      {
        case MouseButton_Left:
          if (window->EngineLeftButtonUp != nullptr)
            window->EngineLeftButtonUp (x, y);
            
          break;
        case MouseButton_Right:
          if (window->EngineRightButtonUp != nullptr)
            window->EngineRightButtonUp (x, y);

          break;
        case MouseButton_Middle:
          if (window->EngineMiddleButtonUp != nullptr)
            window->EngineMiddleButtonUp (x, y);

          break;
        default:
          break;
      }
    }

    void OnMouseDoubleClick (MouseButton button, int x, int y)
    {
      switch (button)
      {
        case MouseButton_Left:
          if (window->EngineLeftButtonDoubleClick != nullptr)
            window->EngineLeftButtonDoubleClick (x, y);
            
          break;
        case MouseButton_Right:
          if (window->EngineRightButtonDoubleClick != nullptr)
            window->EngineRightButtonDoubleClick (x, y);

          break;
        case MouseButton_Middle:
          if (window->EngineMiddleButtonDoubleClick != nullptr)
            window->EngineMiddleButtonDoubleClick (x, y);

          break;
        default:
          break;
      }
    }
*/
    void OnKeyDown (const char* key)
    {      
      if (Listener)
        Listener->OnEngineKeyDown (gcnew KeyEventArgs ((Keys)keys_converter->ConvertFromString (gcnew String (key))));
    }

    void OnKeyUp (const char* key)
    {
      if (Listener)
        Listener->OnEngineKeyUp (gcnew KeyEventArgs ((Keys)keys_converter->ConvertFromString (gcnew String (key))));
    }

  private:
    msclr::auto_gcroot<KeysConverter^> keys_converter; //конвертер строк в коды клавиш
};

}

///Открытый интерфейс окна движка
public ref class FunnerView: public UserControl, public IFunnerViewListener
{
  public:
    ///Конструктор
    FunnerView ()
      : engine (0)
      , window (0)
      , window_listener (0)
    {
      if (LicenseManager::UsageMode == LicenseUsageMode::Designtime)
        return;
        
      engine = Engine::GetEngine ();

      if (!engine)
        throw gcnew ArgumentException ("Null engine passed to constructor of Funner.FunnerView class");
        
//      if (!name)
//        throw gcnew ArgumentException ("Null name passed to constructor of Funner.FunnerView class");

      graphics = CreateGraphics ();
        
      window_listener = new FunnerViewListener ();

      try
      {
        window = engine->CreateWindow ("Engine window");

        if (!window)
          throw gcnew InvalidOperationException ("Engine::CreateWindow failed");                 

        window->AttachListener (window_listener);
        
        HandleCreated   += gcnew EventHandler (this, &FunnerView::OnCreateHandle);
        HandleDestroyed += gcnew EventHandler (this, &FunnerView::OnDestroyHandle);
        Resize          += gcnew EventHandler (this, &FunnerView::OnResize);
      }
      catch (...)
      {
        delete window;
        delete window_listener;
        throw;
      }
    }

    ///Деструктор
    ~FunnerView ()
    {
      try
      {
        window->DetachListener (window_listener);
        
        delete window;
        delete window_listener;
        
        window = 0;
        window_listener = 0;
      }
      catch (...)
      {
        ///подавление всех исключений
      }
    }
    
///Обработка событий встроенного окна
    virtual void OnEngineKeyDown (KeyEventArgs^ args)
    {
      UserControl::OnKeyDown (args);
    }

    virtual void OnEngineKeyUp (KeyEventArgs^ args)
    {
      UserControl::OnKeyUp (args);
    }
    
    virtual void OnEnginePaint ()
    {
      UserControl::OnPaint (gcnew PaintEventArgs (graphics, ClientRectangle));
    }
    
    virtual void OnEngineResize (EventArgs^ args)
    {
      UserControl::OnResize (args);
    }
    
    virtual void OnEngineMouseEnter (EventArgs^ args)
    {
      UserControl::OnMouseEnter (args);
    }
    
    virtual void OnEngineMouseLeave (EventArgs^ args)
    {
      UserControl::OnMouseLeave (args);
    }
    
    virtual void OnEngineMouseHover (EventArgs^ args)
    {
      UserControl::OnMouseHover (args);
    }
    
    virtual void OnEngineMouseMove (MouseEventArgs^ args)
    {
      UserControl::OnMouseMove (args);
    }
    
    virtual void OnEngineMouseDown (MouseEventArgs^ args)
    {
      UserControl::OnMouseDown (args);
    }
    
    virtual void OnEngineMouseUp (MouseEventArgs^ args)
    {
      UserControl::OnMouseUp (args);
    }    

  private:
///Обновление родительского окна
    void UpdateParent ()
    {
      window->SetParentHandle ((void*)Handle);
    }

///Обновление размеров
    void UpdateRect ()
    {
      System::Drawing::Rectangle^ rect = ClientRectangle;

      window->SetPosition (rect->Left, rect->Top);
      window->SetSize     (rect->Width, rect->Height);
    }

///Обработка события создания дескриптора формы
    void OnCreateHandle (System::Object^, EventArgs^)
    {
      window_listener->Listener = this;      
      
        //обновление параметров

      UpdateParent ();
      UpdateRect ();

        //отображение окна

      window->Show (true);
    }

///Обработка события уничтожения дескриптора формы
    void OnDestroyHandle (System::Object^, EventArgs^)
    {
      window_listener->Listener = nullptr;
      window->Show (false);
      window->SetParentHandle (0);
    }

///Обработка события изменения размеров формы
    void OnResize (Object^, EventArgs^)
    {
      UpdateRect ();
    }            

  private:
    IEngine*            engine;
    IWindow*            window;
    FunnerViewListener* window_listener;
    Graphics^           graphics;
};

}
