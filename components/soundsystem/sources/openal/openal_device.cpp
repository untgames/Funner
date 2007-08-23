#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <al.h>
#include <alc.h>
#include <stl/vector>
#include <xtl/function.h>
#include <common/exception.h>
#include <common/strlib.h>
#include <sound/openal_device.h>
#include <media/sound.h>

#define MAX_CHANNELS_COUNT 1024

using namespace common;
using namespace stl;
using namespace medialib;
using namespace sound;
using namespace sound::openal_device;

namespace
{

void DefaultLogHandler (const char* log_message)
{
  printf ("%s\n", log_message);
}

const char* StrALError (ALenum error)
{
  switch (error)
  {
    case AL_NO_ERROR:          return "No error.";
    case AL_INVALID_NAME:      return "Invalid Name paramater passed to AL call.";
    case AL_INVALID_ENUM:      return "Invalid enum parameter passed to AL call.";
    case AL_INVALID_VALUE:     return "Invalid enum parameter value.";
    case AL_INVALID_OPERATION: return "Invalid operation.";
    case AL_OUT_OF_MEMORY:     return "Out of memory.";
    default:                   return "Unknown error.";
  }
}

const char* StrALCError (ALCenum error)
{
  switch (error)
  {
    case AL_NO_ERROR:         return "No error.";
    case ALC_INVALID_DEVICE:  return "Invalid device.";
    case ALC_INVALID_CONTEXT: return "Invalid context.";
    case ALC_INVALID_ENUM:    return "Invalid enum.";
    case ALC_INVALID_VALUE:   return "Invalid value.";
    case ALC_OUT_OF_MEMORY:   return "Out of memory.";
    default:                  return "Unknown error.";
  }
}

struct OpenALSource
{
  OpenALSource  ();
  ~OpenALSource ();

  SoundSample sound_sample;    //звук
  Source      source;          //общий источник звука
  size_t      name;            //имя источника в OpenAL
  size_t      buffer_name;     //OpenAL буффер
  char*       buffer;          //буффер декодированного звука
};

OpenALSource::OpenALSource ()
  : buffer (NULL)
{
  alGenSources (1, &name);
  if (alGetError () != AL_NO_ERROR)
    Raise <Exception> ("OpenALSource::OpenALSource", "Can't create source");
  alGenBuffers(1, &buffer_name);
  if (alGetError () != AL_NO_ERROR)
    Raise <Exception> ("OpenALSource::OpenALSource", "Can't create buffer");
}

OpenALSource::~OpenALSource ()
{
  if (buffer)
    delete [] buffer;
  if (buffer_name)
    alDeleteBuffers (1, &buffer_name);
  if (alGetError () != AL_NO_ERROR)
    Raise <Exception> ("OpenALSource::~OpenALSource", "Can't delete buffer");
  alDeleteSources (1, &name);
  if (alGetError () != AL_NO_ERROR)
    Raise <Exception> ("OpenALSource::~OpenALSource", "Can't delete source");
}

}

/*
    Описание реализации OpenALSoundSystem
*/

struct OpenALSoundSystem::Impl
{
  ALCdevice              *device;       //устройство OpenAL
  ALCcontext             *context;      //контекст OpenAL
  float                  gain;          //gain
  float                  last_gain;     //предыдущий gain
  bool                   is_muted;      //состояние блокировки проигрывания
  SystemInfo             info;          //информация о устройстве
  LogHandler             log_handler;   //функция лога
  Listener               listener;      //слушатель
  vector <OpenALSource*> sources;       //источники звука

  Impl (ALCdevice* in_device, ALCcontext* in_context);
  ~Impl ();
};

