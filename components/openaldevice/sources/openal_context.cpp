#include "shared.h"

using namespace sound::low_level;
using namespace common;
using namespace stl;
using namespace xtl;

namespace
{

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

//получение текстового имени константы OpenAL
const char* get_al_constant_name (ALenum value)
{
  switch (value)
  {
    case AL_SOURCE_RELATIVE                        : return "AL_SOURCE_RELATIVE";
    case AL_CONE_INNER_ANGLE                       : return "AL_CONE_INNER_ANGLE";
    case AL_CONE_OUTER_ANGLE                       : return "AL_CONE_OUTER_ANGLE";
    case AL_PITCH                                  : return "AL_PITCH";
    case AL_POSITION                               : return "AL_POSITION";
    case AL_DIRECTION                              : return "AL_DIRECTION";
    case AL_VELOCITY                               : return "AL_VELOCITY";
    case AL_LOOPING                                : return "AL_LOOPING";
    case AL_BUFFER                                 : return "AL_BUFFER";
    case AL_GAIN                                   : return "AL_GAIN";
    case AL_MIN_GAIN                               : return "AL_MIN_GAIN";
    case AL_MAX_GAIN                               : return "AL_MAX_GAIN";
    case AL_ORIENTATION                            : return "AL_ORIENTATION";
    case AL_CHANNEL_MASK                           : return "AL_CHANNEL_MASK";
    case AL_SOURCE_STATE                           : return "AL_SOURCE_STATE";
    case AL_INITIAL                                : return "AL_INITIAL";
    case AL_PLAYING                                : return "AL_PLAYING";
    case AL_PAUSED                                 : return "AL_PAUSED";
    case AL_STOPPED                                : return "AL_STOPPED";
    case AL_BUFFERS_QUEUED                         : return "AL_BUFFERS_QUEUED";
    case AL_BUFFERS_PROCESSED                      : return "AL_BUFFERS_PROCESSED";
    case AL_SEC_OFFSET                             : return "AL_SEC_OFFSET";
    case AL_SAMPLE_OFFSET                          : return "AL_SAMPLE_OFFSET";
    case AL_BYTE_OFFSET                            : return "AL_BYTE_OFFSET";
    case AL_SOURCE_TYPE                            : return "AL_SOURCE_TYPE";
    case AL_STATIC                                 : return "AL_STATIC";
    case AL_STREAMING                              : return "AL_STREAMING";
    case AL_UNDETERMINED                           : return "AL_UNDETERMINED";
    case AL_FORMAT_MONO8                           : return "AL_FORMAT_MONO8";
    case AL_FORMAT_MONO16                          : return "AL_FORMAT_MONO16";
    case AL_FORMAT_STEREO8                         : return "AL_FORMAT_STEREO8";
    case AL_FORMAT_STEREO16                        : return "AL_FORMAT_STEREO16";
    case AL_REFERENCE_DISTANCE                     : return "AL_REFERENCE_DISTANCE";
    case AL_ROLLOFF_FACTOR                         : return "AL_ROLLOFF_FACTOR";
    case AL_CONE_OUTER_GAIN                        : return "AL_CONE_OUTER_GAIN";
    case AL_MAX_DISTANCE                           : return "AL_MAX_DISTANCE";
    case AL_FREQUENCY                              : return "AL_FREQUENCY";
    case AL_BITS                                   : return "AL_BITS";
    case AL_CHANNELS                               : return "AL_CHANNELS";
    case AL_SIZE                                   : return "AL_SIZE";
    case AL_UNUSED                                 : return "AL_UNUSED";
    case AL_PENDING                                : return "AL_PENDING";
    case AL_PROCESSED                              : return "AL_PROCESSED";
    case AL_INVALID_NAME                           : return "AL_INVALID_NAME";
//    case AL_ILLEGAL_ENUM                           : return "AL_ILLEGAL_ENUM";
    case AL_INVALID_ENUM                           : return "AL_INVALID_ENUM";
    case AL_INVALID_VALUE                          : return "AL_INVALID_VALUE";
//    case AL_ILLEGAL_COMMAND                        : return "AL_ILLEGAL_COMMAND";
    case AL_INVALID_OPERATION                      : return "AL_INVALID_OPERATION";
    case AL_OUT_OF_MEMORY                          : return "AL_OUT_OF_MEMORY";
    case AL_VENDOR                                 : return "AL_VENDOR";
    case AL_VERSION                                : return "AL_VERSION";
    case AL_RENDERER                               : return "AL_RENDERER";
    case AL_EXTENSIONS                             : return "AL_EXTENSIONS";
    case AL_DOPPLER_FACTOR                         : return "AL_DOPPLER_FACTOR";
    case AL_DOPPLER_VELOCITY                       : return "AL_DOPPLER_VELOCITY";
    case AL_SPEED_OF_SOUND                         : return "AL_SPEED_OF_SOUND";
    case AL_DISTANCE_MODEL                         : return "AL_DISTANCE_MODEL";
    case AL_INVERSE_DISTANCE                       : return "AL_INVERSE_DISTANCE";
    case AL_INVERSE_DISTANCE_CLAMPED               : return "AL_INVERSE_DISTANCE_CLAMPED";
    case AL_LINEAR_DISTANCE                        : return "AL_LINEAR_DISTANCE";
    case AL_LINEAR_DISTANCE_CLAMPED                : return "AL_LINEAR_DISTANCE_CLAMPED";
    case AL_EXPONENT_DISTANCE                      : return "AL_EXPONENT_DISTANCE";
    case AL_EXPONENT_DISTANCE_CLAMPED              : return "AL_EXPONENT_DISTANCE_CLAMPED";
    default                                        : return 0;
  }
}

/*
    Обёртки для информативности вывода
*/

//обёртка для вывода названий констант OpenAL
struct EnumWrapper
{
  EnumWrapper (ALenum in_value) : value (in_value) {}
  
