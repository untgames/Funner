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
///Сохранение библиотеки
    void SaveLibrary ()
    {
      XmlWriter::Scope scope1 (writer, "material_library");
      
      SearchSharedEntries ();
      SaveLayouts ();
      SaveTexmaps ();
      SaveMaterials ();
    }
    
///Поиск объектов совместного использования
    void SearchSharedEntries ()
    {
      for (MaterialLibrary::ConstIterator iter=library.CreateIterator (); iter; ++iter)
      {
        const Material& material = *iter;
        
        MaterialMap::iterator mtl_iter = materials.find (material.Id ());
        
        if (mtl_iter != materials.end ())
        {
          mtl_iter->second.shared_count++;
          
          continue;
        }
        
        const PropertyLayout& layout = material.Properties ().Layout ();
        
        LayoutMap::iterator layout_iter = layouts.find (layout.Id ());
        
        if (layout_iter == layouts.end ())
        {
          layouts.insert_pair (layout.Id (), SharedEntry<PropertyLayout> (layout));
        }
        else
        {
          layout_iter->second.shared_count++;
        }
        
        for (size_t i=0, count=material.TexmapCount (); i<count; i++)
        {
          const Texmap& texmap = material.Texmap (i);
          
          TexmapMap::iterator texmap_iter = texmaps.find (texmap.Id ());
          
          if (texmap_iter == texmaps.end ())
          {
            texmaps.insert_pair (texmap.Id (), SharedEntry<Texmap> (texmap));
          }
          else
          {
            texmap_iter->second.shared_count++;
          }
        }
        
        materials.insert_pair (material.Id (), SharedEntry<Material> (material));
        
        SharedEntry<Material>& mtl_entry = materials [material.Id ()];
        
        mtl_entry.id = library.ItemId (iter);
      }
    }
    
///Сохранение шаблонов
    void SaveLayouts ()
    {
      XmlWriter::Scope layouts_scope (writer, "layouts");
      
      size_t name_index = 1;
      
      for (LayoutMap::iterator iter=layouts.begin (), end=layouts.end (); iter!=end; ++iter)
        if (iter->second.shared_count > 1)
        {
          const PropertyLayout& layout = iter->second.object;
          
          XmlWriter::Scope layout_scope (writer, "layout");
          
          iter->second.id = format ("layout#%u", name_index++);
          
          writer.WriteAttribute ("id", iter->second.id.c_str ());
          
          SaveLayout (layout);
        }
    }
    
///Сохранение шаблона
    void SaveLayout (const PropertyLayout& layout)
    {
      for (size_t i=0, count=layout.Size (); i<count; i++)
      {
        const PropertyDesc& desc = layout.Properties ()[i];
        
        XmlWriter::Scope scope (writer, "property");
        
        SaveProperty (desc);
      }
    }
    
///Сохранение описания свойства
    void SaveProperty (const PropertyDesc& desc)
    {
      writer.WriteAttribute ("name",  desc.name);

      switch (desc.type)
      {
        case PropertyType_String:
          writer.WriteAttribute ("type", "string");
          break;
        case PropertyType_Integer:
          writer.WriteAttribute ("type", "int");
          break;
        case PropertyType_Float:
          writer.WriteAttribute ("type", "float");
          break;
        case PropertyType_Vector:
          writer.WriteAttribute ("type", "vector");
          break;
        case PropertyType_Matrix:
          writer.WriteAttribute ("type", "matrix");
          break;
        default:
          throw xtl::format_operation_exception ("media::rfx::XmlSaver::SaveProperty", "Unexpected property type '%s'", get_name (desc.type));
      }

      if (desc.elements_count > 1)
        writer.WriteAttribute ("count", desc.elements_count);
    }
    
///Сохранение текстурных карт
    void SaveTexmaps ()
    {
      XmlWriter::Scope scope (writer, "texmaps");      
      
      size_t name_index = 1;
      
      for (TexmapMap::iterator iter=texmaps.begin (), end=texmaps.end (); iter!=end; ++iter)
        if (iter->second.shared_count > 1)
        {
          const Texmap& texmap = iter->second.object;
          
          XmlWriter::Scope texmap_scope (writer, "texmap");
          
          iter->second.id = format ("texmap#%u", name_index++);
          
          writer.WriteAttribute ("id", iter->second.id.c_str ());
          
          SaveTexmap (texmap);
        }      
    }
    
///Сохранение текстурной карты
    void SaveTexmap (const Texmap& texmap)
    {
      writer.WriteAttribute ("image", texmap.Image ());
      
      if (*texmap.Sampler ())
        writer.WriteAttribute ("sampler", texmap.Sampler ());
    }
    
///Сохранение материалов
    void SaveMaterials ()
    {
      XmlWriter::Scope scope (writer, "materials");
      
      for (MaterialMap::iterator iter=materials.begin (), end=materials.end (); iter!=end; ++iter)
        if (iter->second.shared_count > 1)
          SaveMaterialContent (iter->second.object, iter->second.id.c_str ());

      for (MaterialLibrary::ConstIterator iter=library.CreateIterator (); iter; ++iter)
        SaveMaterial (*iter, library.ItemId (iter));
    }

