#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stl/vector>
#include <xtl/function.h>
#include <xtl/bind.h>
#include <syslib/timer.h>
#include <common/strlib.h>
#include <media/sound.h>

#include "shared.h"

#define MAX_CHANNELS_COUNT 1024

using namespace common;
using namespace stl;
using namespace medialib;
using namespace sound::low_level;

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

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

struct OpenALSource
{
  OpenALSource  ();
  ~OpenALSource ();

  SoundSample sound_sample;    //звук
  Source      source;          //общий источник звука
  bool        looping;         //зацикленность
  bool        play_from_start; //нужно ли делать rewind при вызове play
  size_t      name;            //имя источника в OpenAL
  size_t      buffer_name[2];  //OpenAL буффер
  size_t      buffer_samples;  //количество сэмплов в буффере
  size_t      buffer_size;     //размер в байтах
  size_t      last_sample;     //номер последнего прочитанного сэмпла
  size_t      start_sample;    //номер начального сэмпла проигрывания
  clock_t     play_start_time; //время последнего seek
  size_t      first_buffer;    //номер первого в очереди буффера    
  char*       buffer[2];       //буффер декодированного звука
};

OpenALSource::OpenALSource ()
  : play_from_start (true)
{
  buffer[0] = NULL;
  buffer[1] = NULL;
  alGenSources (1, &name);
  if (alGetError () != AL_NO_ERROR)
    Raise <Exception> ("OpenALSource::OpenALSource", "Can't create source");
  alGenBuffers(2, buffer_name);
  if (alGetError () != AL_NO_ERROR)
    Raise <Exception> ("OpenALSource::OpenALSource", "Can't create buffer");
}

OpenALSource::~OpenALSource ()
{
  alSourceStop (name);

  if (buffer[0])
    delete [] buffer[0];
  if (buffer[1])
    delete [] buffer[1];
  if (buffer_name)
    alDeleteBuffers (2, buffer_name);
  alDeleteSources (1, &name);
}

bool FillSourceBuffer (OpenALSource* source, size_t buffer_index, ISoundDevice::LogHandler log_handler)
{
  size_t sample_size = source->sound_sample.SamplesToBytes (1);
  size_t readed_samples;
  ALenum error_code;

  if (source->looping)
  {
    for (size_t total_readed_samples = 0; total_readed_samples < source->buffer_samples; total_readed_samples += readed_samples)
    {
      if (source->last_sample == source->sound_sample.SamplesCount ())
        source->last_sample = 0;
      source->last_sample += readed_samples = source->sound_sample.Read (source->last_sample, source->buffer_samples - total_readed_samples, source->buffer[buffer_index] + source->sound_sample.SamplesToBytes (total_readed_samples));
    
      if (!readed_samples)
      {
        log_handler (format ("Can't read data from sample '%s'", source->sound_sample.Name ()).c_str ());
        return false;
      }
    }
    readed_samples = total_readed_samples;
  }
  else
  {
    source->last_sample += readed_samples = source->sound_sample.Read (source->last_sample, source->buffer_samples, source->buffer[buffer_index]);
    if (!readed_samples)
      return false;
  }

  if (source->sound_sample.Channels () == 1)
    alBufferData (source->buffer_name[buffer_index], AL_FORMAT_MONO16, source->buffer[buffer_index], readed_samples * sample_size, source->sound_sample.Frequency ());
  else
    alBufferData (source->buffer_name[buffer_index], AL_FORMAT_STEREO16, source->buffer[buffer_index], readed_samples * sample_size, source->sound_sample.Frequency ());

  error_code = alGetError ();
  
  if (error_code != AL_NO_ERROR)
  {
    log_handler (format ("Can't set buffer data. '%s'", StrALError (error_code)).c_str ());
    return false;
  }

  return true;
}

void UpdateSourceBuffer (OpenALSource* source, ISoundDevice::LogHandler log_handler, OpenALContext* context)
{
  if (!source->buffer[0])
    return;

  int    processed_buffers = 0;
  
  context->alGetSourcei (source->name, AL_BUFFERS_QUEUED, &processed_buffers);

  if (!processed_buffers)
  {
    if (FillSourceBuffer (source, 0, log_handler))
    {
      if (FillSourceBuffer (source, 1, log_handler))
        context->alSourceQueueBuffers (source->name, 2, source->buffer_name);
      else
        context->alSourceQueueBuffers (source->name, 1, &source->buffer_name[0]);
    }
    else
    {
      log_handler (format ("No buffers was queued from source '%s'", source->sound_sample.Name ()).c_str ());
      return;
    }

    return;
  }

  for (;;)
  {
    context->alGetSourcei (source->name, AL_BUFFERS_PROCESSED, &processed_buffers);

    if (!processed_buffers)
      return;

    context->alSourceUnqueueBuffers (source->name, 1, &source->buffer_name[source->first_buffer]);
    
    if (!FillSourceBuffer (source, source->first_buffer, log_handler))
      return;

    context->alSourceQueueBuffers (source->name, 1, &source->buffer_name[source->first_buffer]);

    source->first_buffer++;
    source->first_buffer %= 2;
  }
}

}

