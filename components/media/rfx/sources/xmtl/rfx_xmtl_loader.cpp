#include "shared.h"

using namespace media::rfx;
using namespace common;

/*
    Вспомогательный класс загрузки библиотеки материалов
*/

namespace
{

class XmlMaterialLibraryLoader
{
  private:
    MaterialLibrary& material_library; //библиотека материалов
    ParseLog         log;              //протокол ошибок загрузки
    Parser           parser;           //парсер

  private:
    /*
        Преобразование string -> int
    */
    
    template <class T> struct Name2Value
    {
      const char* string;
      T           value;
    };
    
    template <class T>
    T GetEnumValue (Parser::Iterator node_iter, const char* attribute, size_t count, const Name2Value<T>* map, const T& default_value)
    {
      Parser::Iterator attr_iter = node_iter->First (attribute);
      
      if (!attr_iter)
      {
        log.Error (node_iter, "No attribute '%s' found");
        return default_value;
      }
      
      const char* value = get<const char*> (log, attr_iter, 0);
      
      if (!value)
        return default_value;

      for (size_t i=0; i<count; i++)
        if (!::strcmp (map [i].string, value))
          return map [i].value;
          
      log.Error (attr_iter, "Wrong value '%s'", value);

      return default_value;
    }
    
    /*
        Разбор профиля common
    */
    
    void ParseCommonMap (Parser::Iterator map_iter, CommonMaterial& material)
    {
      static const Name2Value<CommonMaterialMap> texmap_map [] = {
        {"diffuse",      CommonMaterialMap_Diffuse},
        {"ambient",      CommonMaterialMap_Ambient},
        {"specular",     CommonMaterialMap_Specular},
        {"transparency", CommonMaterialMap_Transparent},
        {"emission",     CommonMaterialMap_Emission},
        {"reflective",   CommonMaterialMap_Reflective},
        {"bump",         CommonMaterialMap_Bump}
      };
      
      CommonMaterialMap map_type = GetEnumValue (map_iter, "type", sizeof (texmap_map) / sizeof (*texmap_map), texmap_map,
                                                 CommonMaterialMap_Num);
      
      if (map_type == CommonMaterialMap_Num)
        return;
      
      Texmap& texmap = material.Map (map_type);
      
        //чтение веса и состояния карты
        
      material.SetMapState  (map_type, get<int> (log, map_iter, "enable", material.MapState (map_type) != 0) ? MapState_Enabled : MapState_Disabled);
      material.SetMapWeight (map_type, get<float> (log, map_iter, "weight", material.MapWeight (map_type)));
      
        //чтение имени картинки
        
      const char* image_name = get<const char*> (log, map_iter, "image");
      
      if (image_name)
        texmap.SetImage (image_name);
      
        //чтение фильтров
        
      static const Name2Value<TexmapFilter> filter_map [] = {
        {"min_filter", TexmapFilter_Min},
        {"mag_filter", TexmapFilter_Mag},
        {"mip_filter", TexmapFilter_Mip}
      };

      static const Name2Value<TexmapFilterType> filter_type_map [] = {
        {"default",     TexmapFilterType_Default},
        {"point",       TexmapFilterType_Point},
        {"bilinear",    TexmapFilterType_Bilinear},
        {"trilinear",   TexmapFilterType_Trilinear},
        {"anisotropic", TexmapFilterType_Anisotropic}
      };
      
      static const size_t filter_map_size      = sizeof (filter_map) / sizeof (*filter_map),
                          filter_type_map_size = sizeof (filter_type_map) / sizeof (*filter_type_map);
      
      for (size_t i=0; i<filter_map_size; i++)
        texmap.SetFilterType (filter_map [i].value, GetEnumValue (map_iter, filter_map [i].string, filter_type_map_size, 
                              filter_type_map, texmap.FilterType (filter_map [i].value)));

        //чтение источников текстурных координат и режимов свёртки
        
      for (Parser::NamesakeIterator iter=map_iter->First ("texcoord"); iter; ++iter)
      {
        static const Name2Value<Texcoord> texcoord_map [] = {
          {"s", Texcoord_S},
          {"t", Texcoord_T},
          {"q", Texcoord_Q}
        };
        
        static const size_t texcoord_map_size = sizeof (texcoord_map) / sizeof (*texcoord_map);
        
        Texcoord texcoord = GetEnumValue (iter, "id", texcoord_map_size, texcoord_map, Texcoord_Num);
        
        if (texcoord == Texcoord_Num)
          continue;
          
          //чтение режима свёртки
          
        static const Name2Value<TexcoordWrap> wrap_map [] = {
          {"repeat",          TexcoordWrap_Repeat},
          {"mirror",          TexcoordWrap_Mirror},
          {"clamp",           TexcoordWrap_Clamp},
          {"clamp_to_border", TexcoordWrap_ClampToBorder}
        };
        
        static const size_t wrap_map_size = sizeof (wrap_map) / sizeof (*wrap_map);
        
        texmap.SetWrap (texcoord, GetEnumValue (iter, "wrap", wrap_map_size, wrap_map, texmap.Wrap (texcoord)));
        
          //чтение источника текстурных координат
          
        const char* source = get<const char*> (log, iter, "source");
        
        if (!source)
        {
          log.Error (iter, "No attribute 'source' found");
          continue;
        }
        
        if      (!::strcmp (source, "sphere_map"))     texmap.SetSource (texcoord, TexcoordSource_SphereMap);
        else if (!::strcmp (source, "reflection_map")) texmap.SetSource (texcoord, TexcoordSource_ReflectionMap);
        else
        {
          int source_channel = -1;
          
          xtl::io::token_iterator<const char*> token_iter (&source, &source + 1);
          
          if (read (token_iter, source_channel))
          {
            texmap.SetSource (texcoord, source_channel);
          }
          else
          {
            log.Error (iter, "Error at read 'source' value='%s'", source);
          }
        }
      }
    }
    
