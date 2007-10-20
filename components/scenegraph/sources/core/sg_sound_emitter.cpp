#include <sg/sound_emitter.h>
#include <xtl/visitor.h>
#include <stl/string>

using namespace scene_graph;

/*
    Описание реализации SoundEmitter
*/

struct SoundEmitter::Impl
{
  stl::string sound_declaration_name;     //имя описания звука

  Impl (const char* in_sound_declaration_name) : sound_declaration_name (in_sound_declaration_name) {}
};

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

SoundEmitter* SoundEmitter::Create (const char* sound_declaration_name)
{
  return new SoundEmitter (sound_declaration_name);
}

/*
   Имя звука
*/

const char* SoundEmitter::SoundDeclarationName () const
{
  return impl->sound_declaration_name.c_str ();
}

/*
    Метод, вызываемый при посещении объекта
*/

void SoundEmitter::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);
}
