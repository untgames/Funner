#include "shared.h"

using namespace media::particles;

/*
    Constants
*/

const size_t SCENE_RESERVE_SIZE = 8;

/*
    Implementation details
*/

typedef stl::vector<ParticleScene> SceneArray;

struct ParticleSystem::Impl: public xtl::reference_counter
{
  SceneArray      scenes;
  RandomGenerator random_generator;

  Impl (const RandomGenerator& in_generator)
    : random_generator (in_generator)
  {
    scenes.reserve (SCENE_RESERVE_SIZE);
  }
};

/*
    Constructors / destructor / assignment
*/

ParticleSystem::ParticleSystem ()
  : impl (new Impl (RandomGenerator ()))
{
}

ParticleSystem::ParticleSystem (const RandomGenerator& random_generator)
  : impl (new Impl (random_generator))
{
}

ParticleSystem::ParticleSystem (const ParticleSystem& ps)
  : impl (ps.impl)
{
  addref (impl);
}

ParticleSystem::~ParticleSystem ()
{
  release (impl);
}

ParticleSystem& ParticleSystem::operator = (const ParticleSystem& ps)
{
  ParticleSystem (ps).Swap (*this);
  return *this;
}

/*
    Scenes enumeration
*/

size_t ParticleSystem::ScenesCount () const
{
  return impl->scenes.size ();
}

ParticleScene& ParticleSystem::Scene (size_t index)
{
  if (index >= impl->scenes.size ())
    throw xtl::make_range_exception ("media::particles::ParticleSystem::Scene", "index", index, impl->scenes.size ());

  return impl->scenes [index];
}

const ParticleScene& ParticleSystem::Scene (size_t index) const
{
  if (index >= impl->scenes.size ())
    throw xtl::make_range_exception ("media::particles::ParticleSystem::Scene", "index", index, impl->scenes.size ());

  return impl->scenes [index];
}

/*
    Add / remove scenes
*/

size_t ParticleSystem::AddScene (const ParticleScene& scene)
{
  impl->scenes.push_back (scene);
  return impl->scenes.size () - 1;
}

void ParticleSystem::RemoveScene (size_t index)
{
  if (index >= impl->scenes.size ())
    return;

  impl->scenes.erase (impl->scenes.begin () + index);
}

void ParticleSystem::RemoveAllScenes ()
{
  impl->scenes.clear ();
}

/*
    Attach / detach particle processors
*/

void ParticleSystem::AttachProcessor (IParticleProcessor* processor)
{
  if (!processor)
    throw xtl::make_null_argument_exception ("media::particles::ParticleSystem::AttachProcessor", "processor");

  for (SceneArray::iterator it=impl->scenes.begin (); it!=impl->scenes.end (); ++it)
    it->AttachProcessor (processor);
}

void ParticleSystem::DetachProcessor (IParticleProcessor* processor)
{
  for (SceneArray::iterator it=impl->scenes.begin (); it!=impl->scenes.end (); ++it)
    it->DetachProcessor (processor);
}

void ParticleSystem::DetachAllProcessors ()
{
  for (SceneArray::iterator it=impl->scenes.begin (); it!=impl->scenes.end (); ++it)
    it->DetachAllProcessors ();
}

/*
    Update scenes
*/

void ParticleSystem::Update (const TimeValue& time, const common::PropertyMap* properties)
{
  try
  {
    for (SceneArray::iterator it=impl->scenes.begin (); it!=impl->scenes.end (); ++it)
    {
      ParticleScene& scene = *it;

      const TimeValue& start_time = scene.StartTimeOffset ();

      if (time < start_time)
        continue;

      TimeValue local_time = time - start_time;

      scene.Update (local_time, impl->random_generator, properties);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::particles::ParticleSystem::Update");
    throw;
  }
}

/*
    Swap
*/

void ParticleSystem::Swap (ParticleSystem& ps)
{
  stl::swap (impl, ps.impl);
}

namespace media {
namespace particles {

void swap (ParticleSystem& ps1, ParticleSystem& ps2)
{
  ps1.Swap (ps2);
}

}}
