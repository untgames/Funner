#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;

///Реализация цепочки обмена
struct SwapChain::Impl
{
  xtl::signal<void (unsigned int, unsigned int)> set_size_signal; //сигнал оповещения о изменении размера
};

/*
   Конструктор / деструктор
*/

SwapChain::SwapChain ()
  : impl (new Impl)
  {}

SwapChain::~SwapChain ()
{
  delete impl;
}

/*
   Обновление размера (должно вызываться системой верхнего уровня при изменении размеров окна)
*/

void SwapChain::SetSize (unsigned int width, unsigned int height)
{
  try
  {
    impl->set_size_signal (width, height);
  }
  catch (xtl::exception& e)
  {
    Log ().Printf ("Exception at SwapChain::SetSize notify: %s\n", e.what ());
  }
  catch (...)
  {
    Log ().Printf ("Unknown exception at SwapChain::SetSize notify\n");
  }
}

/*
   Подписка на событие изменения размера
*/

xtl::connection SwapChain::RegisterSizeChangeHandler (const SizeChangeHandler& handler)
{
  return impl->set_size_signal.connect (handler);
}
