#include "shared.h"

int main ()
{
  printf ("Results of sound_emitter_test:\n");
  
  SoundEmitter::Pointer sound_emitter (SoundEmitter::Create ("sound_declaration1"));

  printf ("Sound emitter declaration = %s\n", sound_emitter->SoundDeclarationName ());  

  printf ("Gain is %.2f\n", sound_emitter->Gain ());
  printf ("PlayStartOffset is %.2f\n", sound_emitter->PlayStartOffset ());

  sound_emitter->SetGain (0.7f);
  sound_emitter->Play ();

  printf ("Gain is %.2f\n", sound_emitter->Gain ());
  printf ("PlayStartOffset is %.2f\n", sound_emitter->PlayStartOffset ());

  sound_emitter->Play (1.f);

  printf ("PlayStartOffset is %.2f\n", sound_emitter->PlayStartOffset ());

  return 0;
}
