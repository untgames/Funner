#include <stdio.h>
#include <media/sound_declaration.h>

using namespace media;

void dump (SoundDeclaration& sound_decl)
{
  printf ("SoundDeclaration type: '%s'\n", sound_decl.Type ());
  printf ("Looping: '%s'\n", sound_decl.Looping () ? "looped" : "not looped");
  printf ("Samples count: %u\n", sound_decl.SamplesCount ());
  
  if (sound_decl.SamplesCount ())
  {
    printf ("Samples:\n");
    for (size_t i = 0; i < sound_decl.SamplesCount (); i++)
      printf ("  '%s'\n", sound_decl.Sample (i));
  }

  printf ("Gain: %f\n", sound_decl.Param (SoundParam_Gain));
  printf ("Minimum gain: %f\n", sound_decl.Param (SoundParam_MinimumGain));
  printf ("Maximum gain: %f\n", sound_decl.Param (SoundParam_MaximumGain));
  printf ("Inner angle: %f\n", sound_decl.Param (SoundParam_InnerAngle));
  printf ("Outer angle: %f\n", sound_decl.Param (SoundParam_OuterAngle));
  printf ("Outer gain: %f\n", sound_decl.Param (SoundParam_OuterGain));
  printf ("Reference distance: %f\n", sound_decl.Param (SoundParam_ReferenceDistance));
  printf ("Maximum distance: %f\n", sound_decl.Param (SoundParam_MaximumDistance));
  printf ("Cull distance: %f\n", sound_decl.Param (SoundParam_CullDistance));
}

int main ()
{
  printf ("Results of sound_declaration_test:\n");

  SoundDeclaration sound_decl, sound_decl2;

  sound_decl.SetType    ("Type1");
  sound_decl.SetLooping (true);
  sound_decl.AddSample  ("sound1.ogg");
  sound_decl.AddSample  ("sound2.ogg");
  sound_decl.SetParam   (SoundParam_Gain, 0.7f);
  sound_decl.SetParam   (SoundParam_MinimumGain, 0.1f);
  sound_decl.SetParam   (SoundParam_MaximumGain, 0.95f);
  sound_decl.SetParam   (SoundParam_InnerAngle, 20.f);
  sound_decl.SetParam   (SoundParam_OuterAngle, 60.f);
  sound_decl.SetParam   (SoundParam_OuterGain, 0.4f);
  sound_decl.SetParam   (SoundParam_ReferenceDistance, 25.f);
  sound_decl.SetParam   (SoundParam_MaximumDistance, 90.f);
  sound_decl.SetParam   (SoundParam_CullDistance, 95.f);
  printf ("Modified sound declaration:\n");
  dump   (sound_decl);

  sound_decl.Swap (sound_decl2);
  printf ("Initial sound declaration:\n");
  dump   (sound_decl);
  
  sound_decl2.RemoveSample (0);
  printf ("Deleting one sound:\n");
  dump   (sound_decl2);

  return 0;
}