///Сохранение материала
    void SaveMaterial (const Material& material, const char* id)
    {
      MaterialMap::iterator iter = materials.find (material.Id ());

      if (iter == materials.end ())
        return;
        
      if (iter->second.shared_count > 1)
      {
        const stl::string& source = iter->second.id;
        
        if (source == id)
          return;
        
        XmlWriter::Scope scope (writer, "instance_material");
        
        writer.WriteAttribute ("id", id);
        writer.WriteAttribute ("source", source.c_str ());
        
        return;
      }
      
      SaveMaterialContent (material, id);      
    }
    
    void SaveMaterialContent (const Material& material, const char* id)
    {
      XmlWriter::Scope scope (writer, "material");
      
      writer.WriteAttribute ("id", id);
      
      if (strcmp (id, material.Name ()))
        writer.WriteAttribute ("name", material.Name ());
        
      if (*material.Program ())
        writer.WriteAttribute ("program", material.Program ());
        
      if (material.TagsCount ())
      {
        stl::string tags;
        
        for (size_t i=0; i<material.TagsCount (); i++)
        {
          if (i)
            tags += " ";
            
          tags += material.Tag (i);
        }
        
        writer.WriteAttribute ("tags", tags);
      }
        
      const PropertyLayout& layout = material.Properties ().Layout ();
      
      LayoutMap::iterator layout_iter = layouts.find (layout.Id ());
      
      if (layout_iter == layouts.end ())
        throw xtl::format_operation_exception ("media::rfx::XmlSaver::SaveMaterial", "Internal error: unexpected layout");
        
      if (layout_iter->second.shared_count > 1)
        writer.WriteAttribute ("layout", layout_iter->second.id);
      
      SaveMaterialProperties (material.Properties (), layout_iter->second.shared_count <= 1);
      SaveMaterialTexmaps (material);            
    }
    
///Сохранение свойств
    void SaveMaterialProperties (const PropertyMap& properties, bool dump_layout)
    {
      for (size_t i=0, count=properties.Size (); i<count; i++)
      {
        const PropertyDesc& desc = properties.Layout ().Properties ()[i];
        
        XmlWriter::Scope scope (writer, "property");
        
        if (dump_layout)
        {
          SaveProperty (desc);
        }
        else
        {
          writer.WriteAttribute ("name", desc.name);
        }
        
        switch (desc.type)
        {
          case PropertyType_String:
          {
            const char* value = properties.GetString (i);
            
            SavePropertyValue (1, &value);
            break;
          }
          case PropertyType_Integer:
            SavePropertyValue (desc.elements_count, properties.GetIntegerArray (i));
            break;
          case PropertyType_Float:
            SavePropertyValue (desc.elements_count, properties.GetFloatArray (i));
            break;
          case PropertyType_Vector:
            SavePropertyValue (desc.elements_count, properties.GetVectorArray (i), true);
            break;
          case PropertyType_Matrix:
            SavePropertyValue (desc.elements_count, properties.GetMatrixArray (i), true);
            break;
          default:
            throw xtl::format_operation_exception ("media::rfx::XmlSaver::SaveMaterialProperties", "Unexpected property type '%s'", get_name (desc.type));
        }
      }
    }
    
///Сохранение значения свойства
    template <class T> void SavePropertyValue (size_t count, const T* data, bool explicit_node_save = false)
    {
      if (count == 1 && !explicit_node_save)
      {
        writer.WriteAttribute ("value", *data);
      }
      else
      {
        writer.WriteData (xtl::make_iterator_range (data, data + count));
      }
    }
    
///Сохранение текстурных карт материала
    void SaveMaterialTexmaps (const Material& material)
    {
      for (size_t i=0; i<material.TexmapCount (); i++)
      {
        const Texmap& texmap = material.Texmap (i);
        
        TexmapMap::iterator iter = texmaps.find (texmap.Id ());
        
        if (iter == texmaps.end ())
          throw xtl::format_operation_exception ("media::rfx::XmtlSaver::SaveMaterialTexmaps", "Internal error: unexpected texmap");
          
        if (iter->second.shared_count > 1)
        {
          XmlWriter::Scope scope (writer, "instance_texmap");
          
          writer.WriteAttribute ("source", iter->second.id);
        }
        else
        {
          XmlWriter::Scope scope (writer, "texmap");          
          
          SaveTexmap (texmap);
        }
      }
    }
    
  private:
    template <class T> struct SharedEntry
    {
      T           object;
      stl::string id;
      size_t      shared_count;
      
      SharedEntry () : shared_count (1) {}
      SharedEntry (const T& in_object) : object (in_object), shared_count (1) {}
    };
  
    typedef stl::hash_map<size_t, SharedEntry<Texmap> >         TexmapMap;
    typedef stl::hash_map<size_t, SharedEntry<PropertyLayout> > LayoutMap;
    typedef stl::hash_map<size_t, SharedEntry<Material> >       MaterialMap;    
    
  private:
    const MaterialLibrary& library;   //сохраняемая библиотека
    XmlWriter              writer;    //XML сериализатор
    MaterialMap            materials; //сохраненные материалы
    TexmapMap              texmaps;   //текстурные карты
    LayoutMap              layouts;   //шаблоны свойств
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
