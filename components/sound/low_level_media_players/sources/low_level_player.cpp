#include "shared.h"

using namespace sound::low_level_media_players;

namespace
{

//Low level player device
template<DriverType driver_type>
class LowLevelPlayerDevice : public common::Lockable
{
  public:
    //Constructor/destructor
    LowLevelPlayerDevice ()
      : sample_read_offset (0)
      , last_read_sample (0)
      , active_player (0)
      , looping (false)
    {
      try
      {
        if (driver_type < 0 || driver_type >= DriverType_Num)
          throw xtl::make_argument_exception ("", "driver_type", driver_type);

        const char* DRIVERS_MASKS [] = { "OpenAL" };
        const char* INIT_STRINGS [] = { "max_channels_count=1" };

        device = DevicePtr (sound::low_level::DriverManager::CreateDevice (DRIVERS_MASKS [driver_type], "*", INIT_STRINGS [driver_type]), false);

        sound::low_level::Source source;

        memset (&source, 0, sizeof (source));

        source.gain               = 1.f;
        source.minimum_gain       = 1.f;
        source.maximum_gain       = 1.f;
        source.inner_angle        = 360.f;
        source.outer_angle        = 360.f;
        source.outer_gain         = 1.f;
        source.reference_distance = 1.f;
        source.maximum_distance   = 1.f;

        device->SetSource (0, source);
      }
      catch (xtl::exception& e)
      {
        e.touch ("sound::low_level_media_players::LowLevelPlayerDevice::LowLevelPlayerDevice");
        throw;
      }
    }

    ///Playback control
    void Play (media::players::IStreamPlayer* player, const media::SoundSample& sound_sample, bool in_looping, float volume, const media::players::StreamPlayerManager::StreamEventHandler& in_handler)
    {
      bool already_playing = false;

      if (low_level_sample)
      {
        if (sound_sample.Frequency () != media_sample.Frequency () || sound_sample.Channels () != media_sample.Channels () || sound_sample.BitsPerSample () != media_sample.BitsPerSample ())
          throw xtl::format_operation_exception ("sound::low_level_media_players::LowLevelPlayerDevice::Play",
                                                 "Can't play samples with different formats without stopping, current sample is '%s' new sample is '%s'",
                                                 media_sample.Name (), sound_sample.Name ());

        already_playing = true;
      }
      else
      {
        sound::low_level::SampleDesc sample_desc;

        memset (&sample_desc, 0, sizeof (sample_desc));

        sample_desc.samples_count   = -1;
        sample_desc.frequency       = sound_sample.Frequency ();
        sample_desc.channels        = sound_sample.Channels ();
        sample_desc.bits_per_sample = sound_sample.BitsPerSample ();

        low_level_sample = SamplePtr (device->CreateSample (sample_desc, xtl::bind (&LowLevelPlayerDevice<driver_type>::ReadSampleData, this, _1, _2, _3)), false);

        device->SetSample (0, low_level_sample.get ());
      }

      media_sample     = sound_sample;
      last_read_sample = 0;
      active_player    = player;
      looping          = in_looping;
      event_handler    = in_handler;

      SetVolume (active_player, volume);

      if (!already_playing)
        device->Play (0, false);

      event_handler (active_player, media::players::StreamEvent_OnPlay);
    }

    void Pause (void* player)
    {
      {
        common::Lock lock (*this);

        if (player != active_player)
          return;
      }

      device->Pause (0);

      if (device->IsPlaying (0))
        event_handler (active_player, media::players::StreamEvent_OnPlay);
      else
        event_handler (active_player, media::players::StreamEvent_OnPause);
    }

    void Stop (void* player)
    {
      {
        common::Lock lock (*this);

        if (player != active_player || !active_player)
          return;
      }

      StopPlayback ();
    }

    ///Seeking
    void SetPosition (void* player, float position)
    {
      if (player != active_player || ! active_player)
        return;

      sample_read_offset += position * media_sample.Frequency () - last_read_sample;
    }

    ///Telling
    float Position (void* player)
    {
      if (player != active_player)
        return 0;

      return last_read_sample / (float)media_sample.Frequency ();
    }

    ///Playback options
    void SetLooping (void* player, bool state)
    {
      if (player != active_player)
        return;

      looping = state;
    }

    void SetVolume (void* player, float volume)
    {
      {
        common::Lock lock (*this);

        if (player != active_player)
          return;
      }

      device->SetVolume (volume);
    }

  private:
    //Stop and cleanup
    void StopPlayback ()
    {
      device->Stop (0);

      low_level_sample   = 0;
      sample_read_offset = 0;
      last_read_sample   = 0;
      active_player      = 0;
      event_handler      = media::players::StreamPlayerManager::StreamEventHandler ();

      media::SoundSample ().Swap (media_sample);
    }

