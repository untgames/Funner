#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <xtl/function.h>
#include <xtl/bind.h>
#include <common/strlib.h>

#include "shared.h"

#define MAX_CHANNELS_COUNT 1024

using namespace common;
using namespace stl;
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

}

/*
    Конструктор / деструктор
*/

OpenALDevice::OpenALDevice (const char* device_name)
 : context (new OpenALContext (device_name, &DefaultLogHandler)), log_handler (DefaultLogHandler), is_muted (false), 
   gain (1.f), last_gain (1.f), timer (xtl::bind (&OpenALDevice::UpdateBuffers, this), (size_t)(BUFFER_UPDATE_TIME * 1000)), 
   ref_count (1)
{
    //временный код!!!

  context->MakeCurrent ();

  char *extensions = (char*)context->alGetString (AL_EXTENSIONS);
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
      context->alDeleteSources (cur_channel, temp_sources);
      break;
    }
  }
  if (!cur_channel)
  {
    delete context;
    Raise <Exception> ("OpenALDevice::OpenALDevice", "Can't create no one channel");
  }
  if (!info.channels_count)
  {
    context->alDeleteSources (MAX_CHANNELS_COUNT, temp_sources);
    info.channels_count = MAX_CHANNELS_COUNT;
  }

  buffer = new char [((size_t)(2.f * (float)MAX_SOUND_CHANNELS * (float)MAX_SOUND_FREQUENCY * BUFFER_TIME) + 1023)&~1023];

  sources.resize (info.channels_count);
  for (size_t i = 0; i < info.channels_count; i++)
    sources[i] = new OpenALSource (context, buffer);
}

OpenALDevice::~OpenALDevice ()
{
  for (size_t i = 0; i < info.channels_count; i++)
    delete sources[i];

  delete [] buffer;
  delete context;
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
  target_info = info;
}

/*
   Количество микшируемых каналов
*/

size_t OpenALDevice::ChannelsCount ()
{
  return info.channels_count;
}
    
/*
   Установка уровня громкости для устройства
*/

void  OpenALDevice::SetVolume (float in_gain)
{
  if (in_gain < 0.f || in_gain > 1.f)
  {
    log_handler (format ("Incorrect gain value received (%f). Gain not changed", in_gain).c_str ());
    return;
  }

  gain = in_gain;

  context->alListenerf (AL_GAIN, in_gain);
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
  if (is_muted != state)
  {
    if (is_muted)
      SetVolume (last_gain);
    else
    {
      last_gain = gain;
      SetVolume (0.f);
    }
  }
  is_muted = state;
}

bool OpenALDevice::IsMuted ()
{
  return is_muted;
}

/*
   Установка параметров слушателя
*/

void OpenALDevice::SetListener (const Listener& source_listener)
{
  listener = source_listener;
  float  orientation [6] = {source_listener.direction.x, source_listener.direction.y, source_listener.direction.z, 
                            source_listener.up.x,        source_listener.up.y,        source_listener.up.z};

  context->alListenerfv (AL_POSITION,    source_listener.position);
  context->alListenerfv (AL_VELOCITY,    source_listener.velocity);
  context->alListenerfv (AL_ORIENTATION, orientation);
}

void OpenALDevice::GetListener (Listener& target_listener)
{
  target_listener = listener;
}

/*
   Установка функции отладочного протоколирования
*/

void OpenALDevice::SetDebugLog (const LogHandler& in_log_handler)
{
  log_handler = in_log_handler;
}

const OpenALDevice::LogHandler& OpenALDevice::GetDebugLog ()
{
  return log_handler;
}

void OpenALDevice::AddRef ()
{
  ref_count++;
}

void OpenALDevice::Release ()
{
  if (!--ref_count)
    delete this;
}
