#include "shared.h"

using namespace math;

typedef stl::set<stl::string> MySet;

int main ()
{
  printf ("Results of xml_writer_test:\n");

  try
  {
    int   array [] = {1,2,3,4,5,6,7,8,9,10};
    MySet set;
    
    set.insert ("world");    
    set.insert ("sunny");
    set.insert ("hello");
    
    XmlWriter writer (&dump, 2);
        
    writer.WriteComment    ("This file is auto generated\nDo not modify it by hand");
    writer.BeginNode       ("root");
    writer.WriteComment    ("Yet one\nblock comment");
    writer.BeginNode       ("test1");
    writer.WriteComment    ("Scalar types serialization");
    writer.BeginNode       ("scalar");
    writer.WriteAttribute  ("int", -1);
    writer.WriteAttribute  ("size_t", (size_t)-1, "hex:");
    writer.WriteAttribute  ("float", math::constf::pi);
    writer.WriteAttribute  ("double", math::constd::pi);
    writer.WriteAttribute  ("long_double", math::constants<long double>::pi);
    writer.WriteNode       ("string", "Hello world");
    writer.WriteNode       ("char", 'x');
    writer.BeginNode       ("text");
    writer.WriteData       ("Hello world!");
    writer.WriteCData      ("Hello world!");
    writer.WriteData       ("Hello world!");
    writer.EndNode         ();
    writer.EndNode         ();
    writer.WriteComment    ("Mathlib types serialization");
    writer.BeginNode       ("mathlib");
    writer.WriteAttribute  ("vec3f", vec3f (1.1f, 2.2f, 3.3f));
    writer.WriteAttribute  ("mat4f", mat4f (1.0f));
    writer.WriteNodeCData  ("quatd", quatd (math::constf::pi));
    writer.EndNode         ();
    writer.WriteComment    ("Intervals serialization");
    writer.BeginNode       ("intervals");
    writer.WriteNode       ("int_array", xtl::make_iterator_range (sizeof (array)/sizeof (*array), array));
    writer.WriteNode       ("stl_set", xtl::make_iterator_range (set));
    writer.EndNode         ();
    writer.EndNode         ();
    writer.WriteNode       ("bool_flag");

    writer.Flush ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
