#include "shared.h"

const char* get_string (LineUsage usage)
{
  switch (usage)
  {
    case LineUsage_Static:   return "static";
    case LineUsage_Dynamic:  return "dynamic";
    case LineUsage_Stream:   return "stream";
    case LineUsage_Batching: return "batching";
    default:                 return "unknown";
  }
}

class MyVisitor: public visitor<void, Line>
{
  public:
    void visit (Line& line)
    {
      printf ("Line '%s': \n", line.Name ());
      printf ("  material:        '%s'\n", line.Material ());
      printf ("  usage:           %s\n", get_string (line.Usage ()));
      printf ("  descs (count=%u):\n", line.LineDescsCount ());
      
      for (size_t i=0; i<line.LineDescsCount (); i++)
      {
        const LineDesc& desc = line.LineDescs () [i];
        
        printf ("    position: [%.2f %.2f %.2f]      - [%.2f %.2f %.2f]\n", desc.point [0].position.x, desc.point [0].position.y, desc.point [0].position.z,
          desc.point [1].position.x, desc.point [1].position.y, desc.point [1].position.z);
        printf ("    color:    [%.2f %.2f %.2f %.2f] - [%.2f %.2f %.2f %.2f]\n", desc.point [0].color.x, desc.point [0].color.y, desc.point [0].color.z, desc.point [0].color.w,
          desc.point [1].color.x, desc.point [1].color.y, desc.point [1].color.z, desc.point [1].color.w);
        printf ("    tex_offs: [%.2f %.2f]           - [%.2f %.2f]\n", desc.point [0].tex_offset.x, desc.point [0].tex_offset.y, desc.point [1].tex_offset.x, desc.point [1].tex_offset.y);
      }
    }
};

int main ()
{
  printf ("Results of line1_test:\n");
  
  Line::Pointer line (Line::Create ());

  line->SetMaterial ("material1");
  line->SetAlpha (0, 0.5f);
  line->SetAlpha (1, 0.5f);
  line->SetTexOffset (0, math::vec2f (0.5f, 0.5f));
  line->SetTexOffset (1, math::vec2f (0, 0.5f));

  MyVisitor visitor;

  line->VisitEach (visitor, NodeTraverseMode_TopToBottom);  

  return 0;
}
