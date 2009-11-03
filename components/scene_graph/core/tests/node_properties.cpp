#include "shared.h"

void dump_properties (const Node& node)
{
  const NodeProperties& properties = *node.Properties ();

  printf ("node has %u properties (hash=%08x, structure_hash=%08x, data_hash=%08x):",
    properties.Size (), properties.Hash (), properties.StructureHash (), properties.DataHash ());
    
  for (size_t i=0, count=properties.Size (); i<count; i++)
  {
    printf (" #%u: name='%s', type=%s, value=", i, properties.PropertyName (i), get_name (properties.PropertyType (i)));
  }
}

int main ()
{
  printf ("Results of node_properties_test:\n");
  
  try
  {
    Node::Pointer node (Node::Create ());
    
    dump_properties (*node);
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
    throw;
  }
}
