#include "shared.h"

using namespace media::rfx;
using namespace common;

namespace
{

/*
    Константы
*/

const char* COMPONENT_NAME = "media.rfx.material.savers.xmtl"; //имя компонента
const char* EXTENSION      = "xmtl";                           //расширение ресурса

/*
    Сериализатор
*/

class XmtlSaver
{
  public:
///Конструктор
    XmtlSaver (const char* in_name, const MaterialLibrary& in_library)
      : library (in_library)
      , writer (in_name)
    {
      SaveLibrary ();
    }

  private:
    typedef stl::hash_map<size_t, stl::string> MaterialMap;
    
  private:
///Сохранение библиотеки
    void SaveLibrary ()
    {
      XmlWriter::Scope scope1 (writer, "MaterialLibrary"), scope2 (writer, "Materials");
      
      for (MaterialLibrary::ConstIterator iter=library.CreateIterator (); iter; ++iter)
        SaveMaterial (*iter, library.ItemId (iter));
    }
    
///Сохранение материала
    void SaveMaterial (const Material& material, const char* id)
    {
      MaterialMap::iterator iter = saved_materials.find (material.Id ());
      
      if (iter != saved_materials.end ())
      {
        const stl::string& source = iter->second;
        
        XmlWriter::Scope scope (writer, "InstanceMaterial");
        
        writer.WriteAttribute ("Name", id);
        writer.WriteAttribute ("Source", source.c_str ());
        
        return;
      }
      
      XmlWriter::Scope scope (writer, "Material");
      
      writer.WriteAttribute ("Name", id);
      
      if (strcmp (id, material.Name ()))
        writer.WriteAttribute ("SubName", material.Name ());
        
      writer.WriteAttribute ("Effect", material.Effect ());
      
      SaveProperties (material.Properties ());
      
      saved_materials.insert_pair (material.Id (), id);
    }
    
///Сохранение свойств
    void SaveProperties (const PropertyMap& properties)
    {
      for (size_t i=0, count=properties.Size (); i<count; i++)
      {
        const char* type_name = "";
        
        switch (properties.PropertyType (i))
        {
          default:
          case PropertyType_String:
            type_name = "string";
            break;
          case PropertyType_Integer:
            type_name = "int";
            break;
          case PropertyType_Float:
            type_name = "float";
            break;
          case PropertyType_Vector:
            type_name = "vector";
            break;
          case PropertyType_Matrix:
            type_name = "matrix";
            break;
        }
        
        stl::string value;
        
        properties.GetProperty (i, value);
        
        XmlWriter::Scope scope (writer, "Property");
        
        writer.WriteAttribute ("Name", properties.PropertyName (i));
        writer.WriteAttribute ("Type", type_name);
        writer.WriteAttribute ("Value", value.c_str ());
      }
    }
    
  private:
    const MaterialLibrary& library;         //сохраняемая библиотека
    XmlWriter              writer;          //XML сериализатор
    MaterialMap            saved_materials; //сохраненные материалы
};

/*
    Компонент
*/

class Component
{
  public:
    Component ()
    {
      MaterialLibraryManager::RegisterSaver (EXTENSION, &SaveLibrary);
    }
    
  private:
    static void SaveLibrary (const char* name, const MaterialLibrary& library)
    {
      XmtlSaver (name, library);
    }
};

}

extern "C"
{

ComponentRegistrator<Component> XmtlSaver (COMPONENT_NAME);

}
