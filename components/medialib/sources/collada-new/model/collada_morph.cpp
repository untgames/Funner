#include <media/collada/morph.h>
#include "collection.h"

using namespace medialib::collada;
using namespace common;

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

/*
    Цель морфинга
*/

struct MorphTarget::Impl
{
  collada::Mesh& mesh;   //меш
  float          weight; //вес
  
  Impl (collada::Mesh& in_mesh) : mesh (in_mesh), weight (1.0f) {}
};

/*
    Конструктор / деструктор
*/

MorphTarget::MorphTarget (medialib::collada::Mesh& mesh)
  : impl (new Impl (mesh))
  {}

MorphTarget::~MorphTarget ()
{
  delete impl;
}

medialib::collada::Mesh& MorphTarget::Mesh ()
{
  return impl->mesh;
}

const medialib::collada::Mesh& MorphTarget::Mesh () const
{
  return impl->mesh;
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

//конструируемый MorphTarget
class ConstructableMorphTarget: public MorphTarget
{
  public:
    ConstructableMorphTarget (medialib::collada::Mesh& mesh) : MorphTarget (mesh) {}
};

//список MorphTarget
class MorphTargetListImpl: public Collection<MorphTarget, ConstructableMorphTarget, true>
{
  public:
    MorphTargetListImpl (Entity& owner) : Collection<MorphTarget, ConstructableMorphTarget, true> (owner) {}
  
    MorphTarget& Create (Mesh& mesh)
    {
      ConstructableMorphTarget* target = new ConstructableMorphTarget (mesh);

      try
      {
        InsertCore (*target);

        return *target;
      }
      catch (...)
      {
        delete target;
        throw;
      }
    }
};

struct Morph::Impl
{
  MorphMethod         method;    //метод морфинга
  Mesh&               base_mesh; //базовый меш
  MorphTargetListImpl targets;   //цели морфинга  
  
  Impl (Entity& owner, Mesh& in_base_mesh) : method (MorphMethod_Normalized), base_mesh (in_base_mesh), targets (owner) {}
};

/*
    Конструктор / деструктор
*/

Morph::Morph (Mesh& base_mesh, ModelImpl* model, const char* id)
  : Entity (model, id),
    impl (new Impl (*this, base_mesh))
    {}

Morph::~Morph ()
{
  delete impl;
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
      RaiseInvalidArgument ("medialib::collada::Morph::SetMethod", "method", method);
      break;
  }
  
  impl->method = method;
}

/*
    Базовый меш
*/

Mesh& Morph::BaseMesh ()
{
  return impl->base_mesh;
}

const Mesh& Morph::BaseMesh () const
{
  return impl->base_mesh;
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
