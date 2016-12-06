#ifndef MEDIALIB_PARTICLES_PARTICLE_SCENE_HEADER
#define MEDIALIB_PARTICLES_PARTICLE_SCENE_HEADER

#include <bv/axis_aligned_box.h>

#include <media/particles/particle_list.h>

namespace xtl
{

//forward declaration
template <class I> class rational;

}

namespace media
{

namespace particles
{

typedef xtl::rational<size_t> TimeValue;  //particle scene time type

//forward declaration
class ParticleScene;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Particle processor class. This class can generate new particles, modify existing or do any
///other particle scene processing
///////////////////////////////////////////////////////////////////////////////////////////////////
class IParticleProcessor
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Attach / detach scene for further processing
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void* AttachScene (ParticleScene& scene) = 0; //returns implementation data (may be null)
    virtual void  DetachScene (ParticleScene& scene, void* private_data) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Process scene
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Process (ParticleScene& scene, void* private_data) = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////
///Reference counting
//////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void AddRef () = 0;
    virtual void Release () = 0;

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Destructor
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ~IParticleProcessor () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Particle texture coordinates desc
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ParticleTexDesc
{
  math::vec2f tex_offset;  //tex coord offset
  math::vec2f tex_size;    //tex coord size
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Particle scene
///////////////////////////////////////////////////////////////////////////////////////////////////
class ParticleScene
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Constructors / destructor / assignment
///////////////////////////////////////////////////////////////////////////////////////////////////
    ParticleScene  ();
    ParticleScene  (const ParticleScene&);
    ~ParticleScene ();

    ParticleScene& operator = (const ParticleScene&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Particle material
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* MaterialName    () const;
    void        SetMaterialName ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Particles animation parameters
///////////////////////////////////////////////////////////////////////////////////////////////////
    unsigned int AnimationFramesCount    () const;
    void         SetAnimationFramesCount (unsigned int count);

    unsigned int AnimationFramesPerSecond    () const;
    void         SetAnimationFramesPerSecond (unsigned int count);

          ParticleTexDesc* AnimationFrames ();
    const ParticleTexDesc* AnimationFrames () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Access to particles data
///////////////////////////////////////////////////////////////////////////////////////////////////
    const ParticleList& Particles () const;
          ParticleList& Particles ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Scene position offset, used if it's needed to apply some offset for whole scene
///(for example random offset for fireworks explosions)
///////////////////////////////////////////////////////////////////////////////////////////////////
    const math::vec3f& Offset    () const;
    void               SetOffset (const math::vec3f&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Start time offset - particle scene will not start for this time
///////////////////////////////////////////////////////////////////////////////////////////////////
    void  SetStartTimeOffset (float time);
    float StartTimeOffset    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Scene bound box. If bound box was set, it will be always returned. If it was not set or was
///reset bound box will be recalculated on request using actual particles data
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                         SetBoundBox   (const bound_volumes::aaboxf& box);
    void                         ResetBoundBox ();
    const bound_volumes::aaboxf& BoundBox      () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Attaching / detaching of particles processors
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AttachProcessor     (IParticleProcessor& processor);
    void DetachProcessor     (IParticleProcessor& processor);
    void DetachAllProcessors ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Scene current time and prev update time
///////////////////////////////////////////////////////////////////////////////////////////////////
    const TimeValue& Time           () const;
    const TimeValue& PrevUpdateTime () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Perform scene update (updates all particles)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Update (const TimeValue& time);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Swap
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (ParticleScene&);

  private:
  	struct Impl;
  	Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Swap
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (ParticleScene&, ParticleScene&);

}

}

#endif
