#include "shared.h"

using namespace sound::low_level;
using namespace common;

//период обновлени€ буферов источника в тиках
const size_t SOURCE_BUFFERS_UPDATE_TICKS = size_t (SOURCE_BUFFERS_UPDATE_PERIOD * CLOCKS_PER_SEC);

/*
     онструктор / деструктор
*/

OpenALSource::OpenALSource (OpenALDevice& in_device)
  : device (in_device), source_need_update (true), sample_need_update (false), is_looped (false), is_playing (false),
    play_time_offset (0), last_buffers_fill_time (clock () - SOURCE_BUFFERS_UPDATE_TICKS)
{
  alGetError   ();
  alGenSources (1, &al_source);  
  
  if (alGetError () != AL_NO_ERROR)
    throw OpenALGenSourceException ();

  try
  {
    device.Context ().alGenBuffers (SOURCE_BUFFERS_COUNT, al_buffers);
  }
  catch (...)
  {
    device.Context ().alDeleteSources (1, &al_source);
    throw;
  }
}

OpenALSource::~OpenALSource ()
{ 
  try
  {
    device.Context ().alDeleteSources (1, &al_source);
  }
  catch (...)
  {
    //подавал€ем все исключени€
  }
  
  try
  {
    device.Context ().alDeleteBuffers (SOURCE_BUFFERS_COUNT, al_buffers);
  }
  catch (...)
  {
    //подавл€ем все исключени€
  }
}

/*
    ќповещени€ об обновлени€х
*/

void OpenALSource::UpdateSourceNotify ()
{
  source_need_update = true;

  Update ();
}

void OpenALSource::UpdateSampleNotify ()
{
  sample_need_update = true;

  Update ();
}

/*
   ”становка параметров источника
*/

void OpenALSource::SetSource (const Source& in_source)
{   
  source = in_source;
  
  UpdateSourceNotify ();
}

/*
   ”становка текущего проигрываемого звука
*/

void OpenALSource::SetSample (const char* sample_name)
{   
  if (!sample_name)
    RaiseNullArgument ("sound::low_level::OpenALSource::SetSample", "sample_name");

  try
  {
    SoundSample new_sample (sample_name);

    if (new_sample.BitsPerSample () != 16)
      RaiseNotSupported ("sound::low_level::OpenALSource::SetSample", "Sound sample '%s' has unsupported bits_per_sample=%u",
                         new_sample.Name (), new_sample.BitsPerSample ());

    switch (new_sample.Channels ())
    {
      case 1:
      case 2:
        break;
      default:
        RaiseNotSupported ("sound::low_level::OpenALSource::SetSample", "Sound sample '%s' has unsupported channels_count=%u", new_sample.Name (), new_sample.Channels ());
        break;
    }
    
    sound_sample.Swap (new_sample);
    
    UpdateSampleNotify ();
  }
  catch (std::exception& exception)
  {                                               
    device.DebugPrintf ("Exception at load sample '%s': %s", sample_name, exception.what ());
    Stop ();
  }                                               
  catch (...)
  {
    device.DebugPrintf ("Unknown exception at load sample '%s'", sample_name);
    Stop ();
  }
}

const char* OpenALSource::GetSample () const
{   
  return sound_sample.Name ();
}

/*
    ѕолучение позиции в тиках
*/

size_t OpenALSource::TellInTicks () const
{
  return is_playing ? clock () - play_time_start + play_time_offset : play_time_offset;
}

/*
   ”правление проигрыванием
*/

void OpenALSource::Play (bool looping)
{
  if (is_playing)
    play_time_offset = 0;

  is_looped       = looping;
  is_playing      = true;
  play_time_start = clock ();

  UpdateSampleNotify ();
}

void OpenALSource::Pause ()
{   
  if (!is_playing)
    return;

  is_playing       = false;
  play_time_offset = TellInTicks ();

  UpdateSampleNotify ();
}

void OpenALSource::Stop ()
{   
  is_playing       = false;
  is_looped        = false;
  play_time_offset = 0;

  UpdateSampleNotify ();
}

/*
    ѕозиционирование
*/

float OpenALSource::Tell () const
{
  float offset   = float (TellInTicks ()) / CLOCKS_PER_SEC,
        duration = (float)sound_sample.Duration ();
  
  if (is_looped) return fmod (offset, duration);
  else           return offset < duration ? offset : 0.0f;
}

void OpenALSource::Seek (float offset)
{
  float duration = (float)sound_sample.Duration ();

  if (offset < 0)        offset = 0.0f;
  if (offset > duration) offset = duration;

  play_time_start  = clock ();
  play_time_offset = size_t (offset * CLOCKS_PER_SEC);

  UpdateSourceNotify ();
}

bool OpenALSource::IsPlaying () const
{
  if (!is_playing)
    return false;

  if (is_looped)
    return true;

  float offset   = float (TellInTicks ()) / CLOCKS_PER_SEC,
        duration = (float)sound_sample.Duration ();

  return offset < duration;
}

/*
    „тение данных в OpenAL буфер с учЄтом цикличности проигрывани€
*/

