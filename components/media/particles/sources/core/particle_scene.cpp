#include "shared.h"

using namespace media::particles;

/*
    Constants
*/

const size_t PARTICLE_PROCESSOR_ARRAY_RESERVE = 8;

/*
    Implementation details
*/

typedef xtl::com_ptr<IParticleProcessor> ParticleProcessorPtr;

struct ParticleProcessorInstance
{
  ParticleProcessorPtr processor;
  void*                instance_data;
};

typedef stl::vector<ParticleProcessorInstance> ParticleProcessorArray;
typedef stl::vector<ParticleTexDesc>           ParticleTexDescArray;

struct ParticleScene::Impl: public xtl::reference_counter
{
  ParticleList           particles;
  stl::string            name;
  stl::string            material_name;
  ParticleTexDescArray   animation_frames;
  unsigned int           animation_fps;
  math::vec3f            offset;
  TimeValue              time_offset;
  bound_volumes::aaboxf  bound_box;
  bool                   is_manual_bound_box;
  bool                   is_dirty_bound_box;
  ParticleProcessorArray processors;
  bool                   first_step;
  TimeValue              time;
  TimeValue              prev_time;

  Impl ()
    : animation_fps ()
    , is_manual_bound_box (false)
    , is_dirty_bound_box (true)
    , first_step (true)
  {
    processors.reserve (PARTICLE_PROCESSOR_ARRAY_RESERVE);
  }
};

/*
    Constructors / destructor / assignment
*/

ParticleScene::ParticleScene ()
  : impl (new Impl)
{
}

ParticleScene::ParticleScene (const ParticleScene& scene)
  : impl (scene.impl)
{
  addref (impl);
}

ParticleScene::~ParticleScene ()
{
  if (impl->decrement ())
  {
    DetachAllProcessors ();
    delete impl;
  }
}

ParticleScene& ParticleScene::operator = (const ParticleScene& scene)
{
  ParticleScene (scene).Swap (*this);
  return *this;
}

/*
    Name
*/

const char* ParticleScene::Name () const
{
  return impl->name.c_str ();
}

void ParticleScene::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::particles::ParticleScene::SetName", "name");

  impl->name = name;
}

/*
    Particle material
*/

const char* ParticleScene::MaterialName () const
{
  return impl->material_name.c_str ();
}

void ParticleScene::SetMaterialName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::particles::ParticleScene::SetMaterialName", "name");

  impl->material_name = name;
}

/*
    Particles animation parameters
*/

unsigned int ParticleScene::AnimationFramesCount () const
{
  return impl->animation_frames.size ();
}

void ParticleScene::SetAnimationFramesCount (unsigned int count)
{
  impl->animation_frames.resize (count);
}

ParticleTexDesc* ParticleScene::AnimationFrames ()
{
  if (!impl->animation_frames.empty ())
    return &impl->animation_frames [0];

  return 0;
}

const ParticleTexDesc* ParticleScene::AnimationFrames () const
{
  if (!impl->animation_frames.empty ())
    return &impl->animation_frames [0];

  return 0;
}

unsigned int ParticleScene::AnimationFramesPerSecond () const
{
  return impl->animation_fps;
}

void ParticleScene::SetAnimationFramesPerSecond (unsigned int count)
{
  impl->animation_fps = count;
}

/*
    Access to particles data
*/

const ParticleList& ParticleScene::Particles () const
{
  return impl->particles;
}

ParticleList& ParticleScene::Particles ()
{
  return impl->particles;
}

/*
    Scene position offset, used if it's needed to apply some offset for whole scene
    (for example random offset for fireworks explosions)
*/

const math::vec3f& ParticleScene::Offset () const
{
  return impl->offset;
}

void ParticleScene::SetOffset (const math::vec3f& offset)
{
  impl->offset = offset;
}

/*
    Start time offset - particle scene will not start for this time
*/

void ParticleScene::SetStartTimeOffset (const TimeValue& offset)
{
  impl->time_offset = offset;
}

const TimeValue& ParticleScene::StartTimeOffset () const
{
  return impl->time_offset;
}

/*
    Scene bound box. If bound box was set, it will be always returned. If it was not set or was
    reset bound box will be recalculated on request using actual particles data
*/

void ParticleScene::SetBoundBox (const bound_volumes::aaboxf& box)
{
  impl->is_manual_bound_box = true;
  impl->is_dirty_bound_box  = false;
  impl->bound_box           = box; 
}

void ParticleScene::ResetBoundBox ()
{
  impl->is_manual_bound_box = false;
  impl->is_dirty_bound_box  = true;
}

const bound_volumes::aaboxf& ParticleScene::BoundBox () const
{
  if (!impl->is_dirty_bound_box)
    return impl->bound_box;

  bound_volumes::aaboxf box;

  ParticleList::ConstIterator it = impl->particles.CreateIterator ();

  if (it)
  {
    box.set_minimum (it->position);
    box.set_maximum (it->position);

    while (++it)
      box += it->position;
  } 

  impl->bound_box          = box;
  impl->is_dirty_bound_box = false;

  return impl->bound_box;
}

