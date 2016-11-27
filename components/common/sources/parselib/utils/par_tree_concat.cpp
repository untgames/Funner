#include <xtl/common_exceptions.h>

#include <common/parse_tree.h>
#include <common/parser.h>
#include <common/strlib.h>

using namespace common;

/*
    Объединение деревьев
*/

namespace
{

//Обход дерева и разрешение ссылок в атрибутах
struct ParseConcatenator
{  
  void Concat (const ParseNode& source_node)
  {
    for (ParseNode child=source_node.First (); child; child=child.Next ())
    {            
      if (strcmp (child.Source (), builder.Source ()))
        builder.SetSource (child.Source ());        
      
      builder.BeginNode (child.Name (), child.LineNumber ());
      builder.AddAttributes (child.AttributesCount (), child.Attributes ());

      Concat (child);
      
      builder.EndNode ();
    }
  }

  ParseTreeBuilder builder;
};

}

namespace common
{

ParseNode concat (const ParseNode& node1, const ParseNode& node2)
{
  ParseNode nodes [] = {node1, node2};

  return concat (2, nodes);
}

ParseNode concat (size_t nodes_count, const ParseNode* nodes)
{
  try
  {
    if (!nodes_count)
      return ParseNode ();

    if (!nodes)
      throw xtl::make_null_argument_exception ("", "nodes");

    ParseConcatenator concatenator;

    for (size_t i=0; i<nodes_count; i++)
      concatenator.Concat (nodes [i]);

    return concatenator.builder.Build ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::concat(size_t,const ParseNode*)");
    throw;
  }
}

}
