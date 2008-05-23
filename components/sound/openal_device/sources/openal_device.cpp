#include "shared.h"

using namespace common;
using namespace stl;
using namespace sound;
using namespace sound::low_level;

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

//время обновления буферов источника в миллисекундах
const size_t SOURCE_BUFFERS_UPDATE_MILLISECONDS = size_t (SOURCE_BUFFERS_UPDATE_PERIOD * 1000);
const float  DEFAULT_SOURCE_PROPERTIES_UPDATE_PERIOD = 0.03f;
const float  DEFAULT_LISTENER_PROPERTIES_UPDATE_PERIOD = 0.03f;
const char*  DEVICE_PARAMS_NAMES = "buffer_update_frequency source_update_frequency listener_update_frequency al_debug_log";
const char*  DEVICE_INT_PARAMS_NAMES = DEVICE_PARAMS_NAMES;

namespace
{

void DefaultLogHandler (const char* log_message)
{
}

}

/*
    Конструктор / деструктор
*/

OpenALDevice::OpenALDevice (const char* driver_name, const char* device_name)
 : context (device_name),
   buffer_update_frequency ((size_t) (1.f / SOURCE_BUFFERS_UPDATE_PERIOD)),
   source_properties_update_frequency ((size_t)(1.f / DEFAULT_SOURCE_PROPERTIES_UPDATE_PERIOD)),
   listener_properties_update_frequency ((size_t)(1.f / DEFAULT_LISTENER_PROPERTIES_UPDATE_PERIOD)),
   buffer_timer   (xtl::bind (&OpenALDevice::BufferUpdate, this), SOURCE_BUFFERS_UPDATE_MILLISECONDS),
   listener_timer (xtl::bind (&OpenALDevice::ListenerUpdate, this), (size_t)(DEFAULT_LISTENER_PROPERTIES_UPDATE_PERIOD * 1000)),
   source_timer   (xtl::bind (&OpenALDevice::SourceUpdate, this), (size_t)(DEFAULT_SOURCE_PROPERTIES_UPDATE_PERIOD * 1000)),
   listener_need_update (false),
   sample_buffer (DEFAULT_SAMPLE_BUFFER_SIZE),
   ref_count (1),
   is_muted (false), 
   gain (1.0f),
   channels_count (0),
   first_active_source (0),
   al_buffers_pool_size (0)
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
  
    //формирование имени устройства
    
  name = format ("%s::%s", driver_name ? driver_name : "", device_name ? device_name : "");
    
    //создание каналов проигрывания

  for (size_t i=0; i<MAX_DEVICE_CHANNELS_COUNT; i++)
  {    
    try
    {
      channels [i] = new OpenALSource (*this);
    }
    catch (...)
    {
      break;
    }

    channels_count++;
  }

  info.channels_count = channels_count;  

  ALCint temp_veriable;

  context.alcGetIntegerv (ALC_EFX_MAJOR_VERSION, 1, &temp_veriable);
  info.efx_major_version = temp_veriable;
  context.alcGetIntegerv (ALC_EFX_MINOR_VERSION, 1, &temp_veriable);
  info.efx_minor_version = temp_veriable;
  context.alcGetIntegerv (ALC_MAX_AUXILIARY_SENDS, 1, &temp_veriable);
  info.max_aux_sends = temp_veriable;
}

OpenALDevice::~OpenALDevice ()
{
    //удаление каналов

  for (size_t i=0; i<channels_count; i++)
    delete channels [i];
    
    //очистка пула буферов
    
  context.alDeleteBuffers (al_buffers_pool_size, al_buffers_pool);
}

/*
    Подсчёт ссылок
*/

void OpenALDevice::AddRef ()
{
  ref_count++;
}

void OpenALDevice::Release ()
{
  if (!--ref_count)
    delete this;
}

/*
   Имя устройства
*/

const char* OpenALDevice::Name ()
{
  return name.c_str ();
}

/*
   Получение информации об устройстве
*/

void OpenALDevice::GetCapabilities (Capabilities& target_info)
{
  target_info = info;
}

/*
    Оповещение о необходимости обновления слушателя
*/

void OpenALDevice::UpdateListenerNotify ()
{
  listener_need_update = true;
  
  ListenerUpdate ();
}
    
/*
   Установка уровня громкости для устройства
*/

void OpenALDevice::SetVolume (float in_gain)
{
  if (in_gain < 0.0f) in_gain = 0.0f;
  if (in_gain > 1.0f) in_gain = 1.0f;

  gain     = in_gain;
  is_muted = false;
  
  UpdateListenerNotify ();
}

float OpenALDevice::GetVolume ()
{
  return gain;
}

/*
   Блокировка проигрывания звука
*/

