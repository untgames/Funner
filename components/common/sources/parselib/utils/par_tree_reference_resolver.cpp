#include <xtl/common_exceptions.h>
#include <xtl/function.h>

#include <common/parse_tree.h>
#include <common/parser.h>
#include <common/property_map.h>
#include <common/strlib.h>

using namespace common;

namespace
{

typedef xtl::function<void (const char* reference_name, stl::string& reference_value)> ResolveHandler;

//Обход дерева и разрешение ссылок в атрибутах
struct ParseResolver
{
  ParseResolver (const ResolveHandler& in_resolve_handler) : resolve_handler (in_resolve_handler) {}
  
  void Resolve (const ParseNode& source_node)
  {
    for (size_t i=0, count=source_node.AttributesCount (); i<count; i++)
    {
      const char *source_attribute = source_node.Attribute (i),
                 *source_iter      = strchr (source_attribute, '{');
      
      if (!source_iter)
      {
        builder.AddAttribute (source_attribute);
        continue;
      }
      
      stl::string destination_attribute;
      
      while (source_iter)
      {      
        if (source_iter [1] == '{')
        {
          const char* end = strchr (source_iter, '}');
          
          if (!end)
            end = source_iter + strlen (source_iter) - 1;
          
          destination_attribute.append (source_iter, end + 1);

          source_iter = strchr (end + 1, '{');
          
          destination_attribute.append (end + 1, source_iter ? source_iter : end + strlen (end));

          continue;
        }
        
        static const char* RESOLVE_PATTERN     = "{ *([^ ]*) *\\}";
        static const char* RESOLVE_REPLACEMENT = "\\1";        

        stl::string destination_attribute_name = replace (source_iter, RESOLVE_PATTERN, RESOLVE_REPLACEMENT), replacement;

        try
        {
          resolve_handler (destination_attribute_name.c_str (), replacement);
        }
        catch (std::exception& e)
        {
          source_node.Log ().Error (source_node, "%s\n    at common::ParseResolver::Resolve", e.what ());

          replacement.clear ();
        }
        catch (...)
        {
          replacement.clear ();
        }
        
        destination_attribute += replacement;
        
        const char* end = strchr (source_iter, '}');
          
        if (!end) end = source_iter + strlen (source_iter);
        else      ++end;

        source_iter = strchr (end, '{');
        
        destination_attribute.append (end, source_iter ? source_iter : end + strlen (end));
      }
      
      builder.AddAttribute (destination_attribute.c_str ());
    }
    
    for (ParseNode child=source_node.First (); child; child=child.Next ())
    {
      if (strcmp (source_node.Source (), builder.Source ()))
      {
        builder.SetSource (child.Source ());
      }
      
      builder.BeginNode (child.Name (), child.LineNumber ());

      Resolve (child);
      
      builder.EndNode ();
    }
  }

  ParseTreeBuilder builder;
  ResolveHandler   resolve_handler;
};

//Разрешение ссылок исходя из узла дерева
struct ParseNodeResolver
{
  ParseNodeResolver (const ParseNode& in_node)
    : node (in_node)
  {
  }
  
  void operator () (const char* name, stl::string& value)
  {
    ParseNode child = node.First (name);

    if (!child)
      throw xtl::format_exception<ParserException> ("common::ParseNodeResolver::operator ()", "No child with name '%s' for node '%s' ('%s'(%u))", name, node.Name (), node.Source (), node.LineNumber ());

    value = get<const char*> (child, "");
  }
  
  ParseNode node;
};

//Разрешение ссылок исходя из карты свойств
struct PropertyMapResolver
{
  PropertyMapResolver (const PropertyMap& in_properties) : properties (in_properties) {}
  
  void operator () (const char* name, stl::string& value)
  {
    properties.GetProperty (name, value);
  }

  PropertyMap properties;
};

}

namespace common
{

/*
    Разрешение ссылок в дереве разбора
*/

ParseNode resolve_references (const ParseNode& source_tree, const ResolveHandler& resolve_handler)
{
  try
  {
    ParseResolver resolver (resolve_handler);

    resolver.Resolve (source_tree);
    
    return resolver.builder.Build (source_tree.Log ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::resolve_references");
    throw;
  }
}

ParseNode resolve_references (const ParseNode& source_tree, const ParseNode& reference_values)
{
  return resolve_references (source_tree, ParseNodeResolver (reference_values));
}

ParseNode resolve_references (const ParseNode& source_tree, const PropertyMap& reference_values)
{
  return resolve_references (source_tree, PropertyMapResolver (reference_values));
}

}
