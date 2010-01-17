#include "shared.h"

using namespace media::rfx;
using namespace common;

namespace
{

/*
    Константы
*/

const char* COMPONENT_NAME = "media.rfx.effect.loaders.xrfx"; //имя компонента
const char* LOG_NAME       = COMPONENT_NAME;                  //имя протокола
const char* EXTENSION      = "xrfx";                          //расширение загружаемого ресурса

/*
    Загрузчик
*/

class XrfxLoader
{
  private:
    struct PropertyTemplate
    {
      PropertyType type;

      PropertyTemplate (PropertyType in_type) : type (in_type) {} 
    };
        
    typedef stl::hash_map<stl::hash_key<const char*>, PropertyTemplate>       PropertyTemplateMap;
    typedef xtl::shared_ptr<PropertyTemplateMap>                              PropertyTemplateMapPtr;
    typedef stl::hash_map<stl::hash_key<const char*>, PropertyTemplateMapPtr> TemplateMap;
    typedef stl::hash_map<stl::hash_key<const char*>, TechniquePass>          PassMap;    
    typedef stl::hash_map<stl::hash_key<const char*>, Technique>              TechniqueMap;
    typedef stl::list<EffectParameter>                                        ParameterList;
    
    struct ParameterBlock
    {
      ParameterList parameters;  
    };

    typedef xtl::shared_ptr<ParameterBlock>                              ParameterBlockPtr;
    typedef stl::hash_map<stl::hash_key<const char*>, ParameterBlockPtr> ParameterBlockMap;

  public:
    XrfxLoader (const char* in_name, EffectLibrary& in_library)
      : parser (in_name, "xml")
      , log (parser.Log ())
      , properties_loader (log)
    {
      try
      {
          //загрузка библиотек
          
        for_each_child (parser.Root (), "EffectLibrary", xtl::bind (&XrfxLoader::LoadEffectLibrary, this, _1));
        
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
        e.touch ("XrfxLoader::XrfxLoader");
        throw;
      }
    }
    
