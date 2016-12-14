#include "shared.h"

using namespace common;
using namespace media::particles;
using namespace media::particles::particle_designer_serialzer;

namespace
{

/*
   Constants
*/

const char* COMPONENT_NAME = "media.particles.loaders.ParticleDesignerPlistLoader"; //component name
const char* LOG_NAME       = "media.particles.loaders.ParticleDesignerPlistLoader"; //log stream name

typedef xtl::com_ptr<ParticleSystemPrototype> ParticleSystemPrototypePtr;

///gets value node for given key node if type matches or throws parser exception
ParseNode get_plist_dict_value(ParseNode key_node, const char* value_type)
{
  ParseNode value_node = key_node.Next ();

  if (!value_node || xtl::xstrcmp (value_node.Name (), value_type))
    raise_parser_exception (key_node, "'%s' value type expected for this key", value_type);

  return value_node;
}

//read plist dict string value for given key node
template<class T>
T read_plist_dict_value (ParseNode key_node, const char* type_name)
{
  return get<T> (get_plist_dict_value (key_node, type_name), "#text");
}

template<class T>
T read_plist_dict_value (ParseNode key_node, const char* type_name, const T& default_value)
{
  return get<T> (get_plist_dict_value (key_node, type_name), "#text", default_value);
}

const char* read_plist_dict_string (ParseNode key_node)
{
  return read_plist_dict_value<const char*> (key_node, "string");
}

//read plist dict to PropertyMap
void read_plist_dictionary (Parser::Iterator dict_iter, PropertyMap& properties)
{
  for (ParseNode key_node = dict_iter->First ("key"); key_node; key_node = key_node.NextNamesake ())
  {
    const char* key = get<const char*> (key_node, "#text");

    ParseNode value_node = key_node.Next ();

    if (!value_node)
      raise_parser_exception (key_node, "No value node for this key");

    const char* value_type = value_node.Name ();

    if (!xtl::xstrcmp (value_type, "string"))
      properties.SetProperty (key, read_plist_dict_value<const char*> (key_node, value_type, ""));  //dict may have strings without #text value (<string></string>), use empty string for such situations
    else if (!xtl::xstrcmp (value_type, "real"))
      properties.SetProperty (key, read_plist_dict_value<float> (key_node, value_type));
    else if (!xtl::xstrcmp (value_type, "integer"))
      properties.SetProperty (key, read_plist_dict_value<int> (key_node, value_type));
    else if (!xtl::xstrcmp (value_type, "false"))
      properties.SetProperty (key, 0);
    else if (!xtl::xstrcmp (value_type, "true"))
      properties.SetProperty (key, 1);
    else
      key_node.Log ().Warning (key_node, "Unsupported key value type '%s', ignored", value_type);
  }
}

}

namespace components
{

namespace particle_designer_plist_loader
{

/*
   Particles library loader for particle designer plist format
*/

class PlistLibraryLoader
{
  public:
    ///Constructor
    PlistLibraryLoader (const char* file_name, ParticleSystemLibrary& out_library)
      : document_folder_path (dir (file_name))
      , parser (file_name, "xml")
    {
      ParseNode plist_root = parser.Root ().First ("plist");

      if (!plist_root)
        raise_parser_exception (parser.Root (), "Can't find 'plist' node at document root");

      if (plist_root.First ("array"))  //scene file has "array" element in root, emitter file has "dictionary" element in root
        ParseSceneArray (plist_root.First ("array"));
      else if (plist_root.First ("dict"))
      {
        ParticleSystemPrototypePtr prototype (new ParticleSystemPrototype (), false);

        prototype->AddEmitter ("", math::vec2f ());

        ParseEmitter (plist_root.First ("dict"), prototype.get ());

        library.Attach (file_name, *prototype);
      }
      else
        raise_parser_exception (plist_root, "Can't find 'array' or 'dict' node at document root");

      //logging
      Log log (LOG_NAME);

      parser.Log ().Print (xtl::bind (&Log::Print, xtl::ref (log), _1));

      MergeLibraries (library, out_library);
    }

