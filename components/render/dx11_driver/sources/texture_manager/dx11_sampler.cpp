#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    Конструктор
*/

SamplerState::SamplerState (const DeviceManager& device_manager, const DxSamplerStatePtr& in_sampler)
  : DeviceObject (device_manager)
  , sampler (in_sampler)
{
  if (!sampler)
    throw xtl::make_null_argument_exception ("render::low_level::dx11::Sampler::Sampler", "sampler");  
}

SamplerState::~SamplerState ()
{
}

/*
    Изменение / получение дескриптора
*/

void SamplerState::SetDesc (const SamplerDesc&)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void SamplerState::GetDesc (SamplerDesc&)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}
