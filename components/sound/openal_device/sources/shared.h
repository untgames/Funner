#ifndef SOUND_SYSTEM_OPENAL_DEVICE_SHARED_HEADER
#define SOUND_SYSTEM_OPENAL_DEVICE_SHARED_HEADER

#include <stdarg.h>
#include <AL/al.h>
#include <AL/alc.h>

#include <stl/auto_ptr.h>
#include <stl/string>
#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/uninitialized_storage.h>
#include <xtl/ref.h>
#include <xtl/reference_counter.h>
#include <xtl/string.h>
#include <xtl/common_exceptions.h>

#include <common/action_queue.h>
#include <common/component.h>
#include <common/log.h>
#include <common/strlib.h>
#include <common/time.h>

#include <media/sound.h>

#include <sound/device.h>
#include <sound/driver.h>

namespace sound
{

namespace low_level
{

namespace openal
{

//implementation forwards
class OpenALDevice;
class OpenALSource;
class OpenALContext;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Константы
///////////////////////////////////////////////////////////////////////////////////////////////////
const size_t DEVICE_BUFFERS_POOL_SIZE        = 32;     //размер пула буферов
const size_t MAX_SOUND_SAMPLE_RATE           = 176400; //максимальная частота дискретизации (Generic устройства проигрывают 192000 некорректно)
const size_t SOURCE_BUFFERS_COUNT            = 4;      //количество буферов проигрывания на источник
const size_t SOURCE_BUFFERS_UPDATE_FREQUENCY = 10;     //частота обновления буферов

///////////////////////////////////////////////////////////////////////////////////////////////////
///Исключение OpenAL
///////////////////////////////////////////////////////////////////////////////////////////////////
struct OpenALException: virtual public xtl::exception {};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Контекст OpenAL
///////////////////////////////////////////////////////////////////////////////////////////////////
class OpenALContext
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    OpenALContext  (const char* device_name, const char* init_string = "");
    ~OpenALContext ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление текущими контекстами
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsCurrent   () const;
    bool MakeCurrent ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обёртки над вызовами OpenAL
///////////////////////////////////////////////////////////////////////////////////////////////////
    void alEnable (ALenum capability);
    void alDisable (ALenum capability);
    ALboolean alIsEnabled (ALenum capability);
    const ALchar* alGetString (ALenum param);
    void alGetBooleanv (ALenum param, ALboolean* data);
    void alGetIntegerv (ALenum param, ALint* data);
    void alGetFloatv (ALenum param, ALfloat* data);
    void alGetDoublev (ALenum param, ALdouble* data);
    ALboolean alGetBoolean (ALenum param);
    ALint alGetInteger (ALenum param);
    ALfloat alGetFloat (ALenum param);
    ALdouble alGetDouble (ALenum param);
    void alGenSources (ALsizei n, ALuint* sources);
    void alDeleteSources (ALsizei n, const ALuint* sources);
    ALboolean alIsSource (ALuint sid);
    void alGetSourcef (ALuint sid, ALenum param, ALfloat* value);
    void alGetSource3f (ALuint sid, ALenum param, ALfloat* value1, ALfloat* value2, ALfloat* value3);
    void alGetSourcefv (ALuint sid, ALenum param, ALfloat* values);
    void alGetSourcei (ALuint sid, ALenum param, ALint* value);
    void alGetSource3i (ALuint sid, ALenum param, ALint* value1, ALint* value2, ALint* value3);
    void alGetSourceiv (ALuint sid, ALenum param, ALint* values);
    void alSourcef (ALuint sid, ALenum param, ALfloat value);
    void alSource3f (ALuint sid, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3);
    void alSourcefv (ALuint sid, ALenum param, const ALfloat* values);
    void alSourcei (ALuint sid, ALenum param, ALint value);
    void alSource3i (ALuint sid, ALenum param, ALint value1, ALint value2, ALint value3);
    void alSourceiv (ALuint sid, ALenum param, const ALint* values);
    void alSourcePlayv (ALsizei ns, const ALuint *sids);
    void alSourceStopv (ALsizei ns, const ALuint *sids);
    void alSourceRewindv (ALsizei ns, const ALuint *sids);
    void alSourcePausev (ALsizei ns, const ALuint *sids);
    void alSourcePlay (ALuint sid);
    void alSourceStop (ALuint sid);
    void alSourceRewind (ALuint sid);
    void alSourcePause (ALuint sid);
    void alSourceQueueBuffers (ALuint sid, ALsizei numEntries, const ALuint *bids);
    void alSourceUnqueueBuffers (ALuint sid, ALsizei numEntries, ALuint *bids);
    void alListenerf (ALenum param, ALfloat value);
    void alListener3f (ALenum param, ALfloat value1, ALfloat value2, ALfloat value3);
    void alListenerfv (ALenum param, const ALfloat* values);
    void alListeneri (ALenum param, ALint value);
    void alListener3i (ALenum param, ALint value1, ALint value2, ALint value3);
    void alListeneriv (ALenum param, const ALint* values);
    void alGetListenerf (ALenum param, ALfloat* value);
    void alGetListener3f (ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3);
    void alGetListenerfv (ALenum param, ALfloat* values);
    void alGetListeneri (ALenum param, ALint* value);
    void alGetListener3i (ALenum param, ALint *value1, ALint *value2, ALint *value3);
    void alGetListeneriv (ALenum param, ALint* values);
    void alGenBuffers (ALsizei n, ALuint* buffers);
    void alDeleteBuffers (ALsizei n, const ALuint* buffers);
    ALboolean alIsBuffer (ALuint bid);
    void alBufferData (ALuint bid, ALenum format, const ALvoid* data, ALsizei size, ALsizei freq);
    void alBufferf (ALuint bid, ALenum param, ALfloat value);
    void alBuffer3f (ALuint bid, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3);
    void alBufferfv (ALuint bid, ALenum param, const ALfloat* values);
    void alBufferi (ALuint bid, ALenum param, ALint value);
    void alBuffer3i (ALuint bid, ALenum param, ALint value1, ALint value2, ALint value3);
    void alBufferiv (ALuint bid, ALenum param, const ALint* values);
    void alGetBufferf (ALuint bid, ALenum param, ALfloat* value);
    void alGetBuffer3f (ALuint bid, ALenum param, ALfloat* value1, ALfloat* value2, ALfloat* value3);
    void alGetBufferfv (ALuint bid, ALenum param, ALfloat* values);
    void alGetBufferi (ALuint bid, ALenum param, ALint* value);
    void alGetBuffer3i (ALuint bid, ALenum param, ALint* value1, ALint* value2, ALint* value3);
    void alGetBufferiv (ALuint bid, ALenum param, ALint* values);
    void alDopplerFactor (ALfloat value);
    void alDopplerVelocity (ALfloat value);
    void alSpeedOfSound (ALfloat value);
    void alDistanceModel (ALenum distanceModel);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обёртки над вызовами OpenALContext
///////////////////////////////////////////////////////////////////////////////////////////////////
    ALCboolean alcIsExtensionPresent (const ALCchar *extname);
    void       alcGetIntegerv        (ALCenum param, ALCsizei size, ALCint *data);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка ошибок после вызова функции
///////////////////////////////////////////////////////////////////////////////////////////////////
    void CheckErrors (const char* function_name);
    void ContextCheckErrors (const char* function_name);

