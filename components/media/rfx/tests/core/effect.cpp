#include "shared.h"

int main ()
{
  printf ("Results of effect_test:\n");

  try
  {
    Effect effect;
    
    dump ("default constructor", effect);
    
    effect.SetName ("effect1");
    
    TechniquePass pass1, pass2, pass3;
    
    pass1.Properties ().SetProperty ("A", "hello");
    pass2.Properties ().SetProperty ("B", "world");
    pass3.Properties ().SetProperty ("C", "!");
    
    Technique technique1, technique2;
    
    technique1.SetName ("technique1");
    technique2.SetName ("technique2");
    
    technique1.AddPass (pass1);
    technique1.AddPass (pass2);
    
    technique2.AddPass (pass2);
    technique2.AddPass (pass3);
    
    EffectParameter param1, param2;
    
    param1.SetName ("param1");
    param1.SetType (EffectParameterType_Float4);
    param1.SetElementsCount (3);
    
    param2.SetName ("param2");
    
    printf ("add parameter: %u\n", effect.AddParameter (param1));
    printf ("add parameter: %u\n", effect.AddParameter (param2));
    
    printf ("add technique: %u\n", effect.AddTechnique (technique1));
    printf ("add technique: %u\n", effect.AddTechnique (technique2));
    
    dump ("after initialize", effect);
    
    effect.RemoveParameter ("param1");
    effect.RemoveTechnique ("technique2");
    technique1.RemovePass (0);
    
    dump ("after remove", effect);
    
    Effect clone_effect = effect.Clone ();
    
    effect.Clear ();
    
    dump ("clone", clone_effect);
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
