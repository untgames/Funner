#ifndef SOUND_DEFAULT_DECODERS_SHARED_HEADER
#define SOUND_DEFAULT_DECODERS_SHARED_HEADER

#define OV_EXCLUDE_STATIC_CALLBACKS

#include <ogg/os_types.h>
#include <vorbis/vorbisfile.h>

#include <stl/algorithm>

#include <xtl/reference_counter.h>
#include <xtl/string.h>
#include <xtl/common_exceptions.h>

#include <common/file.h>
#include <common/component.h>
#include <common/log.h>
#include <common/singleton.h>

#include <media/sound.h>

#endif
