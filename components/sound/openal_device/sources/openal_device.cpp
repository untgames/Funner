#include "shared.h"

#include <time.h>

using namespace common;
using namespace stl;
using namespace xtl;
using namespace sound;
using namespace sound::low_level;
using namespace sound::low_level::openal;

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
///Константы
///////////////////////////////////////////////////////////////////////////////////////////////////
const size_t MAX_DEVICE_CHANNELS_COUNT  = 1024;   //максимальное количество каналов проигрывания
const size_t MAX_SOUND_CHANNELS         = 2;      //максимальное количество каналов
const size_t MAX_SOUND_BYTES_PER_SAMPLE = 2;      //максимальная разрядность звука

//время обновления буферов источника в миллисекундах
const size_t SOURCE_BUFFERS_UPDATE_MILLISECONDS = size_t (1000.f / (float)SOURCE_BUFFERS_UPDATE_FREQUENCY);

const float  DEFAULT_SOURCE_PROPERTIES_UPDATE_PERIOD = 0.03f;
const float  DEFAULT_LISTENER_PROPERTIES_UPDATE_PERIOD = 0.03f;

const char*  DEVICE_PARAMS_NAMES        = "al_debug_log buffer_update_frequency AL_DISTANCE_MODEL listener_update_frequency source_update_frequency";
const char*  DEVICE_INT_PARAMS_NAMES    = "al_debug_log buffer_update_frequency listener_update_frequency source_update_frequency";
const char*  DEVICE_STRING_PARAMS_NAMES = "AL_DISTANCE_MODEL";

namespace
{

struct OpenALDeviceProperties
{
  size_t min_channels_count;
  size_t max_channels_count;

  OpenALDeviceProperties () : min_channels_count (0), max_channels_count (MAX_DEVICE_CHANNELS_COUNT) {}
};

void process_init_string (const char* property, const char* value, OpenALDeviceProperties& properties)
{
  if (!xstricmp (property, "min_channels_count"))
    properties.min_channels_count = atoi (value);
  else if (!xstricmp (property, "max_channels_count"))
    properties.max_channels_count = atoi (value);
}

}

/*
    Конструктор / деструктор
*/

OpenALDevice::OpenALDevice (const char* driver_name, const char* device_name, const char* init_string)
 : context        (device_name, init_string),
   sample_buffer  (MAX_SOUND_SAMPLE_RATE * MAX_SOUND_CHANNELS * MAX_SOUND_BYTES_PER_SAMPLE / SOURCE_BUFFERS_UPDATE_FREQUENCY / (SOURCE_BUFFERS_COUNT - 2))
{
  common::Lock lock (*this);

  listener_need_update                 = false;
  gain                                 = 1.f;
  is_muted                             = false;
  buffer_update_frequency              = SOURCE_BUFFERS_UPDATE_FREQUENCY;
  source_properties_update_frequency   = (size_t)(1.f / DEFAULT_SOURCE_PROPERTIES_UPDATE_PERIOD);
  listener_properties_update_frequency = (size_t)(1.f / DEFAULT_LISTENER_PROPERTIES_UPDATE_PERIOD);
  first_active_source                  = 0;
  al_buffers_pool_size                 = 0;  
  
  try
  {
    buffer_action   = common::ActionQueue::PushAction (xtl::bind (&OpenALDevice::BufferUpdate, this), common::ActionThread_Background, 0, SOURCE_BUFFERS_UPDATE_MILLISECONDS / 1000.0f);
    listener_action = common::ActionQueue::PushAction (xtl::bind (&OpenALDevice::ListenerUpdate, this), common::ActionThread_Current, 0, DEFAULT_LISTENER_PROPERTIES_UPDATE_PERIOD);
    source_action   = common::ActionQueue::PushAction (xtl::bind (&OpenALDevice::SourceUpdate, this), common::ActionThread_Current, 0, DEFAULT_SOURCE_PROPERTIES_UPDATE_PERIOD);
  
    context.MakeCurrent ();
    
      //формирование имени устройства

    name = format ("%s::%s", driver_name ? driver_name : "", device_name ? device_name : "");

    OpenALDeviceProperties properties;

    common::parse_init_string (init_string, xtl::bind (&process_init_string, _1, _2, xtl::ref (properties)));

    channels.reserve (properties.max_channels_count);

      //создание каналов проигрывания

    for (size_t i = 0; i < properties.max_channels_count; i++)
    {
      try
      {
        channels.push_back (new OpenALSource (*this));
      }
      catch (...)
      {
        break;
      }
    }

    if (channels.size () < properties.min_channels_count)
    {
      try
      {
        ClearALData ();
      }
      catch (...)
      {
      }

      throw xtl::format_not_supported_exception ("sound::low_level::OpenALDevice::OpenALDevice", "Device doesn't support %u channels. Only %u channels supported.",
                          properties.min_channels_count, channels.size ());
    }

    info.channels_count = channels.size ();
  }
  catch (...)
  {
    buffer_action.Cancel ();
    listener_action.Cancel ();
    source_action.Cancel ();

    throw;
  }
}

