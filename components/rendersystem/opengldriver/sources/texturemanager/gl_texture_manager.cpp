#include <common/exception.h>
#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
    Описание реализации SoundDeclarationLibrary
*/

struct TextureManager::Impl
{
};


/*
   Конструктор / деструктор
*/

TextureManager::TextureManager ()
  : impl (new Impl)
{
}

TextureManager::~TextureManager ()
{
}
    
/*
   Создание текстуры и сэмплера
*/

ITexture* TextureManager::CreateTexture (const TextureDesc&)
{
  RaiseNotSupported ("render::low_level::opengl::TextureManager::CreateTexture");
  return NULL;
}

ISamplerState* TextureManager::CreateSamplerState (const SamplerDesc&)
{
  RaiseNotSupported ("render::low_level::opengl::TextureManager::CreateSamplerState");
  return NULL;
}

/*
   Установка текущей текстуры и сэмплера
*/

void TextureManager::SetTexture (size_t sampler_slot, ITexture* texture)
{
  RaiseNotSupported ("render::low_level::opengl::TextureManager::SetTexture");
}

void TextureManager::SetSampler (size_t sampler_slot, ISamplerState* state)
{
  RaiseNotSupported ("render::low_level::opengl::TextureManager::SetSampler");
}

ITexture* TextureManager::GetTexture (size_t sampler_slot) const
{
  RaiseNotSupported ("render::low_level::opengl::TextureManager::GetTexture");
  return NULL;
}

ISamplerState* TextureManager::GetSampler (size_t sampler_slot) const
{
  RaiseNotSupported ("render::low_level::opengl::TextureManager::GetSampler");
  return NULL;
}
