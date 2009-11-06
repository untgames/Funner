#include "shared.h"

int main ()
{
  printf ("Results of particle_emitter_test:\n");

  Node::Pointer particles_parent (Node::Create ());
  
  ParticleEmitter::Pointer particle_emitter (ParticleEmitter::Create ("particle_system1", particles_parent));

  printf ("Declaration = '%s'\n", particle_emitter->DeclarationName ());
  printf ("Particles parent exist = %s\n", particle_emitter->ParticlesParent ().get () ? "true" : "false");

  particles_parent = 0;

  printf ("Particles parent exist = %s\n", particle_emitter->ParticlesParent ().get () ? "true" : "false");

  return 0;
}
