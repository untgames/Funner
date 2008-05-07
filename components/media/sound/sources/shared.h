#ifndef SOUNDLIB_SHARED_HEADER
#define SOUNDLIB_SHARED_HEADER

#include <vorbis/vorbisfile.h>

#include <common/exception.h>
#include <common/singleton.h>
#include <common/file.h>
#include <common/xml_writer.h>
#include <common/parser.h>

#include <stl/vector>
#include <stl/string>
#include <stl/hash_set> //??убрать!!!
#include <stl/hash_map>

#include <xtl/function.h>
#include <xtl/reference_counter.h>

#include <media/sound.h>
#include <media/sound_declaration.h>

#include <shared/clone.h>
#include <shared/resource_library.h>
#include <shared/resource_manager.h>

#endif