void OpenALSource::FillBuffer (size_t al_buffer)
{
  size_t max_samples_count       = sound_sample.BytesToSamples (device.GetSampleBufferSize ()),
         available_samples_count = max_samples_count;
  char*  buffer                  = (char*)device.GetSampleBuffer ();

  if (is_looped)
  {   
    while (available_samples_count)
    {    
      if (play_sample_position == sound_sample.SamplesCount ())
        play_sample_position = 0;

      size_t samples_count = sound_sample.Read (play_sample_position, available_samples_count, buffer);

      if (!samples_count)
        break;
      
      available_samples_count -= samples_count;
      buffer                  += sound_sample.SamplesToBytes (samples_count);
      play_sample_position    += samples_count;
    }    
  }
  else
  {
    size_t samples_count     = sound_sample.Read (play_sample_position, available_samples_count, buffer);    
    available_samples_count -= samples_count;
    play_sample_position    += samples_count;
  }      
  
  size_t readed_samples_count = max_samples_count - available_samples_count;
  ALenum format               = sound_sample.Channels () == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
  
  OpenALContext& context = device.Context ();  
  
//  printf ("fill (%p, %u)\n", al_buffer, sound_sample.SamplesToBytes (readed_samples_count));

  context.alBufferData (al_buffer, format, device.GetSampleBuffer (), sound_sample.SamplesToBytes (readed_samples_count),
                        sound_sample.Frequency ());
  context.alSourceQueueBuffers (al_source, 1, &al_buffer);
}

void OpenALSource::FillBuffers ()
{
  if (size_t (clock () - last_buffers_fill_time) < SOURCE_BUFFERS_UPDATE_TICKS)
    return;

  ALint          queued_buffers_count = 0, processed_buffers_count = 0;
  OpenALContext& context              = device.Context ();

  context.alGetSourcei (al_source, AL_BUFFERS_QUEUED, &queued_buffers_count);
  context.alGetSourcei (al_source, AL_BUFFERS_PROCESSED, &processed_buffers_count);
  
    printf ("queued=%u processed=%u\n", queued_buffers_count, processed_buffers_count);  

  if (!queued_buffers_count)
  {  
      //первоначальное заполнение буферов      

    for (size_t i=0; i<SOURCE_BUFFERS_COUNT; i++)
      FillBuffer (al_buffers [i]);
      
    printf ("!queued=%u processed=%u\n", queued_buffers_count, processed_buffers_count);        
  }
  else if (processed_buffers_count)
  {
    ALuint buffers [SOURCE_BUFFERS_COUNT];   

    context.alSourceUnqueueBuffers (al_source, processed_buffers_count, buffers);    

    for (int i=0; i<processed_buffers_count; i++)
      FillBuffer (buffers [i]);
  }

  last_buffers_fill_time = clock ();    
}

/*
    ќбновление источника
*/

void OpenALSource::Update ()
{
  if (!is_playing && !sample_need_update)
    return;    

  try
  {
    OpenALContext& context = device.Context ();
    
      //обновление параметров источника
    
    if (source_need_update)
    {
      source_need_update = false;

      context.alSourcefv (al_source, AL_POSITION, source.position);
      context.alSourcefv (al_source, AL_DIRECTION, source.direction);
      context.alSourcefv (al_source, AL_VELOCITY, source.velocity);
      context.alSourcef  (al_source, AL_GAIN, source.gain);
      context.alSourcef  (al_source, AL_MIN_GAIN, source.minimum_gain);
      context.alSourcef  (al_source, AL_MAX_GAIN, source.maximum_gain);
      context.alSourcef  (al_source, AL_CONE_INNER_ANGLE, source.inner_angle);
      context.alSourcef  (al_source, AL_CONE_OUTER_ANGLE, source.outer_angle);
      context.alSourcef  (al_source, AL_CONE_OUTER_GAIN, source.outer_gain);
      context.alSourcef  (al_source, AL_REFERENCE_DISTANCE, source.reference_distance);
    }

      //определение состо€ни€ проигрывани€

    bool playing = IsPlaying ();
    int  status  = AL_STOPPED;

    context.alGetSourcei (al_source, AL_SOURCE_STATE, &status);

      //возобновление прекращенного проигрывани€
      
    if (status != AL_PLAYING)
    {
      if (playing) sample_need_update = true;
      else         is_playing         = false;
    }

      //обновление буферов      

    if (sample_need_update)
    {     
      sample_need_update = false;      
      
        //останавливаем проигрывание

      context.alSourceStop (al_source);

        //очищаем очередь проигрываемых буферов

      ALint  queued_buffers_count = 0;
      ALuint queued_buffers [SOURCE_BUFFERS_COUNT];

      context.alGetSourcei (al_source, AL_BUFFERS_QUEUED, &queued_buffers_count);

      if (queued_buffers_count)
        context.alSourceUnqueueBuffers (al_source, queued_buffers_count, queued_buffers);

      play_sample_position = sound_sample.SecondsToSamples (Tell ());      

      if (playing)
      {
        FillBuffers  ();
        alSourcePlay (al_source);
      }
      else alSourceStop (al_source);      
    }
    else FillBuffers ();
  }
  catch (std::exception& exception)
  {
    device.DebugPrintf ("Exception at update source: %s", exception.what ());
  }
  catch (...)
  {
  }
}
