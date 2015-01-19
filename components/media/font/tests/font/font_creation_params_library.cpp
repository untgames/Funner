#include "shared.h"

using namespace media;

void dump (const FontCreationParamsLibrary& library)
{
  printf ("library '%s' (%u params)\n", library.Name (), library.Size ());

  for (FontCreationParamsLibrary::ConstIterator i=library.CreateIterator (); i; ++i)
    printf ("  params '%s' font size '%u'\n", library.ItemId (i), i->font_size);
}

int main ()
{
  printf ("Results of font_creation_params_library_test:\n");
  
  try
  {
    FontCreationParamsLibrary library;
    
    printf ("Library default dpi = %ux%u\n", library.DefaultHorizontalDpi (), library.DefaultVerticalDpi ());

    library.SetDefaultHorizontalDpi (10);
    library.SetDefaultVerticalDpi (20);

    printf ("Library new default dpi = %ux%u\n", library.DefaultHorizontalDpi (), library.DefaultVerticalDpi ());

    library.Rename ("library1");
    
    const char* params_name [] = {"params1", "params2", "params3", "params4"};
    
    printf ("attach params\n");
    
    for (size_t i=0; i<4; i++)
    {
      FontCreationParams params;

      params.font_size = i;

      library.Attach (params_name [i], params);
    }  
    
    dump (library);
    
    printf ("detach params\n");
    
    library.Detach ("params3");
    
    dump (library);
    
    printf ("clear library\n");
    
    library.Clear ();
    
    dump (library);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
