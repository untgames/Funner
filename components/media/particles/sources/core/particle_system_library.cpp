#include "shared.h"

using namespace media::particles;

namespace
{

typedef xtl::com_ptr<IParticleSystemPrototype> ParticleSystemPrototypePtr;

/*
   Particle system prototype descriptor
*/

struct ParticleSystemPrototypeDesc
{
  ParticleSystemPrototypePtr prototype;  //prototype object
  IParticleSystemPrototype*  pointer;    //prototype pointer (used for iterators)
  stl::string                name;       //name

  ParticleSystemPrototypeDesc (IParticleSystemPrototype& in_prototype, const char* in_name)
    : prototype (&in_prototype)
    , pointer (&in_prototype)
    , name (in_name)
    {}
};

typedef stl::hash_map<stl::hash_key<const char*>, ParticleSystemPrototypeDesc> ParticleSystemPrototypeMap;

///A more effect random number getter function, get from ejoy2d.
static void RANDOM_M11(size_t count, float* values)
{
  unsigned int seed = rand ();

  for (size_t i = 0; i < count; i++)
  {
    seed = seed * 134775813 + 1;

    union
    {
        uint32_t d;
        float f;
    } u;

    u.d = (((uint32_t)(seed) & 0x7fff) << 8) | 0x40000000;

    values [i] = u.f - 3.0f;
  }
}

}

/*
   Particle system library implementation
*/

struct ParticleSystemLibrary::Impl : public xtl::reference_counter
{
  stl::string                name;                       //library name
  ParticleSystemPrototypeMap particle_system_prototypes; //loaded particle system prototypes
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
    SetName (file_name);
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
    Identifier
*/

size_t ParticleSystemLibrary::Id () const
{
  return reinterpret_cast<size_t> (impl);
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

ParticleSystem ParticleSystemLibrary::CreateParticleSystem (const char* id, const RandomGenerator& random_generator) const
{
  static const char* METHOD_NAME = "media::particles::ParticleSystemLibrary::CreateParticleSystem";

  if (!id)
    throw xtl::make_null_argument_exception (METHOD_NAME, "id");

  IParticleSystemPrototype* prototype = const_cast<IParticleSystemPrototype*> (Find (id));

  if (!prototype)
    throw xtl::format_operation_exception (METHOD_NAME, "Can't find prototype with id '%s'", id);

  ParticleSystem return_value (random_generator);

  prototype->Configure (return_value);

  return return_value;
}

ParticleSystem ParticleSystemLibrary::CreateParticleSystem (const char* id) const
{
  return CreateParticleSystem (id, RandomGenerator (&RANDOM_M11));
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
  template <class T1> T& operator () (T1& value) const { return value.second.pointer; }
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

const char* ParticleSystemLibrary::ItemId (const ConstIterator& i) const
{
  const ParticleSystemPrototypeMap::iterator* iter = i.target<ParticleSystemPrototypeMap::iterator> ();

  if (!iter)
    throw xtl::make_argument_exception ("media::particles::ParticleSystemLibrary::ItemId", "iterator", "wrong-type");

  return (*iter)->second.name.c_str ();
}

/*
   Find element
*/

IParticleSystemPrototype* ParticleSystemLibrary::Find (const char* id)
{
  return const_cast<IParticleSystemPrototype*> (const_cast<const ParticleSystemLibrary&> (*this).Find (id));
}

const IParticleSystemPrototype* ParticleSystemLibrary::Find (const char* id) const
{
  if (!id)
    return 0;

  ParticleSystemPrototypeMap::const_iterator iter = impl->particle_system_prototypes.find (id);

  return iter != impl->particle_system_prototypes.end () ? iter->second.pointer : 0;
}

/*
   Attach / detach element
*/

void ParticleSystemLibrary::Attach (const char* id, const IParticleSystemPrototype& prototype)
{
  if (!id)
    throw xtl::make_null_argument_exception ("media::particles::ParticleSystemLibrary::Attach", "id");

  impl->particle_system_prototypes.insert_pair (id, ParticleSystemPrototypeDesc (const_cast<IParticleSystemPrototype&> (prototype), id));

}

void ParticleSystemLibrary::Detach (const char* id)
{
  if (!id)
    return;

  impl->particle_system_prototypes.erase (id);
}

void ParticleSystemLibrary::DetachAll ()
{
  impl->particle_system_prototypes.clear ();
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
    RegisterDefaultLoaders ();

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
    return;

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
   Register default serializers
*/

void ParticleSystemLibrary::RegisterDefaultLoaders ()
{
  static common::ComponentLoader loader ("media.particles.loaders.*");
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