void OpenALDevice::SetMute (bool state)
{
  is_muted = state;

  UpdateListenerNotify ();
}

bool OpenALDevice::IsMuted ()
{
  return is_muted;
}

/*
    Установка параметров слушателя
*/

void OpenALDevice::SetListener (const Listener& in_listener)
{
  listener = in_listener;

  UpdateListenerNotify ();
}

void OpenALDevice::GetListener (Listener& target_listener)
{
  target_listener = listener;
}

/*
    Отладочное протоколирование
*/

void OpenALDevice::SetDebugLog (const LogHandler& in_log_handler)
{
  log_handler = in_log_handler ? in_log_handler : &DefaultLogHandler;
  
  context.SetDebugLog (log_handler);
}

const OpenALDevice::LogHandler& OpenALDevice::GetDebugLog ()
{
  return log_handler;
}

void OpenALDevice::DebugPrintf (const char* format, ...)
{
  va_list list;
  
  va_start (list, format);
  DebugVPrintf (format, list);
}

void OpenALDevice::DebugVPrintf (const char* format, va_list list)
{
  try
  {
    log_handler (vformat (format, list).c_str ());
  }
  catch (...)
  {
    //подавление всех исключений
  }
}

/*
    Установка текущего проигрываемого звука
*/

void OpenALDevice::SetSample (size_t channel, const media::SoundSample& sample)
{
  if (channel >= channels_count)
    RaiseOutOfRange ("sound::low_level::OpenALDevice::SetSample", "channel", channel, channels_count);
    
  channels [channel]->SetSample (sample);
}

const media::SoundSample& OpenALDevice::GetSample (size_t channel)
{
  if (channel >= channels_count)
    RaiseOutOfRange ("sound::low_level::OpenALDevice::GetSample", "channel", channel, channels_count);
    
  return channels [channel]->GetSample ();
}

/*
    Проверка цикличности проигрывания канала
*/

bool OpenALDevice::IsLooped (size_t channel)
{
  if (channel >= channels_count)
    RaiseOutOfRange ("sound::low_level::OpenALDevice::IsLooped", "channel", channel, channels_count);
    
  return channels [channel]->IsLooped ();
}
    
/*
    Установка параметров источника
*/

void OpenALDevice::SetSource (size_t channel, const Source& source)
{
  if (channel >= channels_count)
    RaiseOutOfRange ("sound::low_level::OpenALDevice::SetSource", "channel", channel, channels_count);
    
  channels [channel]->SetSource (source);
}

void OpenALDevice::GetSource (size_t channel, Source& source)
{
  if (channel >= channels_count)
    RaiseOutOfRange ("sound::low_level::OpenALDevice::GetSource", "channel", channel, channels_count);

  source = channels [channel]->GetSource ();
}

/*
    Управление проигрыванием
*/

void OpenALDevice::Play (size_t channel, bool looping)
{
  if (channel >= channels_count)
    RaiseOutOfRange ("sound::low_level::OpenALDevice::Play", "channel", channel, channels_count);
    
  channels [channel]->Play (looping);
}

void OpenALDevice::Pause (size_t channel)
{
  if (channel >= channels_count)
    RaiseOutOfRange ("sound::low_level::OpenALDevice::Pause", "channel", channel, channels_count);
    
  channels [channel]->Pause ();
}

void OpenALDevice::Stop (size_t channel)
{
  if (channel >= channels_count)
    RaiseOutOfRange ("sound::low_level::OpenALDevice::Stop", "channel", channel, channels_count);
    
  channels [channel]->Stop ();
}

void OpenALDevice::Seek (size_t channel, float time_in_seconds)
{
  if (channel >= channels_count)
    RaiseOutOfRange ("sound::low_level::OpenALDevice::Seek", "channel", channel, channels_count);

  channels [channel]->Seek (time_in_seconds);
}

float OpenALDevice::Tell (size_t channel)
{
  if (channel >= channels_count)
    RaiseOutOfRange ("sound::low_level::OpenALDevice::Tell", "channel", channel, channels_count);
    
  return channels [channel]->Tell (); 
}

bool OpenALDevice::IsPlaying (size_t channel)
{
  if (channel >= channels_count)
    RaiseOutOfRange ("sound::low_level::OpenALDevice::IsPlaying", "channel", channel, channels_count);
    
  return channels [channel]->IsPlaying ();
}

/*
    Обновление
*/

void OpenALDevice::BufferUpdate ()
{
    //если нет активных источников нет необходимости что-либо обновлять

  if (!first_active_source)
    return;
    
    //обновление источников

  for (OpenALSource* source=first_active_source; source; source=source->NextActive ())
    source->BufferUpdate ();
}

