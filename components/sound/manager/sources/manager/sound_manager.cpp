#include <stl/algorithm>
#include <stl/hash_map>
#include <stl/list>
#include <stl/stack>
#include <stl/string>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/iterator.h>
#include <xtl/shared_ptr.h>
#include <xtl/signal.h>

#include <common/log.h>
#include <common/time.h>

#include <sound/device.h>
#include <sound/driver.h>
#include <sound/manager.h>

#include <media/sound.h>
#include <media/sound_declaration.h>

using namespace sound;
using namespace sound::low_level;
using namespace stl;
using namespace xtl;
using namespace common;
using namespace media;

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

namespace
{

const char* LOG_NAME = "sound::SoundManager";  //имя потока протоколирования

SeekMode get_seek_mode (bool looping)
{
  if (looping) return SeekMode_Repeat;
  else         return SeekMode_Clamp;
}

typedef stl::list<SoundDeclarationLibrary> SoundDeclarationLibraryList;
typedef xtl::com_ptr<ISample>              SamplePtr;

}

/*
    Описание реализации SoundManager
*/

struct SoundManagerEmitter
{
  int              channel_number;               //номер канала проигрывания (-1 - отсечён или не проигрывается)
  float            cur_position;                 //текущая позиция проигрывания в секундах
  size_t           play_start_time;              //время начала проигрывания
  bool             is_playing;                   //статус проигрывания
  SoundDeclaration sound_declaration;            //описание звука
  SamplePtr        sound_sample;                 //сэмпл звука
  double           duration;                     //длительность звука
  float            sound_declaration_gain;       //громкость, заданная в описании звука (установка громкости производится относительно этого значения)
  bool             sample_chosen;                //эммитер ещё не проигрывался
  Source           source;                       //излучатель звука
  string           source_name;                  //имя источника
  size_t           sample_index;                 //индекс сэмпла
  auto_connection  update_volume_connection;     //соединение события изменения громкости
  auto_connection  update_properties_connection; //соединение события изменения свойств

  SoundManagerEmitter  (connection in_update_volume_connection, connection in_update_properties_connection)
    : channel_number (-1), is_playing (false), sample_chosen (false), sample_index (0),
      update_volume_connection (in_update_volume_connection), update_properties_connection (in_update_properties_connection)
    {}
};

typedef xtl::shared_ptr<SoundManagerEmitter>            SoundManagerEmitterPtr;
typedef stl::hash_map<Emitter*, SoundManagerEmitterPtr> EmitterSet;
typedef xtl::com_ptr<low_level::IDevice>                DevicePtr;
typedef xtl::com_ptr<low_level::IDriver>                DriverPtr;
typedef stl::stack<size_t>                              ChannelsSet;

struct SoundManager::Impl : public xtl::trackable
{
  DevicePtr                   device;                      //устройство воспроизведения
  float                       volume;                      //добавочная громкость
  bool                        is_muted;                    //флаг блокировки проигрывания звука
  sound::Listener             listener;                    //параметры слушателя
  EmitterSet                  emitters;                    //излучатели звука
  ChannelsSet                 free_channels;               //номера свободных каналов
  Capabilities                capabilities;                //возможности устройства
  SoundDeclarationLibraryList sound_declaration_libraries; //библиотека описаний звуков
  common::Log                 log;                         //протокол
  xtl::trackable              trackable;

  Impl (const char* driver_mask, const char* device_mask, const char* init_string)
    : volume (1.f), log (LOG_NAME)
  {
    try
    {
      device = DevicePtr (DriverManager::CreateDevice (driver_mask, device_mask, init_string), false);

      device->GetCapabilities (capabilities);

      for (size_t i = 0; i < capabilities.channels_count; i++)
        free_channels.push (i);
    }
    catch (xtl::exception& exception)
    {
      exception.touch ("sound::low_level::SoundManager::SoundManager");
      throw;
    }
  }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Блокировка проигрывания звука
///////////////////////////////////////////////////////////////////////////////////////////////////
  void SetMute (bool state)
  {
    is_muted = state;
    device->SetMute (state);
  }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обновление свойств эмиттеров
///////////////////////////////////////////////////////////////////////////////////////////////////
  void UpdateEmitterVolume (Emitter& emitter, EmitterEvent event)
  {
    EmitterSet::iterator emitter_iter = emitters.find (&emitter);

    if (emitter_iter == emitters.end ())
      return;

    emitter_iter->second->source.gain = emitter_iter->second->sound_declaration_gain * emitter_iter->first->Volume ();

    if (emitter_iter->second->channel_number != -1)
      device->SetSource (emitter_iter->second->channel_number, emitter_iter->second->source);
  }

