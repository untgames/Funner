#ifndef MEDIALIB_SOUND_SOURCE_SHARED_HEADER
#define MEDIALIB_SOUND_SOURCE_SHARED_HEADER

#include <stl/vector>
#include <stl/string>

#include <xtl/reference_counter.h>

#include <common/singleton.h>
#include <common/exception.h>
#include <media/sound_source.h>

#include <shared/clone.h>
#include <shared/resource_holder.h>
#include <shared/resource_manager.h>

namespace media
{

/*
    Система управления звуками
*/

class SoundSourceSystemImpl: public ResourceManager<SoundSourceSystem::LoadHandler, SoundSourceSystem::SaveHandler>
{
  public:
    SoundSourceSystemImpl ();
};

typedef common::Singleton<SoundSourceSystemImpl> SoundSourceSystemSingleton;

}

#endif