  private:
///Загрузка библиотеки материалов
    void LoadEffectLibrary (Parser::Iterator parse_iter)
    {
      for_each_child (*parse_iter, "Templates", xtl::bind (&XrfxLoader::LoadTemplates, this, _1));
      for_each_child (*parse_iter, "Passes", xtl::bind (&XrfxLoader::LoadPasses, this, _1));
      for_each_child (*parse_iter, "Techniques", xtl::bind (&XrfxLoader::LoadTechniques, this, _1));
      for_each_child (*parse_iter, "Effects", xtl::bind (&XrfxLoader::LoadEffects, this, _1)); 
    }
    
///Загрузка шаблонов
    void LoadTemplates (Parser::Iterator parse_iter)
    {
      for_each_child (*parse_iter, "Properties", xtl::bind (&XrfxLoader::LoadPropertiesTemplate, this, _1));
      for_each_child (*parse_iter, "EffectParameters", xtl::bind (&XrfxLoader::LoadEffectParametersBlock, this, _1));
    }
    
///Загрузка проходов
    void LoadPasses (Parser::Iterator parse_iter)
    {
      for_each_child (*parse_iter, "Pass", xtl::bind (&XrfxLoader::LoadPass, this, _1));
      for_each_child (*parse_iter, "InstancePass", xtl::bind (&XrfxLoader::LoadInstancePass, this, _1));
    }
    
///Загрузка техник
    void LoadTechniques (Parser::Iterator parse_iter)
    {
      for_each_child (*parse_iter, "Technique", xtl::bind (&XrfxLoader::LoadTechnique, this, _1));
      for_each_child (*parse_iter, "InstanceTechnique", xtl::bind (&XrfxLoader::LoadInstanceTechnique, this, _1));
    }
    
///Загрузка эффектов
    void LoadEffects (Parser::Iterator parse_iter)
    {
      for_each_child (*parse_iter, "Effect", xtl::bind (&XrfxLoader::LoadEffect, this, _1));
      for_each_child (*parse_iter, "InstanceEffect", xtl::bind (&XrfxLoader::LoadInstanceEffect, this, _1));
    }
    
///Загрузка шаблона
    void LoadPropertiesTemplate (Parser::Iterator parse_iter)
    {
      properties_loader.LoadTemplate (parse_iter);
    }
    

///Загрузка свойств
    void LoadProperties (Parser::Iterator parse_iter, PropertyMap& properties)
    {
      properties_loader.LoadProperties (parse_iter, properties);
    }
    
///Загрузка прохода
    void LoadPass (Parser::Iterator parse_iter)
    {
      const char* id = get<const char*> (*parse_iter, "Name");
      
      TechniquePass pass;
      
      LoadProperties (parse_iter, pass.Properties ());
      
      passes.insert_pair (id, pass);
    }
    
///Инстансинг прохода
    void LoadInstancePass (Parser::Iterator parse_iter)
    {
      const char* id     = get<const char*> (*parse_iter, "Name");
      const char* source = get<const char*> (*parse_iter, "Source");
      
      PassMap::iterator iter = passes.find (source);
      
      if (iter == passes.end ())
      {
        log.Error (*parse_iter, "Undefined pass '%s' used", source);
        return;
      }
      
      passes.insert_pair (id, iter->second);
    }
    
///Загрузка техники
    void LoadTechnique (Parser::Iterator parse_iter)
    {
      const char* id   = get<const char*> (*parse_iter, "Name");
      
      Technique technique;
      
      technique.SetName (id);
      
      for_each_child (*parse_iter, "Passes", xtl::bind (&XrfxLoader::LoadTechniquePasses, this, _1, xtl::ref (technique)));
      for_each_child (*parse_iter, "Properties", xtl::bind (&XrfxLoader::LoadProperties, this, _1, xtl::ref (technique.Properties ())));
      
      techniques.insert_pair (id, technique);
    }
    
///Инстансинг техники
    void LoadInstanceTechnique (Parser::Iterator parse_iter)
    {
      const char* id     = get<const char*> (*parse_iter, "Name");
      const char* source = get<const char*> (*parse_iter, "Source");
      
      TechniqueMap::iterator iter = techniques.find (source);
      
      if (iter == techniques.end ())
      {
        log.Error (*parse_iter, "Undefined technique '%s' used", source);
        return;
      }

      techniques.insert_pair (id, iter->second);
    }
    
///Загрузка проходов техники
    void LoadTechniquePasses (Parser::Iterator parse_iter, Technique& technique)
    {
      for (Parser::Iterator iter=parse_iter->First (); iter; ++iter)
      {
        if (!strcmp (iter->Name (), "Pass"))
        {
          LoadTechniquePass (iter, technique);
        }
        else if (!strcmp (iter->Name (), "InstancePass"))
        {
          LoadTechniqueInstancePass (iter, technique);
        }
      }
    }
    
///Загрузка прохода техники
    void LoadTechniquePass (Parser::Iterator parse_iter, Technique& technique)
    {
      TechniquePass pass;
      
      LoadProperties (parse_iter, pass.Properties ());
      
      technique.AddPass (pass);
    }
    
///Инстансинг прохода техники
    void LoadTechniqueInstancePass (Parser::Iterator parse_iter, Technique& technique)
    {
      const char* source = get<const char*> (*parse_iter, "Source");
      
      PassMap::iterator iter = passes.find (source);
      
      if (iter == passes.end ())
      {
        log.Error (*parse_iter, "Undefined pass '%s' used", source);
        return;
      }

      technique.AddPass (iter->second);
    }
    
///Загрузка параметров эффекта
    void LoadEffectParameters (Parser::Iterator parse_iter, Effect& effect)
    {
      const char* source = get<const char*> (*parse_iter, "Source", (const char*)0);
      
      if (source)
      {
        ParameterBlockMap::iterator iter = parameter_blocks.find (source);
        
        if (iter == parameter_blocks.end ())
        {
          log.Error (*parse_iter, "Undefined parameters block '%s' used", source);
        }
        else
        {
          ParameterBlock& block = *iter->second;
          
          for (ParameterList::iterator iter=block.parameters.begin (), end=block.parameters.end (); iter!=end; ++iter)
          {
            effect.AddParameter (*iter);
          }
        }
      }
      
      for_each_child (*parse_iter, "Parameter", xtl::bind (&XrfxLoader::LoadEffectParameter, this, _1, xtl::ref (effect))); 
    }

///Загрузка параметра эффекта
    void LoadEffectParameter (Parser::Iterator parse_iter, Effect& effect)
    {
      EffectParameter param;
      
      if (LoadParameter (parse_iter, param))
        effect.AddParameter (param);
    }
    
///Загрузка блока параметров
    void LoadEffectParametersBlock (Parser::Iterator parse_iter)
    {
      const char* id = get<const char*> (*parse_iter, "Name");
      
      ParameterBlockPtr block (new ParameterBlock);
      
      for_each_child (*parse_iter, "Parameter", xtl::bind (&XrfxLoader::LoadEffectBlockParameter, this, _1, xtl::ref (*block)));
      
      parameter_blocks.insert_pair (id, block);
    }
    
///Загрузка параметра блока
    void LoadEffectBlockParameter (Parser::Iterator parse_iter, ParameterBlock& block)
    {
      EffectParameter param;
      
      if (LoadParameter (parse_iter, param))
        block.parameters.push_back (param);
    }
    
///Загрузка параметра эффекта
    bool LoadParameter (Parser::Iterator parse_iter, EffectParameter& param)
    {
      const char* name = get<const char*> (*parse_iter, "Name");
      
      param.SetName (name);
      param.SetElementsCount (get<size_t> (*parse_iter, "ElementsCount", 1u));
      
      const char* type = get<const char*> (*parse_iter, "Type");
      
      struct Name2Type
      {
        const char*         name;
        EffectParameterType type;
      };
      
      static const Name2Type type_map [] = {
        {"int",      EffectParameterType_Int},
        {"float",    EffectParameterType_Float},
        {"int2",     EffectParameterType_Int2},
        {"float2",   EffectParameterType_Float2},
        {"int3",     EffectParameterType_Int3},
        {"float3",   EffectParameterType_Float3},
        {"int4",     EffectParameterType_Int4},
        {"float4",   EffectParameterType_Float4},
        {"float2x2", EffectParameterType_Float2x2},
        {"float3x3", EffectParameterType_Float3x3},
        {"float4x4", EffectParameterType_Float4x4}
      };
      
      static const size_t type_map_size = sizeof (type_map) / sizeof (*type_map);
      
      size_t type_index = 0;
      
      for (; type_index<type_map_size; type_index++)
        if (!strcmp (type_map [type_index].name, type))
          break;
          
      if (type_index == type_map_size)
      {
        log.Error (*parse_iter, "Unknown parameter '%s' type '%s'", name, type);
        return false;
      }
      
      param.SetType (type_map [type_index].type);
      
      return true;
    }
    
///Загрузка техник эффекта
    void LoadEffectTechniques (Parser::Iterator parse_iter, Effect& effect)
    {
      for_each_child (*parse_iter, "Technique", xtl::bind (&XrfxLoader::LoadEffectTechnique, this, _1, xtl::ref (effect)));
      for_each_child (*parse_iter, "InstanceTechnique", xtl::bind (&XrfxLoader::LoadEffectInstanceTechnique, this, _1, xtl::ref (effect)));
    }
    
///Загрузка техники для эффекта
    void LoadEffectTechnique (Parser::Iterator parse_iter, Effect& effect)
    {
      const char* name = get<const char*> (*parse_iter, "Name");
      
      Technique technique;
      
      technique.SetName (name);
      
      for_each_child (*parse_iter, "Passes", xtl::bind (&XrfxLoader::LoadTechniquePasses, this, _1, xtl::ref (technique)));
      for_each_child (*parse_iter, "Properties", xtl::bind (&XrfxLoader::LoadProperties, this, _1, xtl::ref (technique.Properties ())));      
      
      effect.AddTechnique (technique);
    }
    
///Инстансинг техники для эффекта
    void LoadEffectInstanceTechnique (Parser::Iterator parse_iter, Effect& effect)
    {
      const char* id     = get<const char*> (*parse_iter, "Name");
      const char* source = get<const char*> (*parse_iter, "Source");
      
      TechniqueMap::iterator iter = techniques.find (source);
      
      if (iter == techniques.end ())
      {
        log.Error (*parse_iter, "Undefined technique '%s' used", source);
        return;
      }

      effect.AddTechnique (iter->second);
    }
    
///Загрузка эффекта
    void LoadEffect (Parser::Iterator parse_iter)
    {
      const char* id = get<const char*> (*parse_iter, "Name");
      
      Effect effect;
      
      effect.SetName (id);
      
      for_each_child (*parse_iter, "Parameters", xtl::bind (&XrfxLoader::LoadEffectParameters, this, _1, xtl::ref (effect)));
      for_each_child (*parse_iter, "Techniques", xtl::bind (&XrfxLoader::LoadEffectTechniques, this, _1, xtl::ref (effect)));
      
      library.Attach (id, effect);
    }

///Инстансинг эффекта
    void LoadInstanceEffect (Parser::Iterator parse_iter)
    {
      const char* id     = get<const char*> (*parse_iter, "Name");
      const char* source = get<const char*> (*parse_iter, "Source");
      
      Effect* effect = library.Find (source);
      
      if (!effect)
      {
        log.Error (*parse_iter, "Undefined effect '%s' used", source);
        return;
      }

      library.Attach (id, *effect);
    }
    
  private:
    EffectLibrary     library;              //библиотека эффектов
    Parser            parser;               //парсер
    ParseLog          log;                  //протокол парсинга
    PropertiesLoader  properties_loader;    //загрузчик свойств
    ParameterBlockMap parameter_blocks;     //блоки параметров
    PassMap           passes;               //проходы
    TechniqueMap      techniques;           //техники
};

/*
    Компонент
*/

class Component
{
  public:
    Component ()
    {
      EffectLibraryManager::RegisterLoader (EXTENSION, &LoadLibrary);
    }
    
  private:
    static void LoadLibrary (const char* name, EffectLibrary& library)
    {
      XrfxLoader (name, library);
    }
};

}

extern "C"
{

ComponentRegistrator<Component> XrfxLoader (COMPONENT_NAME);

}

