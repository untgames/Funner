#include <common/strlib.h>

#include "shared.h"

using namespace sound::low_level;
using namespace common;

namespace
{

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
      source->last_sample += readed_samples = source->sound_sample.Read (source->last_sample, source->buffer_samples - total_readed_samples, source->buffer + source->sound_sample.SamplesToBytes (total_readed_samples));
    
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
    source->last_sample += readed_samples = source->sound_sample.Read (source->last_sample, source->buffer_samples, source->buffer);
    if (!readed_samples)
      return false;
  }

  if (source->sound_sample.Channels () == 1)
    alBufferData (source->buffer_name[buffer_index], AL_FORMAT_MONO16, source->buffer, readed_samples * sample_size, source->sound_sample.Frequency ());
  else
    alBufferData (source->buffer_name[buffer_index], AL_FORMAT_STEREO16, source->buffer, readed_samples * sample_size, source->sound_sample.Frequency ());

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
  int    processed_buffers = 0;
  
  context->alGetSourcei (source->name, AL_BUFFERS_QUEUED, &processed_buffers);

  if (!processed_buffers)
  {
    if (FillSourceBuffer (source, 0, log_handler))
    {
      context->alSourceQueueBuffers (source->name, 1, &source->buffer_name[0]);
      if (FillSourceBuffer (source, 1, log_handler))
        context->alSourceQueueBuffers (source->name, 1, &source->buffer_name[1]);
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

OpenALSource::OpenALSource (OpenALContext* in_context, char* in_buffer)
  : play_from_start (true), context (in_context), buffer (in_buffer), playing (false), looping (false)
{
  alGenSources (1, &name);
  if (alGetError () != AL_NO_ERROR)
    Raise <Exception> ("OpenALSource::OpenALSource", "Can't create source");
  alGenBuffers(2, buffer_name);
  if (alGetError () != AL_NO_ERROR)
    Raise <Exception> ("OpenALSource::OpenALSource", "Can't create buffer");
}

OpenALSource::~OpenALSource ()
{
  context->alSourceStop (name);

  int queued_buffers = 0;
  
  context->alGetSourcei (name, AL_BUFFERS_QUEUED, &queued_buffers);
  if (queued_buffers)
    context->alSourceUnqueueBuffers (name, queued_buffers, buffer_name);

  if (buffer_name)
    context->alDeleteBuffers (2, buffer_name);
  context->alDeleteSources (1, &name);
}

/*
   ”становка текущего проигрываемого звука
*/

void OpenALDevice::SetSample (size_t channel, const char* sample_name)
{
  if (channel >= info.channels_count)
    RaiseOutOfRange ("OpenALDevice::SetSample", "channel", channel, info.channels_count);
    
  if (!sample_name)
    RaiseNullArgument ("OpenALDevice::SetSample", "sample_name");
    
  bool played = IsPlaying (channel), looped = IsLooped (channel);

  Stop (channel);

  try
  {
    sources[channel]->sound_sample.Load (sample_name);
    
    if (sources[channel]->sound_sample.BitsPerSample () != 16)
      RaiseNotSupported ("OpenALDevice::SetSample", "Supported only 16 bit audio depth, sample depth - %u.", sources[channel]->sound_sample.BitsPerSample ());
    if (sources[channel]->sound_sample.Channels () > 2 || !sources[channel]->sound_sample.Channels ())
      RaiseNotSupported ("OpenALDevice::SetSample", "Supported only mono and stereo sources, sample has %u channels.", sources[channel]->sound_sample.Channels ());

    sources[channel]->buffer_samples = (size_t)(BUFFER_TIME * sources[channel]->sound_sample.Frequency ());
  }
  catch (std::exception& exception)
  {                                               
    log_handler (format ("Can't load file '%s'. Exception: '%s'.", sample_name, exception.what ()).c_str());
    return;
  }                                               
  catch (...)
  {
    log_handler (format ("Can't load file '%s'. Unknown exception.", sample_name).c_str());
    return;
  }

  if (played)
    Play (channel, looped);
}

const char* OpenALDevice::GetSample (size_t channel)
{
  if (channel >= info.channels_count)
    RaiseOutOfRange ("OpenALDevice::GetSample", "channel", channel, info.channels_count);
    
  return sources[channel]->sound_sample.Name ();
}

/*
   ѕроверка цикличности проигрывани€ канала
*/

bool OpenALDevice::IsLooped (size_t channel)
{
  if (channel >= info.channels_count)
    RaiseOutOfRange ("OpenALDevice::IsLooped", "channel", channel, info.channels_count);
    
  return sources[channel]->looping; 
}
    
/*
   ”становка параметров источника
*/

void OpenALDevice::SetSource (size_t channel, const Source& source_source)
{
  if (channel >= info.channels_count)
    RaiseOutOfRange ("OpenALDevice::SetSource", "channel", channel, info.channels_count);
    
  sources[channel]->source = source_source;

  context->alSourcefv (sources[channel]->name, AL_POSITION, source_source.position);
  context->alSourcefv (sources[channel]->name, AL_DIRECTION, source_source.direction);
  context->alSourcefv (sources[channel]->name, AL_VELOCITY, source_source.velocity);
  context->alSourcef  (sources[channel]->name, AL_GAIN, source_source.gain);
  context->alSourcef  (sources[channel]->name, AL_MIN_GAIN, source_source.minimum_gain);
  context->alSourcef  (sources[channel]->name, AL_MAX_GAIN, source_source.maximum_gain);
  context->alSourcef  (sources[channel]->name, AL_CONE_INNER_ANGLE, source_source.inner_angle);
  context->alSourcef  (sources[channel]->name, AL_CONE_OUTER_ANGLE, source_source.outer_angle);
  context->alSourcef  (sources[channel]->name, AL_CONE_OUTER_GAIN, source_source.outer_gain);
  context->alSourcef  (sources[channel]->name, AL_REFERENCE_DISTANCE, source_source.reference_distance);
}

void OpenALDevice::GetSource (size_t channel, Source& target_source)
{
  if (channel >= info.channels_count)
    RaiseOutOfRange ("OpenALDevice::GetSource", "channel", channel, info.channels_count);
    
  target_source = sources[channel]->source;
}

/*
   ”правление проигрыванием
*/

void OpenALDevice::Play (size_t channel, bool looping)
{
  if (channel >= info.channels_count)
    RaiseOutOfRange ("OpenALDevice::Play", "channel", channel, info.channels_count);
    
  sources[channel]->looping = looping;
  sources[channel]->playing = true;

  if (sources[channel]->play_from_start)
    Seek (channel, 0);

  sources[channel]->play_from_start = true;
  sources[channel]->play_start_time = clock ();
  context->alSourcePlay (sources[channel]->name);
}

void OpenALDevice::Pause (size_t channel)
{
  if (channel >= info.channels_count)
    RaiseOutOfRange ("OpenALDevice::Pause", "channel", channel, info.channels_count);
    
  sources[channel]->playing = false;

  context->alSourcePause (sources[channel]->name);
}

void OpenALDevice::Stop (size_t channel)
{
  if (channel >= info.channels_count)
    RaiseOutOfRange ("OpenALDevice::Stop", "channel", channel, info.channels_count);
    
  sources[channel]->play_from_start = true;
  sources[channel]->playing = false;
  sources[channel]->looping = false;

  context->alSourceStop (sources[channel]->name);
}

void OpenALDevice::Seek (size_t channel, float time_in_seconds)
{
  if (channel >= info.channels_count)
    RaiseOutOfRange ("OpenALDevice::Seek", "channel", channel, info.channels_count);

  if (time_in_seconds >= sources[channel]->sound_sample.SamplesToSeconds (sources[channel]->sound_sample.SamplesCount ()))
  {
    log_handler ("Attempting to seek more than sample range. Seeking to begin.");
    time_in_seconds = 0;
  }

  sources[channel]->last_sample = sources[channel]->start_sample = sources[channel]->sound_sample.SecondsToSamples (time_in_seconds);

  int    queued_buffers;
  
  context->alGetSourcei (sources[channel]->name, AL_BUFFERS_QUEUED, &queued_buffers);

  context->alSourceUnqueueBuffers (sources[channel]->name, queued_buffers, sources[channel]->buffer_name);

  UpdateSourceBuffer (sources[channel], log_handler, context);

  sources[channel]->play_start_time = clock ();
  sources[channel]->play_from_start = false;
}

float OpenALDevice::Tell (size_t channel)
{
  if (channel >= info.channels_count)
    RaiseOutOfRange ("OpenALDevice::Tell", "channel", channel, info.channels_count);
    
  float  offset;

  offset = ((float)(clock () - sources[channel]->play_start_time)) / CLOCKS_PER_SEC + (float)sources[channel]->sound_sample.SamplesToSeconds (sources[channel]->start_sample);
  offset = fmod (offset, (float)sources[channel]->sound_sample.SamplesToSeconds (sources[channel]->sound_sample.SamplesCount ()));

  return offset;
}

bool OpenALDevice::IsPlaying (size_t channel)
{
  if (channel >= info.channels_count)
    RaiseOutOfRange ("OpenALDevice::IsPlaying", "channel", channel, info.channels_count);
    
  if (sources[channel]->playing)
  {
    int status = AL_STOPPED;

    context->alGetSourcei (sources[channel]->name, AL_SOURCE_STATE, &status);

    if (status == AL_PLAYING)
      return true;
    else if (Tell (channel) > sources[channel]->sound_sample.SamplesToSeconds (sources[channel]->sound_sample.SamplesCount ()))
    {
      sources[channel]->playing = false;
      return false;
    }
    else
    {
      sources[channel]->play_from_start = false;
      Play (channel, sources[channel]->looping);
      return true;
    }
  }
  else 
    return false;
}

void OpenALDevice::UpdateBuffers ()
{
  for (size_t i = 0; i < sources.size (); i++)
  {
    if (sources[i]->playing)
    {
      UpdateSourceBuffer (sources[i], log_handler, context);
      IsPlaying (i);
    }
  }
}
