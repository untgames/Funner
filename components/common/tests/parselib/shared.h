#ifndef COMMONLIB_PARSER_TESTS_SHARED_HEADER
#define COMMONLIB_PARSER_TESTS_SHARED_HEADER

#include <cstdio>
#include <cstring>
#include <exception>

#include <stl/vector>
#include <stl/insert_iterators.h>

#include <xtl/function.h>

#include <math/matrix.h>
#include <math/quat.h>

#include <common/parser.h>
#include <common/strlib.h>

using namespace common;

void print_indent (size_t indent)
{
  while (indent--)
    printf (" ");
}

void print (const ParseNode& node, size_t indent)
{
  print_indent (indent);
  printf       ("%s (source='%s', line=%lu): {", node.Name (), node.Source (), node.LineNumber ());

  for (size_t i=0, count=node.AttributesCount (); i<count; i++)
  {
    if (i)
      printf (", ");

    printf ("%s", node.Attribute (i));
  }

  printf ("}\n");

  for (ParseIterator iter=node.First (); iter; ++iter)
    print (*iter, indent + 1);
}

void print_log (const char* message)
{
  printf ("%s\n", message);
}

#endif
