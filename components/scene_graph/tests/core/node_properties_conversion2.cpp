#include "shared.h"

#define TEST(X) try { printf ("SetProperty ('%s', %s): ", name, #X); properties.SetProperty (name, X); stl::string result; properties.GetProperty (name, result), printf ("%s\n", result.c_str ()); }catch (std::exception& e) { printf ("exception: %s\n", e.what ()); }

void test (NodeProperties& properties, const char* name)
{
  TEST ("");
  TEST ("hello world");
  TEST ("3.14");
  TEST ("1 2.5 3 4");
  TEST ("1 2.5 4");
  TEST ("1 2 3 4 5");
  TEST ("1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16");
  TEST (13);
  TEST (1.5f);
  TEST (math::vec4f (1.0f, 2.0f, 3.0f, 4.0f));
  TEST (math::mat4f (3.0f));
}

int main ()
{
  printf ("Results of node_properties_conversion1_test:\n");
  
  try
  {
    Node::Pointer node (Node::Create ());          
    
    node->SetProperties (NodeProperties::Create ());       
    
    NodeProperties& properties = *node->Properties ();
    
    properties.SetProperty ("String",  "");
    properties.SetProperty ("Integer", int (0));
    properties.SetProperty ("Float",   0.0f);
    properties.SetProperty ("Vector",  math::vec4f (0.0f));
    properties.SetProperty ("Matrix",  math::mat4f (1.0f));   

    for (size_t i=0; i<properties.Size (); i++)
      test (properties, properties.PropertyName (i));    
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
    throw;
  }
}
