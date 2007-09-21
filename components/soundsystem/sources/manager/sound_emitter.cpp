#include <sound/manager.h>
#include <stl/string>
#include <xtl/signal.h>

using namespace sound;
using namespace math;

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
};

/*
    Конструктор / деструктор
*/

Emitter::Emitter (const char* source_name)
{
}

Emitter::~Emitter ()
{
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
}

float Emitter::Volume () const
{
  return impl->volume;
}

/*
    Установка динамических параметров излучателя
*/

void Emitter::SetPosition  (const math::vec3f&)
{
}

void Emitter::SetDirection (const math::vec3f&)
{
}

void Emitter::SetVelocity (const math::vec3f&)
{
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
}

void Emitter::Deactivate ()
{
}

bool Emitter::IsActive () const
{
  return impl->activation_count != 0;
}

/*
    Подписка на события
*/

//xtl::connection Emitter::RegisterEventHandler (EmitterEvent, const EventHandler&)
//{
//}
