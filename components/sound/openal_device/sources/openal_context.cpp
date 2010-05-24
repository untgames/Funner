#include "shared.h"

using namespace sound::low_level;
using namespace sound::low_level::openal;
using namespace common;
using namespace stl;
using namespace xtl;

namespace
{

const char* LOG_NAME = "sound::low_level::openal";                    //имя потока протоколирования

//получение текстового описания ошибки контекста OpenAL
const char* get_alc_error_message (ALCenum error)
{
  switch (error)
  {
    case AL_NO_ERROR:         return "No error";
    case ALC_INVALID_DEVICE:  return "Invalid device";
    case ALC_INVALID_CONTEXT: return "Invalid context";
    case ALC_INVALID_ENUM:    return "Invalid enum";
    case ALC_INVALID_VALUE:   return "Invalid value";
    case ALC_OUT_OF_MEMORY:   return "Out of memory";
    default:                  return "Unknown error";
  }
}

//получение текстового описания ошибки OpenAL
const char* get_al_error_message (ALenum error)
{
  switch (error)
  {
    case AL_NO_ERROR:          return "No error";
    case AL_INVALID_NAME:      return "Invalid Name paramater passed to AL call";
    case AL_INVALID_ENUM:      return "Invalid enum parameter passed to AL call";
    case AL_INVALID_VALUE:     return "Invalid enum parameter value";
    case AL_INVALID_OPERATION: return "Invalid operation";
    case AL_OUT_OF_MEMORY:     return "Out of memory";
    default:                   return "Unknown error";
  }
}

//настройки инициализации контекста
struct ContextInitProperties
{
  size_t frequency;

  ContextInitProperties () : frequency (0) {}
};

void process_init_string (const char* property, const char* value, ContextInitProperties& properties)
{
  if (!xstricmp (property, "frequency"))
    properties.frequency = atoi (value);
}

}

/*
    Конструктор / деструктор
*/

