#include "shared.h"

using namespace media::rfx;
using namespace media;
using namespace stl;
using namespace common;

/*
    Описание реализации Shader
*/

typedef bitset<ShaderPin_Num> PinSet;

struct Shader::Impl: public xtl::reference_counter
{
  string name;      //имя
  size_t name_hash; //хэш имени
  PinSet pins;      //пины
  
  Impl ();
};

/*
    Impl::Impl
*/

Shader::Impl::Impl ()
  : name_hash (strhash (""))
  {}

/*
    Конструкторы / деструктор
*/

Shader::Shader ()
  : impl (new Impl)
  {}

Shader::Shader (const Shader& shader)
  : impl (new Impl (*shader.impl))
  {}

Shader::~Shader ()
{
}

/*
    Имя
*/

const char* Shader::Name () const
{
  return impl->name.c_str ();
}

size_t Shader::NameHash () const
{
  return impl->name_hash;
}

void Shader::Rename (const char* name)
{
  if (!name)
    RaiseNullArgument ("media::rfx::Shader::Rename", "name");
    
  impl->name      = name;
  impl->name_hash = strhash (name);
}

/*
    Подсчёт ссылок
*/

void Shader::AddRef () const
{
  addref (*impl);
}

void Shader::Release () const
{
  release (impl.get ());
}

/*
    Копирование
*/

Shader::Pointer Shader::Clone (CloneMode mode)
{
  switch (mode)
  {
    case CloneMode_Instance: return this;
    case CloneMode_Copy:     return Pointer (CloneCore (), false);
    default:                 RaiseInvalidArgument ("media::rfx::Shader::Clone", "mode", mode);
  }
  
  return 0;
}

Shader* Shader::CloneCore () const
{
  return new Shader (*this);
}

/*
    Работа с логическими свойствами
*/

bool Shader::IsEnabled (ShaderPin pin) const
{
  if (pin < 0 || pin >= ShaderPin_Num)
    RaiseInvalidArgument ("media::rfx::Shader::IsEnabled", "pin", pin);
    
  return impl->pins.test (pin);
}

void Shader::SetPin (ShaderPin pin, bool state)
{
  if (pin < 0 || pin >= ShaderPin_Num)
    RaiseInvalidArgument ("media::rfx::Shader::SetPin", "pin", pin); 
    
  impl->pins.set (pin, state);
}

/*
    Динамическая диспетчеризация
*/

void Shader::Accept (Visitor& visitor)
{
  AcceptCore (visitor);
}

void Shader::AcceptCore (Visitor& visitor)
{
  visitor (*this);
}