OpenALDevice::~OpenALDevice ()
{
  common::Lock lock (*this);

  try
  {
    ClearALData ();
    
    buffer_action.Cancel ();
    listener_action.Cancel ();
    source_action.Cancel ();    
  }
  catch (...)
  {
  }
}

/*
    Удаление каналов и буферов
*/

void OpenALDevice::ClearALData ()
{
  common::Lock lock (*this);

    //удаление каналов

  for (size_t i = 0; i < channels.size (); i++)
    delete channels [i];

    //очистка пула буферов

  if (al_buffers_pool_size)
  {
    context.MakeCurrent ();

    context.alDeleteBuffers (al_buffers_pool_size, al_buffers_pool);
  }
}

/*
    Подсчёт ссылок
*/

void OpenALDevice::AddRef ()
{
  addref (this);
}

void OpenALDevice::Release ()
{
  release (this);
}

/*
   Имя устройства
*/

const char* OpenALDevice::Name ()
{
  common::Lock lock (*this);

  return name.c_str ();
}

/*
   Получение информации об устройстве
*/

void OpenALDevice::GetCapabilities (Capabilities& target_info)
{
  common::Lock lock (*this);

  target_info = info;
}

/*
   Количество микшируемых каналов
*/

size_t OpenALDevice::ChannelsCount ()
{
  common::Lock lock (*this);

  return channels.size ();
}

/*
    Оповещение о необходимости обновления слушателя
*/

void OpenALDevice::UpdateListenerNotify ()
{
  common::Lock lock (*this);

  listener_need_update = true;

  ListenerUpdate ();
}

/*
   Установка уровня громкости для устройства
*/

void OpenALDevice::SetVolume (float in_gain)
{
  common::Lock lock (*this);

  if (in_gain < 0.0f) in_gain = 0.0f;
  if (in_gain > 1.0f) in_gain = 1.0f;

  gain     = in_gain;
  is_muted = false;

  UpdateListenerNotify ();
}

float OpenALDevice::GetVolume ()
{
  common::Lock lock (*this);

  return gain;
}

/*
   Блокировка проигрывания звука
*/

void OpenALDevice::SetMute (bool state)
{
  common::Lock lock (*this);

  is_muted = state;

  UpdateListenerNotify ();
}

bool OpenALDevice::IsMuted ()
{
  common::Lock lock (*this);

  return is_muted;
}

/*
    Установка параметров слушателя
*/

void OpenALDevice::SetListener (const Listener& in_listener)
{
  common::Lock lock (*this);

  listener = in_listener;

  UpdateListenerNotify ();
}

void OpenALDevice::GetListener (Listener& target_listener)
{
  common::Lock lock (*this);

  target_listener = listener;
}

/*
   Работа со списком активных источников
*/

void OpenALDevice::SetFirstActiveSource (OpenALSource* source)
{
  common::Lock lock (*this);

  first_active_source = source;
}

OpenALSource* OpenALDevice::GetFirstActiveSource () const
{
  common::Lock lock (*this);

  return first_active_source;
}

/*
   OpenAL контекст
*/

const OpenALContext& OpenALDevice::Context () const
{
  return const_cast<OpenALDevice*> (this)->Context ();
}

OpenALContext& OpenALDevice::Context ()
{
  common::Lock lock (*this);

  return context;
}

/*
   Буфер сэмплирования
*/

const void* OpenALDevice::GetSampleBuffer () const
{
  return const_cast<OpenALDevice*> (this)->GetSampleBuffer ();
}

void* OpenALDevice::GetSampleBuffer ()
{
  common::Lock lock (*this);

  return sample_buffer.data ();
}

size_t OpenALDevice::GetSampleBufferSize () const
{
  common::Lock lock (*this);

  return sample_buffer.size ();
}

