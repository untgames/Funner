#include "shared.h"

using namespace scene_graph;

/*
    ParticleEmitter implementation
*/

typedef xtl::signal<void (ParticleEmitter& sender, ParticleEmitterEvent event)> ParticleEmitterSignal;

struct ParticleEmitter::Impl: public xtl::instance_counter<ParticleEmitter>
{
  stl::string             particle_system_id;                   //particle system identifier (as loaded in media::ParticleSystemLibrary)
  xtl::auto_connection    particles_parent_destroy_connection;  //particles parent node's destroy connection
  ParticleEmitterSignal   signals [ParticleEmitterEvent_Num];   //signals
  Node*                   particles_parent;                     //particles are emitted in coordinate space of this node
  scene_graph::SpriteMode sprite_mode;                          //sprite mode
  bool                    is_playing;                           //is emitter emits particles now

  Impl (const char* in_particle_system_id, Node* in_particles_parent, scene_graph::SpriteMode in_sprite_mode)
    : particles_parent (in_particles_parent)
    , sprite_mode (in_sprite_mode)
    , is_playing (true)
  {
    if (!in_particle_system_id)
      throw xtl::make_null_argument_exception ("scene_graph::ParticleEmitter::Impl::Impl", "particle_system_id");

    particle_system_id = in_particle_system_id;

    particles_parent_destroy_connection = particles_parent->RegisterEventHandler (NodeEvent_BeforeDestroy, xtl::bind (&ParticleEmitter::Impl::OnBaseNodeDestroy, this));
  }

  void OnBaseNodeDestroy ()
  {
    particles_parent = 0;
  }

  ///Notify about event
  void Notify (ParticleEmitter& emitter, ParticleEmitterEvent event)
  {
      //ignore if we have no handlers for this event

    if (!signals [event])
      return;

      //call event handlers

    try
    {
      signals [event] (emitter, event);
    }
    catch (...)
    {
      //suppress all exceptions
    }
  }
};

/*
    Constructor / destructor
*/

ParticleEmitter::ParticleEmitter (const char* particle_system_id, Node::Pointer particles_parent, scene_graph::SpriteMode sprite_mode)
  : impl (new Impl (particle_system_id, particles_parent ? particles_parent.get () : this, sprite_mode))
  {}

ParticleEmitter::~ParticleEmitter ()
{
  delete impl;
}

/*
    Emitter creation
*/

ParticleEmitter::Pointer ParticleEmitter::Create (const char* particle_system_id, Node::Pointer particles_parent, scene_graph::SpriteMode sprite_mode)
{
  return Pointer (new ParticleEmitter (particle_system_id, particles_parent, sprite_mode), false);
}

/*
   Particle system identifier (as loaded in media::ParticleSystemLibrary)
*/

const char* ParticleEmitter::ParticleSystemId () const
{
  return impl->particle_system_id.c_str ();
}

/*
   Particles are emitted in coordinate space of this node
*/

Node::Pointer ParticleEmitter::ParticlesParent () const
{
  return impl->particles_parent;
}

/*
    Sprite mode
*/

scene_graph::SpriteMode ParticleEmitter::SpriteMode () const
{
  return impl->sprite_mode;
}


/*
   Control simualtion process
*/

void ParticleEmitter::Play ()
{
  if (impl->is_playing)
    return;

  impl->is_playing = true;

  impl->Notify (*this, ParticleEmitterEvent_OnPlay);
}

bool ParticleEmitter::IsPlaying () const
{
  return impl->is_playing;
}

void ParticleEmitter::Pause ()
{
  if (!impl->is_playing)
    return;

  impl->is_playing = false;

  impl->Notify (*this, ParticleEmitterEvent_OnPause);
}

/*
   Registration for ParticleEmitter events
*/

xtl::connection ParticleEmitter::RegisterEventHandler (ParticleEmitterEvent event, const EventHandler& handler) const
{
  if (event < 0 || event >= ParticleEmitterEvent_Num)
    throw xtl::make_argument_exception ("scene_graph::ParticleEmitter::Event", "event", event);

  return impl->signals [event].connect (handler);
}

/*
    Method which is called when this node is visited
*/

void ParticleEmitter::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    VisualModel::AcceptCore (visitor);
}
