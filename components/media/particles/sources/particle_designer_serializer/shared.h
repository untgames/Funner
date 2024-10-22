#ifndef MEDIALIB_PARTICLES_PARTICLE_DESIGNER_SERIALIZER_SHARED_HEADER
#define MEDIALIB_PARTICLES_PARTICLE_DESIGNER_SERIALIZER_SHARED_HEADER

#include <stl/string>
#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/iterator.h>
#include <xtl/ref.h>
#include <xtl/string.h>

#include <common/component.h>
#include <common/file.h>
#include <common/log.h>
#include <common/parser.h>
#include <common/property_map.h>
#include <common/strlib.h>

#include <media/particles/particle_system_library.h>

namespace media
{

namespace particles
{

namespace particle_designer_serialzer
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Particle processor class. This class can generate new particles, modify existing or do any
///other particle scene processing
///////////////////////////////////////////////////////////////////////////////////////////////////
class ParticleProcessor : public IParticleProcessor, public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Constructor / destructor
///////////////////////////////////////////////////////////////////////////////////////////////////
    ParticleProcessor ();
    ~ParticleProcessor ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Set parameters
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetParameters (const common::PropertyMap& parameters);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Attach / detach scene for further processing
///////////////////////////////////////////////////////////////////////////////////////////////////
    void* AttachScene (ParticleScene& scene); //returns implementation data (may be null)
    void  DetachScene (ParticleScene& scene, void* private_data);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Process scene
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Process (ParticleScene& scene, const RandomGenerator& random_generator, void* private_data, const common::PropertyMap* properties);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Get particle size
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t ParticleSize ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Particle initializer
///////////////////////////////////////////////////////////////////////////////////////////////////
    void InitParticle (void* particle_data);

//////////////////////////////////////////////////////////////////////////////////////////////////
///Reference counting
//////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef () { addref (this); }
    void Release () { release (this); }

  private:
    ParticleProcessor (const ParticleProcessor&);             //no impl
    ParticleProcessor& operator = (const ParticleProcessor&); //no impl

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Class which is used for particle system setup
///////////////////////////////////////////////////////////////////////////////////////////////////
class ParticleSystemPrototype : public IParticleSystemPrototype, public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Constructor / destructor
///////////////////////////////////////////////////////////////////////////////////////////////////
    ParticleSystemPrototype ();
    ~ParticleSystemPrototype ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Add emitter
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddEmitter (const char* name, const math::vec2f& offset);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Set parameters
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetParameters (const common::PropertyMap& parameters);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Particle material
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetMaterialName (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Particles animation parameters
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetAnimationFramesCount (unsigned int count);

    ParticleTexDesc* AnimationFrames ();

    void SetAnimationFramesPerSecond (float count);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Configure particle system
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Configure (ParticleSystem& system);

//////////////////////////////////////////////////////////////////////////////////////////////////
///Reference counting
//////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef () { addref (this); }
    void Release () { release (this); }

  private:
    ParticleSystemPrototype (const ParticleSystemPrototype&);             //no impl
    ParticleSystemPrototype& operator = (const ParticleSystemPrototype&); //no impl

  private:
    struct Impl;
    Impl* impl;
};

}

}

}

#endif

