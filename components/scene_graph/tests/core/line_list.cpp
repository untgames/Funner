#include "shared.h"

class MyVisitor: public visitor<void, LineList>
{
  public:
    void visit (LineList& line_list)
    {
      printf ("LineList (material='%s'):\n", line_list.Material ());

      for (size_t i=0; i<line_list.LinesCount (); i++)
      {
        const LineDesc& s = line_list.Lines ()[i];

        printf ("  #%u: position=[%.0f %.0f %.0f]-[%.0f %.0f %.0f] color=[%.2f %.2f %.2f %.2f]-[%.2f %.2f %.2f %.2f] tex_offset=[%.0f %.0f]-[%.0f %.0f]\n",
          i, s.point [0].position.x, s.point [0].position.y, s.point [0].position.z, s.point [0].color.x, s.point [0].color.y, s.point [0].color.z, s.point [0].color.w, s.point [0].tex_offset.x, s.point [0].tex_offset.y,
             s.point [1].position.x, s.point [1].position.y, s.point [1].position.z, s.point [1].color.x, s.point [1].color.y, s.point [1].color.z, s.point [1].color.w, s.point [1].tex_offset.x, s.point [1].tex_offset.y);
      }
    }
};

int main ()
{
  printf ("Results of line_list_test:\n");
  
  LineList::Pointer list = LineList::Create ();

  list->SetMaterial ("material1");

  list->Reserve (10);

  for (size_t i=0; i<10; i++)
  {
    LineDesc s;
    
    for (int j=0; j<2; j++)
    {
      s.point [j].position   = vec3f (float (i), 0, float (j));
      s.point [j].color      = vec4f (1.0f, float (j), 0.5f, 0.25f);
      s.point [j].tex_offset = vec2f (float (i), float (j));
    }

    list->Insert (s);
  }    

  MyVisitor visitor;

  list->VisitEach (visitor, NodeTraverseMode_TopToBottom);  

  return 0;
}
