#include "shared.h"

struct TestException : public std::exception
{
  const char* what () const throw ()
  {
    return "Test exception";
  }
};

void f (const ParseNode& node)
{
  printf ("node '%s': source='%s' line=%lu\n", node.Name (), node.Source (), node.LineNumber ());

  throw TestException ();
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
