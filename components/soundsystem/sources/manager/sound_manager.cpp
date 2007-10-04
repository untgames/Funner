#include <time.h>
#include <sound/manager.h>
#include <sound/device.h>
#include <stl/hash_map>
#include <stl/algorithm>
#include <stl/deque>
#include <xtl/intrusive_ptr.h>
#include <xtl/signal.h>
#include <xtl/bind.h>
#include <syslib/window.h>
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

/*
    ќписание реализации SoundManager
*/

struct SoundManagerEmitter
{
  int               channel_number;     //номер канала проигрывани€ (-1 - отсечЄн или не проигрываетс€)
  float             start_offset;       //нормализованный сдвиг начала проигрывани€
  float             start_position;     //позици€ начала проигрывани€
  float             cur_position;       //текуща€ позици€ проигрывани€
  clock_t           play_start_time;    //врем€ начала проигрывани€
  bool              is_playing;         //статус проигрывани€
  SoundDeclaration* sound_declaration;  //описание звука
  SoundSample       sound_sample;       //звуковой сэмпл (используетс€ дл€ определени€ длительности звука)
  size_t            sample_number;      //номер проигрываемого сэмпла

  SoundManagerEmitter  (float normalized_offset);
};

typedef stl::hash_map<Emitter*, SoundManagerEmitter> EmitterSet;
typedef xtl::com_ptr<low_level::ISoundDevice>        DevicePtr;
typedef stl::deque<size_t>                           ChannelsSet;

struct SoundManager::Impl
{
  Window&                     window;               //окно
  DevicePtr                   device;               //устройство воспроизведени€
  sound::WindowMinimizeAction minimize_action;      //поведение при сворачивании окна
  float                       volume;               //добавочна€ громкость
  bool                        is_muted;             //флаг блокировки проигрывани€ звука
  bool                        was_muted;            //предыдущее состо€ние флага блокировки проигрывани€ звука
  sound::Listener             listener;             //параметры слушател€
  EmitterSet                  emitters;             //излучатели звука
  ChannelsSet                 free_channels;        //номера свободных каналов
  Capabilities                capabilities;         //возможности устройства
  auto_connection             minimize_connection;  //соединени€ событи€ потери фокуса
  auto_connection             maximize_connection;  //соединени€ событи€ получени€ фокуса
  SoundDeclarationLibrary     sound_decl_library;   //библиотека описаний звуков

  Impl (Window& target_window);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Ѕлокировка проигрывани€ звука
///////////////////////////////////////////////////////////////////////////////////////////////////
  void SetMute (bool state);

///////////////////////////////////////////////////////////////////////////////////////////////////
////‘ункци€, вызываема€ при сворачивании окна
///////////////////////////////////////////////////////////////////////////////////////////////////
  void OnMinimize (Window& window, WindowEvent event, const WindowEventContext& context);
  void OnMaximize (Window& window, WindowEvent event, const WindowEventContext& context);

///////////////////////////////////////////////////////////////////////////////////////////////////
///ѕроигрывание звуков
///////////////////////////////////////////////////////////////////////////////////////////////////
  void PlaySound    (Emitter& emitter);
  void PauseSound   (Emitter& emitter);
  void StopSound    (Emitter& emitter);
  void StopPlaying  (EmitterSet::iterator emitter_iter);

  float GetNormalizedOffset (Emitter& emitter);
};

SoundManagerEmitter::SoundManagerEmitter (float normalized_offset)
  : channel_number (-1), is_playing (false), start_offset (normalized_offset)
  {}

SoundManager::Impl::Impl (Window& target_window)
  : window (target_window), minimize_action (WindowMinimizeAction_Ignore),
    volume (1.f),
    minimize_connection (window.RegisterEventHandler (WindowEvent_OnLostFocus, bind (&SoundManager::Impl::OnMinimize, this, _1, _2, _3))),
    maximize_connection (window.RegisterEventHandler (WindowEvent_OnSetFocus,  bind (&SoundManager::Impl::OnMaximize, this, _1, _2, _3)))
  {}

/*
   Ѕлокировка проигрывани€ звука
*/

void SoundManager::Impl::SetMute (bool state)
{
  was_muted = is_muted;
  is_muted = state;
  device->SetMute (state);
}

/*
   ‘ункци€, вызываема€ при сворачивании окна
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
   ѕроигрывание звуков
*/

void SoundManager::Impl::PlaySound (Emitter& emitter)
{
  EmitterSet::iterator emitter_iter = emitters.find (&emitter);

  if (emitter_iter == emitters.end ())
    return;

  emitter_iter->second.sound_declaration = sound_decl_library.Find (emitter.Source ());
  if (!emitter_iter->second.sound_declaration)
    return;

  if (emitter_iter->second.cur_position == emitter_iter->second.start_position)
  {
    emitter_iter->second.sample_number = rand () % emitter_iter->second.sound_declaration->SamplesCount ();
    emitter_iter->second.sound_sample.Load (emitter_iter->second.sound_declaration->Sample (emitter_iter->second.sample_number));
    emitter_iter->second.start_position = (float)emitter_iter->second.sound_sample.Duration () * emitter_iter->second.start_offset;
    emitter_iter->second.cur_position = emitter_iter->second.start_position;
  }
  emitter_iter->second.is_playing      = true;
  emitter_iter->second.play_start_time = clock ();

  if (!free_channels.empty ())
  {
    size_t channel_to_use = free_channels.back ();
    free_channels.pop_back ();

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
    emitter_iter->second.cur_position = (float)(clock () - emitter_iter->second.play_start_time) / (float)CLOCKS_PER_SEC;
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
    free_channels.push_back (emitter_iter->second.channel_number);
    emitter_iter->second.channel_number = -1;
  }
}

float SoundManager::Impl::GetNormalizedOffset (Emitter& emitter)
{
  EmitterSet::iterator emitter_iter = emitters.find (&emitter);

  if (emitter_iter == emitters.end ())
    return 0.f;

  return 0.f;
}

/*
    онструктор / деструктор
*/

SoundManager::SoundManager (Window& target_window, const char* target_configuration, const char* init_string)
  : impl (new Impl (target_window))
{
  try
  {
    impl->device = SoundSystem::CreateDevice (target_configuration, &impl->window, init_string);

    if (impl->device)
    {
      impl->device->GetCapabilities (impl->capabilities);
      
      for (size_t i = 0; i < impl->capabilities.channels_count; i++)
        impl->free_channels.push_back (i);
    }
  }
  catch (...)
  {
  }
}

SoundManager::~SoundManager ()
{
}

/*
   ѕоиск конфигурации (делегирование от low_level::SoundDeviceSystem)
*/

const char* SoundManager::FindConfiguration (const char* driver_mask, const char* device_mask)
{
  return SoundSystem::FindConfiguration (driver_mask, device_mask);
}

/*
   ”становка поведени€ при сворачивании окна
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
   ”ровень громкости
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
   Ѕлокировка проигрывани€ звука
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
   ѕроигрывание звуков
*/

void SoundManager::PlaySound (Emitter& emitter, float normalized_offset)
{
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
   ѕрименение операции ко всем слушател€м
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
   ѕрименение операции ко всем слушател€м c заданным типом
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
   ”становка слушател€
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
