#include <stdio.h>
#include <media/sound_source.h>

using namespace media;

void dump (SoundSource& sound_source)
{
  printf ("SoundSource type: '%s'\n", sound_source.Type ());
  printf ("Looping: '%s'\n", sound_source.Looping () ? "looped" : "not looped");
  printf ("Sounds count: %u\n", sound_source.SoundsCount ());
  if (sound_source.SoundsCount ())
  {
    printf ("Sounds:\n");
    for (size_t i = 0; i < sound_source.SoundsCount (); i++)
      printf ("  '%s'\n", sound_source.Sound (i));
  }
  printf ("Gain: %f\n", sound_source.Param (SoundSourceParam_Gain));
  printf ("Minimum gain: %f\n", sound_source.Param (SoundSourceParam_MinimumGain));
  printf ("Maximum gain: %f\n", sound_source.Param (SoundSourceParam_MaximumGain));
  printf ("Inner angle: %f\n", sound_source.Param (SoundSourceParam_InnerAngle));
  printf ("Outer angle: %f\n", sound_source.Param (SoundSourceParam_OuterAngle));
  printf ("Outer gain: %f\n", sound_source.Param (SoundSourceParam_OuterGain));
  printf ("Reference distance: %f\n", sound_source.Param (SoundSourceParam_ReferenceDistance));
  printf ("Maximum distance: %f\n", sound_source.Param (SoundSourceParam_MaximumDistance));
  printf ("Cull distance: %f\n", sound_source.Param (SoundSourceParam_CullDistance));
}

int main ()
{
  SoundSource sound_source, sound_source2;

  printf ("Results of sound_source_test:\n");

  sound_source.SetType    ("Type1");
  sound_source.SetLooping (true);
  sound_source.AddSound   ("sound1.ogg");
  sound_source.AddSound   ("sound2.ogg");
  sound_source.SetParam   (SoundSourceParam_Gain, 0.7f);
  sound_source.SetParam   (SoundSourceParam_MinimumGain, 0.1f);
  sound_source.SetParam   (SoundSourceParam_MaximumGain, 0.95f);
  sound_source.SetParam   (SoundSourceParam_InnerAngle, 20.f);
  sound_source.SetParam   (SoundSourceParam_OuterAngle, 60.f);
  sound_source.SetParam   (SoundSourceParam_OuterGain, 0.4f);
  sound_source.SetParam   (SoundSourceParam_ReferenceDistance, 25.f);
  sound_source.SetParam   (SoundSourceParam_MaximumDistance, 90.f);
  sound_source.SetParam   (SoundSourceParam_CullDistance, 95.f);
  printf ("Modified sound source:\n");
  dump   (sound_source);

  sound_source.Swap (sound_source2);
  printf ("Initial sound source:\n");
  dump   (sound_source);
  
  sound_source2.RemoveSound (0);
  printf ("Deleting one sound:\n");
  dump   (sound_source2);  

  return 0;
}