  void UpdateEmitterProperties (Emitter& emitter, EmitterEvent event)
  {
    EmitterSet::iterator emitter_iter = emitters.find (&emitter);

    if (emitter_iter == emitters.end ())
      return;

    emitter_iter->second->source.position  = emitter_iter->first->Position  ();
    emitter_iter->second->source.direction = emitter_iter->first->Direction ();
    emitter_iter->second->source.velocity  = emitter_iter->first->Velocity  ();

    if (emitter_iter->second->channel_number != -1)
      device->SetSource (emitter_iter->second->channel_number, emitter_iter->second->source);
  }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проигрывание звуков
///////////////////////////////////////////////////////////////////////////////////////////////////
  void PlaySound (Emitter& emitter, float offset)
  {
    try
    {
      emitter.Activate ();

      EmitterSet::iterator emitter_iter = emitters.find (&emitter);

      SoundManagerEmitterPtr manager_emitter;

      if (emitter_iter == emitters.end ())
        manager_emitter = SoundManagerEmitterPtr (new SoundManagerEmitter (emitter.RegisterEventHandler (EmitterEvent_OnUpdateVolume, xtl::bind (&SoundManager::Impl::UpdateEmitterVolume, this, _1, _2)),
            emitter.RegisterEventHandler (EmitterEvent_OnUpdateProperties, xtl::bind (&SoundManager::Impl::UpdateEmitterProperties, this, _1, _2)))); //создаем эмиттер, в карту добавляем позже, если найдем описание звука
      else
        manager_emitter = emitter_iter->second;

      if (strcmp (manager_emitter->source_name.c_str (), emitter.Source ()))
      {
        SoundDeclaration *emitter_sound_declaration = 0;

        for (SoundDeclarationLibraryList::iterator iter = sound_declaration_libraries.begin (),
             end = sound_declaration_libraries.end (); iter != end; ++iter)
        {
          emitter_sound_declaration = iter->Find (emitter.Source ());

          if (emitter_sound_declaration)
            break;
        }

        if (!emitter_sound_declaration)
        {
          log.Printf ("Can't find sound declaration for id '%s'.", emitter.Source ());
          StopSound (emitter);
          return;
        }

        manager_emitter->sound_declaration = *emitter_sound_declaration;

        manager_emitter->sound_declaration_gain = manager_emitter->sound_declaration.Param (SoundParam_Gain);

        manager_emitter->source.gain               = emitter.Volume() * manager_emitter->sound_declaration_gain;
        manager_emitter->source.minimum_gain       = manager_emitter->sound_declaration.Param (SoundParam_MinimumGain);
        manager_emitter->source.maximum_gain       = manager_emitter->sound_declaration.Param (SoundParam_MaximumGain);
        manager_emitter->source.inner_angle        = manager_emitter->sound_declaration.Param (SoundParam_InnerAngle);
        manager_emitter->source.outer_angle        = manager_emitter->sound_declaration.Param (SoundParam_OuterAngle);
        manager_emitter->source.outer_gain         = manager_emitter->sound_declaration.Param (SoundParam_OuterGain);
        manager_emitter->source.reference_distance = manager_emitter->sound_declaration.Param (SoundParam_ReferenceDistance);
        manager_emitter->source.maximum_distance   = manager_emitter->sound_declaration.Param (SoundParam_MaximumDistance);

        manager_emitter->source_name = emitter.Source ();

        if (emitter_iter == emitters.end ())
          emitters.insert_pair (&emitter, manager_emitter);
      }

      if (!manager_emitter->sample_chosen || manager_emitter->sample_index != emitter.SampleIndex ())
      {
        manager_emitter->sample_index = emitter.SampleIndex ();

        manager_emitter->sound_sample = SamplePtr (device->CreateSample (manager_emitter->sound_declaration.Sample (manager_emitter->sample_index % manager_emitter->sound_declaration.SamplesCount ())), false);

        SampleDesc sample_desc;

        manager_emitter->sound_sample->GetDesc (sample_desc);

        manager_emitter->duration = sample_desc.samples_count / (double)sample_desc.frequency;

        manager_emitter->sample_chosen = true;
      }

      if (!manager_emitter->is_playing || manager_emitter->channel_number == -1)
      {
        manager_emitter->is_playing = true;

        if (!free_channels.empty ())
        {
          size_t channel_to_use = free_channels.top ();
          free_channels.pop ();

          manager_emitter->channel_number = channel_to_use;
        }
        else
        {
          log.Printf ("Can't play sound %s, no free channels", manager_emitter->sound_sample->GetName ());
          manager_emitter->channel_number = -1;
        }
      }

      manager_emitter->play_start_time = milliseconds ();

      manager_emitter->cur_position = offset;

      UpdateEmitterProperties (emitter, EmitterEvent_OnUpdateProperties);

      if (manager_emitter->channel_number != -1)
      {
        device->Stop (manager_emitter->channel_number);
        device->SetSample (manager_emitter->channel_number, manager_emitter->sound_sample.get ());
        device->SetSource (manager_emitter->channel_number, manager_emitter->source);
        device->Seek (manager_emitter->channel_number, manager_emitter->cur_position, get_seek_mode (manager_emitter->sound_declaration.Looping ()));
        device->Play (manager_emitter->channel_number, manager_emitter->sound_declaration.Looping ());
      }
    }
    catch (xtl::exception& e)
    {
      log.Printf ("Can't play sound. Exception: '%s'", e.what ());
      StopSound (emitter);
      throw;
    }
    catch (...)
    {
      log.Printf ("Can't play sound. Unknown exception");
      StopSound (emitter);
      throw;
    }
  }