/*
    Описание реализации OpenALDevice
*/

struct OpenALDevice::Impl
{
  syslib::Timer          timer;         //таймер обновления буффера
  OpenALContext          context;       //контекст
  float                  gain;          //gain
  float                  last_gain;     //предыдущий gain
  bool                   is_muted;      //состояние блокировки проигрывания
  Capabilities           info;          //информация о устройстве
  LogHandler             log_handler;   //функция лога
  Listener               listener;      //слушатель
  vector <OpenALSource*> sources;       //источники звука
  size_t                 ref_count;     //количество ссылок

  Impl (const char* device_name, OpenALDevice* sound_system);
  ~Impl ();
};

OpenALDevice::Impl::Impl (const char* device_name, OpenALDevice* sound_system)
 : context (device_name, &DefaultLogHandler), log_handler (DefaultLogHandler), is_muted (false), gain (1.f), last_gain (1.f),
   timer (xtl::bind (&OpenALDevice::UpdateBuffers, sound_system), (size_t)(BUFFER_UPDATE_TIME * 1000)), ref_count (1)
{
    //временный код!!!

  context.MakeCurrent ();

  char *extensions = (char*)context.alGetString (AL_EXTENSIONS);
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
  size_t cur_channel = 0;
  for (; cur_channel < MAX_CHANNELS_COUNT; cur_channel++)
  {
    alGenSources (1, &temp_sources[cur_channel]);
    if (alGetError () != AL_NO_ERROR)
    {
      info.channels_count = cur_channel;
      context.alDeleteSources (cur_channel, temp_sources);
      break;
    }
  }
  if (!cur_channel)
    Raise <Exception> ("OpenALDevice::OpenALDevice", "Can't create no one channel");
  if (!info.channels_count)
  {
    context.alDeleteSources (MAX_CHANNELS_COUNT, temp_sources);
    info.channels_count = MAX_CHANNELS_COUNT;
  }

  sources.resize (info.channels_count);
  for (size_t i = 0; i < info.channels_count; i++)
    sources[i] = new OpenALSource;
}

OpenALDevice::Impl::~Impl ()
{
  for (size_t i = 0; i < info.channels_count; i++)
    delete sources[i];
}

/*
    Конструктор / деструктор
*/

OpenALDevice::OpenALDevice (const char* device_name)
  : impl (new Impl (device_name, this))
  {}

OpenALDevice::~OpenALDevice ()
{
  delete impl;
}

/*
   Имя устройства
*/

const char* OpenALDevice::Name ()
{
  return "openal"; //в будущем более подробно!!!
//  return alcGetString (impl->device, ALC_DEVICE_SPECIFIER);
}

/*
   Получение информации об устройстве
*/

void OpenALDevice::GetCapabilities (Capabilities& target_info)
{
  target_info = impl->info;
}

/*
   Количество микшируемых каналов
*/

size_t OpenALDevice::ChannelsCount ()
{
  return impl->info.channels_count;
}
    
/*
   Установка текущего проигрываемого звука
*/

