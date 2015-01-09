#ifndef SOUND_LOW_LEVEL_MEDIA_PLAYERS_SHARED_HEADER
#define SOUND_LOW_LEVEL_MEDIA_PLAYERS_SHARED_HEADER

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>

#include <common/component.h>
#include <common/lockable.h>
#include <common/singleton.h>

#include <media/players/stream.h>
#include <media/sound.h>

#include <sound/driver.h>

namespace sound
{

namespace low_level_media_players
{

enum DriverType
{
  DriverType_OpenAL,

  DriverType_Num
};

///Low level sound device backed media player
template<DriverType driver_type>
class LowLevelPlayer: public media::players::IStreamPlayer
{
  public:
///Constructor/destructor
    LowLevelPlayer (const char* source_name, const media::players::StreamPlayerManager::StreamEventHandler& handler);
    ~LowLevelPlayer ();

    ///Stream duration
    float Duration ();

    ///Playback control
    void Play  ();
    void Pause ();
    void Stop  ();

    ///Seeking
    void SetPosition (float position);

    ///Telling
    float Position ();

    ///Playback options
    void SetLooping (bool state);
    void SetVolume  (float volume);

  private:
    LowLevelPlayer (const LowLevelPlayer& impl);             //no impl
    LowLevelPlayer& operator = (const LowLevelPlayer& impl); //no impl

  private:
    struct Impl;
    Impl* impl;
};

}

}

#endif
