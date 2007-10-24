#include <time.h>
#include <sound/manager.h>
#include <sound/device.h>
#include <stl/hash_map>
#include <stl/algorithm>
#include <stl/stack>
#include <stl/string>
#include <xtl/intrusive_ptr.h>
#include <xtl/signal.h>
#include <xtl/bind.h>
#include <syslib/window.h>
#include <syslib/timer.h>
#include <common/exception.h>
#include <media/sound.h>
#include <media/sound_declaration.h>

using namespace sound;
using namespace sound::low_level;
using namespace syslib;
using namespace stl;
using namespace xtl;
using namespace common;
using namespace media;

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

const float DEFAULT_EMITTER_PROPERTIES_UPDATE_PERIOD = 0.03f;

/*
    Описание реализации SoundManager
*/

struct SoundManagerEmitter
{
  int               channel_number;     //номер канала проигрывания (-1 - отсечён или не проигрывается)
  float             start_offset;       //нормализованный сдвиг начала проигрывания
  float             start_position;     //позиция начала проигрывания в секундах
  float             cur_position;       //текущая позиция проигрывания в секундах
  clock_t           play_start_time;    //время начала проигрывания
  bool              is_playing;         //статус проигрывания
  SoundDeclaration* sound_declaration;  //описание звука
  SoundSample       sound_sample;       //звуковой сэмпл (используется для определения длительности звука)
  size_t            sample_number;      //номер проигрываемого сэмпла
  bool              sample_chosen;      //эммитер ещё не проигрывался
  Source            source;             //излучатель звука

  SoundManagerEmitter  (float normalized_offset);
};

typedef stl::hash_map<Emitter*, SoundManagerEmitter> EmitterSet;
typedef xtl::com_ptr<low_level::ISoundDevice>        DevicePtr;
typedef stl::stack<size_t>                           ChannelsSet;

struct SoundManager::Impl
{
  Window&                     window;                   //окно
  DevicePtr                   device;                   //устройство воспроизведения
  sound::WindowMinimizeAction minimize_action;          //поведение при сворачивании окна
  float                       volume;                   //добавочная громкость
  bool                        is_muted;                 //флаг блокировки проигрывания звука
  bool                        was_muted;                //предыдущее состояние флага блокировки проигрывания звука
  sound::Listener             listener;                 //параметры слушателя
  EmitterSet                  emitters;                 //излучатели звука
  ChannelsSet                 free_channels;            //номера свободных каналов
  Capabilities                capabilities;             //возможности устройства
  auto_connection             minimize_connection;      //соединения события потери фокуса
  auto_connection             maximize_connection;      //соединения события получения фокуса
  auto_connection             change_handle_connection; //соединение собятия изменения хэндла
  SoundDeclarationLibrary     sound_decl_library;       //библиотека описаний звуков
  string                      target_configuration;     //конфигурация устройства вывода
  string                      init_string;              //строка инициализации
  Timer                       emitter_timer;            //таймер обновления эмиттеров

  Impl (Window& target_window, const char* in_target_configuration, const char* in_init_string);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Инициализация устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
  void Init ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Блокировка проигрывания звука
///////////////////////////////////////////////////////////////////////////////////////////////////
  void SetMute (bool state);

///////////////////////////////////////////////////////////////////////////////////////////////////
////Функция, вызываемая при сворачивании окна
///////////////////////////////////////////////////////////////////////////////////////////////////
  void OnMinimize (Window& window, WindowEvent event, const WindowEventContext& context);
  void OnMaximize (Window& window, WindowEvent event, const WindowEventContext& context);

///////////////////////////////////////////////////////////////////////////////////////////////////
////Функция, вызываемая при изменении хэндла
///////////////////////////////////////////////////////////////////////////////////////////////////
  void OnChangeHandle (Window& in_window, WindowEvent event, const WindowEventContext& context);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обновление свойств эмиттеров
///////////////////////////////////////////////////////////////////////////////////////////////////
  void EmitterUpdate ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проигрывание звуков
///////////////////////////////////////////////////////////////////////////////////////////////////
  void PlaySound    (Emitter& emitter);
  void PauseSound   (Emitter& emitter);
  void StopSound    (Emitter& emitter);
  void StopPlaying  (EmitterSet::iterator emitter_iter);

