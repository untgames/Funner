#include "shared.h"

int main ()
{
  printf ("Results of sound_emitter_test:\n");
  
  SoundEmitter::Pointer sound_emitter (SoundEmitter::Create ("sound_declaration1"));

  printf ("Sound emitter declaration = %s\n", sound_emitter->SoundDeclarationName ());  

  return 0;
}
