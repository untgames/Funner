#include "shared.h"

using namespace media::particles;
using namespace render::scene::client;

/*
    Particle system manager implementation
*/

typedef stl::list<ParticleSystemLibrary> ParticleSystemLibraryList;

struct ParticleSystemManager::Impl
{
  ParticleSystemLibraryList           particle_system_libraries;       //attached particle system libraries
  ParticleSystemRenderingTempCachePtr particle_system_rendering_cache; //particle system rendering cache
};

/*
    Constructor / destructor
*/

ParticleSystemManager::ParticleSystemManager ()
  : impl (new Impl ())
{
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

    for (ParticleSystemLibraryList::iterator iter = impl->particle_system_libraries.begin (), end = impl->particle_system_libraries.end (); iter != end; ++iter)
    {
      ParticleSystemLibrary&    library                   = *iter;
      IParticleSystemPrototype* particle_system_prototype = library.Find (particle_system_id);

      if (!particle_system_prototype)
        continue;

      ParticleSystem return_value;

      particle_system_prototype->Configure (return_value);

      return return_value;
    }

    throw xtl::format_operation_exception ("", "Can't create particle system with id '%s'. None of attached particle system libraries can do this", particle_system_id);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::ParticleSystemManager::CreateParticleSystem");
    throw;
  }
}

/*
   Attach / detach particle system libraries
*/

void ParticleSystemManager::AttachParticleSystemLibrary (const ParticleSystemLibrary& library)
{
  try
  {
    size_t id = library.Id ();

    for (ParticleSystemLibraryList::iterator iter = impl->particle_system_libraries.begin (), end = impl->particle_system_libraries.end (); iter != end; ++iter)
      if (id == iter->Id ())
        throw xtl::format_operation_exception ("", "Particle system library '%s' (with id %u) has been already defined", library.Name (), library.Id ());

    impl->particle_system_libraries.push_back (library);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::ParticleSystemManager::AttachParticleSystemLibrary");
    throw;
  }
}

void ParticleSystemManager::DetachParticleSystemLibrary (const ParticleSystemLibrary& library)
{
  size_t id = library.Id ();

  for (ParticleSystemLibraryList::iterator iter = impl->particle_system_libraries.begin (), end = impl->particle_system_libraries.end (); iter != end; ++iter)
    if (id == iter->Id ())
    {
      impl->particle_system_libraries.erase (iter);
      break;
    }
}

void ParticleSystemManager::DetachAllParticleSystemLibraries ()
{
  impl->particle_system_libraries.clear ();
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