  void PauseSound (Emitter& emitter)
  {
    EmitterSet::iterator emitter_iter = emitters.find (&emitter);

    if (emitter_iter == emitters.end ())
      return;

    if (emitter_iter->second->is_playing)
    {
      float offset = (milliseconds () - emitter_iter->second->play_start_time) / 1000.f + emitter_iter->second->cur_position;

      if (emitter_iter->second->sound_declaration.Looping ())
        emitter_iter->second->cur_position = fmod (offset, (float)emitter_iter->second->duration);
      else
        emitter_iter->second->cur_position = offset < emitter_iter->second->duration ? offset : 0.0f;

      StopPlaying (emitter_iter);
    }
  }

  void StopSound (Emitter& emitter)
  {
    EmitterSet::iterator emitter_iter = emitters.find (&emitter);

    if (emitter_iter == emitters.end ())
    {
      emitter.Deactivate ();
      return;
    }

    if (emitter_iter->second->is_playing)
      StopPlaying (emitter_iter);

    emitter.Deactivate ();

    emitters.erase (emitter_iter);
  }

  void StopPlaying (EmitterSet::iterator emitter_iter)
  {
    emitter_iter->second->is_playing = false;

    if (emitter_iter->second->channel_number != -1)
    {
      device->Stop (emitter_iter->second->channel_number);
      free_channels.push (emitter_iter->second->channel_number);
      emitter_iter->second->channel_number = -1;
    }
  }

  float Tell (Emitter& emitter)
  {
    EmitterSet::iterator emitter_iter = emitters.find (&emitter);

    if (emitter_iter == emitters.end ())
      return 0.f;

    if (emitter_iter->second->is_playing)
    {
      float offset = (milliseconds () - emitter_iter->second->play_start_time) / 1000.f + emitter_iter->second->cur_position;

      if (emitter_iter->second->sound_declaration.Looping ()) return fmod (offset, (float)emitter_iter->second->duration);
      else                                                    return offset < emitter_iter->second->duration ? offset : 0.0f;
    }
    else
      return emitter_iter->second->cur_position;
  }

  float Duration (Emitter& emitter) const
  {
    EmitterSet::const_iterator emitter_iter = emitters.find (&emitter);

    if (emitter_iter == emitters.end ())
      return 0.f;

    return (float)emitter_iter->second->duration;
  }

  bool IsLooping (Emitter& emitter) const
  {
    EmitterSet::const_iterator emitter_iter = emitters.find (&emitter);

    if (emitter_iter == emitters.end ())
      return false;

    return emitter_iter->second->sound_declaration.Looping ();
  }

