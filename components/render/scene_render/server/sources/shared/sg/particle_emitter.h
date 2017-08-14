#ifndef RENDER_SCENE_SERVER_PARTICLE_EMITTER_SHARED_HEADER
#define RENDER_SCENE_SERVER_PARTICLE_EMITTER_SHARED_HEADER

#include <shared/sg/sprite_list.h>

namespace render
{

namespace scene
{

namespace server
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Particle emitter
///////////////////////////////////////////////////////////////////////////////////////////////////
class ParticleEmitter: public SpriteList
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Constructor / destructor
///////////////////////////////////////////////////////////////////////////////////////////////////
    ParticleEmitter (RenderManager& render_manager);
    ~ParticleEmitter ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Particle system id
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetParticleSystemId (const char* id);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Update particle system
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateParticleSystem (interchange::uint32 new_time, const common::PropertyMap& properties);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

}

}

}

#endif
