#ifndef SOUND_DEFAULT_DECODERS_SHARED_HEADER
#define SOUND_DEFAULT_DECODERS_SHARED_HEADER

#include <ogg/os_types.h>
#include <vorbis/vorbisfile.h>

#include <stl/algorithm>

#include <xtl/reference_counter.h>
#include <xtl/string.h>
#include <xtl/common_exceptions.h>

#include <common/file.h>
#include <common/component.h>
#include <common/log.h>

#include <media/sound.h>

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
