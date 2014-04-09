#include "shared.h"

using namespace sound::low_level;
using namespace sound::low_level::openal;
using namespace common;

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

namespace
{

const char*  LOG_NAME                           = "sound::low_level::openal";                               //им€ потока протоколировани€
const size_t SOURCE_BUFFERS_UPDATE_MILLISECONDS = size_t (1000.f / (float)SOURCE_BUFFERS_UPDATE_FREQUENCY); //период обновлени€ буферов источника в тиках

}

/*
     онструктор / деструктор
*/

OpenALSource::OpenALSource (OpenALDevice& in_device)
  : device (in_device),
    source_need_update (true),
    sample_need_update (false),
    is_looped (false),
    is_playing (false),
    is_active (false),
    play_time_offset (0),
    last_buffers_fill_time (milliseconds () - SOURCE_BUFFERS_UPDATE_MILLISECONDS),
    prev_active (0),
    next_active (0),
    log (LOG_NAME)
{
  alGetError ();
  device.Context ().alGenSources (1, &al_source);

  if (alGetError () != AL_NO_ERROR)
    throw xtl::format_exception<OpenALException> ("sound::low_level::OpenALSource::OpenALSource", "No enough sources");
}

OpenALSource::~OpenALSource ()
{
  Deactivate ();

  try
  {
    OpenALContextManager::Instance context_manager;

    context_manager->SetCurrentContext (device.Context ());

    device.Context ().alDeleteSources (1, &al_source);
  }
  catch (...)
  {
    //подавал€ем все исключени€
  }
}

/*
    –абота со списком активных источников
*/

void OpenALSource::Activate ()
{
  if (is_active)
    return;

  for (size_t i=0; i<SOURCE_BUFFERS_COUNT; i++)
  {
    try
    {
      al_buffers [i] = device.AllocateSourceBuffer ();
    }
    catch (...)
    {
      for (size_t j=0; j<i; j++)
        device.DeallocateSourceBuffer (al_buffers [j]);

      throw;
    }
  }

  OpenALSource* first = device.GetFirstActiveSource ();

  next_active = first;

  if (next_active) next_active->prev_active = this;

  device.SetFirstActiveSource (this);

  is_active = true;
}

void OpenALSource::Deactivate ()
{
  if (!is_active)
    return;

  OpenALContext& context = device.Context ();

  OpenALContextManager::Instance context_manager;

  context_manager->SetCurrentContext (context);

  context.alSourceStop (al_source);

  context.alSourcei (al_source, AL_BUFFER, 0); //detach all buffers

  for (size_t i=0; i<SOURCE_BUFFERS_COUNT; i++)
    device.DeallocateSourceBuffer (al_buffers [i]);

  if (next_active) next_active->prev_active = prev_active;
  if (prev_active) prev_active->next_active = next_active;
  else             device.SetFirstActiveSource (next_active);

  prev_active = 0;
  is_active   = false;
}

/*
    ќповещени€ об обновлени€х
*/

void OpenALSource::UpdateSourceNotify ()
{
  source_need_update = true;

  PropertiesUpdate ();
}

