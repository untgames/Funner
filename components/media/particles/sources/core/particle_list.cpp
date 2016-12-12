#include "shared.h"

using namespace media::particles;

/*
===================================================================================================
    ParticlePool
===================================================================================================
*/

const size_t MIN_SUBPOOL_CAPACITY = 1024; //minimum particles count in pool

struct ParticleSubPool: public xtl::reference_counter
{
  typedef xtl::uninitialized_storage<char> Buffer;

  /// linear buffer with particles
  struct BufferDesc
  {
    Buffer buffer;          //buffer with particles data
    size_t capacity;        //capacity of this buffer
    size_t available_count; //available for allocation particles count

    BufferDesc () : capacity (), available_count () {}
  };

  typedef stl::list<BufferDesc> BufferList;

  ParticleNode* first;           //first node
  BufferList    buffers;         //buffers with particles
  size_t        particle_size;   //size of particle
  size_t        capacity;        //capacity of subpool
  
  ParticleSubPool (size_t in_particle_size)
    : first ()
    , particle_size (in_particle_size)
    , capacity ()
  {
  }

  ParticleNode* Allocate ()
  {
    for (;;)
    {
      if (first)
      {
        ParticleNode* result = first;

        first       = first->next;
        first->prev = 0;

        static_cast<BufferDesc*> (result->pool_link)->available_count--;

        return result;
      }

      Reserve (capacity >= MIN_SUBPOOL_CAPACITY ? capacity * 2 : MIN_SUBPOOL_CAPACITY);
    }
  }

  void Free (ParticleNode* particle)
  {
    particle->next = first;
    particle->prev = 0;

    if (first)
      first->prev = particle;

    first = particle;

    static_cast<BufferDesc*> (particle->pool_link)->available_count++;
  }

  void Reserve (size_t count)
  {
    if (capacity >= count)
      return;

    count -= capacity;

    if (count < MIN_SUBPOOL_CAPACITY)
      count = MIN_SUBPOOL_CAPACITY;

    Buffer buffer (particle_size * count);

    buffers.push_back ();

    BufferDesc& desc = buffers.back ();

    desc.capacity        = count;
    desc.available_count = count;
    
    swap (desc.buffer, buffer);

    char* it = desc.buffer.data ();

    for (size_t i=0; i<count; i++, it += particle_size)
    {
      ParticleNode* particle = reinterpret_cast<ParticleNode*> (it);

      particle->pool_link = &desc;
      particle->prev      = reinterpret_cast<ParticleNode*> (it - particle_size);
      particle->next      = reinterpret_cast<ParticleNode*> (it + particle_size);
    }

    reinterpret_cast<ParticleNode*> (desc.buffer.data ())->prev = 0;

    ParticleNode* last = reinterpret_cast<ParticleNode*> (desc.buffer.data () + desc.buffer.size () - particle_size);

    last->next  = first;
    first->prev = last;
    first       = last;

    capacity += count;
  }

  size_t Capacity ()
  {
    return capacity;
  }

  void Shrink (size_t count)
  {
    if (!count)
    {
      bool clear = true;

      for (BufferList::iterator it=buffers.begin (); it!=buffers.end (); ++it)
        if (it->available_count != it->capacity)
        {
          clear = false;
          break;
        }

      if (clear)
      {
        first    = 0;
        capacity = 0;

        buffers.clear ();

        return;
      }
    }

    for (BufferList::reverse_iterator it=buffers.rbegin (); count < capacity && it!=buffers.rend ();)
    {
      BufferDesc& desc = *it;

      if (desc.capacity != desc.available_count)
      {
        ++it;
        continue;
      }

      char* particle_ptr = desc.buffer.data ();

      for (size_t i=0; i<count; i++, particle_ptr += particle_size)
      {
        ParticleNode* particle = reinterpret_cast<ParticleNode*> (particle_ptr);

        if (particle->prev)
        {
          particle->prev->next = particle->next;
        }
        else
        {
          first       = particle->next;
          first->prev = 0;
        }

        if (particle->next)
          particle->next->prev = particle->prev;
      }

      capacity -= desc.capacity;

      BufferList::reverse_iterator next = it;

      ++next;

      buffers.erase (--(it.base ()));

      it = next;
    }
  }
};