    //Read data for device
    size_t ReadSampleData (size_t first_sample, size_t samples_count, void* data)
    {
      common::Lock (*this);

      first_sample -= sample_read_offset;

      size_t return_value = media_sample.Read (first_sample, samples_count, data);

      last_read_sample = first_sample + return_value;

      if (last_read_sample == media_sample.SamplesCount ())
      {
        if (looping)
        {
          sample_read_offset = sample_read_offset + last_read_sample;
          last_read_sample   = 0;

          if (samples_count > return_value)
            return_value += ReadSampleData (sample_read_offset, samples_count - return_value, (char*)data + media_sample.SamplesToBytes (return_value));
        }
        else
        {
          media::players::IStreamPlayer* current_player = active_player;

          active_player       = 0;
          sample_read_offset += last_read_sample;
          last_read_sample    = 0;

          event_handler (current_player, media::players::StreamEvent_OnAutoStop);

          if (active_player) //track changed
          {
            if (samples_count > return_value)
              return_value += ReadSampleData (sample_read_offset, samples_count - return_value, (char*)data + media_sample.SamplesToBytes (return_value));
          }
          else
            StopPlayback ();
        }
      }

      return return_value;
    }

  private:
    LowLevelPlayerDevice (const LowLevelPlayerDevice&);             //no impl
    LowLevelPlayerDevice& operator = (const LowLevelPlayerDevice&); //no impl

  private:
    typedef xtl::com_ptr<sound::low_level::IDevice> DevicePtr;
    typedef xtl::com_ptr<sound::low_level::ISample> SamplePtr;

  private:
    media::SoundSample                                      media_sample;       //media sample
    DevicePtr                                               device;             //low level device
    SamplePtr                                               low_level_sample;   //low level sample
    size_t                                                  sample_read_offset; //offset to use for calculation in read function
    size_t                                                  last_read_sample;   //index of last readed sample from media sample
    media::players::IStreamPlayer*                          active_player;      //current playing player
    bool                                                    looping;            //looping state for current playback
    media::players::StreamPlayerManager::StreamEventHandler event_handler;      //events handler
};

}

///Low level sound device backed media player
template<DriverType driver_type>
struct LowLevelPlayer<driver_type>::Impl : public common::Lockable
{
  LowLevelPlayer<driver_type>                             *owner;         //parent object
  LowLevelPlayerDevice<driver_type>                       *device;        //low level device singleton
  media::SoundSample                                      sound_sample;   //sound sample
  bool                                                    looping;        //looping state
  float                                                   volume;         //volume
  media::players::StreamPlayerManager::StreamEventHandler event_handler;  //event handler

  Impl (LowLevelPlayer<driver_type>* in_owner, const char* source_name, const media::players::StreamPlayerManager::StreamEventHandler& in_handler)
    : owner (in_owner)
    , sound_sample (source_name)
    , looping (false)
    , volume (1.f)
    , event_handler (in_handler)
  {
    switch (driver_type)
    {
      case DriverType_OpenAL:
        device = &*common::Singleton<LowLevelPlayerDevice<DriverType_OpenAL> >::Instance ();
        break;
      default:
        throw xtl::make_argument_exception ("sound::low_level_media_players::LowLevelPlayer::LowLevelPlayer", "driver_type", driver_type);
    }
  }

  ///Playback control
  void Play ()
  {
    common::Lock lock (*device);

    device->Play (owner, sound_sample, looping, volume, event_handler);
  }

  void Pause ()
  {
    device->Pause (owner);
  }

  void Stop ()
  {
    device->Stop (owner);
  }

  ///Seeking
  void SetPosition (float position)
  {
    common::Lock lock (*device);

    device->SetPosition (owner, position);
  }

  ///Telling
  float Position ()
  {
    common::Lock lock (*device);

    return device->Position (owner);
  }

  ///Playback options
  void SetLooping (bool state)
  {
    common::Lock lock (*device);

    looping = state;

    device->SetLooping (owner, state);
  }

  void SetVolume (float in_volume)
  {
    volume = in_volume;

    device->SetVolume (owner, volume);
  }
};

/*
   Constructor/destructor
*/

template<DriverType driver_type>
LowLevelPlayer<driver_type>::LowLevelPlayer (const char* source_name, const media::players::StreamPlayerManager::StreamEventHandler& handler)
  : impl (new Impl (this, source_name, handler))
  {}

template<DriverType driver_type>
LowLevelPlayer<driver_type>::~LowLevelPlayer ()
{
  delete impl;
}

/*
   Stream duration
*/

template<DriverType driver_type>
float LowLevelPlayer<driver_type>::Duration ()
{
  return impl->sound_sample.Duration ();
}

/*
   Playback control
*/

template<DriverType driver_type>
void LowLevelPlayer<driver_type>::Play ()
{
  impl->Play ();
}

template<DriverType driver_type>
void LowLevelPlayer<driver_type>::Pause ()
{
  impl->Pause ();
}

template<DriverType driver_type>
void LowLevelPlayer<driver_type>::Stop ()
{
  impl->Stop ();
}

/*
   Seeking
*/

template<DriverType driver_type>
void LowLevelPlayer<driver_type>::SetPosition (float position)
{
  impl->SetPosition (position);
}

/*
   Telling
*/

template<DriverType driver_type>
float LowLevelPlayer<driver_type>::Position ()
{
  return impl->Position ();
}

/*
   Playback options
*/

template<DriverType driver_type>
void LowLevelPlayer<driver_type>::SetLooping (bool state)
{
  impl->SetLooping (state);
}

template<DriverType driver_type>
void LowLevelPlayer<driver_type>::SetVolume (float volume)
{
  impl->SetVolume (volume);
}

namespace sound
{

namespace low_level_media_players
{

template class LowLevelPlayer<sound::low_level_media_players::DriverType_OpenAL>;

}

}