/*
    Attaching / detaching of particles processors
*/

void ParticleScene::AttachProcessor (IParticleProcessor* processor)
{
  try
  {
    if (!processor)
      throw xtl::make_null_argument_exception ("", "processor");

    ParticleProcessorInstance instance;

    instance.processor     = processor;
    instance.instance_data = processor->AttachScene (*this);

    try
    {
      impl->processors.push_back (instance);
    }
    catch (...)
    {
      processor->DetachScene (*this, instance.instance_data);
      throw;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::particles::ParticleScene::AttachProcessor");
    throw;
  }
}

namespace
{

struct ParticleProcessorComparator
{
  IParticleProcessor* processor;

  ParticleProcessorComparator (IParticleProcessor* in_processor) : processor (in_processor) {}

  bool operator () (const ParticleProcessorInstance& instance) const { return processor == instance.processor; }
};

}

void ParticleScene::DetachProcessor (IParticleProcessor* processor)
{
  ParticleProcessorArray::iterator first = stl::remove_if (impl->processors.begin (), impl->processors.end (), ParticleProcessorComparator (processor));

  for (ParticleProcessorArray::iterator it=first; it!=impl->processors.end (); ++it)
  {
    ParticleProcessorInstance& instance = *it;

    processor->DetachScene (*this, instance.instance_data);
  }
  
  impl->processors.erase (first, impl->processors.end ());
}

void ParticleScene::DetachAllProcessors ()
{
  for (ParticleProcessorArray::iterator it=impl->processors.begin (); it!=impl->processors.end (); ++it)
  {
    ParticleProcessorInstance& instance = *it;

    instance.processor->DetachScene (*this, instance.instance_data);
  }
  
  impl->processors.clear ();
}

/*
    Scene current time and prev update time
*/

const TimeValue& ParticleScene::Time () const
{
  return impl->time;
}

const TimeValue& ParticleScene::PrevUpdateTime () const
{
  return impl->prev_time;
}

/*
    Perform scene update (updates all particles)
*/

void ParticleScene::Update (const TimeValue& time, const RandomGenerator& generator)
{
  try
  {
    impl->prev_time = impl->time;
    impl->time      = time;

      //mark bound volumes as dirty

    impl->is_dirty_bound_box = !impl->is_manual_bound_box;

      //update particle scene by all particle processors

    for (ParticleProcessorArray::iterator it=impl->processors.begin (), end=impl->processors.end (); it!=end; ++it)
    {
      ParticleProcessorInstance& instance = *it;

      instance.processor->Process (*this, generator, instance.instance_data);
    }

    if (impl->first_step)
    {
      impl->first_step = false;

      for (ParticleList::Iterator it=impl->particles.CreateIterator (); it; ++it)
      {
        Particle& p = *it;
  
        p.position_acceleration  = math::vec3f ();
        p.rotation_speed  = math::anglef ();
        p.size_speed  = math::vec2f ();
        p.color_speed  = math::vec4f ();
        p.animation_frame = p.animation_frame_offset;

        if (!p.creation_time.denominator ())
          p.creation_time = time;
      }

      return;
    }

      //do common logic for updates

    float        dt                     = (time - impl->prev_time).cast<float> ();
    unsigned int animation_fps          = impl->animation_fps,
                 animation_frames_count = impl->animation_frames.size ();

    for (ParticleList::Iterator it=impl->particles.CreateIterator (); it;)
    {
      Particle& p = *it;

      if (!p.creation_time.denominator ())
        p.creation_time = time;

      p.position_speed        += p.position_acceleration * dt;
      p.position              += p.position_speed * dt;
      p.position_acceleration  = math::vec3f ();

      p.rotation       += p.rotation_speed * dt;
      p.rotation_speed  = math::anglef ();

      p.size       += p.size_speed * dt;
      p.size        = math::max (p.size, math::vec2f ());
      p.size_speed  = math::vec2f ();

      p.color       += p.color_speed * dt;
      p.color_speed  = math::vec4f ();

      if (animation_frames_count)
        p.animation_frame = static_cast<unsigned int> ((time - p.creation_time).cast<float> () * animation_fps + p.animation_frame_offset) % animation_frames_count;
      else
        p.animation_frame = p.animation_frame_offset;

      p.lifetime = stl::max (0.0f, p.lifetime - dt);

      if (p.lifetime != 0.0f)
      {
        ++it;
      }
      else
      {
        ParticleList::Iterator next = it;

        ++next;

        impl->particles.Remove (it);

        it = next;
      }
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::particles::ParticleScene::Update");
    throw;
  }
}

/*
    Swap
*/

void ParticleScene::Swap (ParticleScene& scene)
{
  stl::swap (impl, scene.impl);
}

namespace media {
namespace particles {

void swap (ParticleScene& scene1, ParticleScene& scene2)
{
  scene1.Swap (scene2);
}

}}
