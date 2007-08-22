#include "shared.h"

template <class Vertex> void dump_format ()
{
  VertexFormat format;

  get_vertex_format ((Vertex*)0, format);

  dump (format, sizeof (Vertex));
}

int main ()
{
  printf ("Results of vertex_test:\n");
  
  dump_format<Vertex<Position3f, Normalf, Tangents, Binormalf> > ();
  dump_format<Vertex<Position3f, TexChannel<3>::Coord2f, Color4ub> > ();
  dump_format<Vertex<Position3f, Color3f, VertexInfluence> > ();

  return 0;
}