typedef xtl::intrusive_ptr<ParticleSubPool>  ParticleSubPoolPtr;
typedef stl::map<size_t, ParticleSubPoolPtr> ParticleSubPoolMap;

struct ParticlePool::Impl: public xtl::reference_counter
{
  ParticleSubPoolMap subpools;              //sub pools for particles of different sizes
  size_t             last_particle_size;    //last used particle size
  ParticleSubPool*   last_particle_subpool; //last used particle subpool

  Impl ()
    : last_particle_size ()
    , last_particle_subpool ()
  {
  }

  static size_t GetNextHigherPowerOfTwo (size_t k)
  {
    if (!k)
      return 1;

    k--;

    for (size_t i=1; i < sizeof (size_t) * 8; i *= 2)
      k |= k >> i;

    return k + 1;
  }

  ParticleSubPool* FindPool (size_t particle_size)
  {
    if (particle_size == last_particle_size)
      return last_particle_subpool;

    ParticleSubPoolMap::iterator it = subpools.find (particle_size);

    if (it != subpools.end ())
    {
      last_particle_size    = particle_size;
      last_particle_subpool = &*it->second;

      return last_particle_subpool;
    }

    return 0;
  }

  ParticleSubPool& GetPool (size_t particle_size)
  {
    if (particle_size == last_particle_size)
      return *last_particle_subpool;

    particle_size = GetNextHigherPowerOfTwo (particle_size);

    ParticleSubPoolMap::iterator it = subpools.lower_bound (particle_size);

    if (it != subpools.end ())
    {
      last_particle_size    = particle_size;
      last_particle_subpool = &*it->second;

      return *last_particle_subpool;
    }

    ParticleSubPoolPtr subpool (new ParticleSubPool (particle_size), false);

    subpools.insert_pair (particle_size, subpool);

    last_particle_subpool = &*subpool;
    last_particle_size    = particle_size;

    return *subpool;
  }

  ParticleNode* Allocate (size_t particle_size)
  {
    ParticleSubPool& subpool = GetPool (particle_size);

    return subpool.Allocate ();
  }

  void Free (ParticleNode* particle, size_t particle_size)
  {
    ParticleSubPool* subpool = FindPool (particle_size);

    if (subpool)
      subpool->Free (particle);
  }

  size_t Capacity (size_t particle_size)
  {
    ParticleSubPool* subpool = FindPool (GetNextHigherPowerOfTwo (particle_size));

    if (subpool)
      return subpool->Capacity ();

    return 0;
  }

  void Reserve (size_t count, size_t particle_size)
  {
    ParticleSubPool& subpool = GetPool (particle_size);

    subpool.Reserve (count);
  }

  void Shrink (size_t count, size_t particle_size)
  {
    ParticleSubPool* subpool = FindPool (GetNextHigherPowerOfTwo (particle_size));

    if (subpool)
      subpool->Shrink (count);
  }
};

/*
    Constructos / destructor / assignment
*/

ParticlePool::ParticlePool ()
  : impl (new Impl)
{
}

ParticlePool::ParticlePool (const ParticlePool& pool)
  : impl (pool.impl)
{
  addref (impl);
}

ParticlePool::~ParticlePool ()
{
  release (impl);
}

ParticlePool& ParticlePool::operator = (const ParticlePool& pool)
{
  ParticlePool tmp = pool;

  stl::swap (impl, tmp.impl);

  return *this;
}

/*
    Change/check capacity
*/

size_t ParticlePool::Capacity (size_t particle_size) const
{
  return impl->Capacity (particle_size);
}

void ParticlePool::Reserve (size_t count, size_t particle_size)
{
  impl->Reserve (count, particle_size);
}

void ParticlePool::Shrink (size_t count, size_t particle_size)
{
  impl->Shrink (count, particle_size);
}