  operator ALenum () const { return value; }
  
  ALenum value;
};

inline EnumWrapper make_wrapper (ALenum value)
{
  return EnumWrapper (value);
}

//обёртка для печати массивов
template <class T> struct ArrayWrapper
{
  ArrayWrapper (size_t in_size, T* in_array) : size (in_size), array (in_array) {}
  
  operator T* () const { return array; }

  size_t size;
  T*     array;
};

template <class T>
inline ArrayWrapper<T> make_wrapper (size_t size, T* array)
{
  return ArrayWrapper<T> (size, array);
}

/*
    Печать аргументов
*/

template <size_t Size> struct ArgumentsCount {};

template <class Tuple>
inline void dump_arguments (const Tuple& args, string& result)
{
  dump_arguments (args, result, ArgumentsCount<tuple_size<Tuple>::value> ());
}

template <class Tuple>
inline void dump_arguments (const Tuple& args, string& result, ArgumentsCount<0>)
{
}

template <class Tuple>
inline void dump_arguments (const Tuple& args, string& result, ArgumentsCount<1>)
{
  dump_argument (get<0> (args), result);
}

template <class Tuple>
inline void dump_arguments (const Tuple& args, string& result, ArgumentsCount<2>)
{
  dump_argument       (get<0> (args), result);
  dump_comma_argument (get<1> (args), result);
}

template <class Tuple>
inline void dump_arguments (const Tuple& args, string& result, ArgumentsCount<3>)
{
  dump_argument       (get<0> (args), result);
  dump_comma_argument (get<1> (args), result);
  dump_comma_argument (get<2> (args), result);
}

template <class Tuple>
inline void dump_arguments (const Tuple& args, string& result, ArgumentsCount<4>)
{
  dump_argument       (get<0> (args), result);
  dump_comma_argument (get<1> (args), result);
  dump_comma_argument (get<2> (args), result);
  dump_comma_argument (get<3> (args), result);
}

template <class Tuple>
inline void dump_arguments (const Tuple& args, string& result, ArgumentsCount<5>)
{
  dump_argument       (get<0> (args), result);
  dump_comma_argument (get<1> (args), result);
  dump_comma_argument (get<2> (args), result);
  dump_comma_argument (get<3> (args), result);
  dump_comma_argument (get<4> (args), result);
}

template <class T>
inline void dump_comma_argument (const T& value, string& result)
{
  result += ',';

  dump_argument (value, result);
}

inline void dump_argument (int value, string& result)
{
  result += format ("%d", value);  
}

inline void dump_argument (size_t value, string& result)
{
  result += format ("%u", value);  
}

inline void dump_argument (double value, string& result)
{
  result += format ("%.3f", value);
}

inline void dump_argument (float value, string& result)
{
  result += format ("%.3f", value);
}

inline void dump_argument (const void* ptr, string& result)
{
  result += format ("%p", ptr);
}

inline void dump_argument (const EnumWrapper& arg, string& result)
{
  const char* name = get_al_constant_name (arg.value);
  
  if (!name) result += format ("ALenum (%d)", arg.value);
  else       result += name;
}

template <class T>
inline void dump_argument (const ArrayWrapper<T>& arg, string& result)
{
  result += '{';

  if (arg.size)
  {
    dump_argument (arg.array [0], result);
    
    for (size_t i=1; i<arg.size; i++)
    {
      result += ',';
      
      dump_argument (arg.array [i], result);
    }
  }

  result += '}';
}

}

