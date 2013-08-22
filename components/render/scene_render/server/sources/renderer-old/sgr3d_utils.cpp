#include "shared.h"

namespace
{

///Преобразователь
struct ParseNodeConverter
{
  common::PropertyMap result; //результирующая карта свойств
  
///Чтение свойства
  void ReadValue (common::Parser::AttributeIterator iter, const char* name)
  {
    switch (iter.available ())
    {
      case 1:
      {
        const char* value = *iter;

        if      (isdigit (*value))         result.SetProperty (name, (float)atof (value));
        else if (!strcmp (value, "false")) result.SetProperty (name, 0);        
        else if (!strcmp (value, "true"))  result.SetProperty (name, 1);        
        else                               result.SetProperty (name, value);

        break;
      }
      case 2:
      case 3:
      case 4:
      {
        size_t tokens = iter.available ();
        
        math::vec4f value;
        
        for (size_t i=0; i<tokens; i++, ++iter)
        {
          const char* token = *iter;

          value [i] = (float)atof (token);
        }
        
        result.SetProperty (name, value);
        
        break;
      }
      case 16:
      {
        math::mat4f value;
        
        for (size_t i=0; i<4; i++)
          for (size_t j=0; j<4; j++, ++iter)
          {
            const char* token = *iter;

            value [i][j] = (float)atof (token);
          }

        result.SetProperty (name, value);

        break;
      }
      default:
        throw xtl::format_operation_exception ("render::scene_render3d::ParseNodeConverter::ReadValue", "Can't parse node '%s' with %u tokens", name, iter.available ());
    }
  }
  
///Преобразование
  void Convert (const common::ParseNode& node, const char* prefix = "")
  {
    stl::string name;  
    
    if (*prefix) name = common::format ("%s.%s", prefix, node.Name ());
    else         name = node.Name (); 
    
    if (node.First ())
    {
      for (common::Parser::Iterator iter=node.First (); iter; ++iter)
        Convert (*iter, name.c_str ());
    }
    else
    {
      ReadValue (make_attribute_iterator (node), name.c_str ());
    }
  }
};

}

namespace render
{

namespace scene_render3d
{

///Преобразование узла конфигурации в карту свойств
common::PropertyMap to_properties (const common::ParseNode& node)
{
  ParseNodeConverter converter;
  
  for (common::Parser::Iterator iter=node.First (); iter; ++iter)
    converter.Convert (*iter);

  return converter.result;  
}

}

}
