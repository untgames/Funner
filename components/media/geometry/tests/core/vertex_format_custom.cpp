#include "shared.h"

#if defined (_MSC_VER) || defined (__GNUC__)
#pragma pack(1)
#endif

int main ()
{
  printf ("Results of vertex_format_custom_test:\n");

  try
  {
    typedef Vertex<Position3f, Normalf, TexChannel<0>::Coord2f, Tangents> CustomVertex;

    VertexFormat vf1;

    get_vertex_format ((CustomVertex*)0, vf1);

    printf ("hash1 correct: %d\n", vf1.Hash () == (sizeof (void*) == 4 ? 0x6816d837 : 0x57abaa38));

    VertexFormat vf2 = vf1.Clone ();

    printf ("hash2 correct: %d\n", vf2.Hash () == (sizeof (void*) == 4 ? 0x6816d837 : 0x57abaa38));

    const VertexAttribute* attr = vf1.FindAttribute ("tangent");

    if (!attr)
    {
      printf ("Attribute 'tangent' not found\n");        
      return 0;
    }

    const char* name = attr->name;

    for (size_t i=0; i<25; i++)
    {
      vf1.AddAttribute (common::format ("binormal%02u", i).c_str (), VertexAttributeSemantic_Position, VertexAttributeType_Float3, 34); 

      const VertexAttribute* attr = vf1.FindAttribute ("tangent");

      if (!attr)
      {
        printf ("Attribute 'tangent' not found\n");        
        return 0;
      }

      if (attr->name == name) printf ("%02u: Name pointer is same\n", i);
      else                    printf ("%02u: Name pointer is not same\n", i);
    }

    printf ("before remove\n");

    dump (vf1);

    printf ("after remove\n");

    vf1.RemoveAttributes (VertexAttributeSemantic_Position);

    dump (vf1);

    printf ("hash1 correct: %d\n", vf1.Hash () == (sizeof (void*) == 4 ? 0x62f6f6f1 : 0xa3373580));
    printf ("hash2 correct: %d\n", vf2.Hash () == (sizeof (void*) == 4 ? 0x6816d837 : 0x57abaa38));
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n", exception.what ());
  }

  return 0;
}
