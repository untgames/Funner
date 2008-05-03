#include <stdio.h>
#include <exception>
#include <media/sound_declaration.h>

using namespace media;

const char* RESULT_FILE_NAME = "/io/stdout/test.snddecl";

int main ()
{
  printf ("Results of snddecl_saver_test:\n");
  
  try
  {
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

    sound_decl2 = clone (sound_decl);
    
    sound_decl2.SetParam (SoundParam_OuterAngle, 2.0f);
    sound_decl2.SetParam (SoundParam_Gain, 0.2f);
    sound_decl2.SetLooping (false);
    sound_decl2.SetType ("Type2");
    
    sound_decl2.RemoveSample (0);
    
    SoundDeclarationLibrary library;
    
    library.Attach ("declaration1", sound_decl);
    library.Attach ("declaration2", sound_decl2);
    
    library.Save (RESULT_FILE_NAME);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