/*
   Создание сэмпла для проигрывания
*/

ISample* OpenALDevice::CreateSample (const char* name)
{
  common::Lock lock (*this);

  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");

    return new OpenALSample (name);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("sound::low_level::OpenALDevice::CreateSample (const char*)");
    throw;
  }
}

ISample* OpenALDevice::CreateSample (const SampleDesc& desc, const SampleReadFunction& fn)
{
  common::Lock lock (*this);

  try
  {
    return new OpenALSample (desc, fn);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("sound::low_level::OpenALDevice::CreateSample (const SampleDesc&, const SampleReadFunction&)");
    throw;
  }
}

/*
    Установка текущего проигрываемого звука
*/

void OpenALDevice::SetSample (size_t channel, ISample* sample)
{
  static const char* METHOD_NAME = "sound::low_level::OpenALDevice::SetSample";

  common::Lock lock (*this);

  if (channel >= channels.size ())
    throw xtl::make_range_exception (METHOD_NAME, "channel", channel, channels.size ());

  OpenALSample* openal_sample = dynamic_cast<OpenALSample*> (sample);

  if (sample && !openal_sample)
    throw xtl::make_argument_exception (METHOD_NAME, "sample", "Incompatible sample type, must be 'OpenALSoundSample'");

  channels [channel]->SetSample (openal_sample);
}

ISample* OpenALDevice::GetSample (size_t channel)
{
  common::Lock lock (*this);

  if (channel >= channels.size ())
    throw xtl::make_range_exception ("sound::low_level::OpenALDevice::GetSample", "channel", channel, channels.size ());

  return channels [channel]->GetSample ();
}

/*
    Проверка цикличности проигрывания канала
*/

bool OpenALDevice::IsLooped (size_t channel)
{
  common::Lock lock (*this);

  if (channel >= channels.size ())
    throw xtl::make_range_exception ("sound::low_level::OpenALDevice::IsLooped", "channel", channel, channels.size ());

  return channels [channel]->IsLooped ();
}

/*
    Установка параметров источника
*/

void OpenALDevice::SetSource (size_t channel, const Source& source)
{
  common::Lock lock (*this);

  if (channel >= channels.size ())
    throw xtl::make_range_exception ("sound::low_level::OpenALDevice::SetSource", "channel", channel, channels.size ());

  channels [channel]->SetSource (source);
}

void OpenALDevice::GetSource (size_t channel, Source& source)
{
  common::Lock lock (*this);

  if (channel >= channels.size ())
    throw xtl::make_range_exception ("sound::low_level::OpenALDevice::GetSource", "channel", channel, channels.size ());

  source = channels [channel]->GetSource ();
}

/*
    Управление проигрыванием
*/

void OpenALDevice::Play (size_t channel, bool looping)
{
  common::Lock lock (*this);

  if (channel >= channels.size ())
    throw xtl::make_range_exception ("sound::low_level::OpenALDevice::Play", "channel", channel, channels.size ());

  channels [channel]->Play (looping);
}

void OpenALDevice::Pause (size_t channel)
{
  common::Lock lock (*this);

  if (channel >= channels.size ())
    throw xtl::make_range_exception ("sound::low_level::OpenALDevice::Pause", "channel", channel, channels.size ());

  channels [channel]->Pause ();
}

void OpenALDevice::Stop (size_t channel)
{
  common::Lock lock (*this);

  if (channel >= channels.size ())
    throw xtl::make_range_exception ("sound::low_level::OpenALDevice::Stop", "channel", channel, channels.size ());

  channels [channel]->Stop ();
}

void OpenALDevice::Seek (size_t channel, float time_in_seconds, SeekMode seek_mode)
{
  common::Lock lock (*this);

  if (channel >= channels.size ())
    throw xtl::make_range_exception ("sound::low_level::OpenALDevice::Seek", "channel", channel, channels.size ());

  channels [channel]->Seek (time_in_seconds, seek_mode);
}

float OpenALDevice::Tell (size_t channel)
{
  common::Lock lock (*this);

  if (channel >= channels.size ())
    throw xtl::make_range_exception ("sound::low_level::OpenALDevice::Tell", "channel", channel, channels.size ());

  return channels [channel]->Tell ();
}

