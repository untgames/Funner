#include "shared.h"

void dump (const ParseNode& node)
{
  printf ("%s {", node.Name ());
  
  for (size_t i=0, count=node.AttributesCount (); i<count; i++)
  {
    if (i)
      printf (", ");

    printf ("%s", node.Attribute (i));
  }

  printf ("}\n");  
}

int main ()
{
  printf ("Results of parse_utils_test:\n");
  
  ParseLog log;

  try
  {
    ParseTreeBuilder builder;
    
    builder.SetSource ("source1");
    
    builder.BeginNode ("A", 1);
    builder.AddAttribute ("1");
    builder.EndNode ();
    builder.BeginNode ("B", 2);
    builder.AddAttribute ("2.2");
    builder.EndNode ();
    builder.BeginNode ("C", 3);
    builder.AddAttribute ("3a");
    builder.EndNode ();
    builder.BeginNode ("D", 4);
    builder.AddAttribute ("1");
    builder.AddAttribute ("2"); 
    builder.AddAttribute ("3");
    builder.EndNode ();
    builder.BeginNode ("E");
    builder.AddAttribute ("a");
    builder.EndNode ();
    builder.BeginNode ("E");
    builder.AddAttribute ("b");
    builder.AddAttribute ("c");
    builder.EndNode ();    
    
    ParseNode root = builder.Build (log);
    
    printf ("A=%d\n", get<int> (root, "A"));
    printf ("B=%.1f\n", get<float> (root, "B"));
    printf ("C=%.1f\n", get<float> (root, "C", 3.1f));
    
    int array [3];
    
    read (root, "D", array, 3);
    
    printf ("D=");
    
    for (int i=0; i<3; i++)
      printf ("%d ", array [i]);

    printf ("\n");
    
    for_each_child (root, "E", &dump);
    
    printf ("ExceptionCheck=%.1f\n", get<float> (root, "C"));
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  log.Print (&print_log);

  return 0;
}
