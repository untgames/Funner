#ifndef MEDIALIB_RFX_DEFAULT_SERIALIZERS_SHARED_HEADER
#define MEDIALIB_RFX_DEFAULT_SERIALIZERS_SHARED_HEADER

#include <stl/vector>
#include <stl/hash_map>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/ref.h>
#include <xtl/shared_ptr.h>

#include <math/matrix.h>

#include <common/component.h>
#include <common/log.h>
#include <common/parser.h>

#include <media/rfx/effect_library.h>
#include <media/rfx/material_library.h>
#include <media/rfx/shader_library.h>

#ifdef _MSC_VER
  #pragma warning (disable : 4503) //decorated name length exceeded, name was truncated
#endif

namespace media
{

namespace rfx
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Загрузчик свойств
///////////////////////////////////////////////////////////////////////////////////////////////////
class PropertiesLoader
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
   PropertiesLoader (common::ParseLog& log);
   
///////////////////////////////////////////////////////////////////////////////////////////////////
///Загрузка шаблонов
///////////////////////////////////////////////////////////////////////////////////////////////////
   void LoadTemplate (common::Parser::Iterator parse_iter);
   
///////////////////////////////////////////////////////////////////////////////////////////////////
///Загрузка свойств
///////////////////////////////////////////////////////////////////////////////////////////////////
   void LoadProperties (common::Parser::Iterator parse_iter, common::PropertyMap& properties);
  
  private:
    struct PropertyTemplate
    {
      common::PropertyType type;

      PropertyTemplate (common::PropertyType in_type) : type (in_type) {} 
    };
        
    typedef stl::hash_map<stl::hash_key<const char*>, PropertyTemplate>       PropertyTemplateMap;
    typedef xtl::shared_ptr<PropertyTemplateMap>                              PropertyTemplateMapPtr;
    typedef stl::hash_map<stl::hash_key<const char*>, PropertyTemplateMapPtr> TemplateMap;

  private:
    void LoadTemplateProperty (common::Parser::Iterator parse_iter, PropertyTemplateMap& tmpl);
    void LoadProperty (common::Parser::Iterator parse_iter, common::PropertyMap& properties);
    void LoadPropertyWithTemplate (common::Parser::Iterator parse_iter, common::PropertyMap& properties, PropertyTemplateMap& tmpl);

  private:
    TemplateMap       properties_templates; //шаблоны свойств
    common::ParseLog log;                  //протокол загрузки
};

}

}

#endif