  private:
    OpenALContext (const OpenALContext&); //no impl
    OpenALContext& operator = (const OpenALContext&); //no impl

  private:
    ALCdevice*  device;          //устройство OpenAL
    ALCcontext* context;         //контекст OpenAL
    bool        efx_present;     //наличие EFX
    common::Log log;             //протокол
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Класс для декодирования звука
///////////////////////////////////////////////////////////////////////////////////////////////////
class ISampleDecoder
{
  public:
//////////////////////////////////////////////////////////////////////////////////////////////////
///Чтение декодированного звука количеством samples_count сэмплов начиная с first sample в data
//////////////////////////////////////////////////////////////////////////////////////////////////
    virtual size_t Read (size_t first_sample, size_t samples_count, void* data) = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////
///Клонирование
//////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ISampleDecoder* Clone () = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////
///Подсчёт ссылок
//////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void AddRef () = 0;
    virtual void Release () = 0;

  protected:
    virtual ~ISampleDecoder () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Класс работы с сэмплами работающий через предоставленную функцию
///////////////////////////////////////////////////////////////////////////////////////////////////
class FunctionSampleDecoder : public ISampleDecoder, public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор/деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    FunctionSampleDecoder (const IDevice::SampleReadFunction& fn);
    ~FunctionSampleDecoder ();

//////////////////////////////////////////////////////////////////////////////////////////////////
///Чтение декодированного звука количеством samples_count сэмплов начиная с first sample в data
//////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Read (size_t first_sample, size_t samples_count, void* data);

//////////////////////////////////////////////////////////////////////////////////////////////////
///Клонирование
//////////////////////////////////////////////////////////////////////////////////////////////////
    FunctionSampleDecoder* Clone ();

//////////////////////////////////////////////////////////////////////////////////////////////////
///Подсчёт ссылок
//////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef () { addref (this); }
    void Release () { release (this); }

