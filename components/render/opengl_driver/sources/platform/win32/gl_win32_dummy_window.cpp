#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl::windows;

namespace
{

/*
    Константы
*/

const char* WINDOW_CLASS_NAME = "OpenGL driver listener window class";

/*
    Обработчик сообщений дочернего следящего окна
*/

LRESULT CALLBACK WindowMessageHandler (HWND wnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    //получение указателя на первичную цепочку обмена

  IWindowListener* listener = reinterpret_cast<IWindowListener*> (GetWindowLongPtr (wnd, GWLP_USERDATA));
  
    //если указатель на пользовательские данные не установлен - делаем попытку его установить
    
  if (!listener)
  {
    if (message == WM_CREATE) 
    {
      CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*> (lparam);
      
      if (!cs->lpCreateParams)
        return 0;

      listener = reinterpret_cast<IWindowListener*> (cs->lpCreateParams);

        //устанавливаем указатель на пользовательские данные

      SetWindowLongPtr (wnd, GWLP_USERDATA, (LONG_PTR)listener);

      return 0;
    }

    return DefWindowProc (wnd, message, wparam, lparam);
  }

    //перехват сообщений  

  try
  {  
    switch (message)
    {
      case WM_DESTROY:
        listener->OnDestroy ();
        return 0;
      case WM_DISPLAYCHANGE:
        listener->OnDisplayModeChange ();
        return 0;
      default:
        break;
    }
    
  }
  catch (...)
  {
    //подавление всех исключений
  }

    //передача управления обработчику "по умолчанию"

  return DefWindowProc (wnd, message, wparam, lparam);    
}

/*
    Регистрация оконного класса
*/

void RegisterWindowClass ()
{
  WNDCLASS wc;

  memset (&wc, 0, sizeof (wc));

  wc.style         = CS_OWNDC;
  wc.lpfnWndProc   = &WindowMessageHandler;
  wc.hInstance     = GetModuleHandle (0);
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wc.lpszClassName = WINDOW_CLASS_NAME;  

  if (!RegisterClass (&wc))
    raise_error ("::RegisterClass");    
}

void UnregisterWindowClass ()
{
  UnregisterClass (WINDOW_CLASS_NAME, GetModuleHandle (0));
}

/*
    Количество экземпляров следящего окна
*/

size_t listener_window_instances = 0;

}

/*
    Конструктор / деструктор
*/

DummyWindow::DummyWindow (HWND parent, IWindowListener* listener)
{
  try
  {
      //регистрация класса окна
    
    if (!listener_window_instances++)
      RegisterWindowClass ();
      
    try
    {
        //создание окна        
      
      window = CreateWindowA (WINDOW_CLASS_NAME, "", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                              parent, 0, GetModuleHandle (0), listener);

      if (!window)
        raise_error ("::CreateWindowA");
    }
    catch (...)
    {
      --listener_window_instances;
      throw;
    }    
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::windows::DummyWindow::DummyWindow");
    throw;
  }
}

DummyWindow::~DummyWindow ()
{
    //уничтожение окна

  DestroyWindow (window);
  
    //отмена регистрации класса окна
  
  if (!--listener_window_instances)
    UnregisterWindowClass ();
}

/*
    Получение дескриптора окна
*/

HWND DummyWindow::GetHandle () const
{
  return window;
}