/*
    Конструктор / деструктор
*/

OpenALContext::OpenALContext  (const char* device_name, const LogHandler& in_log_handler)
  : log_handler (in_log_handler)
{
  if (!device_name)
    RaiseNullArgument ("sound::low_level::OpenALContext::OpenALContext", "device_name");
    
  device = alcOpenDevice (device_name);
  
  if (!device) 
    Raise<OpenALException> ("sound::low_level::OpenALContext::OpenALContext", "Can't open device '%s'", device_name);

  context = alcCreateContext (device, 0);

  if (!context)
  {
    alcCloseDevice (device);
    Raise<OpenALException> ("OpenALDevice::OpenALDevice", "Can't create context. %s", get_alc_error_message (alcGetError (device)));
  }
}

OpenALContext::~OpenALContext ()
{
  if (IsCurrent ())
    alcMakeContextCurrent (0);

  alcDestroyContext (context);

  ALCenum error = alcGetError (device);

  if (error != AL_NO_ERROR)
    LogError ("Error at alcDestroyContext(%p). %s", context, get_alc_error_message (error));

  if (!alcCloseDevice (device))
    LogError ("Error at alcCloseDevice(%p). %s", device, get_alc_error_message (alcGetError (device)));
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

  if (!alcMakeContextCurrent (context))
  {
    LogError ("Error at alcMakeContextCurrent(%p). %s", context, get_alc_error_message (alcGetError (device)));
    return false;
  }
  
  return true;
}

/*
    Протоколирование ошибок
*/

void OpenALContext::LogError (const char* message, ...)
{
  if (!log_handler)
    return;

  try
  {
    va_list list;

    va_start (list, message);
    
    log_handler (vformat (message, list).c_str ());
  }
  catch (...)
  {
    //подавляем все исключения
  }
}

/*
    Проверка ошибок после вызова функции
*/

template <class Tuple>
void OpenALContext::CheckErrors (const char* function_name, const Tuple& args)
{
  ALenum error = alGetError ();
  
  try
  {  
    if (error != AL_NO_ERROR)
    {
      string args_string;
      
      dump_arguments (args, args_string);
      
      LogError ("Error at call %s(%s). %s", function_name, args_string.c_str (), get_al_error_message (error));
    }
  }
  catch (...)
  {
    LogError ("Error at call %s(...). %s", function_name, get_al_error_message (error));
  }
}

/*
    Диспетчеры обращений к OpenAL
*/

template <class Fn, class Tuple>
void OpenALContext::Dispatch (const char* function_name, Fn fn, const Tuple& args)
{
  apply<void> (fn, args);
  CheckErrors (function_name, args);
}

template <class Ret, class Fn, class Tuple>
Ret OpenALContext::Dispatch (const char* function_name, Fn fn, const Tuple& args)
{
  Ret result = apply<Ret> (fn, args);

  CheckErrors (function_name, args);

  return result;
}

/*
    Обёртки над вызовами OpenAL
*/

void OpenALContext::alEnable (ALenum capability)
{
  Dispatch ("alEnable", &::alEnable, tie (make_wrapper (capability)));
}

