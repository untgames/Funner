#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    Конструктор / деструктор
*/

SwapChain::SwapChain (const SwapChainDesc& desc)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

SwapChain::~SwapChain ()
{
}

/*
    Получение адаптера
*/

IAdapter* SwapChain::GetAdapter ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Получение дескриптора
*/

void SwapChain::GetDesc (SwapChainDesc&)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Получение устройства вывода с максимальным размером области перекрытия
*/

IOutput* SwapChain::GetContainingOutput ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Установка / взятие состояния full-screen mode
*/

void SwapChain::SetFullscreenState (bool state)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

bool SwapChain::GetFullscreenState ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Обмен текущего заднего буфера и переднего буфера
*/

void SwapChain::Present ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Список свойств цепочки обмена
*/

IPropertyList* SwapChain::GetProperties ()
{
  return &properties;
}
