#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    Описание реализации менеджера входного уровня
*/

struct InputManager::Impl: public DeviceObject
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

InputManager::InputManager (const DeviceManager& device_manager)
  : impl (new Impl (device_manager))
{
}

InputManager::~InputManager ()
{
}

/*
    Создание ресурсов уровня
*/

InputLayout* InputManager::CreateInputLayout (const InputLayoutDesc& desc)
{
  try
  {
    return new InputLayout (desc);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::InputManager::CreateInputLayout");
    throw;
  }
}

IBuffer* InputManager::CreateBuffer (const BufferDesc& desc, const void* data)
{
  try
  {
    return new InputBuffer (impl->GetDeviceManager (), desc, data);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::InputManager::CreateBuffer");
    throw;
  }
}

/*
    Получение имени семантики
*/

const char* InputManager::GetVertexAttributeSemanticName (VertexAttributeSemantic semantic)
{
  try
  {
    return get_semantic_name (semantic);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::InputManager::GetVertexAttributeSemanticName");
    throw;
  }
}
