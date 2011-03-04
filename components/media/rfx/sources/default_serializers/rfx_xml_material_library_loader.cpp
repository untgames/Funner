#include "shared.h"

using namespace media::rfx;
using namespace common;

namespace
{

/*
    Константы
*/

const char*  COMPONENT_NAME      = "media.rfx.material.loaders.xmtl"; //имя компонента
const char*  LOG_NAME            = COMPONENT_NAME;                    //имя протокола
const char*  EXTENSION           = "xmtl";                            //расширение загружаемого ресурса
const size_t TEXMAP_CACHE_SIZE   = 32;                                //резервируемый размер кэша загружаемых текстурных карт
const size_t ELEMENTS_CACHE_SIZE = 8;                                 //резервируемый размер кэшей загрузки массивов

/*
    Загрузчик
*/

class XmtlLoader
{
  public:
    XmtlLoader (const char* in_name, MaterialLibrary& in_library)
      : parser (in_name, "xml")
      , log (parser.Log ())
    {
      try
      {
          //резервирование памяти кэшей загрузки
        
        texmap_cache.reserve (TEXMAP_CACHE_SIZE);
        int_cache.reserve    (ELEMENTS_CACHE_SIZE);
        float_cache.reserve  (ELEMENTS_CACHE_SIZE);
        vector_cache.reserve (ELEMENTS_CACHE_SIZE);
        matrix_cache.reserve (ELEMENTS_CACHE_SIZE);
        
          //загрузка библиотек
          
        for_each_child (parser.Root (), "material_library", xtl::bind (&XmtlLoader::LoadLibrary, this, _1));
        
          //вывод протокола загрузки
          
        Log out_log (LOG_NAME);
          
        for (size_t i=0; i<log.MessagesCount (); i++)
          out_log.Print (log.Message (i));
          
          //проверка ошибок
          
        if (log.HasErrors ())
        {
          for (size_t i=0; i<log.MessagesCount (); i++)
            switch (log.MessageType (i))
            {
              case ParseLogMessageType_FatalError:
              case ParseLogMessageType_Error:
                throw xtl::format_operation_exception ("", "%s\n    at parse '%s'", log.Message (i), in_name);
              default:
              case ParseLogMessageType_Warning:
                break;
            }
        }

          //замена
                
        in_library.Swap (library);
      }
      catch (xtl::exception& e)
      {
        e.touch ("XmtlLoader::XmtlLoader");
        throw;
      }
    }
    
  private:
///Загрузка библиотеки материалов
    void LoadLibrary (Parser::Iterator parse_iter)
    {
      for_each_child (*parse_iter, "layouts", xtl::bind (&XmtlLoader::LoadLayouts, this, _1));
      for_each_child (*parse_iter, "texmaps", xtl::bind (&XmtlLoader::LoadTexmaps, this, _1));
      for_each_child (*parse_iter, "materials", xtl::bind (&XmtlLoader::LoadMaterials, this, _1));
    }
    
///Загрузка шаблонов
    void LoadLayouts (Parser::Iterator parse_iter)
    {
      for_each_child (*parse_iter, "layout", xtl::bind (&XmtlLoader::LoadLayout, this, _1));
    }
    
///Загрузка шаблона
    void LoadLayout (Parser::Iterator parse_iter)
    {
      const char* id = get<const char*> (*parse_iter, "id");
      
      common::PropertyLayout layout;
      
      for_each_child (*parse_iter, "property", xtl::bind (&XmtlLoader::LoadLayoutProperty, this, _1, layout));
      
      if (layouts.find (id) != layouts.end ())
        log.Warning (*parse_iter, "Layout '%s' has been already defined. Will be replaced", id);
        
      layouts [id] = layout;
    }    
    
///Определение типа свойства
    common::PropertyType GetPropertyType (const char* value, common::ParseNode& node)
    {
      if (!xtl::xstrcmp (value, "string")) return common::PropertyType_String;
      if (!xtl::xstrcmp (value, "int"))    return common::PropertyType_Integer;
      if (!xtl::xstrcmp (value, "float"))  return common::PropertyType_Float;
      if (!xtl::xstrcmp (value, "vector")) return common::PropertyType_Vector;
      if (!xtl::xstrcmp (value, "matrix")) return common::PropertyType_Matrix;
      
      log.Error (node, "Unexpected property type '%s'", value);
      
      return common::PropertyType_String;
    }
    
///Загрузка свойства шаблона
    size_t LoadLayoutProperty (Parser::Iterator parse_iter, common::PropertyLayout& layout)
    {
      const char*          name  = get<const char*> (*parse_iter, "name");
      common::PropertyType type  = GetPropertyType (get<const char*> (*parse_iter, "type"), *parse_iter);
      size_t               count = get<size_t> (*parse_iter, "count", 1);
      
      return layout.AddProperty (name, type, count);
    }
    
///Загрузка текстурных карт
    void LoadTexmaps (Parser::Iterator parse_iter)
    {
      for_each_child (*parse_iter, "texmap", xtl::bind (&XmtlLoader::LoadGlobalTexmap, this, _1));
    }
    
///Загрузка текстурной карты
    void LoadGlobalTexmap (Parser::Iterator parse_iter)
    {
      const char* id = get<const char*> (*parse_iter, "id");
      
      Texmap texmap = LoadTexmap (parse_iter);
      
      if (texmaps.find (id) != texmaps.end ())
        log.Warning (*parse_iter, "Texmap '%s' has been already defined. Will be replaced", id);
        
      texmaps [id] = texmap;
    }
    
