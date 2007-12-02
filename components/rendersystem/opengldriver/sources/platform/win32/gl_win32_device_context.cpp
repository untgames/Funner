#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
===================================================================================================
    DeviceContext
===================================================================================================
*/

/*
    Конструктор / деструктор
*/

DeviceContext::DeviceContext ()
  : device_context (0)
{
}

DeviceContext::DeviceContext (HDC dc)
  : device_context (dc)
{
}

/*
    Установка контекста
*/

void DeviceContext::SetDC (HDC dc)
{
  device_context = dc;
}

/*
===================================================================================================
    WindowDeviceContext
===================================================================================================
*/

/*
    Конструктор / деструктор
*/

WindowDeviceContext::WindowDeviceContext (HWND in_window)
  : window (in_window)
{
  if (!window)
    RaiseNullArgument ("render::low_level::opengl::WindowDeviceContext::WindowDeviceContext", "window");

  HDC dc = ::GetDC (window);
  
  if (!dc)
    raise_error ("GetDC");
    
  SetDC (dc);
}

WindowDeviceContext::~WindowDeviceContext ()
{
  ReleaseDC (window, GetDC ());
}

/*
===================================================================================================
    OutputDeviceContext
===================================================================================================
*/

/*
    Конструктор / деструктор
*/

OutputDeviceContext::OutputDeviceContext (IOutput* in_output)
{
  if (!in_output)
    RaiseNullArgument ("render::low_level::opengl::OutputDeviceContext", "output");

  Output* output = cast_object<Output> (in_output, "render::low_level::opengl::OutputDeviceContext::OutputDeviceContext", "output");  

  HDC dc = CreateDC (output->GetWinName (), 0, 0, 0);
  
  if (!dc)
    raise_error ("CreateDC");

  SetDC (dc);
}

OutputDeviceContext::~OutputDeviceContext ()
{
  DeleteDC (GetDC ());
}