void OpenALDevice::SourceUpdate ()
{
    //если нет активных источников нет необходимости что-либо обновлять

  if (!first_active_source)
    return;
    
    //обновление источников

  for (OpenALSource* source=first_active_source; source; source=source->NextActive ())
    source->PropertiesUpdate ();
}

void OpenALDevice::ListenerUpdate ()
{
    //если нет активных источников нет необходимости что-либо обновлять

  if (!first_active_source)
    return;
    
    //обновление слушателя

  if (listener_need_update)
  {
    listener_need_update = false;
    
    float orientation [6] = {listener.direction.x, listener.direction.y, listener.direction.z, 
                             listener.up.x,        listener.up.y,        listener.up.z};

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
  return DEVICE_PARAMS_NAMES;
}

bool OpenALDevice::IsIntegerParam  (const char* name)
{
  if (!name)
    return false;
    
  return strstr (DEVICE_INT_PARAMS_NAMES, name) != 0;
}

bool OpenALDevice::IsStringParam (const char*)
{
  return false;
}

void OpenALDevice::SetIntegerParam (const char* name, int value)
{
  if (!name)
    RaiseNullArgument ("sound::low_level::OpenALDevice::SetIntegerParam", "name");    
    
    //заменить на дефолтные значения!!!    
    
  if (!value)
    RaiseNullArgument ("sound::low_level::OpenALDevice::SetIntegerParam", "value");

  if (!::strcmp (name, "buffer_update_frequency"))
  {
    buffer_update_frequency = (size_t)value;
    buffer_timer.SetPeriod ((size_t)(1000.0f / value));
  }
  else if (!::strcmp (name, "source_update_frequency"))
  {
    source_properties_update_frequency = (size_t)value;
    source_timer.SetPeriod ((size_t)(1000.0f / value));    
  }
  else if (!::strcmp (name, "listener_update_frequency"))
  {
    listener_properties_update_frequency = (size_t)value;
    listener_timer.SetPeriod ((size_t)(1000.0f / value));    
  }
  else if (!::strcmp (name, "al_debug_log"))
  {
    context.SetDebugLogState (value != 0);
  }
  else
  {
    RaiseInvalidArgument ("sound::low_level::OpenALDevice::SetIntegerParam", "name", name);
  }
}

int OpenALDevice::GetIntegerParam (const char* name)
{
  if (!name)
    RaiseNullArgument ("sound::low_level::OpenALDevice::GetIntegerParam", "name");
    
  if      (!::strcmp (name, "buffer_update_frequency"))   return static_cast<int> (buffer_update_frequency);
  else if (!::strcmp (name, "source_update_frequency"))   return static_cast<int> (source_properties_update_frequency);
  else if (!::strcmp (name, "listener_update_frequency")) return static_cast<int> (listener_properties_update_frequency);
  else if (!::strcmp (name, "al_debug_log"))              return context.GetDebugLogState ();
  else
  {
    RaiseInvalidArgument ("sound::low_level::OpenALDevice::GetIntegerParam", "name", name);
  }

  return 0;
}

void OpenALDevice::SetStringParam (const char* name, const char*)
{
  if (!name)
    RaiseNullArgument ("sound::low_level::OpenALDevice::SetStringParam", "name");


  RaiseInvalidArgument ("sound::low_level::OpenALDevice::SetStringParam", "name", name);
}

const char* OpenALDevice::GetStringParam (const char* name)
{
  if (!name)
    RaiseNullArgument ("sound::low_level::OpenALDevice::GetStringParam", "name");

  RaiseInvalidArgument ("sound::low_level::OpenALDevice::GetStringParam", "name", name);

  return "";
}

/*
    Управление распределением буферов проигрывания
*/

ALuint OpenALDevice::AllocateSourceBuffer ()
{
  if (al_buffers_pool_size)
    return al_buffers_pool [--al_buffers_pool_size];

  const ALuint WRONG_BUFFER_ID = 0;

  ALuint buffer = WRONG_BUFFER_ID;
  
  context.alGenBuffers (1, &buffer);
  
  if (buffer == WRONG_BUFFER_ID)
    throw OpenALGenBufferException ();

  return buffer;
}

void OpenALDevice::DeallocateSourceBuffer (ALuint buffer)
{
  if (al_buffers_pool_size == DEVICE_BUFFERS_POOL_SIZE)
  {
    size_t flush_size = DEVICE_BUFFERS_POOL_SIZE / 2 + 1;
    
    context.alDeleteBuffers (flush_size, al_buffers_pool);
    
    al_buffers_pool_size -= flush_size;
  }

  al_buffers_pool [al_buffers_pool_size++] = buffer;
}