void OpenALSource::UpdateSampleNotify ()
{
  sample_need_update = true;

  BufferUpdate ();
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

void OpenALSource::SetSample (OpenALSample* sample)
{
  static const char* METHOD_NAME = "sound::low_level::OpenALSource::SetSample";

  if (sample == sound_sample)
    return;

  if (!sample)
  {
    Stop ();

    sound_sample_decoder = 0;
    sound_sample         = 0;
  }
  else
  {
    SampleDesc sample_desc;

    sample->GetDesc (sample_desc);

    if (!sample_desc.samples_count)
      return;

    try
    {
      if (sample_desc.frequency > MAX_SOUND_SAMPLE_RATE)
        throw xtl::format_not_supported_exception (METHOD_NAME, "Sound sample '%s' has unsupported sample rate=%u. Maximum supported sample rate is %u",
                           sample->GetName (), sample_desc.frequency, MAX_SOUND_SAMPLE_RATE);

      if (sample_desc.bits_per_sample != 16)
        throw xtl::format_not_supported_exception (METHOD_NAME, "Sound sample '%s' has unsupported bits_per_sample=%u",
                           sample->GetName (), sample_desc.bits_per_sample);

      switch (sample_desc.channels)
      {
        case 1:
        case 2:
          break;
        default:
          throw xtl::format_not_supported_exception (METHOD_NAME, "Sound sample '%s' has unsupported channels_count=%u", sample->GetName (), sample_desc.channels);
          break;
      }

      sound_sample_decoder = SampleDecoderPtr (sample->CreateDecoder (), false);
      sound_sample         = sample;

      UpdateSampleNotify ();
    }
    catch (std::exception&)
    {
      Stop ();
      throw;
    }
    catch (...)
    {
      Stop ();
      throw;
    }
  }
}

OpenALSample* OpenALSource::GetSample () const
{
  return sound_sample.get ();
}

/*
    ѕолучение позиции в миллисекундах
*/

size_t OpenALSource::TellInMilliseconds () const
{
  return is_playing ? milliseconds () - play_time_start + play_time_offset : play_time_offset;
}

/*
   ”правление проигрыванием
*/

void OpenALSource::Play (bool looping)
{
  if (!sound_sample || !sound_sample->SamplesCount ())
  {
    log.Printf ("Warning: can't play, empty sound sample '%s'", sound_sample->GetName ());
    return;
  }

  if (is_playing)
    play_time_offset = 0;

  is_looped       = looping;
  is_playing      = true;
  play_time_start = milliseconds ();

  UpdateSourceNotify ();
  UpdateSampleNotify ();
}

void OpenALSource::Pause ()
{
  if (!is_playing)
    return;

  is_playing       = false;
  play_time_offset = TellInMilliseconds ();

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
  if (!sound_sample || !sound_sample->SamplesCount ())
    throw xtl::format_operation_exception ("sound::low_level::openal::OpenALSource::Tell", "Empty sound sample");

  float offset   = float (TellInMilliseconds ()) / 1000.f,
        duration = (float)sound_sample->Duration ();

  if (is_looped) return fmod (offset, duration);
  else           return offset < duration ? offset : 0.0f;
}

void OpenALSource::Seek (float offset, SeekMode seek_mode)
{
  static const char* METHOD_NAME = "sound::low_level::openal::OpenALSource::Seek";

  if (!sound_sample || !sound_sample->SamplesCount ())
    throw xtl::format_operation_exception (METHOD_NAME, "Empty sound sample");

  float duration = (float)sound_sample->Duration ();

  if (offset < 0) offset = 0.0f;

  if (offset > duration)
    switch (seek_mode)
    {
      case SeekMode_Clamp:
        offset = duration;
        break;
      case SeekMode_Repeat:
        offset = fmod (offset, duration);
        break;
      default:
        throw xtl::make_argument_exception (METHOD_NAME, "seek_mode", seek_mode);
    }

  play_time_start  = milliseconds ();
  play_time_offset = size_t (offset * 1000.f);

  UpdateSampleNotify ();
}

bool OpenALSource::IsPlaying () const
{
  if (!is_playing)
    return false;

  if (is_looped)
    return true;

  float offset   = float (TellInMilliseconds ()) / 1000.f,
        duration = (float)sound_sample->Duration ();

  return offset < duration;
}

/*
    „тение данных в OpenAL буфер с учЄтом цикличности проигрывани€
*/

void OpenALSource::FillBuffer (ALuint al_buffer)
{
  if (!sound_sample)
    throw xtl::format_operation_exception ("sound::low_level::openal::OpenALSource::FillBuffer", "Can't fill buffer, empty sound sample");

  size_t max_samples_count       = sound_sample->BytesToSamples (device.GetSampleBufferSize ()),
         available_samples_count = max_samples_count;
  char*  buffer                  = (char*)device.GetSampleBuffer ();

  if (is_looped)
  {
    while (available_samples_count)
    {
      if (play_sample_position == sound_sample->SamplesCount ())
        play_sample_position = 0;

      size_t samples_count = sound_sample_decoder->Read (play_sample_position, available_samples_count, buffer);

      if (!samples_count)
        break;

      available_samples_count -= samples_count;
      buffer                  += sound_sample->SamplesToBytes (samples_count);
      play_sample_position    += samples_count;
    }
  }
  else
  {
#ifdef __APPLE__  //Mac OSX and iOS skips some samples at the beginning of each sound
    const size_t NULL_SAMPLES = 150 * sound_sample->Channels ();

    size_t null_samples      = stl::min ((int)available_samples_count, stl::max (0, (int)NULL_SAMPLES - (int)play_sample_position));
    size_t null_samples_size = sound_sample->SamplesToBytes (null_samples);

    if (null_samples)
      memset (buffer, 0, null_samples_size);

    size_t samples_count = sound_sample_decoder->Read (stl::max (0, (int)play_sample_position - (int)NULL_SAMPLES), stl::max (0, (int)available_samples_count - (int)null_samples), buffer + null_samples_size);

    available_samples_count -= samples_count + null_samples;
    play_sample_position    += samples_count + null_samples;
#else
    size_t samples_count     = sound_sample_decoder->Read (play_sample_position, available_samples_count, buffer);
    available_samples_count -= samples_count;
    play_sample_position    += samples_count;
#endif
  }

  size_t readed_samples_count = max_samples_count - available_samples_count;

  if (!readed_samples_count)
    return;

  ALenum format = sound_sample->Channels () == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

  OpenALContext& context = device.Context ();

  OpenALContextManager::Instance context_manager;

  context_manager->SetCurrentContext (context);

  context.alBufferData (al_buffer, format, device.GetSampleBuffer (), sound_sample->SamplesToBytes (readed_samples_count),
                        sound_sample->Frequency ());
  context.alSourceQueueBuffers (al_source, 1, &al_buffer);
}

void OpenALSource::FillBuffers ()
{
  if (size_t (milliseconds () - last_buffers_fill_time) < SOURCE_BUFFERS_UPDATE_MILLISECONDS)
    return;

  ALint          queued_buffers_count = 0, processed_buffers_count = 0;
  OpenALContext& context              = device.Context ();

  OpenALContextManager::Instance context_manager;

  context_manager->SetCurrentContext (context);

  context.alGetSourcei (al_source, AL_BUFFERS_QUEUED, &queued_buffers_count);
  context.alGetSourcei (al_source, AL_BUFFERS_PROCESSED, &processed_buffers_count);

  if (!queued_buffers_count)
  {
      //первоначальное заполнение буферов

    for (size_t i=0; i<SOURCE_BUFFERS_COUNT; i++)
      FillBuffer (al_buffers [i]);
  }
  else if (processed_buffers_count)
  {
    ALuint buffers [SOURCE_BUFFERS_COUNT];

    context.alSourceUnqueueBuffers (al_source, processed_buffers_count, buffers);

    for (int i=0; i<processed_buffers_count; i++)
      FillBuffer (buffers [i]);
  }

  last_buffers_fill_time = milliseconds ();
}

/*
    ќбновление источника
*/

void OpenALSource::BufferUpdate ()
{
  if (!is_active && !sample_need_update)
    return;

  try
  {
    OpenALContext& context = device.Context ();

      //определение состо€ни€ проигрывани€

    int status = AL_STOPPED;

    OpenALContextManager::Instance context_manager;

    context_manager->SetCurrentContext (context);

    context.alGetSourcei (al_source, AL_SOURCE_STATE, &status);

      //возобновление прекращенного проигрывани€ / установка флага, сигнализирующего о конце проигрывани€

    if (status != AL_PLAYING)
    {
      if (IsPlaying ()) sample_need_update = true;
      else              is_playing         = false;
    }

      //добавление / удаление источника в список активных, заказ буферов

    if (is_playing != is_active)
    {
      if (is_playing)
        Activate ();
      else                                      //проигрывание завершено
      {
        Deactivate ();

        return;
      }
    }

      //обновление буферов

    if (sample_need_update)
    {
      sample_need_update = false;

        //останавливаем проигрывание

      context.alSourceStop (al_source);

      context.alSourcei (al_source, AL_BUFFER, 0);  //detach all buffers

      play_sample_position = sound_sample ? sound_sample->SecondsToSamples (Tell ()) : 0;

      if (is_playing)
      {
        FillBuffers  ();
        context.alSourcePlay (al_source);
      }
    }
    else
      FillBuffers ();
  }
  catch (std::exception& exception)
  {
    log.Printf ("Exception at update source: %s", exception.what ());
  }
  catch (...)
  {
  }
}

//обновление параметров источника
void OpenALSource::PropertiesUpdate ()
{
  if (!is_playing || !source_need_update)
    return;

  try
  {
    OpenALContext& context = device.Context ();

    OpenALContextManager::Instance context_manager;

    context_manager->SetCurrentContext (context);

    context.alSourcefv (al_source, AL_POSITION, &source.position [0]);
    context.alSourcefv (al_source, AL_DIRECTION, &source.direction [0]);
    context.alSourcefv (al_source, AL_VELOCITY, &source.velocity [0]);
    context.alSourcef  (al_source, AL_GAIN, source.gain);
    context.alSourcef  (al_source, AL_MIN_GAIN, source.minimum_gain);
    context.alSourcef  (al_source, AL_MAX_GAIN, source.maximum_gain);
    context.alSourcef  (al_source, AL_CONE_INNER_ANGLE, source.inner_angle);
    context.alSourcef  (al_source, AL_CONE_OUTER_ANGLE, source.outer_angle);
    context.alSourcef  (al_source, AL_CONE_OUTER_GAIN, source.outer_gain);
    context.alSourcef  (al_source, AL_REFERENCE_DISTANCE, source.reference_distance);
    context.alSourcef  (al_source, AL_MAX_DISTANCE, source.maximum_distance);

    source_need_update = false;
  }
  catch (std::exception& exception)
  {
    log.Printf ("Exception at update source properties: %s", exception.what ());
  }
  catch (...)
  {
  }
}
