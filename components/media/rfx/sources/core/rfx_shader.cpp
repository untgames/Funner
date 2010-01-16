#include "shared.h"

using namespace common;
using namespace media::rfx;

/*
    Описание реализации шейдера
*/

struct Shader::Impl: public xtl::reference_counter
{
  stl::string name;        //имя шейдера
  stl::string profile;     //профиль шейдера
  stl::string source_code; //исходный код
};

/*
    Конструкторы / деструктор / присваивание
*/

Shader::Shader ()
  : impl (new Impl)
{
}

Shader::Shader (Impl* in_impl)
  : impl (in_impl)
{
}

Shader::Shader (const Shader& shader)
  : impl (shader.impl)
{
  addref (impl);
}

Shader::~Shader ()
{
  release (impl);
}

Shader& Shader::operator = (const Shader& shader)
{
  Shader (shader).Swap (*this);
  
  return *this;
}

/*
    Копирование
*/

Shader Shader::Clone () const
{
  return Shader (new Impl (*impl));
}

/*
    Идентификатор
*/

size_t Shader::Id () const
{
  return reinterpret_cast<size_t> (impl);
}

/*
    Имя шейдера
*/

const char* Shader::Name () const
{
  return impl->name.c_str ();
}

void Shader::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::rfx::Shader::SetName", "name");
    
  impl->name = name;
}

/*
    Профиль шейдера
*/

const char* Shader::Profile () const
{
  return impl->profile.c_str ();
}

void Shader::SetProfile (const char* profile)
{
  if (!profile)
    throw xtl::make_null_argument_exception ("media::rfx::SetProfile", "profile");
    
  impl->profile = profile;
}

/*
    Исходный код и его полный размер (не включая завершающий '\0')
*/

const char* Shader::SourceCode () const
{
  return impl->source_code.c_str (); 
}

size_t Shader::SourceCodeSize () const
{
  return impl->source_code.size ();
}

void Shader::SetSourceCode (const char* source)
{
  if (!source)
    throw xtl::make_null_argument_exception ("media::rfx::Shader::SetSourceCode(const char*)", "source");
    
  impl->source_code = source;
}

void Shader::SetSourceCode (const char* source, size_t size)
{
  if (!source && size)
    throw xtl::make_null_argument_exception ("media::rfx::Shader::SetSourceCode(const char*,size_t)", "source");
    
  if (!source && !size) impl->source_code.clear ();
  else                  impl->source_code.assign (source, size);
}

/*
    Обмен
*/

void Shader::Swap (Shader& shader)
{
  stl::swap (impl, shader.impl);
}

namespace media
{

namespace rfx
{

void swap (Shader& shader1, Shader& shader2)
{
  shader1.Swap (shader2);
}

}

}
