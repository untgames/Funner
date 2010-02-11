#include "shared.h"

using namespace media::rfx;
using namespace common;

/*
    Вспомогательные классы для сохранения материалов
*/

namespace
{

typedef stl::hash_multimap<const Material*, stl::string> MaterialIdMap;
typedef stl::vector<MaterialIdMap::iterator>             MaterialSaveList;

class XmlMaterialLibraryPreSaver: public xtl::visitor<void, MultiPassMaterial>
{
  private:
    typedef stl::hash_set<stl::hash_key<const char*> > IdSet;

  private:
    MaterialIdMap&    material_map; //сопоставление материала его имени
    MaterialSaveList& save_list;    //упорядоченный список сохранения материалов
    IdSet             id_set;       //множество зарегистрированных идентификаторов
    
  private:
      //регистрация нового материала
    MaterialIdMap::iterator RegisterNewMaterial (const char* id, Material& material)
    {
      if (id_set.count (id))
        return material_map.end ();
      
      id_set.insert (id);
      
      try
      {
        return material_map.insert_pair (&material, id);
      }
      catch (...)
      {
        id_set.erase (id);
        throw;
      }
    }

    MaterialIdMap::iterator RegisterNewMaterial (Material& material)
    {
      MaterialIdMap::iterator iter = RegisterNewMaterial (material.Name (), material);

      for (size_t i=1; iter == material_map.end (); i++)
        iter = RegisterNewMaterial (common::format ("%s#%u", material.Name (), i).c_str (), material);
        
      return iter;
    }
    
    void InsertToSaveList (MaterialIdMap::iterator iter)
    {
      save_list.push_back (iter);
    }
  
      //посещение многопроходного материала
    void visit (MultiPassMaterial& material)
    {
        //сохранение проходов
        
      for (size_t i=0, count=material.PassesCount (); i<count; ++i)
      {
        Material&               sub_material = *material.Material (i);
        MaterialIdMap::iterator iter         = material_map.find (&sub_material);
        
        if (iter != material_map.end ())
          continue;

          //регистрация нового материала
          
        iter = RegisterNewMaterial (sub_material);
        
        sub_material.Accept (*this);
        
        InsertToSaveList (iter);
      }      
    }    
    
  public:
    
      //конструктор
    XmlMaterialLibraryPreSaver (const MaterialLibrary& material_library, MaterialIdMap& in_material_map, MaterialSaveList& in_save_list) 
      : material_map (in_material_map), save_list (in_save_list)
    {
      for (MaterialLibrary::ConstIterator i=material_library.CreateIterator (); i; ++i)
      {
        Material& material = const_cast<Material&> (**i);
        
        MaterialIdMap::iterator iter = RegisterNewMaterial (material_library.ItemId (i), material);
        
        if (iter == material_map.end ())
          continue;
        
        material.Accept (*this);
        
        InsertToSaveList (iter);
      }
    }
};

class XmlMaterialLibrarySaver: public xtl::visitor<void, CommonMaterial, MultiPassMaterial, SpriteMaterial>
{
  private: 
    typedef stl::hash_set<const Material*> MaterialSet;
    
  private:
    XmlWriter              writer;           //сериализатор XML
    const MaterialLibrary& material_library; //библиотека материалов
    MaterialIdMap          material_map;     //сопоставление материала его имени
    MaterialSet            saved_materials;  //сохранённые материалы
    MaterialSaveList       save_list;        //список сохранения

  private:
    /*
        Сохранение многопроходного материала
    */
    
    void visit (MultiPassMaterial& material)
    {
      XmlWriter::Scope scope (writer, "multipass_profile");

        //сохранение проходов

      writer.WriteAttribute ("passes_count", material.PassesCount ());

      for (size_t i=0, count=material.PassesCount (); i<count; ++i)
      {
        MaterialIdMap::const_iterator iter = material_map.find (&*material.Material (i));
        
        if (iter == material_map.end ())
          continue;
          
        XmlWriter::Scope scope (writer, "pass");
        
        writer.WriteAttribute ("material", iter->second);
        writer.WriteAttribute ("enable", material.IsPassEnabled (i));
      }
    }
    
    /*
        Сохранение SpriteMaterial
    */

    void visit (SpriteMaterial& material)
    {
      XmlWriter::Scope scope (writer, "sprite_profile");
            
      writer.WriteAttribute ("image", material.Image ());
      writer.WriteAttribute ("blend_mode", get_name (material.BlendMode ()));
      writer.WriteAttribute ("tiling", material.IsTiled ());            

      if (material.IsTiled () || material.TileOffsetX ())
        writer.WriteAttribute ("tile_offset_x", material.TileOffsetX ());

      if (material.IsTiled () || material.TileOffsetY ())
        writer.WriteAttribute ("tile_offset_y", material.TileOffsetY ());

      if (material.IsTiled () || material.TileWidth ())
        writer.WriteAttribute ("tile_width", material.TileWidth ());
        
      if (material.IsTiled () || material.TileHeight ())        
        writer.WriteAttribute ("tile_height", material.TileHeight ());        
    }
    
    /*
        Сохранение CommonMaterial
    */