void OpenALContext::alDisable (ALenum capability)
{
  Dispatch ("alDisable", &::alDisable, tie (make_wrapper (capability)));
}

ALboolean OpenALContext::alIsEnabled (ALenum capability)
{
  return Dispatch<ALboolean> ("alIsEnabled", &::alIsEnabled, tie (make_wrapper (capability)));
}

const ALchar* OpenALContext::alGetString (ALenum param)
{
  return Dispatch<const ALchar*> ("alGetString", &::alGetString, tie (make_wrapper (param)));
}

void OpenALContext::alGetBooleanv (ALenum param, ALboolean* data)
{
  Dispatch ("alGetBooleanv", &::alGetBooleanv, tie (make_wrapper (param), data));
}

void OpenALContext::alGetIntegerv (ALenum param, ALint* data)
{
  Dispatch ("alGetIntegerv", &::alGetIntegerv, tie (make_wrapper (param), data));
}

void OpenALContext::alGetFloatv (ALenum param, ALfloat* data)
{
  Dispatch ("alGetFloatv", &::alGetFloatv, tie (make_wrapper (param), data));
}

void OpenALContext::alGetDoublev (ALenum param, ALdouble* data)
{
  Dispatch ("alGetDoublev", &::alGetDoublev, tie (make_wrapper (param), data));
}

ALboolean OpenALContext::alGetBoolean (ALenum param)
{
  return Dispatch<ALboolean> ("alGetBoolean", &::alGetBoolean, tie (make_wrapper (param)));
}

ALint OpenALContext::alGetInteger (ALenum param)
{
  return Dispatch<ALint> ("alGetInteger", &::alGetInteger, tie (make_wrapper (param)));
}

ALfloat OpenALContext::alGetFloat (ALenum param)
{
  return Dispatch<ALfloat> ("alGetFloat", &::alGetFloat, tie (make_wrapper (param)));
}

ALdouble OpenALContext::alGetDouble (ALenum param)
{
  return Dispatch<ALdouble> ("alGetDouble", &::alGetDouble, tie (make_wrapper (param)));
}

void OpenALContext::alGenSources (ALsizei n, ALuint* sources)
{
  Dispatch ("alGenSources", &::alGenSources, tie (n, sources));
}

void OpenALContext::alDeleteSources (ALsizei n, const ALuint* sources)
{
  Dispatch ("alDeleteSources", &::alDeleteSources, tie (n, make_wrapper (n, sources)));
}

ALboolean OpenALContext::alIsSource (ALuint sid)
{
  return Dispatch<ALboolean> ("alIsSource", &::alIsSource, tie (sid));
}

void OpenALContext::alGetSourcef (ALuint sid, ALenum param, ALfloat* value)
{
  Dispatch ("alGetSourcef", &::alGetSourcef, tie (sid, make_wrapper (param), value));
}

void OpenALContext::alGetSource3f (ALuint sid, ALenum param, ALfloat* value1, ALfloat* value2, ALfloat* value3)
{
  Dispatch ("alGetSource3f", &::alGetSource3f, tie (sid, make_wrapper (param), value1, value2, value3));
}

void OpenALContext::alGetSourcefv (ALuint sid, ALenum param, ALfloat* values)
{
  Dispatch ("alGetSourcefv", &::alGetSourcefv, tie (sid, make_wrapper (param), values));
}

void OpenALContext::alGetSourcei (ALuint sid, ALenum param, ALint* value)
{
  Dispatch ("alGetSourcei", &::alGetSourcei, tie (sid, make_wrapper (param), value));
}

void OpenALContext::alGetSource3i (ALuint sid, ALenum param, ALint* value1, ALint* value2, ALint* value3)
{
  Dispatch ("alGetSource3i", &::alGetSource3i, tie (sid, make_wrapper (param), value1, value2, value3));
}

void OpenALContext::alGetSourceiv (ALuint sid, ALenum param, ALint* values)
{
  Dispatch ("alGetSourceiv", &::alGetSourceiv, tie (sid, make_wrapper (param), values));
}

void OpenALContext::alSourcePlayv (ALsizei ns, const ALuint *sids)
{
  Dispatch ("alSourcePlayv", &::alSourcePlayv, tie (ns, make_wrapper (ns, sids)));
}

