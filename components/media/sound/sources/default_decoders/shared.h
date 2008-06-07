#ifndef SOUND_DEFAULT_DECODERS_SHARED_HEADER
#define SOUND_DEFAULT_DECODERS_SHARED_HEADER

#include <stl/algorithm>

#include <xtl/reference_counter.h>
#include <xtl/string.h>

#include <common/exception.h>
#include <common/file.h>
#include <common/component.h>

#include <media/sound.h>

#include <ogg/os_types.h>
#include <vorbis/vorbisfile.h>

namespace
{

inline void dummy ()
{
    //игнорирование предпреждений об неиспользованных переменных

  (void)OV_CALLBACKS_DEFAULT;
  (void)OV_CALLBACKS_NOCLOSE;
  (void)OV_CALLBACKS_STREAMONLY;
  (void)OV_CALLBACKS_STREAMONLY_NOCLOSE;
}

}

#endif
