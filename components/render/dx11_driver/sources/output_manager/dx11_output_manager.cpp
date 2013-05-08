#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    Описание реализации менеджера выходного уровня
*/

struct OutputManager::Impl: public DeviceObject
{
/// Конструктор
  Impl (const DeviceManager& device_manager)
    : DeviceObject (device_manager)
  {
  }
};

/*
    Конструктор / деструктор
*/

OutputManager::OutputManager (const DeviceManager& device_manager)
  : impl (new Impl (device_manager))
{
}

OutputManager::~OutputManager ()
{
}

/*
    Создание состояний выходного уровня
*/

IBlendState* OutputManager::CreateBlendState (const BlendDesc& desc)
{
  try
  {
    return new BlendState (impl->GetDeviceManager (), desc);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::OutputManager::CreateBlendState");
    throw;
  }
}

IDepthStencilState* OutputManager::CreateDepthStencilState (const DepthStencilDesc& desc)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::OutputManager::CreateDepthStencilState");
    throw;
  }
}

IRasterizerState* OutputManager::CreateRasterizerState (const RasterizerDesc& desc)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::OutputManager::CreateRasterizerState");
    throw;
  }
}
