#include "shared.h"

void particle_emitter_event_handler (ParticleEmitter& sender, ParticleEmitterEvent event)
{
  printf ("Particle emitter event %d, is playing - %s\n", event, sender.IsPlaying () ? "true" : "false");
}

int main ()
{
  printf ("Results of particle_emitter_test:\n");

  Node::Pointer particles_parent (Node::Create ());
  
  ParticleEmitter::Pointer particle_emitter (ParticleEmitter::Create ("particle_system1", particles_parent));

  particle_emitter->RegisterEventHandler (ParticleEmitterEvent_OnPlay, &particle_emitter_event_handler);
  particle_emitter->RegisterEventHandler (ParticleEmitterEvent_OnPause, &particle_emitter_event_handler);

  printf ("Particle system id = '%s'\n", particle_emitter->ParticleSystemId ());
  printf ("Particles parent exist = %s\n", particle_emitter->ParticlesParent ().get () ? "true" : "false");
  printf ("Particle system playing = %s\n", particle_emitter->IsPlaying () ? "true" : "false");

  particles_parent = 0;

  printf ("Particles parent exist = %s\n", particle_emitter->ParticlesParent ().get () ? "true" : "false");

  particle_emitter->Pause ();

  printf ("Particle system playing after stop = %s\n", particle_emitter->IsPlaying () ? "true" : "false");

  particle_emitter->Play ();

  printf ("Particle system playing after play = %s\n", particle_emitter->IsPlaying () ? "true" : "false");

  return 0;
}
