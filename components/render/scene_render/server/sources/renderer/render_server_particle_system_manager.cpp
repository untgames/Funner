#include "shared.h"

using namespace media::particles;
using namespace render::scene::server;

/*
    Particle system manager implementation
*/

struct ParticleSystemManager::Impl
{
  ParticleSystemLibrary               library;                         //particle system library to load resources to
  ParticleSystemRenderingTempCachePtr particle_system_rendering_cache; //particle system rendering cache
};

/*
    Constructor / destructor
*/

ParticleSystemManager::ParticleSystemManager ()
  : impl (new Impl ())
{
  ParticleSystemLibrary::RegisterDefaultLoaders ();
}

ParticleSystemManager::~ParticleSystemManager ()
{
}

/*
   Create particle system
*/

ParticleSystem ParticleSystemManager::CreateParticleSystem (const char* particle_system_id)
{
  try
  {
    if (!particle_system_id)
      throw xtl::make_null_argument_exception ("", "particle_system_id");

    IParticleSystemPrototype* particle_system_prototype = impl->library.Find (particle_system_id);

    if (!particle_system_prototype)
      throw xtl::format_operation_exception ("", "Can't create particle system with id '%s'.", particle_system_id);

    ParticleSystem return_value;

    particle_system_prototype->Configure (return_value);

    return return_value;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::ParticleSystemManager::CreateParticleSystem");
    throw;
  }
}

/*
   Load / unload particle systems
*/

void ParticleSystemManager::Load (const char* name)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");

    impl->library.Load (name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::ParticleSystemManager::Load");
    throw;
  }
}

void ParticleSystemManager::Unload (const char* name)
{
  try
  {
    if (!name)
      return;

    impl->library.Unload (name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::ParticleSystemManager::Unload");
    throw;
  }
}

/*
   Check if given resource name can be loaded by this manager
*/

bool ParticleSystemManager::IsSupportedResource (const char* name)
{
  return name && (media::particles::ParticleSystemLibraryManager::FindLoader (name, common::SerializerFindMode_ByName) != 0);
}

/*
    Particle system rendering cache
*/

void ParticleSystemManager::SetParticleSystemRenderingTempCache (const ParticleSystemRenderingTempCachePtr& cache)
{
  impl->particle_system_rendering_cache = cache;
}

const ParticleSystemRenderingTempCachePtr& ParticleSystemManager::ParticleSystemRenderingTempCache () const
{
  return impl->particle_system_rendering_cache;
}
