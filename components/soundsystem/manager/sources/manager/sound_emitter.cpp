#include <sound/manager.h>
#include <stl/string>
#include <xtl/signal.h>
#include <common/exception.h>

using namespace sound;
using namespace math;
using namespace common;

/*
    Описание реализации излучателя звука
*/

typedef xtl::signal<void (Emitter&, EmitterEvent)> EmitterSignal;

struct Emitter::Impl
{
  stl::string   source;                     //имя источника
  float         volume;                     //добавочная громкость
  vec3f         position;                   //положение
  vec3f         direction;                  //направление
  vec3f         velocity;                   //скорость
  EmitterSignal signals [EmitterEvent_Num]; //сигналы
  size_t        activation_count;           //счётчик активаций

  Impl (const char* source_name);

  void Notify (EmitterEvent event, Emitter& emitter);
};

Emitter::Impl::Impl (const char* source_name)
  : source (source_name), volume (1.f)
  {}

void Emitter::Impl::Notify (EmitterEvent event, Emitter& emitter)
{
  try
  {
    signals [event] (emitter, event);
  }
  catch (...)
  {
  }
}

/*
    Конструктор / деструктор
*/

Emitter::Emitter (const char* source_name)
  : impl (new Impl (source_name))
  {}

Emitter::~Emitter ()
{
  impl->Notify (EmitterEvent_OnDestroy, *this);
}

/*
    Источник звука
*/

const char* Emitter::Source () const
{
  return impl->source.c_str ();
}

/*
    Установка добавочной громкости звука
*/

void Emitter::SetVolume (float volume)
{
  impl->volume = volume;
  impl->Notify (EmitterEvent_OnUpdateVolume, *this);
}

float Emitter::Volume () const
{
  return impl->volume;
}

/*
    Установка динамических параметров излучателя
*/

void Emitter::SetPosition  (const math::vec3f& position)
{
  impl->position = position;
  impl->Notify (EmitterEvent_OnUpdateProperties, *this);
}

void Emitter::SetDirection (const math::vec3f& direction)
{
  impl->direction = direction;
  impl->Notify (EmitterEvent_OnUpdateProperties, *this);
}

void Emitter::SetVelocity (const math::vec3f& velocity)
{
  impl->velocity = velocity;
  impl->Notify (EmitterEvent_OnUpdateProperties, *this);
}

const vec3f& Emitter::Position () const
{
  return impl->position;
}

const vec3f& Emitter::Direction () const
{
  return impl->direction;
}

const vec3f& Emitter::Velocity () const
{
  return impl->velocity;
}

/*
    Управление активностью
      - первый вызов Activate генерирует событие EmitterEvent_OnActivate
      - последний вызов Deactivate генерирует событие EmitterEvent_OnDeactivate
      - излучатель является активным до тех пор, пока проигрывается хотя бы в одном менеджере
*/

void Emitter::Activate ()
{
  if (!impl->activation_count)
    impl->Notify (EmitterEvent_OnActivate, *this);

  impl->activation_count++;
}

void Emitter::Deactivate ()
{
  if (impl->activation_count)
  {
    impl->activation_count--;
    if (!impl->activation_count)
      impl->Notify (EmitterEvent_OnDeactivate, *this);
  }
}

bool Emitter::IsActive () const
{
  return impl->activation_count != 0;
}

/*
   Подписка на события
*/

xtl::connection Emitter::RegisterEventHandler (EmitterEvent event, const EventHandler& handler)
{
  if (event < 0 || event >= EmitterEvent_Num)
    RaiseInvalidArgument ("sound::Emitter::RegisterEventHandler", "event", event);
    
  return impl->signals [event].connect (handler);
}