void OpenALContext::alSourceStopv (ALsizei ns, const ALuint *sids)
{
  Dispatch ("alSourceStopv", &::alSourceStopv, tie (ns, make_wrapper (ns, sids)));
}

void OpenALContext::alSourceRewindv (ALsizei ns, const ALuint *sids)
{
  Dispatch ("alSourceRewindv", &::alSourceRewindv, tie (ns, make_wrapper (ns, sids)));
}

void OpenALContext::alSourcePausev (ALsizei ns, const ALuint *sids)
{
  Dispatch ("alSourcePausev", &::alSourcePausev, tie (ns, make_wrapper (ns, sids)));
}

void OpenALContext::alSourcePlay (ALuint sid)
{
  Dispatch ("alSourcePlay", &::alSourcePlay, tie (sid));
}

void OpenALContext::alSourceStop (ALuint sid)
{
  Dispatch ("alSourceStop", &::alSourceStop, tie (sid));
}

void OpenALContext::alSourceRewind (ALuint sid)
{
  Dispatch ("alSourceRewind", &::alSourceRewind, tie (sid));
}

void OpenALContext::alSourcePause (ALuint sid)
{
  Dispatch ("alSourcePause", &::alSourcePause, tie (sid));
}

void OpenALContext::alSourceQueueBuffers (ALuint sid, ALsizei n, const ALuint* buffers)
{
  Dispatch ("alSourceQueueBuffers", &::alSourceQueueBuffers, tie (sid, n, buffers));
}

void OpenALContext::alSourceUnqueueBuffers (ALuint sid, ALsizei n, ALuint* buffers)
{
  Dispatch ("alSourceUnqueueBuffers", &::alSourceUnqueueBuffers, tie (sid, n, buffers));
}

void OpenALContext::alListenerf (ALenum param, ALfloat value)
{
  Dispatch ("alListenerf", &::alListenerf, tie (make_wrapper (param), value));
}

void OpenALContext::alListener3f (ALenum param, ALfloat value1, ALfloat value2, ALfloat value3)
{
  Dispatch ("alListener3f", &::alListener3f, tie (make_wrapper (param), value1, value2, value3));
}

void OpenALContext::alListenerfv (ALenum param, const ALfloat* values)
{
  Dispatch ("alListenerfv", &::alListenerfv, tie (make_wrapper (param), values));
}

void OpenALContext::alListeneri (ALenum param, ALint value)
{
  Dispatch ("alListeneri", &::alListeneri, tie (make_wrapper (param), value));
}

void OpenALContext::alListener3i (ALenum param, ALint value1, ALint value2, ALint value3)
{
  Dispatch ("alListener3i", &::alListener3i, tie (make_wrapper (param), value1, value2, value3));
}

void OpenALContext::alListeneriv (ALenum param, const ALint* values)
{
  Dispatch ("alListeneriv", &::alListeneriv, tie (make_wrapper (param), values));
}

void OpenALContext::alGetListenerf (ALenum param, ALfloat* value)
{
  Dispatch ("alGetListenerf", &::alGetListenerf, tie (make_wrapper (param), value));
}

void OpenALContext::alGetListener3f (ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3)
{
  Dispatch ("alGetListener3f", &::alGetListener3f, tie (make_wrapper (param), value1, value2, value3));
}

void OpenALContext::alGetListenerfv (ALenum param, ALfloat* values)
{
  Dispatch ("alGetListenerfv", &::alGetListenerfv, tie (make_wrapper (param), values));
}

void OpenALContext::alGetListeneri (ALenum param, ALint* value)
{
  Dispatch ("alGetListeneri", &::alGetListeneri, tie (make_wrapper (param), value));
}

void OpenALContext::alGetListener3i (ALenum param, ALint *value1, ALint *value2, ALint *value3)
{
  Dispatch ("alGetListener3i", &::alGetListener3i, tie (make_wrapper (param), value1, value2, value3));
}

void OpenALContext::alGetListeneriv (ALenum param, ALint* values)
{
  Dispatch ("alGetListeneriv", &::alGetListeneriv, tie (make_wrapper (param), values));
}

