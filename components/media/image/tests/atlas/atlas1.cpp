#include <cstdio>

#include <media/atlas.h>

using namespace media;
using namespace math;

void print (const Tile& tile)
{
  printf ("Tile name is '%s'\n", tile.name);
  printf ("Tile origin is %ux%u\n", tile.origin.x, tile.origin.y);
  printf ("Tile size is %ux%u\n", tile.size.x, tile.size.y);
}

int main ()
{
  try
  {
    printf ("Results of atlas1 test:\n");

    Atlas atlas;

    printf ("Atlas image is '%s'\n", atlas.Image ());

    atlas.SetImage ("atlas_image.tga");
    
    printf ("Atlas image is '%s'\n", atlas.Image ());

    printf ("Tiles count is %u\n", atlas.TilesCount ());

    Tile new_tile;

    new_tile.name   = "tile1";
    new_tile.origin = vec2ui (16, 8);
    new_tile.size   = vec2ui (8, 8);

    atlas.Insert (new_tile);
    atlas.Insert ("tile2", vec2ui (8, 8), vec2ui (16, 16));

    printf ("Tiles count is %u\n", atlas.TilesCount ());

    printf ("Tile 1 is:\n");
    print (atlas.Tile (1));

    printf ("Tile 'tile1' is:\n");
    print (atlas.Tile ("tile1"));

    const Tile *founded_tile = atlas.Find ("tile2");

    if (founded_tile)
      print (*founded_tile);

    new_tile.name   = "tilex";
    new_tile.origin = vec2ui (24, 16);
    new_tile.size   = vec2ui (4, 4);

    atlas.Set (0, new_tile);
    atlas.Set (1, "tilexx", vec2ui (18, 18), vec2ui (6, 6));

    printf ("Tiles count is %u\n", atlas.TilesCount ());

    printf ("Tile 0 is:\n");
    print (atlas.Tile (0u));
    printf ("Tile 1 is:\n");
    print (atlas.Tile (1));

    atlas.Remove ("tilexx");

    printf ("Tiles count is %u\n", atlas.TilesCount ());

    atlas.Remove (0u);

    printf ("Tiles count is %u\n", atlas.TilesCount ());

    atlas.Insert (new_tile);

    printf ("Tiles count is %u\n", atlas.TilesCount ());

    atlas.RemoveAllTiles ();

    printf ("Tiles count is %u\n", atlas.TilesCount ());
  }
  catch (std::exception& exception)
  {                                               
    printf ("exception: %s\n",exception.what ()); 
  }
                                    
  return 0;
} 