    Texmap LoadTexmap (Parser::Iterator parse_iter)
    {
      const char* image   = get<const char*> (*parse_iter, "image");
      const char* sampler = get<const char*> (*parse_iter, "sampler", "");
      
      Texmap texmap;
      
      texmap.SetImage (image);
      texmap.SetSampler (sampler);
      
      return texmap;
    }

///Загрузка материалов
    void LoadMaterials (Parser::Iterator parse_iter)
    {
      for_each_child (*parse_iter, "material", xtl::bind (&XmtlLoader::LoadMaterial, this, _1));
      for_each_child (*parse_iter, "instance_material", xtl::bind (&XmtlLoader::LoadInstanceMaterial, this, _1));
    }
    
///Загрузка значения свойства из блока #text
    template <class T> void LoadPropertyValueFromNode (Parser::Iterator parse_iter, T* destination, size_t count)
    {
      read (*parse_iter, "#text", destination, count);
    }
    
///Загрузка значения свойства из блока value
    template <class T> bool LoadPropertyValueFromAttribute (Parser::Iterator parse_iter, T* destination, size_t count)
    {
      const char* value = get<const char*> (*parse_iter, "value", "");
      
      if (!*value)
        return false;
        
      common::StringArray tokens = common::split (value);
      
      size_t read_count = 0;
      
      if (tokens.Size ())
      {
        xtl::io::token_iterator<const char*> iter = xtl::io::make_token_iterator (tokens.Data (), tokens.Data () + tokens.Size ());
      
        read_count = xtl::io::read_range (iter, destination, count);
      }
      
      if (read_count != count)
        log.Error (*parse_iter, "Bad value (%u elements readed, required is %u)", read_count, count);
        
      return true;
    }    
    
///Загрузка значения свойства
    template <class T> void LoadPropertyValue (Parser::Iterator parse_iter, common::PropertyMap& properties, size_t property_index, stl::vector<T>& cache)
    {
      size_t elements_count = properties.PropertyElementsCount (property_index);
      
      if (elements_count == 1)
      {
        T value = T ();
        
        if (!LoadPropertyValueFromAttribute (parse_iter, &value, 1))
          LoadPropertyValueFromNode (parse_iter, &value, 1);
          
        properties.SetProperty (property_index, value);
      }
      else
      {
        cache.clear ();
        cache.resize (elements_count);
        
        if (!LoadPropertyValueFromAttribute (parse_iter, &cache [0], cache.size ()))
          LoadPropertyValueFromNode (parse_iter, &cache [0], cache.size ());
          
        properties.SetProperty (property_index, elements_count, &cache [0]);
      }
    }
    
///Загрузка свойства материала
    void LoadMaterialProperty (Parser::Iterator parse_iter, common::PropertyMap& properties, const char* layout)
    {
      const char* name = get<const char*> (*parse_iter, "name");      
      
      size_t property_index = 0;
      
      if (*layout)
      {
        property_index = properties.IndexOf (name);
        
        if ((int)property_index == -1)
        {
          log.Error (*parse_iter, "Property '%s' has not been declared in layout '%s'", name, layout);
          return;
        }
        
        if (parse_iter->First ("type") || parse_iter->First ("count"))
          log.Warning (*parse_iter, "Property of fixed layout material can't have 'type' or 'count' attributes");
      }
      else
      {
        common::PropertyType type  = GetPropertyType (get<const char*> (*parse_iter, "type"), *parse_iter);
        size_t               count = get<size_t> (*parse_iter, "count", 1);
        
        property_index = properties.AddProperty (name, type, count);
      }
      
      common::PropertyType type = properties.PropertyType (property_index);
      
      switch (type)
      {
        case common::PropertyType_String:
        {
          const char* value = get<const char*> (*parse_iter, "value");
          
          properties.SetProperty (property_index, value);
          
          break;
        }
        case common::PropertyType_Integer:
          LoadPropertyValue (parse_iter, properties, property_index, int_cache);
          break;
        case common::PropertyType_Float:
          LoadPropertyValue (parse_iter, properties, property_index, float_cache);        
          break;
        case common::PropertyType_Vector:
          LoadPropertyValue (parse_iter, properties, property_index, vector_cache);        
          break;
        case common::PropertyType_Matrix:
          LoadPropertyValue (parse_iter, properties, property_index, matrix_cache);
          break;
        default:
          log.Error (*parse_iter, "Unexpected property type '%s'", get_name (type));
          return;
      }
    }
    
///Загрузка текстурной карты материала
    void LoadMaterialTexmap (Parser::Iterator parse_iter, stl::vector<Texmap>& material_texmaps)
    {
      material_texmaps.push_back (LoadTexmap (parse_iter));
    }
    
///Загрузка инстанцированной текстурной карты материала
    void LoadMaterialInstanceTexmap (Parser::Iterator parse_iter, stl::vector<Texmap>& material_texmaps)
    {
      const char* source = get<const char*> (*parse_iter, "source");
      
      TexmapMap::iterator iter = texmaps.find (source);
      
      if (iter == texmaps.end ())
      {
        log.Error (*parse_iter, "Texmap '%s' has not been declared", source);
        return;
      }
        
      material_texmaps.push_back (iter->second);
    }
    
///Загрузка материала
    void LoadMaterial (Parser::Iterator parse_iter)
    {
      const char* id      = get<const char*> (*parse_iter, "id");
      const char* name    = get<const char*> (*parse_iter, "name", id);
      const char* tags    = get<const char*> (*parse_iter, "tags", "");      
      const char* program = get<const char*> (*parse_iter, "program", "");
      const char* layout  = get<const char*> (*parse_iter, "layout", "");
      
      texmap_cache.clear ();
      
      Material material;
      
      material.SetName (name);
      material.SetProgram (program);
      material.SetTags (tags);
      
      if (*layout)
      {
        LayoutMap::iterator iter = layouts.find (layout);
        
        if (iter == layouts.end ())
        {
          log.Error (*parse_iter, "Layout '%s' has not been declared", layout);
          return;
        }

        common::PropertyMap (iter->second).Swap (material.Properties ());        
      }      
      
      common::PropertyMap properties = material.Properties ();
      
      for (Parser::Iterator iter=parse_iter->First (); iter; ++iter)
      {
        if      (!xtl::xstrcmp (iter->Name (), "property"))        LoadMaterialProperty (iter, properties, layout);
        else if (!xtl::xstrcmp (iter->Name (), "texmap"))          LoadMaterialTexmap (iter, texmap_cache);
        else if (!xtl::xstrcmp (iter->Name (), "instance_texmap")) LoadMaterialInstanceTexmap (iter, texmap_cache);
      }
      
      material.SetTexmapCount (texmap_cache.size ());
      
      for (size_t i=0; i<texmap_cache.size (); i++)
        material.Texmap (i) = texmap_cache [i];

      library.Attach (id, material);
    }

///Инстансинг материала
    void LoadInstanceMaterial (Parser::Iterator parse_iter)
    {
      const char* id     = get<const char*> (*parse_iter, "id");
      const char* source = get<const char*> (*parse_iter, "source");
      
      Material* material = library.Find (source);
      
      if (!material)
      {
        log.Error (*parse_iter, "Material '%s' has not been declared", source);
        return;
      }

      library.Attach (id, *material);
    }
    
