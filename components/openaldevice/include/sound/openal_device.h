#ifndef SOUND_SYSTEM_OPENAL_DEVICE_HEADER
#define SOUND_SYSTEM_OPENAL_DEVICE_HEADER

#include <sound/device.h>

namespace sound
{

namespace low_level
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация драйвера воспроизведения звука на базе OpenAL
///////////////////////////////////////////////////////////////////////////////////////////////////
void register_openal_driver (const char* name = "OpenAL");

}

}

#endif
