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

ISwapChain* ContextManager::CreateSwapChain (OutputManager& output_manager, const SwapChainDesc& swap_chain_desc)
{
  return new SwapChain (output_manager, swap_chain_desc);
}

