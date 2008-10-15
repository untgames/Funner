#include "shared.h"

size_t get_nodes_count1 (const ParseNode& node, const char* name)
{
  size_t count = strcmp (node.Name (), name) == 0;

  for (ParseNamesakeIterator iter=node.First (name); ! ! iter; ++iter) // ! ! - для проверки работы 'operator !'
    count++;

  return count;
}

size_t get_nodes_count2 (const ParseNode& node, const char* name)
{
  size_t count = strcmp (node.Name (), name) == 0;

  for (ParseNode iter=node.First (name); !iter.IsEmpty (); iter = iter.Next (name))
    count++;

  return count;
}

void test_namesakes (const ParseNode& node, const char* name)
{
  printf ("Found %lu nodes with name '%s' (NextNamesake test)\n", get_nodes_count1 (node, name), name);
  printf ("Found %lu nodes with name '%s' (Next(const char*) test)\n", get_nodes_count2 (node, name), name);
}

int main ()
{
  printf ("Results of parse_tree_builder:\n");

  try
  {
    ParseNode root;

    printf ("empty tree test:\n");

    print (root, 0);

    ParseTreeBuilder builder;

    builder.SetSource    ("source1");
    builder.BeginNode    ("A", 12);
    builder.AddAttribute ("1");
    builder.AddAttribute ("2");
    builder.AddAttribute ("3");
    builder.BeginNode    ("B");

    builder.AddAttribute ("x");
    builder.AddAttribute ("y");

    printf ("current node: '%s' (depth=%lu)\n", builder.NodeName (), builder.NodeDepth ());

    ParseTreeBuilder builder_copy = builder;

    builder_copy.AddAttribute ("zzz");

    builder_copy.BeginNode ("C", 189);
    builder_copy.SetSource ("source2");
    builder_copy.BeginNode ("D");

    printf ("current node: '%s' (depth=%lu)\n", builder_copy.NodeName (), builder_copy.NodeDepth ());

    builder_copy.EndNode ();

    builder.ReserveBuffer (16384);
    builder.AddAttribute ("w");

    builder.EndNode ();
    builder.EndNode ();
    builder.SetSource ("source3");
    builder.BeginNode ("A", 14);

    printf ("builder-copy results (nodes=%lu, attributes=%lu, buffer_size=%lu):\n", builder_copy.TotalNodesCount (),
      builder_copy.TotalAttributesCount (), builder_copy.BufferSize ());

    root = builder_copy.Build ();

    print (root, 0);

    test_namesakes (root, "A");

    printf ("builder results (nodes=%lu, attributes=%lu, builder_size=%lu):\n", builder.TotalNodesCount (),
      builder.TotalAttributesCount (), builder.BufferSize ());

    ParseNode tmp = builder.Build ();

    swap (tmp, root);

    print (root, 0);

    test_namesakes (root, "A");
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
