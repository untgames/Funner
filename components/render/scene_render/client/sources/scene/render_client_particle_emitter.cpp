#include "shared.h"

using namespace media::particles;
using namespace render::scene;
using namespace render::scene::client;

namespace
{

/*
    Node controller which tracks particle emitter play time
*/

class ParticleEmitterController : public scene_graph::Controller
{
  public:
    typedef xtl::com_ptr<ParticleEmitterController> Pointer;

    ///Controller creation
    static Pointer Create (scene_graph::ParticleEmitter& node)
    {
      return Pointer (new ParticleEmitterController (node), false);
    }

    const TimeValue& PlayTime ()
    {
      return play_time;
    }

  protected:
    ///Constructor
    ParticleEmitterController (scene_graph::ParticleEmitter& node)
      : scene_graph::Controller (node, scene_graph::ControllerTimeMode_Delta)
      , emitter (&node)
    {
      //we do not need to own emitter or controller, because controller will be destroyed together with emitter render, which will be destroyed together with scene graph emitter
      SetOwnerMode (scene_graph::ControllerOwnerMode_None);
    }

  private:
    ///Update handler
    void Update (const scene_graph::TimeValue& dt)
    {
      if (emitter->IsPlaying ())
        play_time += dt;
    }

  private:
    scene_graph::ParticleEmitter* emitter;    //emitter node
    TimeValue                     play_time;  //total play time
};

/*
    Particle emitter
*/

class ParticleEmitter: public VisualModel
{
  public:
///Constructor
    ParticleEmitter (scene_graph::ParticleEmitter& model, SceneManager& manager)
      : VisualModel (model, manager, interchange::NodeType_ParticleEmitter)
    {
      manager.Context ().SetParticleEmitterSystemId (Id (), model.ParticleSystemId ());

      try
      {
        emitter_controller = ParticleEmitterController::Create (model);
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::scene::client::ParticleEmitter::ParticleEmitter");
        throw;
      }
    }

///Source node
    scene_graph::ParticleEmitter& SourceNode () const { return static_cast<scene_graph::ParticleEmitter&> (Node::SourceNode ()); }

  protected:
///Synchronization implementation
    void UpdateCore (client::Context& context)
    {
      try
      {
        //update base class
        VisualModel::UpdateCore (context);

        const TimeValue& current_play_time = emitter_controller->PlayTime ();

        //Check if server object should be updated

        if (current_play_time != last_play_time)
        {
          object_id_t id = Id ();

          context.SetParticleEmitterSystemTime (id, current_play_time.numerator () / (current_play_time.denominator () / 1000));

          last_play_time = current_play_time;
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::scene::client::ParticleEmitter::UpdateCore");
        throw;
      }
    }

  private:
    ParticleEmitterController::Pointer  emitter_controller;       //emitter controller for tracking play time
    TimeValue                           last_play_time;           //last emitter play time
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
