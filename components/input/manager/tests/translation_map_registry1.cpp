#include <stdio.h>

#include <input/translation_map_registry.h>

using namespace input;

const char* TRANSLATION_MAP1_PROFILE = "game_type_1.mouse.microsoft_wireless_mouse";
const char* TRANSLATION_MAP2_PROFILE = "game_type_2.mouse.microsoft_wireless_mouse";
const char* TRANSLATION_MAP3_PROFILE = "game_type_1.mouse.logitech_wired_mouse";
const char* TRANSLATION_MAP4_PROFILE = "game_type_1";
const char* TRANSLATION_MAP1_NAME    = "microsoft_wireless_mouse.keymap";
const char* TRANSLATION_MAP2_NAME    = "microsoft_wireless_mouse2.keymap";
const char* TRANSLATION_MAP3_NAME    = "logitech_wired_mouse.keymap";
const char* TRANSLATION_MAP4_NAME    = "game_type_1.keymap";

class MyEnumerator : public TranslationMapRegistry::IEnumerator
{
  void Process (const char* profile, const char* translation_map)
  {
    printf ("  Profile '%s' map is '%s'\n", profile, translation_map);
  }
};

int main ()
{
  printf ("Results of translation_map_registry1_test:\n");
  
  try
  {
    TranslationMapRegistry translation_map_registry;

    printf ("Has '%s' - '%c'\n", TRANSLATION_MAP1_PROFILE, translation_map_registry.Find (TRANSLATION_MAP1_PROFILE) ? 'y' : 'n');
    printf ("Has nearest '%s' - '%c'\n", TRANSLATION_MAP1_PROFILE, translation_map_registry.FindNearest (TRANSLATION_MAP1_PROFILE) ? 'y' : 'n');

    translation_map_registry.Register (TRANSLATION_MAP2_PROFILE, TRANSLATION_MAP2_NAME);
    translation_map_registry.Register (TRANSLATION_MAP3_PROFILE, TRANSLATION_MAP3_NAME);
    translation_map_registry.Register (TRANSLATION_MAP4_PROFILE, TRANSLATION_MAP4_NAME);

    MyEnumerator enumerator;

    printf ("Enumerating profiles:\n");

    translation_map_registry.Enumerate (enumerator);

    printf ("Has '%s' - '%c'\n", TRANSLATION_MAP1_PROFILE, translation_map_registry.Find (TRANSLATION_MAP1_PROFILE) ? 'y' : 'n');

    const char* translation_map = translation_map_registry.FindNearest (TRANSLATION_MAP1_PROFILE);
    
    printf ("Has nearest '%s' - '%c'\n", TRANSLATION_MAP1_PROFILE, translation_map ? 'y' : 'n');
    if (translation_map)
      printf ("Nearest name is '%s'\n", translation_map);

    translation_map_registry.Register (TRANSLATION_MAP1_PROFILE, TRANSLATION_MAP1_NAME);

    translation_map = translation_map_registry.Find (TRANSLATION_MAP1_PROFILE);
    printf ("Has '%s' - '%c'\n", TRANSLATION_MAP1_PROFILE, translation_map ? 'y' : 'n');
    if (translation_map)
      printf ("Name is '%s'\n", translation_map);
    
    translation_map = translation_map_registry.FindNearest (TRANSLATION_MAP1_PROFILE);
    
    printf ("Has nearest '%s' - '%c'\n", TRANSLATION_MAP1_PROFILE, translation_map ? 'y' : 'n');
    if (translation_map)
      printf ("Nearest name is '%s'\n", translation_map);

    translation_map_registry.Unregister (TRANSLATION_MAP1_PROFILE);

    printf ("Enumerating profiles:\n");

    translation_map_registry.Enumerate (enumerator);

    printf ("Has '%s' - '%c'\n", TRANSLATION_MAP1_PROFILE, translation_map_registry.Find (TRANSLATION_MAP1_PROFILE) ? 'y' : 'n');
    printf ("Has nearest '%s' - '%c'\n", TRANSLATION_MAP1_PROFILE, translation_map_registry.FindNearest (TRANSLATION_MAP1_PROFILE) ? 'y' : 'n');

    translation_map_registry.Clear ();
    
    printf ("Enumerating profiles:\n");

    translation_map_registry.Enumerate (enumerator);

    printf ("Has '%s' - '%c'\n", TRANSLATION_MAP1_PROFILE, translation_map_registry.Find (TRANSLATION_MAP1_PROFILE) ? 'y' : 'n');
    printf ("Has nearest '%s' - '%c'\n", TRANSLATION_MAP1_PROFILE, translation_map_registry.FindNearest (TRANSLATION_MAP1_PROFILE) ? 'y' : 'n');
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