  private:
    typedef stl::hash_map<stl::hash_key<const char*>, common::PropertyLayout> LayoutMap;
    typedef stl::hash_map<stl::hash_key<const char*>, Texmap>                 TexmapMap;
    typedef stl::vector<Texmap>                                               TexmapArray;
    typedef stl::vector<int>                                                  IntArray;
    typedef stl::vector<float>                                                FloatArray;
    typedef stl::vector<math::vec4f>                                          VectorArray;
    typedef stl::vector<math::mat4f>                                          MatrixArray;

  private:
    MaterialLibrary  library;           //загружаемая библиотека
    Parser           parser;            //парсер
    ParseLog         log;               //протокол загрузки
    LayoutMap        layouts;           //шаблоны
    TexmapMap        texmaps;           //текстурные карты
    TexmapArray      texmap_cache;      //кэш загружаемых текстурных карт для материала
    IntArray         int_cache;         //кэш загрузки массивов целых чисел
    FloatArray       float_cache;       //кэш загрузки массивов чисел с плавающей точкой
    VectorArray      vector_cache;      //кэш загрузки массивов векторов
    MatrixArray      matrix_cache;      //кэш загрузки массивов матриц
};

/*
    Компонент
*/

class Component
{
  public:
    Component ()
    {
      MaterialLibraryManager::RegisterLoader (EXTENSION, &LoadLibrary);
    }
    
  private:
    static void LoadLibrary (const char* name, MaterialLibrary& library)
    {
      XmtlLoader (name, library);
    }
};

}

extern "C"
{

ComponentRegistrator<Component> XmtlLoader (COMPONENT_NAME);

}