  private:
    FunctionSampleDecoder (const FunctionSampleDecoder&);             //no impl
    FunctionSampleDecoder operator = (const FunctionSampleDecoder&);  //no impl

  private:
    struct Impl;

  private:
    FunctionSampleDecoder (Impl* impl);

  private:
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Класс работы с сэмплами на основе media::Sample
///////////////////////////////////////////////////////////////////////////////////////////////////
class MediaSampleDecoder : public ISampleDecoder, public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    MediaSampleDecoder (const char* name);

//////////////////////////////////////////////////////////////////////////////////////////////////
///Чтение декодированного звука количеством samples_count сэмплов начиная с first sample в data
//////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Read (size_t first_sample, size_t samples_count, void* data);

//////////////////////////////////////////////////////////////////////////////////////////////////
///Клонирование
//////////////////////////////////////////////////////////////////////////////////////////////////
    MediaSampleDecoder* Clone ();

//////////////////////////////////////////////////////////////////////////////////////////////////
///Подсчёт ссылок
//////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef () { addref (this); }
    void Release () { release (this); }

  private:
    MediaSampleDecoder (const media::SoundSample& sample);

  private:
    MediaSampleDecoder (const MediaSampleDecoder&);             //no impl
    MediaSampleDecoder operator = (const MediaSampleDecoder&);  //no impl

  private:
    media::SoundSample sample;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Класс работы с сэмплами, хранящий декодированный буфер звука
///////////////////////////////////////////////////////////////////////////////////////////////////
class BufferedSampleDecoder : public ISampleDecoder, public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы/деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    BufferedSampleDecoder (const char* name);
    BufferedSampleDecoder (const SampleDesc& desc, const IDevice::SampleReadFunction& fn);
    ~BufferedSampleDecoder ();

//////////////////////////////////////////////////////////////////////////////////////////////////
///Чтение декодированного звука количеством samples_count сэмплов начиная с first sample в data
//////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Read (size_t first_sample, size_t samples_count, void* data);

//////////////////////////////////////////////////////////////////////////////////////////////////
///Клонирование
//////////////////////////////////////////////////////////////////////////////////////////////////
    BufferedSampleDecoder* Clone ();

//////////////////////////////////////////////////////////////////////////////////////////////////
///Подсчёт ссылок
//////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef () { addref (this); }
    void Release () { release (this); }

  private:
    BufferedSampleDecoder (const BufferedSampleDecoder&);            //no impl
    BufferedSampleDecoder operator = (const BufferedSampleDecoder&); //no impl

  private:
    struct Impl;

  private:
    BufferedSampleDecoder (Impl* impl);

