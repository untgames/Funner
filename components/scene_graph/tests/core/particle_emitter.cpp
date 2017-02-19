#include "shared.h"

int main ()
{
  printf ("Results of particle_emitter_test:\n");

  Node::Pointer particles_parent (Node::Create ());
  
  ParticleEmitter::Pointer particle_emitter (ParticleEmitter::Create ("particle_system1", particles_parent));

  printf ("Particle system id = '%s'\n", particle_emitter->ParticleSystemId ());
  printf ("Particles parent exist = %s\n", particle_emitter->ParticlesParent ().get () ? "true" : "false");

  particles_parent = 0;

  printf ("Particles parent exist = %s\n", particle_emitter->ParticlesParent ().get () ? "true" : "false");

  return 0;
}