OpenALSoundSystem::Impl::Impl (ALCdevice* in_device, ALCcontext* in_context)
 : device (in_device), context (in_context), log_handler (DefaultLogHandler), is_muted (false), gain (1.f), last_gain (1.f)
{
  char *extensions = (char*)alGetString (AL_EXTENSIONS);
  int  compare_value;

  info.eax_major_version = 0;
  info.eax_minor_version = 0;

  do
  {
    extensions = strstr (extensions, "EAX");
    if (extensions)
    {
      extensions += 3;
      if (isdigit (*extensions))
      {
        compare_value = atoi (extensions);
        if (compare_value > (int)info.eax_major_version)
        {
          info.eax_major_version = compare_value;
          extensions = strstr (extensions, ".");
          info.eax_minor_version = atoi (++extensions);
        }
        else if (compare_value == info.eax_major_version)
        {
          info.eax_major_version = compare_value;
          extensions = strstr (extensions, ".");
          compare_value = atoi (++extensions);
          if (compare_value > (int)info.eax_minor_version)
            info.eax_minor_version = compare_value;
        }
      }
    }
  } while (extensions);

  size_t temp_sources[MAX_CHANNELS_COUNT];
  info.channels_count = 0;
  for (size_t i = 0; i < MAX_CHANNELS_COUNT; i++)
  {
    alGenSources (1, &temp_sources[i]);
    if (alGetError () != AL_NO_ERROR)
    {
      info.channels_count = i;
      alDeleteSources (i, temp_sources);
      if (alGetError () != AL_NO_ERROR)
        Raise <Exception> ("OpenALSoundSystem::OpenALSoundSystem", "Can't delete sources generated during initialization");
      break;
    }
  }
  if (!info.channels_count)
  {
    alDeleteSources (MAX_CHANNELS_COUNT, temp_sources);
    info.channels_count = MAX_CHANNELS_COUNT;
  }

  sources.resize (info.channels_count);
  for (size_t i = 0; i < info.channels_count; i++)
    sources[i] = new OpenALSource;
}

OpenALSoundSystem::Impl::~Impl ()
{
  for (size_t i = 0; i < info.channels_count; i++)
    delete sources[i];
  alcMakeContextCurrent (NULL);
  alcDestroyContext (context);
  if (alcGetError (device) == ALC_INVALID_CONTEXT)
    Raise <Exception> ("OpenALSoundSystem::~Impl", "Can't destroy context. Invalid context");
  if (!alcCloseDevice (device))
    Raise <Exception> ("OpenALSoundSystem::~Impl", "Can't close device. '%s'", StrALCError (alcGetError (device)));
}

/*
    Конструктор / деструктор
*/

OpenALSoundSystem::OpenALSoundSystem ()
{
  ALCdevice  *device;
  ALCcontext *context;

  device = alcOpenDevice (NULL);
  if (!device) 
    Raise <Exception> ("OpenALSoundSystem::OpenALSoundSystem", "Can't open default device");

  context = alcCreateContext (device, NULL);
  if (!context)
    Raise <Exception> ("OpenALSoundSystem::OpenALSoundSystem", "Can't create additional context. '%s'", StrALCError (alcGetError (impl->device)));
  if (!alcMakeContextCurrent (context))
    Raise <Exception> ("OpenALSoundSystem::OpenALSoundSystem", "Can't make context current. '%s'", StrALCError (alcGetError (impl->device)));

  impl = new Impl (device, context); 
}

OpenALSoundSystem::~OpenALSoundSystem ()
{
  delete impl;
}

/*
   Имя устройства
*/

const char* OpenALSoundSystem::Name ()
{
  return alcGetString (impl->device, ALC_DEVICE_SPECIFIER);
}

/*
   Получение информации об устройстве
*/

void OpenALSoundSystem::GetInfo (SystemInfo& target_info)
{
  target_info = impl->info;
}

/*
   Количество микшируемых каналов
*/

size_t OpenALSoundSystem::ChannelsCount ()
{
  return impl->info.channels_count;
}
    
/*
   Установка текущего проигрываемого звука
*/

