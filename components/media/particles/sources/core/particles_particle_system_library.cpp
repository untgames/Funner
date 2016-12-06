#include "shared.h"

using namespace media::particles;

/*
   Particle system library implementation
*/

typedef xtl::com_ptr<IParticleSystemPrototype> ParticleSystemPrototypePtr;
typedef stl::hash_map<stl::hash_key<const char*>, ParticleSystemPrototypePtr> ParticleSystemPrototypesMap;

struct ParticleSystemLibrary::Impl : public xtl::reference_counter
{
  stl::string                 name;                       //library name
  ParticleSystemPrototypesMap particle_system_prototypes; //loaded particle system prototypes
};

/*
   Constructors / destructor / assignment
*/

ParticleSystemLibrary::ParticleSystemLibrary ()
  : impl (new Impl)
{
}

ParticleSystemLibrary::ParticleSystemLibrary (const ParticleSystemLibrary& source)
  : impl (source.impl)
{
  addref (impl);
}

ParticleSystemLibrary::ParticleSystemLibrary (const char* file_name)
  : impl (new Impl)
{
  try
  {
    Load (file_name);
  }
  catch (xtl::exception& e)
  {
    release (impl);
    e.touch ("media::particles::ParticleSystemLibrary::ParticleSystemLibrary (const char*)");
    throw;
  }
  catch (...)
  {
    release (impl);
    throw;
  }
}

ParticleSystemLibrary::~ParticleSystemLibrary ()
{
  release (impl);
}

ParticleSystemLibrary& ParticleSystemLibrary::operator = (const ParticleSystemLibrary& source)
{
  ParticleSystemLibrary (source).Swap (*this);

  return *this;
}
    
/*
   Library name
*/

const char* ParticleSystemLibrary::Name () const
{
  return impl->name.c_str ();
}

void ParticleSystemLibrary::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::particles::ParticleSystemLibrary::SetName", name);

  impl->name = name;
}

/*
   Create particle system using particle system config handler attached to given id
*/

ParticleSystem ParticleSystemLibrary::CreateParticleSystem (const char* id) const
{

}

/*
   Attached config handlers count / check for emptiness
*/

size_t ParticleSystemLibrary::Size () const
{
  return impl->particle_system_prototypes.size ();
}

bool ParticleSystemLibrary::IsEmpty () const
{
  return impl->particle_system_prototypes.empty ();
}

/*
   Iterators
*/

namespace
{

//particle system prototype selector
template <class T>
struct prototype_selector
{
  template <class T1> T* operator () (T1& value) const { return value.second.get (); }
};

}

ParticleSystemLibrary::Iterator ParticleSystemLibrary::CreateIterator ()
{
  return Iterator (impl->particle_system_prototypes.begin (), impl->particle_system_prototypes.begin (), impl->particle_system_prototypes.end (), prototype_selector<IParticleSystemPrototype*> ());
}

ParticleSystemLibrary::ConstIterator ParticleSystemLibrary::CreateIterator () const
{
  return ConstIterator (impl->particle_system_prototypes.begin (), impl->particle_system_prototypes.begin (), impl->particle_system_prototypes.end (), prototype_selector<IParticleSystemPrototype*> ());
}

/*
   Get element identifier
*/

const char* ParticleSystemLibrary::ItemId (const ConstIterator&) const
{

}

/*
   Find element
*/

IParticleSystemPrototype* ParticleSystemLibrary::Find (const char* id)
{

}

const IParticleSystemPrototype* ParticleSystemLibrary::Find (const char* id) const
{

}

/*
   Attach / detach element
*/

void ParticleSystemLibrary::Attach (const char* id, const IParticleSystemPrototype&)
{

}

void ParticleSystemLibrary::Detach (const char* id)
{

}

void ParticleSystemLibrary::DetachAll ()
{

}

/*
   Clear library (detach all items and clear name)
*/

void ParticleSystemLibrary::Clear ()
{
  DetachAll ();

  impl->name.clear ();
}

/*
   Load / Unload
*/

void ParticleSystemLibrary::Load (const char* file_name)
{
  if (!file_name)
    throw xtl::make_null_argument_exception ("media::particles::ParticleSystemLibrary::Load", "file_name");

  try
  {
    static common::ComponentLoader loader ("media.particles.loaders.*");

    ParticleSystemLibraryManager::GetLoader (file_name, common::SerializerFindMode_ByName)(file_name, *this);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::particles::ParticleSystemLibrary::Load");
    throw;
  }
}

void ParticleSystemLibrary::Unload (const char* file_name)
{
  if (!file_name)
    throw xtl::make_null_argument_exception ("media::particles::ParticleSystemLibrary::Unload", "file_name");

  try
  {
    //TODO
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::particles::ParticleSystemLibrary::Unload");
    throw;
  }
}

/*
   Swap
*/

void ParticleSystemLibrary::Swap (ParticleSystemLibrary& source)
{
  stl::swap (impl, source.impl);
}

namespace media
{

namespace particles
{

/*
   Swap
*/

void swap (ParticleSystemLibrary& library1, ParticleSystemLibrary& library2)
{
  library1.Swap (library2);
}

}

}