  float GetNormalizedOffset (Emitter& emitter);
};

SoundManagerEmitter::SoundManagerEmitter (float normalized_offset)
  : channel_number (-1), is_playing (false), start_offset (normalized_offset), sample_chosen (false)
  {}

SoundManager::Impl::Impl (Window& target_window, const char* in_target_configuration, const char* in_init_string)
  : window (target_window), minimize_action (WindowMinimizeAction_Ignore),
    volume (1.f),
    target_configuration (in_target_configuration),
    init_string (in_init_string),
    minimize_connection (window.RegisterEventHandler (WindowEvent_OnLostFocus, bind (&SoundManager::Impl::OnMinimize, this, _1, _2, _3))),
    maximize_connection (window.RegisterEventHandler (WindowEvent_OnSetFocus,  bind (&SoundManager::Impl::OnMaximize, this, _1, _2, _3))),
    change_handle_connection (window.RegisterEventHandler (WindowEvent_OnChangeHandle, bind (&SoundManager::Impl::OnChangeHandle, this, _1, _2, _3))),
    emitter_timer (xtl::bind (&SoundManager::Impl::EmitterUpdate, this), (size_t)(DEFAULT_EMITTER_PROPERTIES_UPDATE_PERIOD * 1000))
{
  Init ();
}

/*
   Инициализация устройства
*/

void SoundManager::Impl::Init ()
{
  try
  {
    device = SoundSystem::CreateDevice (target_configuration.c_str (), &window, init_string.c_str ());

    if (device)
    {
      device->GetCapabilities (capabilities);
      
      for (size_t i = free_channels.size (); i; i--)
        free_channels.pop ();

      for (size_t i = 0; i < capabilities.channels_count; i++)
        free_channels.push (i);

      for (EmitterSet::iterator i = emitters.begin (); i != emitters.end (); ++i)
        if (i->second.is_playing)
        {
          PauseSound (*(i->first));
          PlaySound  (*(i->first));
        }
    }
  }
  catch (...)
  {
  }
}

/*
   Блокировка проигрывания звука
*/

void SoundManager::Impl::SetMute (bool state)
{
  was_muted = is_muted;
  is_muted = state;
  device->SetMute (state);
}

/*
   Функция, вызываемая при сворачивании окна
*/

void SoundManager::Impl::OnMinimize (Window& window, WindowEvent event, const WindowEventContext& context)
{
  switch (minimize_action)
  {
    case WindowMinimizeAction_Mute:  SetMute (true); break;
    case WindowMinimizeAction_Pause: 
      for (EmitterSet::iterator i = emitters.begin (); i != emitters.end (); ++i)
        PauseSound (*(i->first));
      break;
  }
}

void SoundManager::Impl::OnMaximize (Window& window, WindowEvent event, const WindowEventContext& context)
{
  switch (minimize_action)
  {
    case WindowMinimizeAction_Mute:
      SetMute (was_muted);
      break;
    case WindowMinimizeAction_Pause:
      for (EmitterSet::iterator i = emitters.begin (); i != emitters.end (); ++i)
        PlaySound (*(i->first));
      break;
  }
}

/*
   Функция, вызываемая при изменении хэндла
*/

void SoundManager::Impl::OnChangeHandle (Window& window, WindowEvent event, const WindowEventContext& context)
{
  Init ();
}

/*
   Обновление свойств эмиттеров
*/

void SoundManager::Impl::EmitterUpdate ()
{
  for (EmitterSet::iterator i = emitters.begin (); i != emitters.end (); ++i)
  {
    i->second.source.position  = i->first->Position  ();
    i->second.source.direction = i->first->Direction ();
    i->second.source.velocity  = i->first->Velocity  ();

    if (i->second.channel_number != -1)
      device->SetSource (i->second.channel_number, i->second.source);
  }
}

/*
   Проигрывание звуков
*/

