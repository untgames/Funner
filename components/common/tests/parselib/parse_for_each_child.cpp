#include "shared.h"

void f (const ParseNode& node)
{
  printf ("node '%s': source='%s' line=%u\n", node.Name (), node.Source (), node.LineNumber ());
  
  throw std::bad_alloc ();
}

int main ()
{
  printf ("Results of parse_for_each_child_test:\n");
  
  ParseLog         log;
  ParseTreeBuilder builder;

  builder.SetSource ("source1");
  
  builder.BeginNode ("A", 1);
  builder.EndNode ();
  builder.BeginNode ("A", 2);
  builder.EndNode ();
  builder.BeginNode ("A", 3);
  builder.EndNode ();

  ParseNode root = builder.Build (log);
  
  for_each_child (root, "A", &f);
  
  log.Print (&print_log);

  return 0;
}
