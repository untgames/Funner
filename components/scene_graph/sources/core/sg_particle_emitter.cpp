#include "shared.h"

using namespace scene_graph;

/*
    ParticleEmitter implementation
*/

struct ParticleEmitter::Impl: public xtl::instance_counter<ParticleEmitter>
{
  stl::string          particle_system_id;                   //particle system identifier (as loaded in media::ParticleSystemLibrary)
  Node*                particles_parent;                     //particles are emitted in coordinate space of this node
  xtl::auto_connection particles_parent_destroy_connection;  //particles parent node's destroy connection

  Impl (const char* in_particle_system_id, Node* in_particles_parent)
    : particle_system_id (in_particle_system_id), particles_parent (in_particles_parent)
  {
    particles_parent_destroy_connection = particles_parent->RegisterEventHandler (NodeEvent_BeforeDestroy, xtl::bind (&ParticleEmitter::Impl::OnBaseNodeDestroy, this));
  }

  void OnBaseNodeDestroy ()
  {
    particles_parent = 0;
  }
};

/*
    Constructor / destructor
*/

ParticleEmitter::ParticleEmitter (const char* particle_system_id, Node::Pointer particles_parent)
  : impl (new Impl (particle_system_id, particles_parent ? particles_parent.get () : this))
  {}

ParticleEmitter::~ParticleEmitter ()
{
  delete impl;
}

/*
    Emitter creation
*/

ParticleEmitter::Pointer ParticleEmitter::Create (const char* particle_system_id, Node::Pointer particles_parent)
{
  return Pointer (new ParticleEmitter (particle_system_id, particles_parent), false);
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
    Method which is called when this node is visited
*/

void ParticleEmitter::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    VisualModel::AcceptCore (visitor);
}
