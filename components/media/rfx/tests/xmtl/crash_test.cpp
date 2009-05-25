#include "shared.h"

const char* SRC_LIBRARY1_FILE_NAME = "data/MainMenu.xmtl";
const char* SRC_LIBRARY2_FILE_NAME = "data/Balls.xmtl";
const char* SRC_LIBRARY3_FILE_NAME = "data/Story.xmtl";

#include <stl/hash_map>

#include <xtl/iterator.h>

typedef SpriteMaterial::Pointer            SpriteMaterialPtr;
typedef stl::hash_map<stl::hash_key<const char*>, int> MaterialMap;

void log_handler (const char*)
{
}

void load_library (MaterialLibrary& library, MaterialMap& map)
{
  for (media::rfx::MaterialLibrary::Iterator iter=library.CreateIterator (); iter; ++iter)
  {
    media::rfx::Material::Pointer material = *iter;
    media::rfx::SpriteMaterial*   sprite_material = dynamic_cast<media::rfx::SpriteMaterial*> (&*material);

    map [library.ItemId (iter)] = (int)sprite_material;
  }
}

void unload_library (MaterialLibrary& library, MaterialMap& map)
{
  for (media::rfx::MaterialLibrary::Iterator iter=library.CreateIterator (); iter; ++iter)
  {
    media::rfx::Material::Pointer material = *iter;

    map.erase (library.ItemId (iter));
  }
}


int main ()
{
  try
  {
    printf ("Results of crash_test:\n");

    MaterialMap materials;

    MaterialLibrary library (SRC_LIBRARY1_FILE_NAME, log_handler);
    load_library (library, materials);

    MaterialLibrary library2 (SRC_LIBRARY2_FILE_NAME, log_handler);
    load_library (library2, materials);

    MaterialLibrary library3 (SRC_LIBRARY1_FILE_NAME, log_handler);
    unload_library (library3, materials);

    MaterialLibrary library4 (SRC_LIBRARY3_FILE_NAME, log_handler);
    load_library (library4, materials);

    printf ("pudum\n");
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  printf ("Success!!!\n");

  return 0;
}
