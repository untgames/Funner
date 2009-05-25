#include "shared.h"

const char* SRC_LIBRARY1_FILE_NAME = "data/MainMenu.xmtl";
const char* SRC_LIBRARY2_FILE_NAME = "data/Balls.xmtl";
const char* SRC_LIBRARY3_FILE_NAME = "data/Story.xmtl";

#include <stl/hash_map>

#include <xtl/iterator.h>

typedef SpriteMaterial::Pointer            SpriteMaterialPtr;
typedef stl::hash_map<stl::hash_key<const char*>, SpriteMaterialPtr> MaterialMap;

void log_handler (const char*)
{
}

void load_library (const char* file_name, MaterialMap& map)
{
  MaterialLibrary library (file_name, log_handler);

  for (media::rfx::MaterialLibrary::Iterator iter=library.CreateIterator (); iter; ++iter)
  {
    media::rfx::Material::Pointer material = *iter;
    media::rfx::SpriteMaterial*   sprite_material = dynamic_cast<media::rfx::SpriteMaterial*> (&*material);

    map [library.ItemId (iter)] = sprite_material;
  }
}

void unload_library (const char* file_name, MaterialMap& map)
{
  MaterialLibrary library (file_name, log_handler);

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

    load_library (SRC_LIBRARY1_FILE_NAME, materials);
    load_library (SRC_LIBRARY2_FILE_NAME, materials);
    unload_library (SRC_LIBRARY1_FILE_NAME, materials);
    load_library (SRC_LIBRARY3_FILE_NAME, materials);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  printf ("Success!!!\n");

  return 0;
}