void OpenALSoundSystem::SetSample (size_t channel, const char* sample_name)
{
  if (channel >= impl->info.channels_count)
    RaiseOutOfRange ("OpenALSoundSystem::SetSample", "channel", channel, impl->info.channels_count);
    
  if (!sample_name)
    RaiseNullArgument ("OpenALSoundSystem::SetSample", "sample_name");

  ALenum error_code;

  try
  {
    impl->sources[channel]->sound_sample.Load (sample_name);
    if (impl->sources[channel]->sound_sample.BitsPerSample () != 16)
      RaiseNotSupported ("OpenALSoundSystem::SetSample", "Supported only 16 bit audio depth, sample depth - %u.", impl->sources[channel]->sound_sample.BitsPerSample ());
    if (impl->sources[channel]->sound_sample. Channels () > 2 || !impl->sources[channel]->sound_sample. Channels ())
      RaiseNotSupported ("OpenALSoundSystem::SetSample", "Supported only mono and stereo sources, sample has %u channels.", impl->sources[channel]->sound_sample. Channels ());
    size_t buffer_size = impl->sources[channel]->sound_sample.SizeInBytes ();
    impl->sources[channel]->buffer = new char [buffer_size];  
    impl->sources[channel]->sound_sample.Read (0, impl->sources[channel]->sound_sample.SizeInBytes (), impl->sources[channel]->buffer);

    if (impl->sources[channel]->sound_sample.Channels () == 1)
      alBufferData (impl->sources[channel]->buffer_name, AL_FORMAT_MONO16, impl->sources[channel]->buffer, buffer_size, impl->sources[channel]->sound_sample.Frequency ());
    else
      alBufferData (impl->sources[channel]->buffer_name, AL_FORMAT_STEREO16, impl->sources[channel]->buffer, buffer_size, impl->sources[channel]->sound_sample.Frequency ());
    error_code = alGetError ();
    if (error_code != AL_NO_ERROR)
    {
      impl->log_handler (format ("Can't attach data to buffer. '%s'", StrALError (error_code)).c_str ());
      delete [] impl->sources[channel]->buffer;
      return;
    }
  }
  catch (std::exception& exception)
  {                                               
    impl->log_handler (format ("Can't load file '%s'. Exception: '%s'.", sample_name, exception.what ()).c_str());
  }                                               
  catch (...)
  {
    impl->log_handler (format ("Can't load file '%s'. Unknown exception.", sample_name).c_str());
  }

  alSourcei(impl->sources[channel]->name, AL_BUFFER, impl->sources[channel]->buffer_name);
  error_code = alGetError ();
  if (error_code != AL_NO_ERROR)
  {
    impl->log_handler (format ("Can't attach buffer to channel %u. '%s'", channel, StrALError (error_code)).c_str ());
    delete [] impl->sources[channel]->buffer;
    return;
  }
}

const char* OpenALSoundSystem::GetSample (size_t channel)
{
  if (channel >= impl->info.channels_count)
    RaiseOutOfRange ("OpenALSoundSystem::GetSample", "channel", channel, impl->info.channels_count);
    
  return impl->sources[channel]->sound_sample.Name ();
}

/*
   Проверка цикличности проигрывания канала
*/

bool OpenALSoundSystem::IsLooped (size_t channel)
{
  if (channel >= impl->info.channels_count)
    RaiseOutOfRange ("OpenALSoundSystem::IsLooped", "channel", channel, impl->info.channels_count);
    
  ALenum error_code;
  int    status;

  alGetSourcei (impl->sources[channel]->name, AL_LOOPING, &status);
  error_code = alGetError ();
  if (error_code != AL_NO_ERROR)
  {
    impl->log_handler (format ("Can't get channel %u looping property. '%s'", channel, StrALError (error_code)).c_str ());
    return false;
  }
  return status != 0;
}
    
/*
   Установка параметров источника
*/

