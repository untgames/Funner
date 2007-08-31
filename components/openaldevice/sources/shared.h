#ifndef SOUND_SYSTEM_OPENAL_DEVICE_SHARED_HEADER
#define SOUND_SYSTEM_OPENAL_DEVICE_SHARED_HEADER

#include <stdarg.h>
#include <sound/openal_device.h>
#include <xtl/function.h>
#include <xtl/tuple>
#include <stl/string>
#include <stl/vector>
#include <common/strlib.h>
#include <common/exception.h>
#include <al.h>
#include <alc.h>
#include <media/sound.h>
#include <syslib/timer.h>

namespace sound
{

namespace low_level
{

struct OpenALExceptionTag;

typedef common::DerivedException<common::Exception, OpenALExceptionTag> OpenALException;

const float  BUFFER_UPDATE_TIME = 0.3f;
const float  BUFFER_TIME = 0.5f;
const size_t MAX_SOUND_FREQUENCY = 48000;
const size_t MAX_SOUND_CHANNELS  = 2;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Cистема воспроизведения звука, реализованная через OpenAL
///////////////////////////////////////////////////////////////////////////////////////////////////
struct OpenALDevice : public sound::low_level::ISoundDevice
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор/деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    OpenALDevice  (const char* device_name = NULL);
    ~OpenALDevice ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Имя устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение информации об устройстве
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GetCapabilities (Capabilities&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Количество микшируемых каналов
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t ChannelsCount ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка текущего проигрываемого звука
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetSample (size_t channel, const char* sample_name);
    const char* GetSample (size_t channel);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка цикличности проигрывания канала
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsLooped (size_t channel);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка параметров источника
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetSource (size_t channel, const Source&);
    void GetSource (size_t channel, Source&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление проигрыванием
///////////////////////////////////////////////////////////////////////////////////////////////////
    void  Play  (size_t channel, bool looping = false);
    void  Pause (size_t channel);
    void  Stop  (size_t channel);
    void  Seek  (size_t channel, float time_in_seconds);
    float Tell  (size_t channel);
    bool  IsPlaying (size_t channel);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка уровня громкости для устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
    void  SetVolume (float gain);
    float GetVolume ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Блокировка проигрывания звука
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetMute (bool state);
    bool IsMuted ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка параметров слушателя
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetListener (const Listener&);
    void GetListener (Listener&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка функции отладочного протоколирования
///////////////////////////////////////////////////////////////////////////////////////////////////
    void              SetDebugLog (const LogHandler&);
    const LogHandler& GetDebugLog ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление подсчётом ссылок
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef  ();
    void Release ();

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Обновление буфферов звука
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateBuffers ();

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Контекст OpenAL
///////////////////////////////////////////////////////////////////////////////////////////////////
class OpenALContext
{
  public:
    typedef OpenALDevice::LogHandler LogHandler;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    OpenALContext  (const char* device_name, const LogHandler& log_handler);
    ~OpenALContext ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление текущими контекстами
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsCurrent   () const;
    bool MakeCurrent ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обёртки над вызовами OpenAL
///////////////////////////////////////////////////////////////////////////////////////////////////
    void alEnable( ALenum capability );
    void alDisable( ALenum capability ); 
    ALboolean alIsEnabled( ALenum capability );
    const ALchar* alGetString( ALenum param );
    void alGetBooleanv( ALenum param, ALboolean* data );
    void alGetIntegerv( ALenum param, ALint* data );
    void alGetFloatv( ALenum param, ALfloat* data );
    void alGetDoublev( ALenum param, ALdouble* data );
    ALboolean alGetBoolean( ALenum param );
    ALint alGetInteger( ALenum param );
    ALfloat alGetFloat( ALenum param );
    ALdouble alGetDouble( ALenum param );
    void alGenSources( ALsizei n, ALuint* sources );     
    void alDeleteSources( ALsizei n, const ALuint* sources );
    ALboolean alIsSource( ALuint sid );      
    void alGetSourcef( ALuint sid, ALenum param, ALfloat* value );
    void alGetSource3f( ALuint sid, ALenum param, ALfloat* value1, ALfloat* value2, ALfloat* value3);
    void alGetSourcefv( ALuint sid, ALenum param, ALfloat* values );
    void alGetSourcei( ALuint sid,  ALenum param, ALint* value );
    void alGetSource3i( ALuint sid, ALenum param, ALint* value1, ALint* value2, ALint* value3);
    void alGetSourceiv( ALuint sid,  ALenum param, ALint* values );
    void alSourcef( ALuint sid, ALenum param, ALfloat value ); 
    void alSource3f( ALuint sid, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3 );
    void alSourcefv( ALuint sid, ALenum param, const ALfloat* values ); 
    void alSourcei( ALuint sid, ALenum param, ALint value ); 
    void alSource3i( ALuint sid, ALenum param, ALint value1, ALint value2, ALint value3 );
    void alSourceiv( ALuint sid, ALenum param, const ALint* values );
    void alSourcePlayv( ALsizei ns, const ALuint *sids );
    void alSourceStopv( ALsizei ns, const ALuint *sids );
    void alSourceRewindv( ALsizei ns, const ALuint *sids );
    void alSourcePausev( ALsizei ns, const ALuint *sids );
    void alSourcePlay( ALuint sid );
    void alSourceStop( ALuint sid );
    void alSourceRewind( ALuint sid );
    void alSourcePause( ALuint sid );
    void alSourceQueueBuffers( ALuint sid, ALsizei numEntries, const ALuint *bids );
    void alSourceUnqueueBuffers( ALuint sid, ALsizei numEntries, ALuint *bids );     
    void alListenerf( ALenum param, ALfloat value );
    void alListener3f( ALenum param, ALfloat value1, ALfloat value2, ALfloat value3 );
    void alListenerfv( ALenum param, const ALfloat* values ); 
    void alListeneri( ALenum param, ALint value );
    void alListener3i( ALenum param, ALint value1, ALint value2, ALint value3 );
    void alListeneriv( ALenum param, const ALint* values );
    void alGetListenerf( ALenum param, ALfloat* value );
    void alGetListener3f( ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3 );
    void alGetListenerfv( ALenum param, ALfloat* values );
    void alGetListeneri( ALenum param, ALint* value );
    void alGetListener3i( ALenum param, ALint *value1, ALint *value2, ALint *value3 );
    void alGetListeneriv( ALenum param, ALint* values );
    void alGenBuffers( ALsizei n, ALuint* buffers );
    void alDeleteBuffers( ALsizei n, const ALuint* buffers );
    ALboolean alIsBuffer( ALuint bid );
    void alBufferData( ALuint bid, ALenum format, const ALvoid* data, ALsizei size, ALsizei freq );
    void alBufferf( ALuint bid, ALenum param, ALfloat value );
    void alBuffer3f( ALuint bid, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3 );
    void alBufferfv( ALuint bid, ALenum param, const ALfloat* values );
    void alBufferi( ALuint bid, ALenum param, ALint value );
    void alBuffer3i( ALuint bid, ALenum param, ALint value1, ALint value2, ALint value3 );
    void alBufferiv( ALuint bid, ALenum param, const ALint* values );
    void alGetBufferf( ALuint bid, ALenum param, ALfloat* value );
    void alGetBuffer3f( ALuint bid, ALenum param, ALfloat* value1, ALfloat* value2, ALfloat* value3);
    void alGetBufferfv( ALuint bid, ALenum param, ALfloat* values );
    void alGetBufferi( ALuint bid, ALenum param, ALint* value );
    void alGetBuffer3i( ALuint bid, ALenum param, ALint* value1, ALint* value2, ALint* value3);
    void alGetBufferiv( ALuint bid, ALenum param, ALint* values );
    void alDopplerFactor( ALfloat value );
    void alDopplerVelocity( ALfloat value );
    void alSpeedOfSound( ALfloat value );
    void alDistanceModel( ALenum distanceModel );

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Функция протоколирования
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LogError (const char* message, ...);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Диспетчеры обращений к OpenAL
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Fn, class Tuple>
    void Dispatch (const char* function_name, Fn fn, const Tuple& args);

    template <class Ret, class Fn, class Tuple>
    Ret Dispatch (const char* function_name, Fn fn, const Tuple& args);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка ошибок после вызова функции
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Tuple>
    void CheckErrors (const char* function_name, const Tuple& args);

  private:
    OpenALContext (const OpenALContext&); //no impl
    OpenALContext& operator = (const OpenALContext&); //no impl

  private:
    ALCdevice*  device;       //устройство OpenAL
    ALCcontext* context;      //контекст OpenAL
    LogHandler  log_handler;  //функтор протоколирования
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Источник звука OpenAL
///////////////////////////////////////////////////////////////////////////////////////////////////
struct OpenALSource
{
  OpenALSource  (OpenALContext* in_context, char* in_buffer);
  ~OpenALSource ();

  medialib::SoundSample sound_sample;    //звук
  Source      source;          //общий источник звука
  bool        looping;         //зацикленность
  bool        playing;         //источник играет
  bool        play_from_start; //нужно ли делать rewind при вызове play
  size_t      name;            //имя источника в OpenAL
  size_t      buffer_name[2];  //OpenAL буффер
  size_t      buffer_samples;  //количество сэмплов в буффере
  size_t      last_sample;     //номер последнего прочитанного сэмпла
  size_t      start_sample;    //номер начального сэмпла проигрывания
  clock_t     play_start_time; //время последнего seek
  size_t      first_buffer;    //номер первого в очереди буффера    
  char*       buffer;          //указатель на общий буффер декодированного звука
  OpenALContext *context;      //контекст
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание реализации OpenALDevice
///////////////////////////////////////////////////////////////////////////////////////////////////
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
  size_t                 ref_count;     //количество ссылок
  char*                  buffer;        //буффер декодированного звука
  stl::vector <OpenALSource*> sources;  //источники звука

  Impl (const char* device_name, OpenALDevice* sound_system);
  ~Impl ();
};

}

}

#endif
