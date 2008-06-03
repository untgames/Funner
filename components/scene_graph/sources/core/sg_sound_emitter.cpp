#include <xtl/visitor.h>
#include <xtl/signal.h>
#include <stl/string>
#include <common/exception.h>
#include <sg/sound_emitter.h>

using namespace scene_graph;
using namespace xtl;
using namespace common;

/*
    Описание реализации SoundEmitter
*/

typedef signal<void (SoundEmitter& sender, SoundEmitterEvent event)> SoundEmitterSignal;

struct SoundEmitter::Impl
{
  stl::string        sound_declaration_name;                   //имя описания звука
  SoundEmitterSignal signals [SoundEmitterEvent_Num];          //сигналы
  bool               signal_process [SoundEmitterEvent_Num];   //флаги обработки сигналов

  Impl (const char* in_sound_declaration_name) : sound_declaration_name (in_sound_declaration_name) {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Оповещение клиентов о наступлении события
///////////////////////////////////////////////////////////////////////////////////////////////////
  void Notify (SoundEmitter& emitter, SoundEmitterEvent event);
};

/*
    Оповещение клиентов о наступлении события
*/

void SoundEmitter::Impl::Notify (SoundEmitter& emitter, SoundEmitterEvent event)
{
    //если обработчиков нет - оповещение игнорируется
    
  if (!signals [event])
    return;

    //проверяем нет ли рекурсивного вызова

  if (signal_process [event])
    return;
    
    //устанавливаем флаг обработки события

  signal_process [event] = true;
  
    //вызываем обработчики событий

  try
  {
    signals [event] (emitter, event);
  }
  catch (...)
  {
    //все исключения клиентских обработчиков событий узла поглощаются
  }
  
    //снимаем флаг обработки события
  
  signal_process [event] = false;
}

/*
    Конструктор / деструктор
*/

SoundEmitter::SoundEmitter (const char* sound_declaration_name)
  : impl (new Impl (sound_declaration_name))
  {}

SoundEmitter::~SoundEmitter ()
{
  delete impl;
}

/*
    Создание слушателя
*/

SoundEmitter::Pointer SoundEmitter::Create (const char* sound_declaration_name)
{
  return Pointer (new SoundEmitter (sound_declaration_name), false);
}

/*
   Имя звука
*/

const char* SoundEmitter::SoundDeclarationName () const
{
  return impl->sound_declaration_name.c_str ();
}

/*
   Проигрывание
*/

void SoundEmitter::Play ()
{
  impl->Notify (*this, SoundEmitterEvent_Play);
}

void SoundEmitter::Stop ()
{
  impl->Notify (*this, SoundEmitterEvent_Stop);
}

/*
   Подписка на события SoundEmitter
*/

xtl::connection SoundEmitter::RegisterEventHandler (SoundEmitterEvent event, const EventHandler& handler) const
{
  if (event < 0 || event >= SoundEmitterEvent_Num)
    raise_invalid_argument ("scene_graph::SoundEmitter::Event", "event", event);

  return impl->signals [event].connect (handler);
}

/*
    Метод, вызываемый при посещении объекта
*/

void SoundEmitter::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);
}
