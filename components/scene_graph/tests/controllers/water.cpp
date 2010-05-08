#include "shared.h"

int main ()
{
  printf ("Results of water test:\n");
  
  HeightMap::Pointer height_map = HeightMap::Create ();
  Water::Pointer     water      = Water::Create (*height_map);
  
  height_map->SetCellsCount (128, 128);
  
  height_map->Update (1.0f);

  return 0;
}