void OpenALSoundSystem::SetSource (size_t channel, const Source& source_source)
{
  if (channel >= impl->info.channels_count)
    RaiseOutOfRange ("OpenALSoundSystem::SetSource", "channel", channel, impl->info.channels_count);
    
  impl->sources[channel]->source = source_source;
  ALenum error_code;

  alSourcefv (impl->sources[channel]->name, AL_POSITION, source_source.position);
  error_code = alGetError ();
  if (error_code != AL_NO_ERROR)
    impl->log_handler (format ("Can't set channel %u position property. '%s'", channel, StrALError (error_code)).c_str ());

  alSourcefv (impl->sources[channel]->name, AL_DIRECTION, source_source.direction);
  error_code = alGetError ();
  if (error_code != AL_NO_ERROR)
    impl->log_handler (format ("Can't set channel %u direction property. '%s'", channel, StrALError (error_code)).c_str ());

  alSourcefv (impl->sources[channel]->name, AL_VELOCITY, source_source.velocity);
  error_code = alGetError ();
  if (error_code != AL_NO_ERROR)
    impl->log_handler (format ("Can't set channel %u velocity property. '%s'", channel, StrALError (error_code)).c_str ());

  alSourcef (impl->sources[channel]->name, AL_GAIN, source_source.gain);
  error_code = alGetError ();
  if (error_code != AL_NO_ERROR)
    impl->log_handler (format ("Can't set channel %u gain property. '%s'", channel, StrALError (error_code)).c_str ());

  alSourcef (impl->sources[channel]->name, AL_MIN_GAIN, source_source.minimum_gain);
  error_code = alGetError ();
  if (error_code != AL_NO_ERROR)
    impl->log_handler (format ("Can't set channel %u minimum gain property. '%s'", channel, StrALError (error_code)).c_str ());

  alSourcef (impl->sources[channel]->name, AL_MAX_GAIN, source_source.maximum_gain);
  error_code = alGetError ();
  if (error_code != AL_NO_ERROR)
    impl->log_handler (format ("Can't set channel %u maximum gain property. '%s'", channel, StrALError (error_code)).c_str ());

  alSourcef (impl->sources[channel]->name, AL_CONE_INNER_ANGLE, source_source.inner_angle);
  error_code = alGetError ();
  if (error_code != AL_NO_ERROR)
    impl->log_handler (format ("Can't set channel %u inner angle property. '%s'", channel, StrALError (error_code)).c_str ());

  alSourcef (impl->sources[channel]->name, AL_CONE_OUTER_ANGLE, source_source.outer_angle);
  error_code = alGetError ();
  if (error_code != AL_NO_ERROR)
    impl->log_handler (format ("Can't set channel %u outer angle property. '%s'", channel, StrALError (error_code)).c_str ());

  alSourcef (impl->sources[channel]->name, AL_CONE_OUTER_GAIN, source_source.outer_gain);
  error_code = alGetError ();
  if (error_code != AL_NO_ERROR)
    impl->log_handler (format ("Can't set channel %u outer gain property. '%s'", channel, StrALError (error_code)).c_str ());

  alSourcef (impl->sources[channel]->name, AL_REFERENCE_DISTANCE, source_source.reference_distance);
  error_code = alGetError ();
  if (error_code != AL_NO_ERROR)
    impl->log_handler (format ("Can't set channel %u reference distance property. '%s'", channel, StrALError (error_code)).c_str ());

}

void OpenALSoundSystem::GetSource (size_t channel, Source& target_source)
{
  if (channel >= impl->info.channels_count)
    RaiseOutOfRange ("OpenALSoundSystem::GetSource", "channel", channel, impl->info.channels_count);
    
  target_source = impl->sources[channel]->source;
}

/*
   Управление проигрыванием
*/

void OpenALSoundSystem::Play (size_t channel, bool looping)
{
  if (channel >= impl->info.channels_count)
    RaiseOutOfRange ("OpenALSoundSystem::Play", "channel", channel, impl->info.channels_count);
    
  ALenum error_code;

  alSourcePlay (impl->sources[channel]->name);
  error_code = alGetError ();
  if (error_code != AL_NO_ERROR)
    impl->log_handler (format ("Can't perform play operation on channel %u. '%s'", channel, StrALError (error_code)).c_str ());

  alSourcei (impl->sources[channel]->name, AL_LOOPING, looping);
  error_code = alGetError ();
  if (error_code != AL_NO_ERROR)
    impl->log_handler (format ("Can't set channel %u looping property. '%s'", channel, StrALError (error_code)).c_str ());
}

void OpenALSoundSystem::Pause (size_t channel)
{
  if (channel >= impl->info.channels_count)
    RaiseOutOfRange ("OpenALSoundSystem::Pause", "channel", channel, impl->info.channels_count);
    
  ALenum error_code;

  alSourcePause (impl->sources[channel]->name);
  error_code = alGetError ();
  if (error_code != AL_NO_ERROR)
    impl->log_handler (format ("Can't perform pause operation on channel %u. '%s'", channel, StrALError (error_code)).c_str ());
}

void OpenALSoundSystem::Stop (size_t channel)
{
  if (channel >= impl->info.channels_count)
    RaiseOutOfRange ("OpenALSoundSystem::Stop", "channel", channel, impl->info.channels_count);
    
  ALenum error_code;

  alSourceStop (impl->sources[channel]->name);
  error_code = alGetError ();
  if (error_code != AL_NO_ERROR)
    impl->log_handler (format ("Can't perform stop operation on channel %u. '%s'", channel, StrALError (error_code)).c_str ());
}

