#include "shared.h"

using namespace media::particles;
using namespace render::scene;
using namespace render::scene::server;

namespace
{

//Constants
const char*  DEFAULT_BATCH_NAME             = "particles"; //default rendering batch name
const size_t DEFAULT_RESERVED_SPRITES_COUNT = 64;          //default reserved sprites count

}

namespace render
{

namespace scene
{

namespace server
{

//Particles rendering cache
typedef xtl::uninitialized_storage<interchange::SpriteDesc> SpriteBuffer;

struct ParticleSystemRenderingTempCache
{
  SpriteBuffer sprites;
};

}

}

}

/*
    Particle emitter implementation
*/

struct ParticleEmitter::Impl
{
  manager::Entity&                    entity;          //entity
  RenderManager                       render_manager;  //render manager
  ParticleSystem                      particle_system; //particle system of this emitter
  ParticleSystemRenderingTempCachePtr cache;           //cache buffer
  SpriteBuffer&                       sprites;         //sprites data buffer

/// Constructor
  Impl (RenderManager& in_render_manager, manager::Entity& in_entity)
    : entity (in_entity)
    , render_manager (in_render_manager)
    , cache (GetRenderingCache (in_render_manager))
    , sprites (cache->sprites)
    {}

  /// Cache initialization
  static ParticleSystemRenderingTempCachePtr GetRenderingCache (RenderManager& manager)
  {
    try
    {
      ParticleSystemManager& particle_system_manager = manager.ParticleSystemManager ();

      ParticleSystemRenderingTempCachePtr cache = particle_system_manager.ParticleSystemRenderingTempCache ();

      if (cache)
        return cache;

      cache = ParticleSystemRenderingTempCachePtr (new ParticleSystemRenderingTempCache);

      particle_system_manager.SetParticleSystemRenderingTempCache (cache);

      return cache;
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::server::ParticleEmitter::GetRenderingCache");
      throw;
    }
  }
};

/*
    Constructor / destructor
*/

ParticleEmitter::ParticleEmitter (RenderManager& render_manager)
  : SpriteList (render_manager)
  , impl (new Impl (render_manager, Entity ()))
{
}

ParticleEmitter::~ParticleEmitter ()
{
}

/*
   Particle system id
*/

void ParticleEmitter::SetParticleSystemId (const char* id)
{
  try
  {
    if (!id)
      throw xtl::make_null_argument_exception ("", "id");

    //do not allow to change scene after first setup
    if (impl->particle_system.ScenesCount ())
      throw xtl::format_operation_exception ("", "Particle system id can not be changed.");

    impl->particle_system = impl->render_manager.ParticleSystemManager ().CreateParticleSystem (id);

    for (size_t i = 0, count = impl->particle_system.ScenesCount (); i < count; i++)
    {
      CreateList (i, interchange::SpriteMode_OrientedBillboard, interchange::PrimitiveUsage_Batching, math::vec3f (0, 1.f, 0.f), DEFAULT_BATCH_NAME);
      SetMaterial (i, impl->particle_system.Scene (i).MaterialName ());
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::ParticleEmitter::SetParticleSystemId");
    throw;
  }
}

/*
   Update particle system
*/

void ParticleEmitter::UpdateParticleSystem (interchange::uint32 new_time, const common::PropertyMap& properties)
{
  try
  {
    //update particle system
    impl->particle_system.Update (TimeValue (new_time, 1000), &properties);

    //update particles list
    for (size_t i = 0, count = impl->particle_system.ScenesCount (); i < count; i++)
    {
      const ParticleScene& scene           = impl->particle_system.Scene (i);
      const ParticleList&  particles       = scene.Particles ();
      size_t               particles_count = particles.Count ();
      math::vec2f          tex_offset, tex_size;
      bool                 needs_update_tex_coord;

      unsigned int animation_frames_count = scene.AnimationFramesCount ();

      if (animation_frames_count == 1)  //most common case, use it as first condition
      {
        const ParticleTexDesc* tex_desc = scene.AnimationFrames ();

        tex_offset             = tex_desc->tex_offset;
        tex_size               = tex_desc->tex_size;
        needs_update_tex_coord = false;
      }
      else if (!animation_frames_count)
      {
        //we do not have animation frames info, use full texture
        tex_size               = 1.f;
        needs_update_tex_coord = false;
      }
      else
        needs_update_tex_coord = true;

      impl->sprites.resize (particles_count, false);

      interchange::SpriteDesc* sprite = impl->sprites.data ();

      for (ParticleList::ConstIterator particle = particles.CreateIterator (); particle; ++particle)
      {
        if (needs_update_tex_coord)
        {
          unsigned int animation_frame = particle->animation_frame;

          if (animation_frame >= animation_frames_count)
          {
            animation_frame %= animation_frames_count;
            impl->render_manager.Log ().Printf ("Particle for system '%s' wants to use animation frame %u, but system has only %u frames.", animation_frame, animation_frames_count);
          }

          const ParticleTexDesc* tex_desc = scene.AnimationFrames () + animation_frame;

          sprite->tex_offset = tex_desc->tex_offset;
          sprite->tex_size   = tex_desc->tex_size;
        }
        else
        {
          sprite->tex_offset = tex_offset;
          sprite->tex_size   = tex_size;
        }

        sprite->position = particle->position;
        sprite->size     = particle->size;
        sprite->color    = particle->color;
        sprite->normal   = math::vec3f (0, 0, 1.0f);
        sprite->rotation = particle->rotation;

        sprite++;
      }

      //update list size
      size_t reserved_sprites_count = Capacity (i);

      if (particles_count > reserved_sprites_count)
        reserved_sprites_count = particles_count;

      if (reserved_sprites_count < DEFAULT_RESERVED_SPRITES_COUNT)
        reserved_sprites_count = DEFAULT_RESERVED_SPRITES_COUNT;

      Reserve (i, reserved_sprites_count);
      Resize (i, particles_count);

      //update sprites list
      if (particles_count)
        SetDescs (i, 0, particles_count, impl->sprites.data ());
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::ParticleEmitter::SetParticleSystemTime");
    throw;
  }
}
