#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    Описание реализации менеджер текстур
*/

struct TextureManager::Impl: public DeviceObject
{
/// Конструктор
  Impl (const DeviceManager& manager)
    : DeviceObject (manager)
  {
  }
};

/*
    Конструктор / деструктор
*/

TextureManager::TextureManager (const DeviceManager& manager)
  : impl (new Impl (manager))
{
}

TextureManager::~TextureManager ()
{
}
    
/*
    Создание текстуры и сэмплера
*/

ITexture* TextureManager::CreateTexture (const TextureDesc& desc, const TextureData* data)
{
  try
  {
    return new Texture (impl->GetDeviceManager (), desc, data);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::TextureManager::CreateTexture");
    throw;
  }
}

ISamplerState* TextureManager::CreateSamplerState (const SamplerDesc& desc)
{
  try
  {
    return new SamplerState (impl->GetDeviceManager (), desc);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::TextureManager::CreateSamplerState");
    throw;
  }
}
