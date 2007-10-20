#include "shared.h"

typedef com_ptr<SoundEmitter> SoundEmitterPtr;

int main ()
{
  printf ("Results of sound_emitter_test:\n");
  
  SoundEmitterPtr sound_emitter (SoundEmitter::Create ("sound_declaration1"), false);

  printf ("Sound emitter declaration = %s\n", sound_emitter->SoundDeclarationName ());  

  return 0;
}