    Material::Pointer ParseCommonMaterial (Parser::Iterator profile_iter)
    {
        //создание материала
      
      CommonMaterial::Pointer material = CommonMaterial::Create ();
      
        //чтение типа шейдера
        
      static const Name2Value<CommonMaterialShaderType> shader_type_map [] = {
        {"flat",    CommonMaterialShaderType_Flat},
        {"gourand", CommonMaterialShaderType_Gourand},
        {"phong",   CommonMaterialShaderType_Phong}
      };
      
      static const size_t shader_type_map_size = sizeof (shader_type_map) / sizeof (*shader_type_map);
        
      material->SetShaderType (GetEnumValue (profile_iter, "shader_type", shader_type_map_size, shader_type_map,
                                             material->ShaderType ()));
      
        //чтение вещественных параметров
        
      material->SetShininess    (get<float> (log, profile_iter, "shininess", material->Shininess ()));
      material->SetTransparency (get<float> (log, profile_iter, "transparency", material->Transparency ()));
      
        //чтение параметров смешивания
        
      static const Name2Value<BlendEquation> blend_equation_map [] = {
        {"add",     BlendEquation_Add},
        {"sub",     BlendEquation_Subtraction},
        {"rev_sub", BlendEquation_ReverseSubtraction},
        {"min",     BlendEquation_Min},
        {"max",     BlendEquation_Max},
      };
      
      static const Name2Value<BlendArgument> blend_argument_map [] = {
        {"zero",          BlendArgument_Zero},
        {"one",           BlendArgument_One},
        {"src_color",     BlendArgument_SourceColor},
        {"src_alpha",     BlendArgument_SourceAlpha},
        {"inv_src_color", BlendArgument_InverseSourceColor},
        {"inv_src_alpha", BlendArgument_InverseSourceAlpha},
        {"dst_color",     BlendArgument_DestinationColor},
        {"dst_alpha",     BlendArgument_DestinationAlpha},
        {"inv_dst_color", BlendArgument_InverseDestinationColor},
        {"inv_dst_alpha", BlendArgument_InverseDestinationAlpha},
      };
      
      static const size_t blend_equation_map_size = sizeof (blend_equation_map) / sizeof (*blend_equation_map),
                          blend_argument_map_size = sizeof (blend_argument_map) / sizeof (*blend_argument_map);
      
      const BlendFunction& default_blend = material->Blend ();
        
      material->SetBlend (GetEnumValue (profile_iter, "blend_equation", blend_equation_map_size, blend_equation_map, default_blend.equation),
                          GetEnumValue (profile_iter, "blend_arg1", blend_argument_map_size, blend_argument_map, default_blend.argument [0]),
                          GetEnumValue (profile_iter, "blend_arg2", blend_argument_map_size, blend_argument_map, default_blend.argument [1]));
                          
        //чтение параметров альфа-отсечения
        
      static const Name2Value<CompareMode> compare_mode_map [] = {
        {"fail",          CompareMode_AlwaysFail},
        {"pass",          CompareMode_AlwaysPass},
        {"equal",         CompareMode_Equal},
        {"not_equal",     CompareMode_NotEqual},
        {"less",          CompareMode_Less},
        {"less_equal",    CompareMode_LessEqual},
        {"greater",       CompareMode_Greater},
        {"greater_equal", CompareMode_GreaterEqual}
      };
      
      static const size_t compare_mode_map_size = sizeof (compare_mode_map) / sizeof (*compare_mode_map);
      
      material->SetAlphaTestMode      (GetEnumValue (profile_iter, "alpha_test_mode", compare_mode_map_size, compare_mode_map,
                                       material->AlphaTestMode ()));
      material->SetAlphaTestReference (get<float> (log, profile_iter, "alpha_test_ref", material->AlphaTestReference ()));
      
        //чтение цветов
        
      static const Name2Value<CommonMaterialColor> color_map [] = {
        {"emission.#text", CommonMaterialColor_Emission},
        {"ambient.#text",  CommonMaterialColor_Ambient},
        {"diffuse.#text",  CommonMaterialColor_Diffuse},
        {"specular.#text", CommonMaterialColor_Specular}
      };
      
      for (size_t i=0; i<CommonMaterialColor_Num; i++)
        material->SetColor (color_map [i].value, get<math::vec3f> (log, profile_iter, color_map [i].string,
                            material->Color (color_map [i].value)));
                            
        //чтение карт
      
      for_each_child (profile_iter, "map", xtl::bind (&XmlMaterialLibraryLoader::ParseCommonMap, this, _1, xtl::ref (*material)));
      
      return material;
    }
    