  private:
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Базовый класс работы с сэмплами
///////////////////////////////////////////////////////////////////////////////////////////////////
class OpenALSample : public ISample, public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы
///////////////////////////////////////////////////////////////////////////////////////////////////
    OpenALSample (const char* name);
    OpenALSample (const SampleDesc& desc, const IDevice::SampleReadFunction& fn);

//////////////////////////////////////////////////////////////////////////////////////////////////
///Получение имени сэмпла
//////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetName ();

//////////////////////////////////////////////////////////////////////////////////////////////////
///Получение дескриптора сэмпла
//////////////////////////////////////////////////////////////////////////////////////////////////
    void GetDesc (SampleDesc& desc);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Функции расчёта одних характеристик через другие (всегда происходит округление в меньшую сторону)
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t SamplesToBytes   (size_t sample_count);   //перевод количества сэмплов в размер в байтах
    size_t BytesToSamples   (size_t byte_count);     //перевод количества байт в размер в сэмплах
    double SamplesToSeconds (size_t sample_count);   //перевод количества сэмплов в размер в секундах
    size_t SecondsToSamples (double second_count);   //переводколичества секунд в размер в сэмплах

//////////////////////////////////////////////////////////////////////////////////////////////////
///Получение параметров
//////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Channels     ();
    size_t Frequency    ();
    size_t SamplesCount ();
    double Duration     ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание декодера
///////////////////////////////////////////////////////////////////////////////////////////////////
    ISampleDecoder* CreateDecoder ();

//////////////////////////////////////////////////////////////////////////////////////////////////
///Подсчёт ссылок
//////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef () { addref (this); }
    void Release () { release (this); }

  private:
    typedef xtl::com_ptr<ISampleDecoder> SampleDecoderPtr;