void SoundManager::Impl::PlaySound (Emitter& emitter)
{
  EmitterSet::iterator emitter_iter = emitters.find (&emitter);

  if (emitter_iter == emitters.end ())
    return;

  if (!emitter_iter->second.sample_chosen)
  {
    emitter_iter->second.sound_declaration = sound_decl_library.Find (emitter.Source ());
    if (!emitter_iter->second.sound_declaration)
      return;

    emitter_iter->second.sample_number = rand () % emitter_iter->second.sound_declaration->SamplesCount ();
    emitter_iter->second.sound_sample.Load (emitter_iter->second.sound_declaration->Sample (emitter_iter->second.sample_number));
    emitter_iter->second.start_position = (float)emitter_iter->second.sound_sample.Duration () * emitter_iter->second.start_offset;
    emitter_iter->second.cur_position = emitter_iter->second.start_position;
    emitter_iter->second.sample_chosen = true;

    emitter_iter->second.source.gain               = emitter_iter->second.sound_declaration->Param (SoundParam_Gain);
    emitter_iter->second.source.minimum_gain       = emitter_iter->second.sound_declaration->Param (SoundParam_MinimumGain);
    emitter_iter->second.source.maximum_gain       = emitter_iter->second.sound_declaration->Param (SoundParam_MaximumGain);
    emitter_iter->second.source.inner_angle        = emitter_iter->second.sound_declaration->Param (SoundParam_InnerAngle);
    emitter_iter->second.source.outer_angle        = emitter_iter->second.sound_declaration->Param (SoundParam_OuterAngle);
    emitter_iter->second.source.outer_gain         = emitter_iter->second.sound_declaration->Param (SoundParam_OuterGain);
    emitter_iter->second.source.reference_distance = emitter_iter->second.sound_declaration->Param (SoundParam_ReferenceDistance);
    emitter_iter->second.source.maximum_distance   = emitter_iter->second.sound_declaration->Param (SoundParam_MaximumDistance);

  }
  emitter_iter->second.is_playing      = true;
  emitter_iter->second.play_start_time = clock ();

  if (!free_channels.empty ())
  {
    size_t channel_to_use = free_channels.top ();
    free_channels.pop ();

    emitter_iter->second.channel_number = channel_to_use;
    device->SetSample (channel_to_use, emitter_iter->second.sound_declaration->Sample (emitter_iter->second.sample_number));
    device->Seek (channel_to_use, emitter_iter->second.cur_position);
    device->Play (channel_to_use, true);
  }
}

void SoundManager::Impl::PauseSound (Emitter& emitter)
{
  EmitterSet::iterator emitter_iter = emitters.find (&emitter);

  if (emitter_iter == emitters.end ())
    return;

  if (emitter_iter->second.is_playing)
  {
    float offset = (clock () - emitter_iter->second.play_start_time) / (float)CLOCKS_PER_SEC + emitter_iter->second.start_position;

    if (emitter_iter->second.sound_declaration->Looping ()) 
      emitter_iter->second.cur_position = fmod (offset, (float)emitter_iter->second.sound_sample.Duration ());
    else
      emitter_iter->second.cur_position = offset < emitter_iter->second.sound_sample.Duration () ? offset : 0.0f;

    StopPlaying (emitter_iter);
  }
}

void SoundManager::Impl::StopSound (Emitter& emitter)
{
  EmitterSet::iterator emitter_iter = emitters.find (&emitter);

  if (emitter_iter == emitters.end ())
    return;

  if (emitter_iter->second.is_playing)
    StopPlaying (emitter_iter);

  emitters.erase (emitter_iter);
}

void SoundManager::Impl::StopPlaying (EmitterSet::iterator emitter_iter)
{
  emitter_iter->second.is_playing = false;

  if (emitter_iter->second.channel_number != -1)
  {
    device->Stop (emitter_iter->second.channel_number);
    free_channels.push (emitter_iter->second.channel_number);
    emitter_iter->second.channel_number = -1;
  }
}

float SoundManager::Impl::GetNormalizedOffset (Emitter& emitter)
{
  EmitterSet::iterator emitter_iter = emitters.find (&emitter);

  if (emitter_iter == emitters.end ())
    return 0.f;

  if (emitter_iter->second.is_playing)
  {
    float offset = (clock () - emitter_iter->second.play_start_time) / (float)CLOCKS_PER_SEC + emitter_iter->second.start_position;

    if (emitter_iter->second.sound_declaration->Looping ()) return fmod (offset, (float)emitter_iter->second.sound_sample.Duration ()) / 
                                                                   (float)emitter_iter->second.sound_sample.Duration ();
    else                                                    return offset < emitter_iter->second.sound_sample.Duration () ? 
                                                                   offset / (float)emitter_iter->second.sound_sample.Duration () : 0.0f;
  }
  else
    return emitter_iter->second.cur_position / (float)emitter_iter->second.sound_sample.Duration ();
}

