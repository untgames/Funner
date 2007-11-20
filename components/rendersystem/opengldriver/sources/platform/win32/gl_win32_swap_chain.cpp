#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    Конструктор / деструктор
*/

SwapChain::SwapChain ()
{
}

SwapChain::~SwapChain ()
{
}

/*
    Получение дескриптора
*/

void SwapChain::GetDesc (SwapChainDesc& out_desc)
{
  out_desc = desc;
}

/*
    Получение устройства вывода
*/

IOutput* SwapChain::GetOutput ()
{
  return output;
}

/*
    Обмен текущего заднего буфера и переднего буфера
*/

void SwapChain::Present ()
{
  if (!SwapBuffers (hDC))
    raise_error (format ("render::low_level::opengl::SwapChain::Present(device-name='%s')", output->GetName ()).c_str ());
}

/*
    Установка / взятие состояния full-screen mode
*/

void SwapChain::SetFullscreenState (bool state)
{
  RaiseNotImplemented ("render::low_level::opengl::SwapChain::SetFullscreenState");
}

bool SwapChain::GetFullscreenState ()
{
  RaiseNotImplemented ("render::low_level::opengl::SwapChain::SetFullscreenState");
  return false;
}
