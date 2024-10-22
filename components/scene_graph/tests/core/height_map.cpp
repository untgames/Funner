#include "shared.h"

void dump (const HeightMap& map)
{
  printf ("height_map %ux%u (material='%s'):\n", map.RowsCount (), map.ColumnsCount (), map.Material ());
  
  for (size_t i=0; i<map.RowsCount (); i++)
  {
    printf ("  row[%u]:\n", i);
    
    for (size_t j=0; j<map.ColumnsCount (); j++)
    {
      printf ("    cell[%u,%u]: [", i, j);
      
      const HeightMap::VertexDesc& v = map.Vertex (i, j);
      
      printf ("h=%.2f n=", v.height);
      dump (v.normal);
      printf (" color=");
      dump (v.color);

      printf ("]\n");
    }
  }
}

void height_map_event_handler (const HeightMap& map, HeightMapEvent event)
{
  switch (event)
  {
    case HeightMapEvent_OnSizesUpdate:
      printf ("height map sizes updated: %ux%u\n", map.RowsCount (), map.ColumnsCount ());
      break;
    case HeightMapEvent_OnVerticesUpdate:
      printf ("height map vertices updated\n");
      break;
    default:
      printf ("unknown height map event occured\n");
  }
}

int main ()
{
  printf ("Results of height_map_test:\n");
  
  HeightMap::Pointer map = HeightMap::Create ();
  
  map->RegisterEventHandler (HeightMapEvent_OnVerticesUpdate, &height_map_event_handler);
  map->RegisterEventHandler (HeightMapEvent_OnSizesUpdate, &height_map_event_handler);
  
  map->SetCellsCount (2, 2);
  
  map->SetMaterial ("material1");
  
  dump (*map);
  
  map->Vertex (1, 1).normal = math::vec3f (1.0f);
  
  map->UpdateVerticesNotify ();

  dump (*map);  

  map->SetCellsCount (3, 3);
  
  dump (*map);

  map->SetVerticesHeight (0.4f);
  map->SetVerticesNormal (math::vec3f (0.1f, 0.2f, 0.3f));
  map->SetVerticesColor  (math::vec4f (0.4f, 0.3f, 0.2f, 0.1f));

  dump (*map);

  map->SetCellsCount (4, 2);

  dump (*map);

  return 0;
}