OpenALContext::OpenALContext (const char* device_name, const char* init_string)
  : log (LOG_NAME)
{
  try
  {
    if (!device_name)
      throw xtl::make_null_argument_exception ("", "device_name");

    if (!xstrcmp (device_name, "default"))
      device_name = 0;

    if (!init_string)
      throw xtl::make_null_argument_exception ("", "init_string");

    ContextInitProperties properties;

    common::parse_init_string (init_string, xtl::bind (&process_init_string, _1, _2, ref (properties)));

    device = alcOpenDevice (device_name);

    if (!device)
      throw xtl::format_exception<OpenALException> ("", "Can't open device '%s'", device_name);

    efx_present = alcIsExtensionPresent ("ALC_EXT_EFX") == ALC_TRUE;

    ALint attribs[4] = {0};

    if (properties.frequency)
    {
      attribs[0] = ALC_FREQUENCY;
      attribs[1] = properties.frequency;
    }

    context = alcCreateContext (device, attribs);

    if (!context)
    {
      const char* alc_error_message = get_alc_error_message (alcGetError (device));

      alcCloseDevice (device);

      throw xtl::format_exception<OpenALException> ("", "Can't create context. %s", alc_error_message);
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("sound::low_level::OpenALContext::OpenALContext");
    throw;
  }
}

OpenALContext::~OpenALContext ()
{
  if (IsCurrent ())
    alcMakeContextCurrent (0);

  alcDestroyContext (context);

  ALCenum error = alcGetError (device);

  if (error != AL_NO_ERROR)
    log.Printf ("Error at alcDestroyContext(%p). %s", context, get_alc_error_message (error));

  if (!alcCloseDevice (device))
    log.Printf ("Error at alcCloseDevice(%p). %s", device, get_alc_error_message (alcGetError (device)));
}

/*
    Управление текущими контекстами
*/

bool OpenALContext::IsCurrent () const
{
  return alcGetCurrentContext () == context;
}

bool OpenALContext::MakeCurrent ()
{
  if (IsCurrent ())
    return true;

  ALCboolean status = alcMakeContextCurrent (context);

  ContextCheckErrors ("alcMakeContextCurrent");

  if (!status)
  {
    log.Printf ("Error at alcMakeContextCurrent(%p). %s", context, get_alc_error_message (alcGetError (device)));
    return false;
  }

  return true;
}

/*
    Проверка ошибок после вызова функции
*/

void OpenALContext::CheckErrors (const char* function_name)
{
  ALenum error = alGetError ();

  try
  {
    if (error != AL_NO_ERROR)
      log.Printf ("Error at call %s. %s", function_name, get_al_error_message (error));
  }
  catch (...)
  {
  }
}

void OpenALContext::ContextCheckErrors (const char* function_name)
{
  ALCenum error = alcGetError (device);

  try
  {
    if (error != ALC_NO_ERROR)
      log.Printf ("Error at call %s. %s", function_name, get_al_error_message (error));
  }
  catch (...)
  {
  }
}

/*
    Обёртки над вызовами OpenAL
*/

void OpenALContext::alEnable (ALenum capability)
{
  ::alEnable (capability); CheckErrors ("alEnable");
}

void OpenALContext::alDisable (ALenum capability)
{
  ::alDisable (capability); CheckErrors ("alDisable");
}

ALboolean OpenALContext::alIsEnabled (ALenum capability)
{
  ALboolean result = ::alIsEnabled (capability); CheckErrors ("alIsEnabled"); return result;
}

const ALchar* OpenALContext::alGetString (ALenum param)
{
  const ALchar* result = ::alGetString (param); CheckErrors ("alGetString"); return result;
}

void OpenALContext::alGetBooleanv (ALenum param, ALboolean* data)
{
  ::alGetBooleanv (param, data); CheckErrors ("alGetBooleanv");
}

void OpenALContext::alGetIntegerv (ALenum param, ALint* data)
{
  ::alGetIntegerv (param, data); CheckErrors ("alGetIntegerv");
}

void OpenALContext::alGetFloatv (ALenum param, ALfloat* data)
{
  ::alGetFloatv (param, data); CheckErrors ("alGetFloatv");
}

void OpenALContext::alGetDoublev (ALenum param, ALdouble* data)
{
  ::alGetDoublev (param, data); CheckErrors ("alGetDoublev");
}

ALboolean OpenALContext::alGetBoolean (ALenum param)
{
  ALboolean result = ::alGetBoolean (param); CheckErrors ("alGetBoolean"); return result;
}

ALint OpenALContext::alGetInteger (ALenum param)
{
  ALint result = ::alGetInteger (param); CheckErrors ("alGetInteger"); return result;
}

ALfloat OpenALContext::alGetFloat (ALenum param)
{
  ALfloat result = ::alGetFloat (param); CheckErrors ("alGetFloat"); return result;
}

ALdouble OpenALContext::alGetDouble (ALenum param)
{
  ALdouble result = ::alGetDouble (param); CheckErrors ("alGetDouble"); return result;
}

void OpenALContext::alGenSources (ALsizei n, ALuint* sources)
{
  ::alGenSources (n, sources); CheckErrors ("alGenSources");
}

void OpenALContext::alDeleteSources (ALsizei n, const ALuint* sources)
{
  ::alDeleteSources (n, sources); CheckErrors ("alDeleteSources");
}

ALboolean OpenALContext::alIsSource (ALuint sid)
{
  ALboolean result = ::alIsSource (sid); CheckErrors ("alIsSource"); return result;
}

void OpenALContext::alGetSourcef (ALuint sid, ALenum param, ALfloat* value)
{
  ::alGetSourcef (sid, param, value); CheckErrors ("alGetSourcef");
}

void OpenALContext::alGetSource3f (ALuint sid, ALenum param, ALfloat* value1, ALfloat* value2, ALfloat* value3)
{
  ::alGetSource3f (sid, param, value1, value2, value3); CheckErrors ("alGetSource3f");
}

void OpenALContext::alGetSourcefv (ALuint sid, ALenum param, ALfloat* values)
{
  ::alGetSourcefv (sid, param, values); CheckErrors ("alGetSourcefv");
}

void OpenALContext::alGetSourcei (ALuint sid, ALenum param, ALint* value)
{
  ::alGetSourcei (sid, param, value); CheckErrors ("alGetSourcei");
}

void OpenALContext::alGetSource3i (ALuint sid, ALenum param, ALint* value1, ALint* value2, ALint* value3)
{
  ::alGetSource3i (sid, param, value1, value2, value3); CheckErrors ("alGetSource3i");
}

void OpenALContext::alGetSourceiv (ALuint sid, ALenum param, ALint* values)
{
  ::alGetSourceiv (sid, param, values); CheckErrors ("alGetSourceiv");
}

void OpenALContext::alSourcef (ALuint sid, ALenum param, ALfloat value)
{
  ::alSourcef (sid, param, value); CheckErrors ("alSourcef");
}

void OpenALContext::alSource3f (ALuint sid, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3)
{
  ::alSource3f (sid, param, value1, value2, value3); CheckErrors ("alSource3f");
}

void OpenALContext::alSourcefv (ALuint sid, ALenum param, const ALfloat* values)
{
  ::alSourcefv (sid, param, values); CheckErrors ("alSourcefv");
}

void OpenALContext::alSourcei (ALuint sid, ALenum param, ALint value)
{
  ::alSourcei (sid, param, value); CheckErrors ("alSourcei");
}

void OpenALContext::alSource3i (ALuint sid, ALenum param, ALint value1, ALint value2, ALint value3)
{
  ::alSource3i (sid, param, value1, value2, value3); CheckErrors ("alSource3i");
}

void OpenALContext::alSourceiv (ALuint sid, ALenum param, const ALint* values)
{
  ::alSourceiv (sid, param, values); CheckErrors ("alSourceiv");
}

void OpenALContext::alSourcePlayv (ALsizei ns, const ALuint *sids)
{
  ::alSourcePlayv (ns, sids); CheckErrors ("alSourcePlayv");
}

void OpenALContext::alSourceStopv (ALsizei ns, const ALuint *sids)
{
  ::alSourceStopv (ns, sids); CheckErrors ("alSourceStopv");
}

void OpenALContext::alSourceRewindv (ALsizei ns, const ALuint *sids)
{
  ::alSourceRewindv (ns, sids); CheckErrors ("alSourceRewindv");
}

void OpenALContext::alSourcePausev (ALsizei ns, const ALuint *sids)
{
  ::alSourcePausev (ns, sids); CheckErrors ("alSourcePausev");
}

void OpenALContext::alSourcePlay (ALuint sid)
{
  ::alSourcePlay (sid); CheckErrors ("alSourcePlay");
}

void OpenALContext::alSourceStop (ALuint sid)
{
  ::alSourceStop (sid); CheckErrors ("alSourceStop");
}

void OpenALContext::alSourceRewind (ALuint sid)
{
  ::alSourceRewind (sid); CheckErrors ("alSourceRewind");
}

void OpenALContext::alSourcePause (ALuint sid)
{
  ::alSourcePause (sid); CheckErrors ("alSourcePause");
}

void OpenALContext::alSourceQueueBuffers (ALuint sid, ALsizei n, const ALuint* buffers)
{
  ::alSourceQueueBuffers (sid, n, buffers); CheckErrors ("alSourceQueueBuffers");
}

void OpenALContext::alSourceUnqueueBuffers (ALuint sid, ALsizei n, ALuint* buffers)
{
  ::alSourceUnqueueBuffers (sid, n, buffers); CheckErrors ("alSourceUnqueueBuffers");
}

void OpenALContext::alListenerf (ALenum param, ALfloat value)
{
  ::alListenerf (param, value); CheckErrors ("alListenerf");
}

void OpenALContext::alListener3f (ALenum param, ALfloat value1, ALfloat value2, ALfloat value3)
{
  ::alListener3f (param, value1, value2, value3); CheckErrors ("alListener3f");
}

void OpenALContext::alListenerfv (ALenum param, const ALfloat* values)
{
  ::alListenerfv (param, values); CheckErrors ("alListenerfv");
}

void OpenALContext::alListeneri (ALenum param, ALint value)
{
  ::alListeneri (param, value); CheckErrors ("alListeneri");
}

void OpenALContext::alListener3i (ALenum param, ALint value1, ALint value2, ALint value3)
{
  ::alListener3i (param, value1, value2, value3); CheckErrors ("alListener3i");
}

void OpenALContext::alListeneriv (ALenum param, const ALint* values)
{
  ::alListeneriv (param, values); CheckErrors ("alListeneriv");
}

void OpenALContext::alGetListenerf (ALenum param, ALfloat* value)
{
  ::alGetListenerf (param, value); CheckErrors ("alGetListenerf");
}

void OpenALContext::alGetListener3f (ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3)
{
  ::alGetListener3f (param, value1, value2, value3); CheckErrors ("alGetListener3f");
}

void OpenALContext::alGetListenerfv (ALenum param, ALfloat* values)
{
  ::alGetListenerfv (param, values); CheckErrors ("alGetListenerfv");
}

void OpenALContext::alGetListeneri (ALenum param, ALint* value)
{
  ::alGetListeneri (param, value); CheckErrors ("alGetListeneri");
}

void OpenALContext::alGetListener3i (ALenum param, ALint *value1, ALint *value2, ALint *value3)
{
  ::alGetListener3i (param, value1, value2, value3); CheckErrors ("alGetListener3i");
}

void OpenALContext::alGetListeneriv (ALenum param, ALint* values)
{
  ::alGetListeneriv (param, values); CheckErrors ("alGetListeneriv");
}

void OpenALContext::alGenBuffers (ALsizei n, ALuint* buffers)
{
  ::alGenBuffers (n, buffers); CheckErrors ("alGenBuffers");
}

void OpenALContext::alDeleteBuffers (ALsizei n, const ALuint* buffers)
{
  ::alDeleteBuffers (n, buffers); CheckErrors ("alDeleteBuffers");
}

ALboolean OpenALContext::alIsBuffer (ALuint bid)
{
  ALboolean result = ::alIsBuffer (bid); CheckErrors ("alIsBuffer"); return result;
}

void OpenALContext::alBufferData (ALuint buffer, ALenum format, const ALvoid *data, ALsizei size, ALsizei freq)
{
  ::alBufferData (buffer, format, data, size, freq); CheckErrors ("alBufferData");
}

void OpenALContext::alBufferf (ALuint bid, ALenum param, ALfloat value)
{
  ::alBufferf (bid, param, value); CheckErrors ("alBufferf");
}

void OpenALContext::alBuffer3f (ALuint bid, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3)
{
  ::alBuffer3f (bid, param, value1, value2, value3); CheckErrors ("alBuffer3f");
}

void OpenALContext::alBufferfv (ALuint bid, ALenum param, const ALfloat* values)
{
  ::alBufferfv (bid, param, values); CheckErrors ("alBufferfv");
}

void OpenALContext::alBufferi (ALuint bid, ALenum param, ALint value)
{
  ::alBufferi (bid, param, value); CheckErrors ("alBufferi");
}

void OpenALContext::alBuffer3i (ALuint bid, ALenum param, ALint value1, ALint value2, ALint value3)
{
  ::alBuffer3i (bid, param, value1, value2, value3); CheckErrors ("alBuffer3i");
}

void OpenALContext::alBufferiv (ALuint bid, ALenum param, const ALint* values)
{
  ::alBufferiv (bid, param, values); CheckErrors ("alBufferiv");
}

void OpenALContext::alGetBufferf (ALuint bid, ALenum param, ALfloat* value)
{
  ::alGetBufferf (bid, param, value); CheckErrors ("alGetBufferf");
}

void OpenALContext::alGetBuffer3f (ALuint bid, ALenum param, ALfloat* value1, ALfloat* value2, ALfloat* value3)
{
  ::alGetBuffer3f (bid, param, value1, value2, value3); CheckErrors ("alGetBuffer3f");
}

void OpenALContext::alGetBufferfv (ALuint bid, ALenum param, ALfloat* values)
{
  ::alGetBufferfv (bid, param, values); CheckErrors ("alGetBufferfv");
}

void OpenALContext::alGetBufferi (ALuint bid, ALenum param, ALint* value)
{
  ::alGetBufferi (bid, param, value); CheckErrors ("alGetBufferi");
}

void OpenALContext::alGetBuffer3i (ALuint bid, ALenum param, ALint* value1, ALint* value2, ALint* value3)
{
  ::alGetBuffer3i (bid, param, value1, value2, value3); CheckErrors ("alGetBuffer3i");
}

void OpenALContext::alGetBufferiv (ALuint bid, ALenum param, ALint* values)
{
  ::alGetBufferiv (bid, param, values); CheckErrors ("alGetBufferiv");
}

void OpenALContext::alDopplerFactor (ALfloat value)
{
  ::alDopplerFactor (value); CheckErrors ("alDopplerFactor");
}

void OpenALContext::alDopplerVelocity (ALfloat value)
{
  ::alDopplerVelocity (value); CheckErrors ("alDopplerVelocity");
}

void OpenALContext::alSpeedOfSound (ALfloat value)
{
  ::alSpeedOfSound (value); CheckErrors ("alSpeedOfSound");
}

void OpenALContext::alDistanceModel (ALenum distanceModel)
{
  ::alDistanceModel (distanceModel); CheckErrors ("alDistanceModel");
}

/*
   Обёртки над вызовами OpenALContext
*/

ALCboolean OpenALContext::alcIsExtensionPresent (const ALCchar *extname)
{
  ALCboolean volatile result = ::alcIsExtensionPresent (device, extname);

  ContextCheckErrors ("alcIsExtensionPresent");

  return result;
}

void OpenALContext::alcGetIntegerv (ALCenum param, ALCsizei size, ALCint *data)
{
  ::alcGetIntegerv (device, param, size, data);

  ContextCheckErrors ("alcGetIntegerv");
}