/*
   Конструктор / деструктор
*/

SoundManager::SoundManager (Window& target_window, const char* target_configuration, const char* init_string)
  : impl (new Impl (target_window, target_configuration, init_string))
{
}

SoundManager::~SoundManager ()
{
}

/*
   Поиск конфигурации (делегирование от low_level::SoundDeviceSystem)
*/

const char* SoundManager::FindConfiguration (const char* driver_mask, const char* device_mask)
{
  return SoundSystem::FindConfiguration (driver_mask, device_mask);
}

/*
   Установка поведения при сворачивании окна
*/

void SoundManager::SetWindowMinimizeAction (sound::WindowMinimizeAction action)
{
  impl->minimize_action = action;
}

WindowMinimizeAction SoundManager::WindowMinimizeAction () const
{
  return impl->minimize_action;
}

/*
   Уровень громкости
*/

void SoundManager::SetVolume (float volume)
{
  if (volume < 0.0f) volume = 0.0f;
  if (volume > 1.0f) volume = 1.0f;

  impl->volume = volume;
  impl->device->SetVolume (volume);
}

float SoundManager::Volume () const
{
  return impl->volume;
}

/*
   Блокировка проигрывания звука
*/

void SoundManager::SetMute (bool state)
{
  impl->SetMute (state);
}

bool SoundManager::IsMuted () const
{
  return impl->is_muted;
}

/*
   Проигрывание звуков
*/

void SoundManager::PlaySound (Emitter& emitter, float normalized_offset)
{                                            \
  SoundManagerEmitter manager_emitter (normalized_offset);

  impl->emitters.insert_pair (&emitter, manager_emitter);
  impl->PlaySound (emitter);
  emitter.Activate ();
}

void SoundManager::StopSound (Emitter& emitter)
{
  impl->StopSound (emitter);
  emitter.Deactivate ();
}

float SoundManager::GetNormalizedOffset (Emitter& emitter) const
{
  return impl->GetNormalizedOffset (emitter);
}

/*
   Применение операции ко всем слушателям
*/

void SoundManager::ForEachEmitter (const EmitterHandler& emitter_handler)
{
  for (EmitterSet::iterator i = impl->emitters.begin (); i != impl->emitters.end (); ++i)
    emitter_handler (*(i->first));
}

void SoundManager::ForEachEmitter (const ConstEmitterHandler& emitter_handler) const
{
  for (EmitterSet::iterator i = impl->emitters.begin (); i != impl->emitters.end (); ++i)
    emitter_handler (*(i->first));
}

/*
   Применение операции ко всем слушателям c заданным типом
*/

void SoundManager::ForEachEmitter (const char* type, const EmitterHandler& emitter_handler)
{
  if (!type)
    RaiseNullArgument ("sound::SoundManager::ForEachEmitter", "type");

  for (EmitterSet::iterator i = impl->emitters.begin (); i != impl->emitters.end (); ++i)
   if (!strcmp (type, i->second.sound_declaration->Type ()))
     emitter_handler (*(i->first));
}

void SoundManager::ForEachEmitter (const char* type, const ConstEmitterHandler& emitter_handler) const
{
  if (!type)
    RaiseNullArgument ("sound::SoundManager::ForEachEmitter", "type");

  for (EmitterSet::iterator i = impl->emitters.begin (); i != impl->emitters.end (); ++i)
   if (!strcmp (type, i->second.sound_declaration->Type ()))
     emitter_handler (*(i->first));
}

/*
   Установка слушателя
*/

void SoundManager::SetListener (const sound::Listener& listener)
{
  impl->listener = listener;
  impl->device->SetListener (listener);
}

const sound::Listener& SoundManager::Listener () const
{
  return impl->listener;
}

/*
   Загрузка библиотеки звуков
*/

void SoundManager::LoadSoundLibrary (const char* file_name)
{
  impl->sound_decl_library.Load (file_name);
}