    void visit (CommonMaterial& material)
    {
      XmlWriter::Scope scope (writer, "common_profile");
      
      writer.WriteAttribute ("shader_type", get_name (material.ShaderType ()));      

      for (size_t i=0; i<CommonMaterialPin_Num; i++)
        writer.WriteAttribute (get_name ((CommonMaterialPin)i), material.IsEnabled ((CommonMaterialPin)i));      
      
      writer.WriteAttribute ("shininess", material.Shininess ());
      writer.WriteAttribute ("transparency", material.Transparency ());
      
      const BlendFunction& blend_function = material.Blend ();
      
      writer.WriteAttribute ("blend_equation", get_name (blend_function.equation));
      writer.WriteAttribute ("blend_arg1", get_name (blend_function.argument [0]));
      writer.WriteAttribute ("blend_arg2", get_name (blend_function.argument [1]));
      
      writer.WriteAttribute ("alpha_test_mode", get_name (material.AlphaTestMode ()));
      writer.WriteAttribute ("alpha_test_ref", material.AlphaTestReference ());
      
        //сохранение цветов
        
      struct MtlColor
      {
        const char*         name;
        CommonMaterialColor color;
      };
        
      static const MtlColor color_map [] = {
        {"emission", CommonMaterialColor_Emission},
        {"ambient",  CommonMaterialColor_Ambient},
        {"diffuse",  CommonMaterialColor_Diffuse},
        {"specular", CommonMaterialColor_Specular},
      };
      
      static const size_t color_map_size = sizeof (color_map) / sizeof (*color_map);
      
      for (size_t i=0; i<color_map_size; i++)
      {
        XmlWriter::Scope scope (writer, color_map [i].name);
        
        writer.WriteData (material.Color (color_map [i].color));
      }
      
        //сохранение карт
      
      for (size_t i=0; i<CommonMaterialMap_Num; i++)
      {
        const Texmap& texmap = material.Map ((CommonMaterialMap)i);
        
        if (!*texmap.Image ())
          continue;
        
        XmlWriter::Scope scope (writer, "map");

        writer.WriteAttribute ("type", get_name ((CommonMaterialMap)i));
        writer.WriteAttribute ("enable", material.IsMapEnabled ((CommonMaterialMap)i));
        writer.WriteAttribute ("weight", material.MapWeight ((CommonMaterialMap)i));

        writer.WriteAttribute ("image", texmap.Image ());
        writer.WriteAttribute ("min_filter", get_name (texmap.FilterType (TexmapFilter_Min)));
        writer.WriteAttribute ("mag_filter", get_name (texmap.FilterType (TexmapFilter_Mag)));
        writer.WriteAttribute ("mip_filter", get_name (texmap.FilterType (TexmapFilter_Mip)));
        
        for (size_t i=0; i<Texcoord_Num; i++)
        {
          XmlWriter::Scope scope (writer, "texcoord");         
          
          Texcoord tc     = (Texcoord)i;          
          int      source = texmap.Source (tc);
          
          writer.WriteAttribute ("id", get_name (tc));
          
          switch (source)
          {
            case TexcoordSource_SphereMap: 
            case TexcoordSource_ReflectionMap:
              writer.WriteAttribute ("source", get_name ((TexcoordSource)texmap.Source (tc)));
              break;
            default:
              writer.WriteAttribute ("source", source);
              break;
          }

          writer.WriteAttribute ("wrap", get_name (texmap.Wrap (tc)));
        }
      }
    }

    /*
        Сохранение материала
    */
    
    void SaveMaterial (const char* id, const Material& material)
    {
      if (saved_materials.find (&material) != saved_materials.end ())
      {
        MaterialIdMap::const_iterator iter = material_map.find (&material);
        
        if (iter == material_map.end ())
          return;
          
          //запись ссылки на материал вместо самого материала

        XmlWriter::Scope scope (writer, "instance_material");

        writer.WriteAttribute ("id", id);
        writer.WriteAttribute ("source", iter->second);

        return;        
      }      
      
        //сохранение заголовка
        
      XmlWriter::Scope scope (writer, "material");
 
      writer.WriteAttribute ("id", id);
      writer.WriteAttribute ("name", material.Name ());
      writer.WriteAttribute ("sort_group", material.SortGroup ());

        //сохранение специализации материала

      const_cast<Material&> (material).Accept (*this);            

        //регистрация материала

      saved_materials.insert (&material);
    }
  
    /*
        Сохранение библиотеки
    */
    
    void SaveLibrary ()
    {
      XmlWriter::Scope scope (writer, "material_library");
        
        //сохранение материалов
      
      for (MaterialSaveList::iterator i=save_list.begin (), end=save_list.end (); i!=end; ++i)      
        SaveMaterial ((*i)->second.c_str (), *(*i)->first);
    }
  
  public:
    /*
        Конструктор
    */
    
    XmlMaterialLibrarySaver (const char* file_name, const MaterialLibrary& in_material_library)
      : writer (file_name), material_library (in_material_library)
    {
        //регистрация материалов
        
      save_list.reserve (material_library.Size ());
      
      XmlMaterialLibraryPreSaver (material_library, material_map, save_list);

        //сохранение библиотеки

      SaveLibrary ();
    }
};

/*
    Регистратор компонента сохранения библиотеки материалов
*/

class XmlMaterialLibrarySaverComponent
{
  public:
    XmlMaterialLibrarySaverComponent ()
    {
      MaterialLibraryManager::RegisterSaver ("xmtl", &SaveLibrary);            
    }
    
  private:
    static void SaveLibrary (const char* file_name, const MaterialLibrary& library, const MaterialLibrary::LogHandler&)
    {
      XmlMaterialLibrarySaver (file_name, library);
    }
};

}

extern "C"
{

ComponentRegistrator<XmlMaterialLibrarySaverComponent> XMtlSaver ("media.rfx.savers.XMtl");

}
