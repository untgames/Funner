#include "shared.h"

using namespace media::collada;
using namespace common;

/*
    Цель морфинга
*/

struct MorphTarget::Impl: public xtl::reference_counter
{
  stl::string mesh;   //меш
  float       weight; //вес
  
  Impl () : weight (0.0f) {}
};

/*
    Конструкторы / деструктор / присваивание
*/

MorphTarget::MorphTarget ()
  : impl (new Impl, false)
  {}
  
MorphTarget::MorphTarget (Impl* in_impl)
  : impl (in_impl, false)
  {}
    
MorphTarget::MorphTarget (const MorphTarget& target)
  : impl (target.impl)
  {}

MorphTarget::~MorphTarget ()
{
}

MorphTarget& MorphTarget::operator = (const MorphTarget& target)
{
  impl = target.impl;
  
  return *this;
}

/*
    Создание копии
*/

MorphTarget MorphTarget::Clone () const
{
  return MorphTarget (new Impl (*impl));
}

/*
    Меш
*/

const char* MorphTarget::Mesh () const
{
  return impl->mesh.c_str ();
}

void MorphTarget::SetMesh (const char* mesh)
{
  if (!mesh)
    throw xtl::make_null_argument_exception ("media::collada::MorphTarget::SetMesh", "mesh");
    
  impl->mesh = mesh;
}

/*
    Вес
*/

void MorphTarget::SetWeight (float weight)
{
  impl->weight = weight;
}

float MorphTarget::Weight () const
{
  return impl->weight;
}

/*
    Морфер    
*/

typedef CollectionImpl<MorphTarget> MorphTargetListImpl;

struct Morph::Impl: public xtl::reference_counter
{
  MorphMethod         method;    //метод морфинга
  MorphTargetListImpl targets;   //цели морфинга
  stl::string         base_mesh; //базовый меш  
  stl::string         id;        //идентификатор морфера
  
  Impl () : method (MorphMethod_Normalized) {}
};

/*
    Конструктор / деструктор
*/

Morph::Morph ()
  : impl (new Impl, false)
  {}
  
Morph::Morph (Impl* in_impl)
  : impl (in_impl, false)
  {}  
  
Morph::Morph (const Morph& morph)
  : impl (morph.impl)
  {}

Morph::~Morph ()
{
}

Morph& Morph::operator = (const Morph& morph)
{
  impl = morph.impl;
  
  return *this;
}

/*
    Создание копии
*/

Morph Morph::Clone () const
{
  return Morph (new Impl (*impl));
}

/*
    Идентификатор морфера
*/

const char* Morph::Id () const
{
   return impl->id.c_str ();
}

void Morph::SetId (const char* id)
{
  if (!id)
    throw xtl::make_null_argument_exception ("media::collada::Morph::SetId", "id");
    
  impl->id = id;
}

/*
    Метод морфинга
*/

MorphMethod Morph::Method () const
{
  return impl->method;
}

void Morph::SetMethod (MorphMethod method)
{
  switch (method)
  {
    case MorphMethod_Normalized:
    case MorphMethod_Relative:
      break;
    default:
      throw xtl::make_argument_exception ("media::collada::Morph::SetMethod", "method", method);
      break;
  }
  
  impl->method = method;
}

/*
    Базовый меш
*/

const char* Morph::BaseMesh () const
{
  return impl->base_mesh.c_str ();
}

void Morph::SetBaseMesh (const char* mesh)
{
  if (!mesh)
    throw xtl::make_null_argument_exception ("media::collada::Morph::SetBaseMesh", "mesh");
    
  impl->base_mesh = mesh;
}

/*
    Список целей морфинга
*/

Morph::TargetList& Morph::Targets ()
{
  return impl->targets;
}

const Morph::TargetList& Morph::Targets () const
{
  return impl->targets;
}