void OpenALSoundSystem::Seek (size_t channel, float time_in_seconds)
{   //!!!!! нужно учитывать разбитие на буфера
  if (channel >= impl->info.channels_count)
    RaiseOutOfRange ("OpenALSoundSystem::Seek", "channel", channel, impl->info.channels_count);
    
  ALenum error_code;

  alSourcef (impl->sources[channel]->name, AL_SEC_OFFSET, time_in_seconds);
  error_code = alGetError ();
  if (error_code != AL_NO_ERROR)
    impl->log_handler (format ("Can't perform seek operation on channel %u. '%s'", channel, StrALError (error_code)).c_str ());
}

float OpenALSoundSystem::Tell (size_t channel)
{
  if (channel >= impl->info.channels_count)
    RaiseOutOfRange ("OpenALSoundSystem::Tell", "channel", channel, impl->info.channels_count);
    
  float  offset;
  ALenum error_code;

  alGetSourcef (impl->sources[channel]->name, AL_SEC_OFFSET, &offset);
  error_code = alGetError ();
  if (error_code != AL_NO_ERROR)
  {
    impl->log_handler (format ("Can't retreive channel %u offset. '%s'", channel, StrALError (error_code)).c_str ());
    return 0.f;
  }

  return offset;
}

bool OpenALSoundSystem::IsPlaying (size_t channel)
{
  if (channel >= impl->info.channels_count)
    RaiseOutOfRange ("OpenALSoundSystem::IsPlaying", "channel", channel, impl->info.channels_count);
    
  int    status;
  ALenum error_code;

  alGetSourcei (impl->sources[channel]->name, AL_SOURCE_STATE, &status);
  error_code = alGetError ();
  if (error_code != AL_NO_ERROR)
  {
    impl->log_handler (format ("Can't retreive channel %u status. '%s'", channel, StrALError (error_code)).c_str ());
    return false;
  }

  return (status == AL_PLAYING);
}
    
/*
   Установка уровня громкости для устройства
*/

void  OpenALSoundSystem::SetVolume (float gain)
{
  if (gain < 0.f || gain > 1.f)
  {
    impl->log_handler (format ("Incorrect gain value received (%f). Gain not changed", gain).c_str ());
    return;
  }

  impl->gain = gain;

  ALenum error_code;

  alListenerf (AL_GAIN, gain);
  error_code = alGetError ();
  if (error_code != AL_NO_ERROR)
    impl->log_handler (format ("Can't set volume. '%s'", StrALError (error_code)).c_str ());
}

float OpenALSoundSystem::GetVolume ()
{
  return impl->gain;
}

/*
   Блокировка проигрывания звука
*/

void OpenALSoundSystem::SetMute (bool state)
{
  if (impl->is_muted != state)
  {
    if (impl->is_muted)
      SetVolume (impl->last_gain);
    else
    {
      impl->last_gain = impl->gain;
      SetVolume (0.f);
    }
  }
  impl->is_muted = state;
}

bool OpenALSoundSystem::IsMuted ()
{
  return impl->is_muted;
}

/*
   Установка параметров слушателя
*/

void OpenALSoundSystem::SetListener (const Listener& source_listener)
{
  impl->listener = source_listener;
  ALenum error_code;
  float  orientation [6] = {source_listener.direction.x, source_listener.direction.y, source_listener.direction.z, 
                            source_listener.up.x,        source_listener.up.y,        source_listener.up.z};

  alListenerfv (AL_POSITION, source_listener.position);
  error_code = alGetError ();
  if (error_code != AL_NO_ERROR)
    impl->log_handler (format ("Can't set listener position property. '%s'", StrALError (error_code)).c_str ());

  alListenerfv (AL_VELOCITY, source_listener.velocity);
  error_code = alGetError ();
  if (error_code != AL_NO_ERROR)
    impl->log_handler (format ("Can't set listener velocity property. '%s'", StrALError (error_code)).c_str ());

  alListenerfv (AL_ORIENTATION, orientation);
  error_code = alGetError ();
  if (error_code != AL_NO_ERROR)
    impl->log_handler (format ("Can't set listener direction and up property. '%s'", StrALError (error_code)).c_str ());
}

void OpenALSoundSystem::GetListener (Listener& target_listener)
{
  target_listener = impl->listener;
}

/*
   Установка функции отладочного протоколирования
*/

void OpenALSoundSystem::SetDebugLog (const LogHandler& log_handler)
{
  impl->log_handler = log_handler;
}

const OpenALSoundSystem::LogHandler& OpenALSoundSystem::GetDebugLog ()
{
  return impl->log_handler;
}
