#include "shared.h"

using namespace common;
using namespace media::particles;
using namespace media::particles::particle_designer_serialzer;

namespace
{

///Emitter entry desc
struct EmitterDesc : public xtl::reference_counter
{
  stl::string name;   //emitter name
  math::vec2f offset; //emitter offset

  EmitterDesc (const char* in_name, const math::vec2f& in_offset)
    : name (in_name)
    , offset (in_offset)
    {}
};

typedef xtl::intrusive_ptr<EmitterDesc> EmitterDescPtr;
typedef stl::vector<EmitterDescPtr>     EmitterDescArray;
typedef xtl::com_ptr<ParticleProcessor> ParticleProcessorPtr;

}

/*
   Implementation of particle designer particle system prototype
*/

struct ParticleSystemPrototype::Impl
{
  EmitterDescArray     emitters;           //emitters entries
  ParticleProcessorPtr particle_processor; //particle processor

  ///Constructor
  Impl ()
    : particle_processor (new ParticleProcessor (), false)
    {}

  ///Configure particle system
  void Configure (ParticleSystem& system)
  {
    for (EmitterDescArray::iterator iter = emitters.begin (), end = emitters.end (); iter != end; ++iter)
    {
      ParticleList  particle_list (particle_processor->ParticleSize (), xtl::bind (&ParticleProcessor::InitParticle, particle_processor.get (), _1));
      ParticleScene scene (particle_list);

      scene.SetName   ((*iter)->name.c_str ());
      scene.SetOffset (math::vec3f ((*iter)->offset.x, (*iter)->offset.y, 0));

      //TODO set material, animation

      scene.AttachProcessor (particle_processor.get ());

      system.AddScene (scene);
    }
  }
};

/*
   Constructor / destructor
*/

ParticleSystemPrototype::ParticleSystemPrototype ()
  : impl (new Impl)
{
}

ParticleSystemPrototype::~ParticleSystemPrototype ()
{
  delete impl;
}

/*
   Add emitter
*/

void ParticleSystemPrototype::AddEmitter (const char* name, const math::vec2f& offset)
{
  impl->emitters.push_back (EmitterDescPtr (new EmitterDesc (name, offset), false));
}

/*
   Set parameters
*/

void ParticleSystemPrototype::SetParameters (const common::PropertyMap& parameters)
{
  impl->particle_processor->SetParameters (parameters);
}

/*
   Configure particle system
*/

void ParticleSystemPrototype::Configure (ParticleSystem& system)
{
  impl->Configure (system);
}