bool OpenALDevice::IsPlaying (size_t channel)
{
  common::Lock lock (*this);

  if (channel >= channels.size ())
    throw xtl::make_range_exception ("sound::low_level::OpenALDevice::IsPlaying", "channel", channel, channels.size ());

  return channels [channel]->IsPlaying ();
}

/*
    Обновление
*/

void OpenALDevice::BufferUpdate ()
{
  common::Lock lock (*this);

    //если нет активных источников нет необходимости что-либо обновлять

  if (!first_active_source)
    return;

    //обновление источников

  for (OpenALSource* source=first_active_source; source; source=source->NextActive ())
    source->BufferUpdate ();
}

void OpenALDevice::SourceUpdate ()
{
  common::Lock lock (*this);

    //если нет активных источников нет необходимости что-либо обновлять

  if (!first_active_source)
    return;

    //обновление источников

  for (OpenALSource* source=first_active_source; source; source=source->NextActive ())
    source->PropertiesUpdate ();
}

void OpenALDevice::ListenerUpdate ()
{
  common::Lock lock (*this);

    //если нет активных источников нет необходимости что-либо обновлять

  if (!first_active_source)
    return;

    //обновление слушателя

  if (listener_need_update)
  {
    listener_need_update = false;

    float orientation [6] = {listener.direction.x, listener.direction.y, listener.direction.z,
                             listener.up.x,        listener.up.y,        listener.up.z};

    context.MakeCurrent ();

    context.alListenerfv (AL_POSITION,    &listener.position [0]);
    context.alListenerfv (AL_VELOCITY,    &listener.velocity [0]);
    context.alListenerfv (AL_ORIENTATION, &orientation [0]);
    context.alListenerf  (AL_GAIN,        is_muted ? 0.0f : gain);
  }
}

/*
    Установка параметров устройства
*/

const char* OpenALDevice::GetParamsNames ()
{
  common::Lock lock (*this);

  return DEVICE_PARAMS_NAMES;
}

bool OpenALDevice::IsIntegerParam  (const char* name)
{
  common::Lock lock (*this);

  if (!name)
    return false;

  return strstr (DEVICE_INT_PARAMS_NAMES, name) != 0;
}

bool OpenALDevice::IsStringParam (const char* name)
{
  common::Lock lock (*this);

  if (!name)
    return false;

  return strstr (DEVICE_STRING_PARAMS_NAMES, name) != 0;
}

void OpenALDevice::SetIntegerParam (const char* name, int value)
{
  common::Lock lock (*this);

  if (!name)
    throw xtl::make_null_argument_exception ("sound::low_level::OpenALDevice::SetIntegerParam", "name");

    //заменить на дефолтные значения!!!

  if (!value)
    throw xtl::make_null_argument_exception ("sound::low_level::OpenALDevice::SetIntegerParam", "value");

  if (!xstrcmp (name, "buffer_update_frequency"))
  {
    buffer_update_frequency = (size_t)value;
    buffer_action           = common::ActionQueue::PushAction (xtl::bind (&OpenALDevice::BufferUpdate, this), common::ActionThread_Current, 0, 1.0f / buffer_update_frequency);
  }
  else if (!xstrcmp (name, "source_update_frequency"))
  {
    source_properties_update_frequency = (size_t)value;
    source_action                      = common::ActionQueue::PushAction (xtl::bind (&OpenALDevice::SourceUpdate, this), common::ActionThread_Current, 0, 1.0f / source_properties_update_frequency);
  }
  else if (!xstrcmp (name, "listener_update_frequency"))
  {
    listener_properties_update_frequency = (size_t)value;
    listener_action                      = common::ActionQueue::PushAction (xtl::bind (&OpenALDevice::ListenerUpdate, this), common::ActionThread_Current, 0, 1.0f / listener_properties_update_frequency);
  }
  else
  {
    throw xtl::make_argument_exception ("sound::low_level::OpenALDevice::SetIntegerParam", "name", name);
  }
}

int OpenALDevice::GetIntegerParam (const char* name)
{
  common::Lock lock (*this);

  if (!name)
    throw xtl::make_null_argument_exception ("sound::low_level::OpenALDevice::GetIntegerParam", "name");

  if      (!xstrcmp (name, "buffer_update_frequency"))   return static_cast<int> (buffer_update_frequency);
  else if (!xstrcmp (name, "source_update_frequency"))   return static_cast<int> (source_properties_update_frequency);
  else if (!xstrcmp (name, "listener_update_frequency")) return static_cast<int> (listener_properties_update_frequency);
  else
  {
    throw xtl::make_argument_exception ("sound::low_level::OpenALDevice::GetIntegerParam", "name", name);
  }

  return 0;
}