void OpenALContext::alGenBuffers (ALsizei n, ALuint* buffers)
{
  Dispatch ("alGenBuffers", &::alGenBuffers, tie (n, buffers));
}

void OpenALContext::alDeleteBuffers (ALsizei n, const ALuint* buffers)
{
  Dispatch ("alDeleteBuffers", &::alDeleteBuffers, tie (n, make_wrapper (n, buffers)));
}

ALboolean OpenALContext::alIsBuffer (ALuint bid)
{
  return Dispatch<ALboolean> ("alIsBuffer", &::alIsBuffer, tie (bid));
}

void OpenALContext::alBufferData (ALuint buffer, ALenum format, const ALvoid *data, ALsizei size, ALsizei freq)
{
  Dispatch ("alBufferData", &::alBufferData, tie (buffer, make_wrapper (format), data, size, freq));
}

void OpenALContext::alBufferf (ALuint bid, ALenum param, ALfloat value)
{
  Dispatch ("alBufferf", &::alBufferf, tie (bid, make_wrapper (param), value));
}

void OpenALContext::alBuffer3f (ALuint bid, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3)
{
  Dispatch ("alBuffer3f", &::alBuffer3f, tie (bid, make_wrapper (param), value1, value2, value3));
}

void OpenALContext::alBufferfv (ALuint bid, ALenum param, const ALfloat* values)
{
  Dispatch ("alBufferfv", &::alBufferfv, tie (bid, param, values));
}

void OpenALContext::alBufferi (ALuint bid, ALenum param, ALint value)
{
  Dispatch ("alBufferi", &::alBufferi, tie (bid, make_wrapper (param), value));
}

void OpenALContext::alBuffer3i (ALuint bid, ALenum param, ALint value1, ALint value2, ALint value3)
{
  Dispatch ("alBuffer3i", &::alBuffer3i, tie (bid, make_wrapper (param), value1, value2, value3));
}

void OpenALContext::alBufferiv (ALuint bid, ALenum param, const ALint* values)
{
  Dispatch ("alBufferiv", &::alBufferiv, tie (bid, make_wrapper (param), values));
}

void OpenALContext::alGetBufferf (ALuint bid, ALenum param, ALfloat* value)
{
  Dispatch ("alGetBufferf", &::alGetBufferf, tie (bid, make_wrapper (param), value));
}

void OpenALContext::alGetBuffer3f (ALuint bid, ALenum param, ALfloat* value1, ALfloat* value2, ALfloat* value3)
{
  Dispatch ("alGetBuffer3f", &::alGetBuffer3f, tie (bid, make_wrapper (param), value1, value2, value3));
}

void OpenALContext::alGetBufferfv (ALuint bid, ALenum param, ALfloat* values)
{
  Dispatch ("alGetBufferfv", &::alGetBufferfv, tie (bid, make_wrapper (param), values));
}

void OpenALContext::alGetBufferi (ALuint bid, ALenum param, ALint* value)
{
  Dispatch ("alGetBufferi", &::alGetBufferi, tie (bid, make_wrapper (param), value));
}

void OpenALContext::alGetBuffer3i (ALuint bid, ALenum param, ALint* value1, ALint* value2, ALint* value3)
{
  Dispatch ("alGetBuffer3i", &::alGetBuffer3i, tie (bid, make_wrapper (param), value1, value2, value3));
}

void OpenALContext::alGetBufferiv (ALuint bid, ALenum param, ALint* values)
{
  Dispatch ("alGetBufferiv", &::alGetBufferiv, tie (bid, make_wrapper (param), values));
}

void OpenALContext::alDopplerFactor (ALfloat value)
{
  Dispatch ("alDopplerFactor", &::alDopplerFactor, tie (value));
}

void OpenALContext::alDopplerVelocity (ALfloat value)
{
  Dispatch ("alDopplerVelocity", &::alDopplerVelocity, tie (value));
}

void OpenALContext::alSpeedOfSound (ALfloat value)
{
  Dispatch ("alSpeedOfSound", &::alSpeedOfSound, tie (value));
}

void OpenALContext::alDistanceModel (ALenum distanceModel)
{
  Dispatch ("alDistanceModel", &::alDistanceModel, tie (make_wrapper (distanceModel)));
}
