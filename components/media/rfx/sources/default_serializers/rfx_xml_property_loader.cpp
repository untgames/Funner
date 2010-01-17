#include "shared.h"

using namespace media::rfx;
using namespace common;

/*
    Конструктор
*/

PropertiesLoader::PropertiesLoader (ParseLog& in_log)
  : log (in_log)
{
}

/*
    Загрузка шаблонов
*/

void PropertiesLoader::LoadTemplate (Parser::Iterator parse_iter)
{
  const char* name = get<const char*> (*parse_iter, "Name");
  
  PropertyTemplateMapPtr tmpl (new PropertyTemplateMap);
  
  for_each_child (*parse_iter, "Property", xtl::bind (&PropertiesLoader::LoadTemplateProperty, this, _1, xtl::ref (*tmpl)));
  
  properties_templates.insert_pair (name, tmpl);
}

///Загрузка шаблона свойства
void PropertiesLoader::LoadTemplateProperty (Parser::Iterator parse_iter, PropertyTemplateMap& tmpl)
{
  const char* name = get<const char*> (*parse_iter, "Name");
  const char* type = get<const char*> (*parse_iter, "Type");
  
  PropertyType result_type = PropertyType_String;
  
  if (!strcmp (type, "string"))
  {
    result_type = PropertyType_String;
  }
  else if (!strcmp (type, "int"))
  {
    result_type = PropertyType_Integer;
  }
  else if (!strcmp (type, "float"))
  {
    result_type = PropertyType_Float;
  }
  else if (!strcmp (type, "vector"))
  {
    result_type = PropertyType_Vector;
  }
  else if (!strcmp (type, "matrix"))
  {
    result_type = PropertyType_Matrix;
  }
  else
  {
    log.Warning (*parse_iter, "Unknown property type '%s'", type);
  }
  
  tmpl.insert_pair (name, result_type);
}

/*
    Загрузка свойств
*/

void PropertiesLoader::LoadProperties (Parser::Iterator parse_iter, PropertyMap& properties)
{
  for_each_child (*parse_iter, xtl::bind (&PropertiesLoader::LoadProperty, this, _1, xtl::ref (properties)));
}

void PropertiesLoader::LoadProperty (Parser::Iterator parse_iter, PropertyMap& properties)
{
  if (!strcmp (parse_iter->Name (), "Property"))
  {
    const char* name = get<const char*> (*parse_iter, "Name");
    const char* type = get<const char*> (*parse_iter, "Type", "string");
    
    if (!strcmp (type, "string"))
    {
      properties.SetProperty (name, get<const char*> (*parse_iter, "Value"));
    }
    else if (!strcmp (type, "int"))
    {
      properties.SetProperty (name, get<int> (*parse_iter, "Value"));
    }
    else if (!strcmp (type, "float"))
    {
      properties.SetProperty (name, get<float> (*parse_iter, "Value"));
    }
    else if (!strcmp (type, "vector"))
    {
      properties.SetProperty (name, math::vec4f ());
      properties.SetProperty (name, get<const char*> (*parse_iter, "Value"));
    }
    else if (!strcmp (type, "matrix"))
    {
      properties.SetProperty (name, math::mat4f ());
      properties.SetProperty (name, get<const char*> (*parse_iter, "Value"));
    }
    else
    {
      log.Warning (*parse_iter, "Unknown property type '%s'", type);
    }
  }
  else if (!parse_iter->First ())
  {
    //игнорирование чтения атрибутов
  }
  else
  {
    TemplateMap::iterator iter = properties_templates.find (parse_iter->Name ());
    
    if (iter == properties_templates.end ())
    {
      log.Error (*parse_iter, "Undefined template '%s' used", parse_iter->Name ());
      return;
    }
    
    for_each_child (*parse_iter, xtl::bind (&PropertiesLoader::LoadPropertyWithTemplate, this, _1, xtl::ref (properties), xtl::ref (*iter->second)));
  }
}

void PropertiesLoader::LoadPropertyWithTemplate (Parser::Iterator parse_iter, PropertyMap& properties, PropertyTemplateMap& tmpl)
{
  const char* name = parse_iter->Name ();
  
  PropertyTemplateMap::iterator iter = tmpl.find (name);
  
  if (iter == tmpl.end ())
  {
    log.Error (*parse_iter, "Undefined property '%s' used", name);
    return;
  }
  
  PropertyType type = iter->second.type;
  
  switch (type)
  {
    case PropertyType_String:
      properties.SetProperty (name, get<const char*> (*parse_iter, ""));
      break;
    case PropertyType_Integer:
      properties.SetProperty (name, get<int> (*parse_iter, ""));
      break;
    case PropertyType_Float:
      properties.SetProperty (name, get<float> (*parse_iter, ""));
      break;
    case PropertyType_Vector:
    {
      properties.SetProperty (name, math::vec4f ());
      properties.SetProperty (name, get<const char*> (*parse_iter, ""));

      break;
    }
    case PropertyType_Matrix:
    {
      properties.SetProperty (name, math::mat4f ());
      properties.SetProperty (name, get<const char*> (*parse_iter, ""));

      break;
    }
    default:
      log.Error (*parse_iter, "Bad property template '%s' type %d. Internal error", name, type);
      break;
  }
}