  private:
    SampleDesc       desc;
    SampleDecoderPtr decoder;
    stl::string      name;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Cистема воспроизведения звука, реализованная через OpenAL
///////////////////////////////////////////////////////////////////////////////////////////////////
class OpenALDevice : public sound::low_level::IDevice, public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор/деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    OpenALDevice  (const char* driver_name, const char* device_name, const char* init_string = "");
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
    size_t ChannelsCount () { return channels.size (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание сэмпла для проигрывания
///////////////////////////////////////////////////////////////////////////////////////////////////
    ISample* CreateSample (const char* name);
    ISample* CreateSample (const SampleDesc& desc, const SampleReadFunction& fn);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка текущего проигрываемого звука
///////////////////////////////////////////////////////////////////////////////////////////////////
    void     SetSample (size_t channel, ISample* sample);
    ISample* GetSample (size_t channel);

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
    void  Seek  (size_t channel, float time_in_seconds, SeekMode seek_mode);
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
///Работа со списком активных источников
///////////////////////////////////////////////////////////////////////////////////////////////////
    void          SetFirstActiveSource (OpenALSource* source) { first_active_source = source; }
    OpenALSource* GetFirstActiveSource () const { return first_active_source; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///OpenAL контекст
///////////////////////////////////////////////////////////////////////////////////////////////////
    const OpenALContext& Context () const { return context; }
          OpenALContext& Context ()       { return context; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Буфер сэмплирования
///////////////////////////////////////////////////////////////////////////////////////////////////
    const void* GetSampleBuffer     () const { return sample_buffer.data (); }
          void* GetSampleBuffer     ()       { return sample_buffer.data (); }
    size_t      GetSampleBufferSize () const { return sample_buffer.size (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление распределением буферов проигрывания
///////////////////////////////////////////////////////////////////////////////////////////////////
    ALuint AllocateSourceBuffer   ();
    void   DeallocateSourceBuffer (ALuint);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление подсчётом ссылок
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef  ();
    void Release ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка параметров устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetParamsNames  ();
    bool        IsIntegerParam  (const char* name);
    bool        IsStringParam   (const char* name);
    void        SetIntegerParam (const char* name, int value);
    int         GetIntegerParam (const char* name);
    void        SetStringParam  (const char* name, const char* value);
    const char* GetStringParam  (const char* name);

  private:
    void BufferUpdate ();
    void ListenerUpdate ();
    void SourceUpdate ();
    void UpdateListenerNotify ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Удаление каналов и буферов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ClearALData ();

    OpenALDevice (const OpenALDevice&);
    OpenALDevice& operator = (const OpenALDevice&);

  private:
    typedef xtl::uninitialized_storage<char> SampleBuffer;
    typedef stl::vector<OpenALSource*>       ChannelsArray;

  private:
    stl::string    name;                                       //имя устройства
    OpenALContext  context;                                    //контекст
    common::Action buffer_action;                              //таймер обновления буфера
    common::Action listener_action;                            //таймер обновления слушателя
    common::Action source_action;                              //таймер обновления источников
    Capabilities   info;                                       //информация о устройстве
    Listener       listener;                                   //слушатель
    bool           listener_need_update;                       //слушатель требует обновления
    SampleBuffer   sample_buffer;                              //буфер сэмплирования
    float          gain;                                       //коэффициент усиления
    bool           is_muted;                                   //флг блокировки проигрывания
    ChannelsArray  channels;                                   //каналы проигрывания
    size_t         buffer_update_frequency;                    //частота обновления буффера
    size_t         source_properties_update_frequency;         //частота обновления свойств источника
    size_t         listener_properties_update_frequency;       //частота обновления свойств слушателя
    OpenALSource*  first_active_source;                        //первый активный источник
    ALuint         al_buffers_pool [DEVICE_BUFFERS_POOL_SIZE]; //пул буферов
    size_t         al_buffers_pool_size;                       //размер пула буферов
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Источник звука OpenAL
///////////////////////////////////////////////////////////////////////////////////////////////////
class OpenALSource
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    OpenALSource  (OpenALDevice& device);
    ~OpenALSource ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка текущего проигрываемого звука
///////////////////////////////////////////////////////////////////////////////////////////////////
    void          SetSample (OpenALSample* sample);
    OpenALSample* GetSample () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка цикличности проигрывания канала
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsLooped () const { return is_looped; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка параметров источника
///////////////////////////////////////////////////////////////////////////////////////////////////
    void          SetSource (const Source&);
    const Source& GetSource () const { return source; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление проигрыванием
///////////////////////////////////////////////////////////////////////////////////////////////////
    void  Play      (bool looping = false);
    void  Pause     ();
    void  Stop      ();
    void  Seek      (float time_in_seconds, SeekMode seek_mode);
    float Tell      () const;
    bool  IsPlaying () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обновление (синхронизация с OpenAL)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void BufferUpdate ();
    void PropertiesUpdate ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа со списком активных источников
///////////////////////////////////////////////////////////////////////////////////////////////////
    OpenALSource* PrevActive () const { return prev_active; }
    OpenALSource* NextActive () const { return next_active; }

  private:
    void   FillBuffer (ALuint al_buffer);
    void   FillBuffers ();
    void   UpdateSourceNotify ();
    void   UpdateSampleNotify ();
    size_t TellInMilliseconds () const;
    void   Activate ();
    void   Deactivate ();

  private:
    OpenALSource (const OpenALSource&); //no impl
    OpenALSource& operator = (const OpenALSource&); //no impl

  private:
    typedef xtl::com_ptr<OpenALSample>   OpenALSamplePtr;
    typedef xtl::com_ptr<ISampleDecoder> SampleDecoderPtr;

  private:
    OpenALDevice&    device;                            //устройство проигрывания
    OpenALSamplePtr  sound_sample;                      //звук
    SampleDecoderPtr sound_sample_decoder;              //декодер звука
    Source           source;                            //параметры источника звука
    bool             source_need_update;                //источник звука требует обновления
    bool             sample_need_update;                //необходимо обновить позицию проигрывания
    bool             is_looped;                         //цикличность проигрывания
    bool             is_playing;                        //проигрывается ли звук
    bool             is_active;                         //является ли источник активным
    size_t           play_time_start;                   //время начала проигрывания
    size_t           play_time_offset;                  //смещение при проигрывании
    size_t           last_buffers_fill_time;            //время последнего обновления буферов
    size_t           play_sample_position;              //позиция проигрывания
    ALuint           al_source;                         //имя источника в OpenAL
    ALuint           al_buffers [SOURCE_BUFFERS_COUNT]; //OpenAL буферы
    OpenALSource     *prev_active, *next_active;        //список активных источников (проигрываемых)
    common::Log      log;                               //протокол
};

const char* get_al_constant_name (ALenum value);

}

}

}

#endif