/*
===================================================================================================
    ParticleList
===================================================================================================
*/

struct ParticleList::Impl: public xtl::reference_counter
{
  ParticlePool        pool;            //pool for particles
  size_t              particle_size;   //size of particle
  size_t              particles_count; //particles count
  bool                has_initializer; //is this list has initializer
  ParticleInitializer initializer;     //initializer of particle
  ParticleNode*       first;           //first particle in list
  ParticleNode*       last;            //last particle in list

  Impl (const ParticlePool& in_pool, size_t in_particle_size, const ParticleInitializer& in_initializer)
    : pool (in_pool)
    , particle_size (in_particle_size)
    , particles_count ()
    , has_initializer (in_initializer)
    , initializer (in_initializer)
    , first ()
    , last ()
  {
    if (particle_size < sizeof (Particle))
      throw xtl::format_operation_exception ("media::particles::ParticleList::Impl::Impl", "particle_size should be >= %u", sizeof (Particle));
  }

  ParticleNode* Add ()
  {
    ParticleNode* particle = pool.impl->Allocate (particle_size);

    if (has_initializer)
      initializer (static_cast<Particle*> (particle));
    else
      new (particle) Particle ();

    particle->prev = last;
    particle->next = 0;
    
    if (last)
      last->next = particle;

    last = particle;

    if (!first)
      first = particle;

    particles_count++;

    return particle;
  }

  void Remove (ParticleNode* particle)
  {
    if (particle->prev) particle->prev->next = particle->next;
    else                first = particle->next;

    if (particle->next) particle->next->prev = particle->prev;
    else                last = particle->prev;

    particles_count--;

    pool.impl->Free (particle, particle_size);
  }
};

/*
    Constructors / destructor / assignment
*/

ParticleList::ParticleList ()
  : impl (new Impl (ParticlePool (), sizeof (Particle), ParticleInitializer ()))
{
}

ParticleList::ParticleList (size_t particle_size, const ParticleInitializer& initializer)
  : impl (new Impl (ParticlePool (), particle_size, initializer))
{
}

ParticleList::ParticleList (const ParticlePool& pool)
  : impl (new Impl (pool, sizeof (Particle), ParticleInitializer ()))
{
}

ParticleList::ParticleList (const ParticlePool& pool, size_t particle_size, const ParticleInitializer& initializer)
  : impl (new Impl (pool, particle_size, initializer))
{
}

ParticleList::ParticleList (const ParticleList& list)
  : impl (list.impl)
{
  addref (impl);
}

ParticleList::~ParticleList ()
{
  release (impl);
}

ParticleList& ParticleList::operator = (const ParticleList& list)
{
  ParticleList (list).Swap (*this);
  return *this;
}

/*
    Associated pool
*/

const ParticlePool& ParticleList::Pool () const
{
  return impl->pool;
}

ParticlePool& ParticleList::Pool ()
{
  return impl->pool;
}

/*
    Size of particle / particles count
*/

size_t ParticleList::ParticleSize () const
{
  return impl->particle_size;
}

size_t ParticleList::Count () const
{
  return impl->particles_count;
}

/*
    Particles iteration
*/

ParticleList::ConstIterator ParticleList::CreateIterator () const
{
  return ConstIterator (impl->first);
}

ParticleList::Iterator ParticleList::CreateIterator ()
{
  return Iterator (impl->first);
}

/*
    Adding / removing particles
*/

ParticleList::Iterator ParticleList::Add ()
{
  try
  {
    return Iterator (impl->Add ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::particles::ParticleList::Add");
    throw;
  }
}

void ParticleList::Remove (Iterator it)
{
  if (!it)
    return;

  impl->Remove (it.node);
}

void ParticleList::Clear ()
{
  while (impl->first)
    Remove (Iterator (impl->first));
}

/*
    Swap
*/

void ParticleList::Swap (ParticleList& list)
{
  stl::swap (impl, list.impl);
}

namespace media {
namespace particles {

void swap (ParticleList& p1, ParticleList& p2)
{
  p1.Swap (p2);
}

}}