    /*
        Разбор профиля "multipass"
    */
    
    MultiPassMaterial::Pointer ParseMultiPassMaterial (Parser::Iterator profile_iter)
    {
        //создание материала
        
      MultiPassMaterial::Pointer material = MultiPassMaterial::Create ();
      
        //чтение проходов
        
      for (Parser::NamesakeIterator pass_iter=profile_iter->First ("pass"); pass_iter; ++pass_iter)
      {
        const char* material_id = get<const char*> (log, pass_iter, "material");
        
        if (!material_id)
          continue;
          
        Material::Pointer sub_material = material_library.Find (material_id);
        
        if (!sub_material)
        {
          log.Error (pass_iter, "Material '%s' not found", material_id);
          continue;
        }
        
        size_t pass_index = material->AddPass (sub_material);
        
        material->SetPassState (pass_index, get<int> (log, pass_iter, "enable", 1) ? PassState_Enabled : PassState_Disabled);
      }
      
      return material;
    }
    
    /*
        Разбор материалов
    */

    void ParseMaterial (Parser::Iterator mtl_iter)
    {
      const char* id = get<const char*> (mtl_iter, "id");
      
      if (!id)
      {
        log.Error (mtl_iter, "No 'id' attribute found");
        return;
      }
       
        //создание материала
        
      Material::Pointer material;
      Parser::Iterator  profile_iter;
      
      if      (profile_iter = mtl_iter->First ("common_profile"))    material = ParseCommonMaterial (profile_iter);
      else if (profile_iter = mtl_iter->First ("multipass_profile")) material = ParseMultiPassMaterial (profile_iter);
      else
      {
        log.Error (mtl_iter, "Unknown profile at load material '%s'", id);
        return;
      }
      
      if (!material)
        return;
      
        //чтение имени материала
        
      const char* name = get<const char*> (log, mtl_iter, "name");
      
      if (name)
        material->Rename (name);
        
        //чтение группы сортировки
        
      int sort_group = get<int> (log, mtl_iter, "sort_group", material->SortGroup ());

      material->SetSortGroup (sort_group);

        //загрузка пинов
             
      static const Name2Value<MaterialPin> pin_name_map [] = {
        {"two_sided",    MaterialPin_TwoSided},
        {"wireframe",    MaterialPin_Wireframe},
        {"lighting",     MaterialPin_Lighting},
        {"cast_shadows", MaterialPin_CastShadows},
        {"recv_shadows", MaterialPin_ReceiveShadows},
        {"self_shadow",  MaterialPin_SelfShadow}
      };
      
      static const size_t pin_name_map_size = sizeof (pin_name_map) / sizeof (*pin_name_map);
      
      for (size_t i=0; i<pin_name_map_size; i++)
        material->SetPin (pin_name_map [i].value, get<size_t> (log, mtl_iter, pin_name_map [i].string,
                          material->IsEnabled (pin_name_map [i].value)) ? true : false);

        //регистрация материала

      material_library.Attach (id, material);
    }
    