void OpenALDevice::SetStringParam (const char* name, const char* value)
{
  static const char* METHOD_NAME = "sound::low_level::OpenALDevice::SetStringParam";

  common::Lock lock (*this);

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");

  if (!xstrcmp (name, "AL_DISTANCE_MODEL"))
  {
    context.MakeCurrent ();

    if (!xstrcmp (value, "AL_NONE"))
      context.alDistanceModel (AL_NONE);
    else if (!xstrcmp (value, "AL_INVERSE_DISTANCE"))
      context.alDistanceModel (AL_INVERSE_DISTANCE);
    else if (!xstrcmp (value, "AL_INVERSE_DISTANCE_CLAMPED"))
      context.alDistanceModel (AL_INVERSE_DISTANCE_CLAMPED);
    else if (!xstrcmp (value, "AL_LINEAR_DISTANCE"))
      context.alDistanceModel (AL_LINEAR_DISTANCE);
    else if (!xstrcmp (value, "AL_LINEAR_DISTANCE_CLAMPED"))
      context.alDistanceModel (AL_LINEAR_DISTANCE_CLAMPED);
    else if (!xstrcmp (value, "AL_EXPONENT_DISTANCE"))
      context.alDistanceModel (AL_EXPONENT_DISTANCE);
    else if (!xstrcmp (value, "AL_EXPONENT_DISTANCE_CLAMPED"))
      context.alDistanceModel (AL_EXPONENT_DISTANCE_CLAMPED);
    else
      throw xtl::make_argument_exception (METHOD_NAME, "value", value);
  }

  throw xtl::make_argument_exception (METHOD_NAME, "name", name);
}

const char* OpenALDevice::GetStringParam (const char* name)
{
  common::Lock lock (*this);

  if (!name)
    throw xtl::make_null_argument_exception ("sound::low_level::OpenALDevice::GetStringParam", "name");

  if (!xstrcmp (name, "al_distance_model"))
  {
    context.MakeCurrent ();

    switch (context.alGetInteger (AL_DISTANCE_MODEL))
    {
      default:
      case AL_NONE:                      return "AL_NONE";
      case AL_INVERSE_DISTANCE:          return "AL_INVERSE_DISTANCE";
      case AL_INVERSE_DISTANCE_CLAMPED:  return "AL_INVERSE_DISTANCE_CLAMPED";
      case AL_LINEAR_DISTANCE:           return "AL_LINEAR_DISTANCE";
      case AL_LINEAR_DISTANCE_CLAMPED:   return "AL_LINEAR_DISTANCE_CLAMPED";
      case AL_EXPONENT_DISTANCE:         return "AL_EXPONENT_DISTANCE";
      case AL_EXPONENT_DISTANCE_CLAMPED: return "AL_EXPONENT_DISTANCE_CLAMPED";
    }
  }
  else
    throw xtl::make_argument_exception ("sound::low_level::OpenALDevice::GetStringParam", "name", name);

  return "";
}

/*
    Управление распределением буферов проигрывания
*/

ALuint OpenALDevice::AllocateSourceBuffer ()
{
  common::Lock lock (*this);

  if (al_buffers_pool_size)
    return al_buffers_pool [--al_buffers_pool_size];

  const ALuint WRONG_BUFFER_ID = 0;

  ALuint buffer = WRONG_BUFFER_ID;

  context.MakeCurrent ();

  context.alGenBuffers (1, &buffer);

  if (buffer == WRONG_BUFFER_ID)
    throw xtl::format_exception<OpenALException> ("sound::low_level::OpenALDevice::AllocateSourceBuffer", "No enough buffers");

  return buffer;
}

void OpenALDevice::DeallocateSourceBuffer (ALuint buffer)
{
  common::Lock lock (*this);

  if (al_buffers_pool_size == DEVICE_BUFFERS_POOL_SIZE)
  {
    size_t flush_size = DEVICE_BUFFERS_POOL_SIZE / 2 + 1;

    context.MakeCurrent ();

    context.alDeleteBuffers (flush_size, al_buffers_pool + DEVICE_BUFFERS_POOL_SIZE - flush_size);

    al_buffers_pool_size -= flush_size;
  }

  al_buffers_pool [al_buffers_pool_size++] = buffer;
}