  private:
    ///Merge readed library to out library
    void MergeLibraries (const ParticleSystemLibrary& source_library, ParticleSystemLibrary& target_library)
    {
      for (ParticleSystemLibrary::ConstIterator iter = source_library.CreateIterator (); iter; ++iter)
        target_library.Attach (source_library.ItemId (iter), **iter);
    }

    ///Parse scene array file
    void ParseSceneArray (Parser::Iterator scene_array_iter)
    {
      for_each_child (*scene_array_iter, "dict", xtl::bind (&PlistLibraryLoader::ParseSceneDict, this, _1));
    }

    ///Parse scene entry of scene array
    void ParseSceneDict (Parser::Iterator scene_iter)
    {
      ParticleSystemPrototypePtr prototype (new ParticleSystemPrototype (), false);
      const char*                prototype_name = 0;

      for (ParseNode key_node = scene_iter->First ("key"); key_node; key_node = key_node.NextNamesake ())
      {
        const char* key = get<const char*> (key_node, "#text");

        if (!xtl::xstrcmp (key, "name"))
        {
          prototype_name = read_plist_dict_string (key_node);
        }
        else if (!xtl::xstrcmp (key, "emitters"))
        {
          ParseNode emitters_array = get_plist_dict_value (key_node, "array");

          for_each_child (emitters_array, "dict", xtl::bind (&PlistLibraryLoader::ParseSceneEmitter, this, _1, prototype.get ()));
        }
        else
          key_node.Log ().Warning (key_node, "Unsupported scene dictionary key '%s'", key);
      }

      if (!prototype_name)
        raise_parser_exception (*scene_iter, "No 'name' key for this scene");

      //read emitter file
      stl::string emitter_file_name = common::format ("%s%s.plist", document_folder_path.c_str (), prototype_name);

      Parser emitter_parser (emitter_file_name.c_str (), "xml");

      ParseNode plist_root = emitter_parser.Root ().First ("plist");

      if (!plist_root)
        raise_parser_exception (*scene_iter, "Can't find 'plist' node at document '%s' root", emitter_file_name.c_str ());

      if (!plist_root.First ("dict"))
        raise_parser_exception (plist_root, "No 'dict' node at document root");

      ParseEmitter (plist_root.First ("dict"), prototype.get ());

      //logging
      Log log (LOG_NAME);

      emitter_parser.Log ().Print (xtl::bind (&Log::Print, xtl::ref (log), _1));

      library.Attach (prototype_name, *prototype);
    }

    ///Parse scene emitter entry of scene entry
    void ParseSceneEmitter (Parser::Iterator emitter_iter, ParticleSystemPrototype* prototype)
    {
      PropertyMap properties;

      read_plist_dictionary (emitter_iter, properties);

      if (!properties.IsPresent ("name"))
        raise_parser_exception (*emitter_iter, "No 'name' key for this emitter");

      math::vec2f offset;

      //let's ignore if x or y key not found, assume zero offset in this case

      if (properties.IsPresent ("x"))
        offset.x = properties.GetFloat ("x");
      if (properties.IsPresent ("y"))
        offset.y = properties.GetFloat ("y");

      prototype->AddEmitter (properties.GetString ("name"), offset);
    }

    ///Parse emitter data
    void ParseEmitter (Parser::Iterator emitter_iter, ParticleSystemPrototype* prototype)
    {
      PropertyMap parameters;

      read_plist_dictionary (emitter_iter, parameters);

      prototype->SetParameters (parameters);
    }
    
  private:
    stl::string           document_folder_path; //path to folder containing plist file
    Parser                parser;               //parser
    ParticleSystemLibrary library;              //library to read data to
};

/*
   Component
*/

class Component
{
  public:
    Component ()
    {
      ParticleSystemLibraryManager::RegisterLoader ("plist", &LoadLibrary);
    }

  private:
    static void LoadLibrary (const char* file_name, ParticleSystemLibrary& library)
    {
      PlistLibraryLoader (file_name, library);
    }
};

extern "C"
{
  ComponentRegistrator<Component> ParticleDesignerPlistLoader (COMPONENT_NAME);
}

}

}
