#include "shared.h"

void dump (const aaboxf& box)
{
  printf ("[");
  dump (box.minimum ());
  printf (" - ");
  dump (box.maximum ());
  printf ("]");
}

void dump_bounds (Sprite& light)
{
  printf ("  local=");

  dump (light.BoundBox ());
  printf ("\n  world=");
  dump (light.WorldBoundBox ());
  printf ("\n");
}

int main ()
{
  printf ("Results of sprite3_test:\n");
  
  Sprite::Pointer sprite (Sprite::Create ());

  printf ("Default sprite bounds:\n");

  dump_bounds (*sprite);

  sprite->Translate (1, 1, 1);
  sprite->Scale (2, 0.5, 4);

  printf ("Changed sprite bounds:\n");

  dump_bounds (*sprite);

  return 0;
}
