#include "shared.h"

using namespace media::particles;
using namespace render::scene;
using namespace render::scene::client;

namespace render {
namespace scene {
namespace client {

//Particles rendering cache
typedef xtl::uninitialized_storage<interchange::SpriteDesc> SpriteBuffer;

struct ParticleSystemRenderingTempCache
{
  SpriteBuffer sprites;
};

}}}

namespace
{

/*
   Constants
*/

const char*  DEFAULT_BATCH_NAME             = "";                    //default rendering batch name
const char*  LOG_NAME                       = "render.scene.client"; //имя потока отладочного протоколирования
const size_t DEFAULT_RESERVED_SPRITES_COUNT = 64;                    //default reserved sprites count

/*
    Particle emitter
*/

class ParticleEmitter: public VisualModel
{
  public:
///Constructor
    ParticleEmitter (scene_graph::ParticleEmitter& model, SceneManager& manager)
      : VisualModel (model, manager, interchange::NodeType_SpriteList)
      , cache (GetRenderingCache (manager))
      , sprites (cache->sprites)
      , start_time (common::milliseconds ())
      , reserved_sprites_count (0)
      , need_create_sprite_lists (true)
    {
      try
      {
        particle_system = manager.Client ().ParticleSystemManager ().CreateParticleSystem (model.ParticleSystemId ());
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::scene::client::ParticleEmitter::ParticleEmitter");
        throw;
      }
    }

///Source node
    scene_graph::ParticleEmitter& SourceNode () const { return static_cast<scene_graph::ParticleEmitter&> (Node::SourceNode ()); }

  private:
/// Cache initialization
    static ParticleSystemRenderingTempCachePtr GetRenderingCache (SceneManager& manager)
    {
      try
      {
        ParticleSystemManager& particle_system_manager = manager.Client ().ParticleSystemManager ();

        ParticleSystemRenderingTempCachePtr cache = particle_system_manager.ParticleSystemRenderingTempCache ();

        if (cache)
          return cache;

        cache = ParticleSystemRenderingTempCachePtr (new ParticleSystemRenderingTempCache);

        particle_system_manager.SetParticleSystemRenderingTempCache (cache);

        return cache;
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::scene::client::ParticleEmitter::GetRenderingCache");
        throw;
      }
    }

  protected:
///Synchronization implementation
    void UpdateCore (client::Context& context)
    {
      try
      {
        //update base class
        VisualModel::UpdateCore (context);

        //Check if sprite lists should be updated
        object_id_t id = Id ();

        if (need_create_sprite_lists)
        {
          for (size_t i = 0, count = particle_system.ScenesCount (); i < count; i++)
          {
            //TODO non batching or different up vector doesn't work
            context.CreateSpriteList      (id, i, interchange::SpriteMode_OrientedBillboard, interchange::PrimitiveUsage_Batching, math::vec3f (0, 1.f, 0.f), DEFAULT_BATCH_NAME);
            context.SetSpriteListMaterial (id, i, particle_system.Scene (i).MaterialName ());
          }

          need_create_sprite_lists = false;
        }

        //update particle system
        particle_system.Update (TimeValue (common::milliseconds () - start_time, 1000));

        //update particles list
        for (size_t i = 0, count = particle_system.ScenesCount (); i < count; i++)
        {
          const ParticleScene& scene           = particle_system.Scene (i);
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

          sprites.resize (particles_count, false);

          interchange::SpriteDesc* sprite = sprites.data ();

          for (ParticleList::ConstIterator particle = particles.CreateIterator (); particle; ++particle)
          {
            if (needs_update_tex_coord)
            {
              unsigned int animation_frame = particle->animation_frame;

              if (animation_frame >= animation_frames_count)
              {
                animation_frame %= animation_frames_count;
                common::Log (LOG_NAME).Printf ("Particle for system '%s' wants to use animation frame %u, but system has only %u frames.", animation_frame, animation_frames_count);
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
          if (particles_count > reserved_sprites_count)
            reserved_sprites_count = particles_count;

          if (reserved_sprites_count < DEFAULT_RESERVED_SPRITES_COUNT)
            reserved_sprites_count = DEFAULT_RESERVED_SPRITES_COUNT;

          context.SetSpriteListBuffer (id, i, (uint32)particles_count, (uint32)reserved_sprites_count);

          //update sprites list
          if (particles_count)
            context.SetSpriteListDescs (id, i, 0, interchange::RawArray<interchange::SpriteDesc> (sprites.data (), (uint32)particles_count));
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::scene::client::ParticleEmitter::UpdateCore");
        throw;
      }
    }

  private:
    ParticleSystemRenderingTempCachePtr cache;                    //cache buffer
    SpriteBuffer&                       sprites;                  //sprites data buffer
    ParticleSystem                      particle_system;          //particle system of this emitter
    size_t                              start_time;               //object creation time                        //TODO should use node time???
    size_t                              reserved_sprites_count;   //context buffer reserved sprites count
    bool                                need_create_sprite_lists; //sprite lists should be updated
};

}

namespace render {
namespace scene {
namespace client {

Node* create_node (scene_graph::ParticleEmitter& model, SceneManager& scene_manager)
{
  return new ParticleEmitter (model, scene_manager);
}

}}}