  bool IsPlaying (Emitter& emitter) const
  {
    EmitterSet::const_iterator emitter_iter = emitters.find (&emitter);

    if (emitter_iter == emitters.end ())
      return false;

    return emitter_iter->second->is_playing;
  }
};


/*
   Конструктор / деструктор
*/

SoundManager::SoundManager (const char* driver_mask, const char* device_mask, const char* init_string)
  : impl (new Impl (driver_mask, device_mask, init_string))
{
}

SoundManager::~SoundManager ()
{
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

void SoundManager::PlaySound (Emitter& emitter, float offset)
{
  impl->PlaySound (emitter, offset);
}

void SoundManager::StopSound (Emitter& emitter)
{
  impl->StopSound (emitter);
}

float SoundManager::Tell (Emitter& emitter) const
{
  return impl->Tell (emitter);
}

float SoundManager::Duration (Emitter& emitter) const
{
  return impl->Duration (emitter);
}

bool SoundManager::IsLooping (Emitter& emitter) const
{
  return impl->IsLooping (emitter);
}

bool SoundManager::IsPlaying (Emitter& emitter) const
{
  return impl->IsPlaying (emitter);
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
    throw xtl::make_null_argument_exception ("sound::SoundManager::ForEachEmitter", "type");

  for (EmitterSet::iterator i = impl->emitters.begin (); i != impl->emitters.end (); ++i)
   if (!strcmp (type, i->second->sound_declaration.Type ()))
     emitter_handler (*(i->first));
}

void SoundManager::ForEachEmitter (const char* type, const ConstEmitterHandler& emitter_handler) const
{
  if (!type)
    throw xtl::make_null_argument_exception ("sound::SoundManager::ForEachEmitter", "type");

  for (EmitterSet::iterator i = impl->emitters.begin (); i != impl->emitters.end (); ++i)
   if (!strcmp (type, i->second->sound_declaration.Type ()))
     emitter_handler (*(i->first));
}

/*
   Регистрация обработчиков события удаления объекта
*/

xtl::connection SoundManager::RegisterDestroyHandler (xtl::trackable::slot_type& handler)
{
  return impl->connect_tracker (handler);
}

xtl::connection SoundManager::RegisterDestroyHandler (const xtl::trackable::function_type& handler)
{
  return impl->connect_tracker (handler);
}

xtl::connection SoundManager::RegisterDestroyHandler (const xtl::trackable::function_type& handler, xtl::trackable& trackable)
{
  return impl->connect_tracker (handler, trackable);
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
   Загрузка/выгрузка библиотек звуков
*/

void SoundManager::LoadSoundLibrary (const char* file_name)
{
  impl->sound_declaration_libraries.push_front (SoundDeclarationLibrary (file_name));

  for (SoundDeclarationLibraryList::iterator iter = ++impl->sound_declaration_libraries.begin (),
       end = impl->sound_declaration_libraries.end (); iter != end; ++iter)
  {
    if (!xtl::xstrcmp (file_name, iter->Name ()))
    {
      impl->sound_declaration_libraries.erase (iter);
      impl->log.Printf ("Warning: sound declaration library '%s' was reloaded", file_name);
      break;
    }
  }

  SoundDeclarationLibrary& new_library = impl->sound_declaration_libraries.front ();

  for (SoundDeclarationLibrary::Iterator iter = new_library.CreateIterator (); iter; ++iter)
  {
    const char* item_id = new_library.ItemId (iter);

    for (SoundDeclarationLibraryList::iterator library_iter = ++impl->sound_declaration_libraries.begin (),
         end = impl->sound_declaration_libraries.end (); library_iter != end; ++library_iter)
    {
      if (library_iter->Find (item_id))
      {
        impl->log.Printf ("Warning: ignoring already loaded sound declaration with id '%s' - redefinition in library '%s'", item_id, file_name);
        continue;
      }
    }
  }
}

void SoundManager::UnloadSoundLibrary (const char* file_name)
{
  if (!file_name)
    return;

  for (SoundDeclarationLibraryList::iterator iter = impl->sound_declaration_libraries.begin (),
       end = impl->sound_declaration_libraries.end (); iter != end; ++iter)
  {
    if (!xtl::xstrcmp (file_name, iter->Name ()))
    {
      impl->sound_declaration_libraries.erase (iter);
      break;
    }
  }
}
