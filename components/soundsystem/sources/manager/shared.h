#include <stl/hash_map>
#include <stl/string>
#include <xtl/signal.h>
#include <common/refcount.h>
#include <syslib/window.h>
#include <math/mathlib.h>
#include <sound/device.h>
#include <sound/manager.h>
#include <media/sound.h>
#include <media/sound_source.h>

using namespace sound;
using namespace sound::low_level;
using namespace stl;
using namespace xtl;
using namespace media;
using namespace math;

/*
   Описание реализации излучателя звука
*/

typedef signal<void (Emitter&, EmitterEvent)> EmitterSignal;

class sound::EmitterImpl : public common::ReferenceCounter
{
  public:
    EmitterImpl (SoundManagerImpl& in_manager);

  public:
    SoundManagerImpl& manager;                   //менеджер
    EmitterSignal     signals[EmitterEvent_Num]; //сигналы излучателя
    SoundSource       sound_source;              //звук
    SoundSample       sound_sample;              //сэмпл звука
    float             gain;                      //усиление
    float             group_gain;                //усиление группы
    bool              is_playing;                //проигрывание
    float             duration;                  //продолжительность
    vec3f             position;                  //позиция
    vec3f             direction;                 //направление
    vec3f             velocity;                  //скорость
    clock_t           play_time_start;           //время начала проигрывания
    clock_t           play_time_offset;          //смещение при проигрывании
};

/*
   Описание реализации группы проигрываемых звуков
*/

class sound::EmitterGroupImpl : public common::ReferenceCounter
{
  public:
    EmitterGroupImpl (const char* group_name);

  public:
    string              name;         //звук
    float               gain;         //усиление
};

/*
   Описание реализации звукового менеджера
*/

class sound::SoundManagerImpl
{
  public:
    SoundManagerImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление проигрыванием
///////////////////////////////////////////////////////////////////////////////////////////////////
    void  Play  (Emitter& emitter);
    void  Pause (Emitter& emitter);
    void  Stop  (Emitter& emitter);
    void  Seek  (Emitter& emitter, float position);
    float Tell  (Emitter& emitter) const;


  public:
    ISoundDevice*                   sound_device;   //устройство воспроизведения
    syslib::Window*                 window;         //окно
    float                           gain;           //усиление
    bool                            mute;           //проигрывание
    sound::Listener                 listener;       //слушатель
    hash_multimap <string, Emitter> emitters;       //источники звука
};