void OpenALDevice::SetSample (size_t channel, const char* sample_name)
{
  if (channel >= impl->info.channels_count)
    RaiseOutOfRange ("OpenALDevice::SetSample", "channel", channel, impl->info.channels_count);
    
  if (!sample_name)
    RaiseNullArgument ("OpenALDevice::SetSample", "sample_name");
    
  Stop (channel);

  try
  {
    impl->sources[channel]->sound_sample.Load (sample_name);
    
    if (impl->sources[channel]->sound_sample.BitsPerSample () != 16)
      RaiseNotSupported ("OpenALDevice::SetSample", "Supported only 16 bit audio depth, sample depth - %u.", impl->sources[channel]->sound_sample.BitsPerSample ());
    if (impl->sources[channel]->sound_sample.Channels () > 2 || !impl->sources[channel]->sound_sample.Channels ())
      RaiseNotSupported ("OpenALDevice::SetSample", "Supported only mono and stereo sources, sample has %u channels.", impl->sources[channel]->sound_sample.Channels ());

    impl->sources[channel]->buffer_samples = (size_t)(BUFFER_UPDATE_TIME * impl->sources[channel]->sound_sample.Frequency ());

    if (impl->sources[channel]->buffer_size)
    {
      size_t new_buffer_size = (size_t)(impl->sources[channel]->sound_sample.BitsPerSample () / 8 * impl->sources[channel]->buffer_samples * impl->sources[channel]->sound_sample.Channels ());
      
      if (impl->sources[channel]->buffer_size != new_buffer_size)
      {
        if (impl->sources[channel]->buffer[0])
          delete [] impl->sources[channel]->buffer[0];
        if (impl->sources[channel]->buffer[1])
          delete [] impl->sources[channel]->buffer[1];

        impl->sources[channel]->buffer_size = new_buffer_size;
        impl->sources[channel]->buffer[0] = new char [impl->sources[channel]->buffer_size];  
        impl->sources[channel]->buffer[1] = new char [impl->sources[channel]->buffer_size];  
      }
    }
    else
    {
      impl->sources[channel]->buffer_size = (size_t)(impl->sources[channel]->sound_sample.BitsPerSample () / 8 * impl->sources[channel]->buffer_samples * impl->sources[channel]->sound_sample.Channels ());
      impl->sources[channel]->buffer[0] = new char [impl->sources[channel]->buffer_size];  
      impl->sources[channel]->buffer[1] = new char [impl->sources[channel]->buffer_size];  
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
}

const char* OpenALDevice::GetSample (size_t channel)
{
  if (channel >= impl->info.channels_count)
    RaiseOutOfRange ("OpenALDevice::GetSample", "channel", channel, impl->info.channels_count);
    
  return impl->sources[channel]->sound_sample.Name ();
}

/*
   Проверка цикличности проигрывания канала
*/

bool OpenALDevice::IsLooped (size_t channel)
{
  if (channel >= impl->info.channels_count)
    RaiseOutOfRange ("OpenALDevice::IsLooped", "channel", channel, impl->info.channels_count);
    
  return impl->sources[channel]->looping; 
}
    
/*
   Установка параметров источника
*/

void OpenALDevice::SetSource (size_t channel, const Source& source_source)
{
  if (channel >= impl->info.channels_count)
    RaiseOutOfRange ("OpenALDevice::SetSource", "channel", channel, impl->info.channels_count);
    
  impl->sources[channel]->source = source_source;

  impl->context.alSourcefv (impl->sources[channel]->name, AL_POSITION, source_source.position);
  impl->context.alSourcefv (impl->sources[channel]->name, AL_DIRECTION, source_source.direction);
  impl->context.alSourcefv (impl->sources[channel]->name, AL_VELOCITY, source_source.velocity);
  impl->context.alSourcef  (impl->sources[channel]->name, AL_GAIN, source_source.gain);
  impl->context.alSourcef  (impl->sources[channel]->name, AL_MIN_GAIN, source_source.minimum_gain);
  impl->context.alSourcef  (impl->sources[channel]->name, AL_MAX_GAIN, source_source.maximum_gain);
  impl->context.alSourcef  (impl->sources[channel]->name, AL_CONE_INNER_ANGLE, source_source.inner_angle);
  impl->context.alSourcef  (impl->sources[channel]->name, AL_CONE_OUTER_ANGLE, source_source.outer_angle);
  impl->context.alSourcef  (impl->sources[channel]->name, AL_CONE_OUTER_GAIN, source_source.outer_gain);
  impl->context.alSourcef  (impl->sources[channel]->name, AL_REFERENCE_DISTANCE, source_source.reference_distance);
}

void OpenALDevice::GetSource (size_t channel, Source& target_source)
{
  if (channel >= impl->info.channels_count)
    RaiseOutOfRange ("OpenALDevice::GetSource", "channel", channel, impl->info.channels_count);
    
  target_source = impl->sources[channel]->source;
}

/*
   Управление проигрыванием
*/

void OpenALDevice::Play (size_t channel, bool looping)
{
  if (channel >= impl->info.channels_count)
    RaiseOutOfRange ("OpenALDevice::Play", "channel", channel, impl->info.channels_count);
    
  impl->sources[channel]->looping = looping;

  if (impl->sources[channel]->play_from_start)
    Seek (channel, 0);

  impl->sources[channel]->play_from_start = true;
  impl->sources[channel]->play_start_time = clock ();
  impl->context.alSourcePlay (impl->sources[channel]->name);
}

void OpenALDevice::Pause (size_t channel)
{
  if (channel >= impl->info.channels_count)
    RaiseOutOfRange ("OpenALDevice::Pause", "channel", channel, impl->info.channels_count);
    
  impl->context.alSourcePause (impl->sources[channel]->name);
}

void OpenALDevice::Stop (size_t channel)
{
  if (channel >= impl->info.channels_count)
    RaiseOutOfRange ("OpenALDevice::Stop", "channel", channel, impl->info.channels_count);
    
  impl->sources[channel]->play_from_start = true;

  impl->context.alSourceStop (impl->sources[channel]->name);
}

void OpenALDevice::Seek (size_t channel, float time_in_seconds)
{
  if (channel >= impl->info.channels_count)
    RaiseOutOfRange ("OpenALDevice::Seek", "channel", channel, impl->info.channels_count);

  if (time_in_seconds >= impl->sources[channel]->sound_sample.SamplesToSeconds (impl->sources[channel]->sound_sample.SamplesCount ()))
  {
    impl->log_handler ("Attempting to seek more than sample range. Seeking to begin.");
    time_in_seconds = 0;
  }

  impl->sources[channel]->last_sample = impl->sources[channel]->start_sample = impl->sources[channel]->sound_sample.SecondsToSamples (time_in_seconds);

  int    queued_buffers;
  
  impl->context.alGetSourcei (impl->sources[channel]->name, AL_BUFFERS_QUEUED, &queued_buffers);

  impl->context.alSourceUnqueueBuffers (impl->sources[channel]->name, queued_buffers, impl->sources[channel]->buffer_name);

  UpdateSourceBuffer (impl->sources[channel], impl->log_handler, &impl->context);

  impl->sources[channel]->play_start_time = clock ();
  impl->sources[channel]->play_from_start = false;
}

float OpenALDevice::Tell (size_t channel)
{
  if (channel >= impl->info.channels_count)
    RaiseOutOfRange ("OpenALDevice::Tell", "channel", channel, impl->info.channels_count);
    
  float  offset;

  offset = ((float)(clock () - impl->sources[channel]->play_start_time)) / CLOCKS_PER_SEC + (float)impl->sources[channel]->sound_sample.SamplesToSeconds (impl->sources[channel]->start_sample);
  offset = fmod (offset, (float)impl->sources[channel]->sound_sample.SamplesToSeconds (impl->sources[channel]->sound_sample.SamplesCount ()));

  return offset;
}

bool OpenALDevice::IsPlaying (size_t channel)
{
  if (channel >= impl->info.channels_count)
    RaiseOutOfRange ("OpenALDevice::IsPlaying", "channel", channel, impl->info.channels_count);
    
  int    status = AL_STOPPED;

  impl->context.alGetSourcei (impl->sources[channel]->name, AL_SOURCE_STATE, &status);

  return (status == AL_PLAYING);
}
    
/*
   Установка уровня громкости для устройства
*/

void  OpenALDevice::SetVolume (float gain)
{
  if (gain < 0.f || gain > 1.f)
  {
    impl->log_handler (format ("Incorrect gain value received (%f). Gain not changed", gain).c_str ());
    return;
  }

  impl->gain = gain;

  impl->context.alListenerf (AL_GAIN, gain);
}

float OpenALDevice::GetVolume ()
{
  return impl->gain;
}

/*
   Блокировка проигрывания звука
*/

void OpenALDevice::SetMute (bool state)
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

bool OpenALDevice::IsMuted ()
{
  return impl->is_muted;
}

/*
   Установка параметров слушателя
*/

void OpenALDevice::SetListener (const Listener& source_listener)
{
  impl->listener = source_listener;
  float  orientation [6] = {source_listener.direction.x, source_listener.direction.y, source_listener.direction.z, 
                            source_listener.up.x,        source_listener.up.y,        source_listener.up.z};

  impl->context.alListenerfv (AL_POSITION, source_listener.position);
  impl->context.alListenerfv (AL_VELOCITY, source_listener.velocity);
  impl->context.alListenerfv (AL_ORIENTATION, orientation);
}

void OpenALDevice::GetListener (Listener& target_listener)
{
  target_listener = impl->listener;
}

/*
   Установка функции отладочного протоколирования
*/

void OpenALDevice::SetDebugLog (const LogHandler& log_handler)
{
  impl->log_handler = log_handler;
}

const OpenALDevice::LogHandler& OpenALDevice::GetDebugLog ()
{
  return impl->log_handler;
}

void OpenALDevice::UpdateBuffers ()
{
  for (vector <OpenALSource*>::iterator i = impl->sources.begin (); i != impl->sources.end (); ++i)
    UpdateSourceBuffer (*i, impl->log_handler, &impl->context);
}

void OpenALDevice::AddRef ()
{
  impl->ref_count++;
}

void OpenALDevice::Release ()
{
  if (!--impl->ref_count)
    delete this;
}
