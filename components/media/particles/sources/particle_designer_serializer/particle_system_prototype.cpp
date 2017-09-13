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
typedef stl::vector<ParticleTexDesc>    ParticleTexDescArray;

}

/*
   Implementation of particle designer particle system prototype
*/

struct ParticleSystemPrototype::Impl
{
  EmitterDescArray     emitters;           //emitters entries
  ParticleProcessorPtr particle_processor; //particle processor
  stl::string          material_name;      //material name
  ParticleTexDescArray animation_frames;   //animation frames
  float                animation_fps;      //animation fps

  ///Constructor
  Impl ()
    : particle_processor (new ParticleProcessor (), false)
    , animation_fps ()
    {}

  ///Configure particle system
  void Configure (ParticleSystem& system)
  {
    for (EmitterDescArray::iterator iter = emitters.begin (), end = emitters.end (); iter != end; ++iter)
    {
      ParticleList  particle_list (particle_processor->ParticleSize (), xtl::bind (&ParticleProcessor::InitParticle, particle_processor.get (), _1));
      ParticleScene scene (particle_list);

      scene.SetName ((*iter)->name.c_str ());
      scene.SetOffset (math::vec3f ((*iter)->offset.x, (*iter)->offset.y, 0));
      scene.SetMaterialName (material_name.c_str ());
      scene.SetAnimationFramesPerSecond (animation_fps);
      scene.SetAnimationFramesCount ((unsigned int)animation_frames.size ());

      if (!animation_frames.empty ())
        memcpy (scene.AnimationFrames (), &animation_frames [0], animation_frames.size () * sizeof (ParticleTexDescArray::value_type));

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
   Particle material
*/

void ParticleSystemPrototype::SetMaterialName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::particles::particle_designer_serializer::ParticleSystemPrototype::SetMaterialName", "name");

  impl->material_name = name;
}

/*
   Particles animation parameters
*/

void ParticleSystemPrototype::SetAnimationFramesCount (unsigned int count)
{
  impl->animation_frames.resize (count);
}

ParticleTexDesc* ParticleSystemPrototype::AnimationFrames ()
{
  if (!impl->animation_frames.empty ())
    return &impl->animation_frames [0];

  return 0;
}

void ParticleSystemPrototype::SetAnimationFramesPerSecond (float count)
{
  impl->animation_fps = count;
}

/*
   Configure particle system
*/

void ParticleSystemPrototype::Configure (ParticleSystem& system)
{
  impl->Configure (system);
}