    /*
        Разбор ссылки на материал
    */
    
    void ParseInstanceMaterial (Parser::Iterator mtl_iter)
    {
      const char *id     = get<const char*> (mtl_iter, "id"),
                 *source = get<const char*> (mtl_iter, "source");
      
      if (!id)
      {
        log.Error (mtl_iter, "No 'id' attribute");
        return;
      }
      
      if (!source)
      {
        log.Error (mtl_iter, "No 'source' attribute");
        return;
      }
      
      Material::Pointer material_ptr = material_library.Find (source);
      
      if (!material_ptr)
      {
        log.Error (mtl_iter, "No material with name '%s' found", source);
      }
      
      material_library.Attach (id, material_ptr);
    }
  
    /*
        Разбор библиотеки
    */
    
    void LoadLibrary (Parser::Iterator library_iter)
    {        
      for_each_child (library_iter, "material", xtl::bind (&XmlMaterialLibraryLoader::ParseMaterial, this, _1));
      for_each_child (library_iter, "instance_material", xtl::bind (&XmlMaterialLibraryLoader::ParseInstanceMaterial, this, _1));
    }

  public:
    XmlMaterialLibraryLoader (const char* file_name, MaterialLibrary& in_library, const MaterialLibrary::LogHandler& log_handler)
      : material_library (in_library), parser (file_name, "xml")
    {
        //загрузка библиотеки
      
      Parser::Iterator library_iter = parser.Root ()->First ("material_library");
      
      if (!library_iter)
      {
        log.Error (parser.Root (), "No 'material_library' tag found");
        return;
      }

      LoadLibrary (library_iter);
      
        //вывод протокола загрузки

      for (size_t i=0, count=log.MessagesCount (); i<count; i++)
        log_handler (log.Message (i));
    }
};

/*
    Регистратор компонента загрузки библиотеки материалов
*/

class XmlMaterialLibraryLoaderComponent
{
  public:
    XmlMaterialLibraryLoaderComponent ()
    {
      MaterialLibraryManager::RegisterLoader ("xmtl", &LoadLibrary);
    }
    
  private:
    static void LoadLibrary (const char* file_name, MaterialLibrary& library, const MaterialLibrary::LogHandler& log_handler)
    {
      XmlMaterialLibraryLoader (file_name, library, log_handler);
    }
};

}

extern "C"
{

ComponentRegistrator<XmlMaterialLibraryLoaderComponent> XMtlLoader ("media.rfx.loaders.XMtl");

}
