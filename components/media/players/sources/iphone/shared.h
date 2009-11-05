#include <stl/string>

#include <xtl/common_exceptions.h>
#include <xtl/function.h>

#include <common/component.h>
#include <common/log.h>

#include <media/players/stream.h>

namespace media
{

namespace players
{

namespace iphone
{

enum VideoPlayerControlsType
{
  VideoPlayerControlsType_NoControls,
  VideoPlayerControlsType_VolumeControls,
  VideoPlayerControlsType_AllControls
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание проигрывателей
///////////////////////////////////////////////////////////////////////////////////////////////////
IStreamPlayer* create_movie_player (const char* stream_name, const StreamPlayerManager::StreamEventHandler* handler, VideoPlayerControlsType controls_type); //Для второго параметра используются указатели, поскольку компилятор не поддерживает передачу параметра по ссылке для mm кода
IStreamPlayer* create_music_player (const char* stream_name, const StreamPlayerManager::StreamEventHandler* handler);

}

}

}
