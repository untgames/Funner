#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    Описание реализации ContextManager
*/

struct ContextManager::Impl
{

};

/*
    Конструктор / деструктор
*/

ContextManager::ContextManager ()
  : impl (new Impl)
{
}

ContextManager::~ContextManager ()
{
}

/*
    Создание цепочки обмена
*/

ISwapChain* ContextManager::CreateSwapChain (IOutput* target_output, const SwapChainDesc& swap_chain_desc)
{
  RaiseNotImplemented ("render::low_level::opengl::ContextManager::CreateSwapChain");
  return 0;
}
