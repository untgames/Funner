#include "shared.h"

using namespace common;

/*
    Получение характеристик
*/

namespace media
{

namespace geometry
{

//размер типа атрибута вершин в байтах
unsigned short get_type_size (VertexAttributeType type)
{
  switch (type)
  {
    case VertexAttributeType_Float2:    return sizeof (float) * 2;
    case VertexAttributeType_Float3:    return sizeof (float) * 3;
    case VertexAttributeType_Float4:    return sizeof (float) * 4;
    case VertexAttributeType_Short2:    return sizeof (short) * 2;
    case VertexAttributeType_Short3:    return sizeof (short) * 3;    
    case VertexAttributeType_Short4:    return sizeof (short) * 4;
    case VertexAttributeType_UByte4:    return sizeof (unsigned char) * 4;
    case VertexAttributeType_Influence: return sizeof (VertexInfluence);
    default:                            throw xtl::make_argument_exception ("media::get_type_size", "type", type);
  }
  
  return 0;
}

//количество компонентов
unsigned char get_components_count (VertexAttributeType type)
{
  switch (type)
  {
    case VertexAttributeType_Short2:
    case VertexAttributeType_Float2:    return 2;
    case VertexAttributeType_Short3:
    case VertexAttributeType_Float3:    return 3;
    case VertexAttributeType_Short4:
    case VertexAttributeType_UByte4:
    case VertexAttributeType_Float4:    return 4;
    case VertexAttributeType_Influence: return 1;
    default:                            throw xtl::make_argument_exception ("media::get_components_count", "type", type);
  }

  return 0;
}

//имя семантики
const char* get_semantic_name (VertexAttributeSemantic semantic)
{
  switch (semantic)
  {
    case VertexAttributeSemantic_Position:     return "position";
    case VertexAttributeSemantic_Normal:       return "normal";
    case VertexAttributeSemantic_Color:        return "color";
    case VertexAttributeSemantic_TexCoord0:    return "texcoord0";
    case VertexAttributeSemantic_TexCoord1:    return "texcoord1";
    case VertexAttributeSemantic_TexCoord2:    return "texcoord2";
    case VertexAttributeSemantic_TexCoord3:    return "texcoord3";
    case VertexAttributeSemantic_TexCoord4:    return "texcoord4";
    case VertexAttributeSemantic_TexCoord5:    return "texcoord5";
    case VertexAttributeSemantic_TexCoord6:    return "texcoord6";
    case VertexAttributeSemantic_TexCoord7:    return "texcoord7";
    case VertexAttributeSemantic_Influence:    return "influence";
    case VertexAttributeSemantic_Custom:       return "custom";
    default:                                   throw xtl::make_argument_exception ("media::geometry::get_semantic_name(VertexAttributeSemantic)", "semantic", semantic);
  }

  return "";
}

//получение семантики по имени
VertexAttributeSemantic get_vertex_attribute_semantic (const char* name, VertexAttributeSemantic default_semantic)
{
  if (!name)
    return default_semantic;
    
  struct Map
  {
    stl::hash_key<const char*> name_hash;
    VertexAttributeSemantic    semantic;
  };
  
  static Map map [] = {
    {"position",     VertexAttributeSemantic_Position},
    {"normal",       VertexAttributeSemantic_Normal},
    {"color",        VertexAttributeSemantic_Color},
    {"texcoord0",    VertexAttributeSemantic_TexCoord0},
    {"texcoord1",    VertexAttributeSemantic_TexCoord1},
    {"texcoord2",    VertexAttributeSemantic_TexCoord2},
    {"texcoord3",    VertexAttributeSemantic_TexCoord3},
    {"texcoord4",    VertexAttributeSemantic_TexCoord4},
    {"texcoord5",    VertexAttributeSemantic_TexCoord5},
    {"texcoord6",    VertexAttributeSemantic_TexCoord6},
    {"texcoord7",    VertexAttributeSemantic_TexCoord7},
    {"influence",    VertexAttributeSemantic_Influence},
    {"custom",       VertexAttributeSemantic_Custom},
  };
  
  static const size_t map_size = sizeof (map) / sizeof (*map);
  
  const Map* map_iter = map;
  size_t     hash     = strhash (name);
  
  for (size_t i=0; i<map_size; i++, map_iter++)
    if (map_iter->name_hash.get_hash () == hash)
      return map_iter->semantic;
  
  return default_semantic;
}

//имя типа
const char* get_type_name (VertexAttributeType type)
{
  switch (type)
  {
    case VertexAttributeType_Float2:    return "float2";
    case VertexAttributeType_Float3:    return "float3";
    case VertexAttributeType_Float4:    return "float4";
    case VertexAttributeType_Short2:    return "short2";
    case VertexAttributeType_Short3:    return "short3";
    case VertexAttributeType_Short4:    return "short4";
    case VertexAttributeType_UByte4:    return "ubyte4";
    case VertexAttributeType_Influence: return "influence";
    default:                            throw xtl::make_argument_exception ("media::geometry::get_type_name(VertexAttributeType)", "type", type);
  }

  return "";
}

//получение типа по имени
VertexAttributeType get_vertex_attribute_type (const char* name, VertexAttributeType default_type)
{
  if (!name)
    return default_type;

  struct Map
  {
    stl::hash_key<const char*> name_hash;
    VertexAttributeType        type;
  };

  static Map map [] = {
    {"float2",    VertexAttributeType_Float2},
    {"float3",    VertexAttributeType_Float3},
    {"float4",    VertexAttributeType_Float4},
    {"short2",    VertexAttributeType_Short2},
    {"short3",    VertexAttributeType_Short3},
    {"short4",    VertexAttributeType_Short4},
    {"ubyte4",    VertexAttributeType_UByte4},
    {"influence", VertexAttributeType_Influence}
  };

  static const size_t map_size = sizeof (map) / sizeof (*map);

  const Map* map_iter = map;
  size_t     hash     = strhash (name);

  for (size_t i=0; i<map_size; i++, map_iter++)
    if (map_iter->name_hash.get_hash () == hash)
      return map_iter->type;

  return default_type;
}

//проверка совместимости
bool is_compatible (VertexAttributeSemantic semantic, VertexAttributeType type)
{
  switch (semantic)
  {
    case VertexAttributeSemantic_Custom:
      switch (type)
      {
        case VertexAttributeType_Influence:
        case VertexAttributeType_Float2:
        case VertexAttributeType_Float3:
        case VertexAttributeType_Float4:
        case VertexAttributeType_Short2:
        case VertexAttributeType_Short3:
        case VertexAttributeType_Short4:
        case VertexAttributeType_UByte4: return true;
        default:                         return false;
      }    
    case VertexAttributeSemantic_Position:
    case VertexAttributeSemantic_TexCoord0:
    case VertexAttributeSemantic_TexCoord1:
    case VertexAttributeSemantic_TexCoord2:
    case VertexAttributeSemantic_TexCoord3:
    case VertexAttributeSemantic_TexCoord4:
    case VertexAttributeSemantic_TexCoord5:
    case VertexAttributeSemantic_TexCoord6:
    case VertexAttributeSemantic_TexCoord7:
      switch (type)
      {
        case VertexAttributeType_Float2:
        case VertexAttributeType_Float3:
        case VertexAttributeType_Float4:
        case VertexAttributeType_Short2:
        case VertexAttributeType_Short3:
        case VertexAttributeType_Short4:
        case VertexAttributeType_UByte4: return true;
        default:                         return false;
      }
    case VertexAttributeSemantic_Normal:
      switch (type)
      {
        case VertexAttributeType_Float3:
        case VertexAttributeType_Short3: return true;
        default:                         return false;
      }
    case VertexAttributeSemantic_Color:
      switch (type)
      {
        case VertexAttributeType_Float3:
        case VertexAttributeType_Float4:
        case VertexAttributeType_Short3:
        case VertexAttributeType_Short4:
        case VertexAttributeType_UByte4: return true;
        default:                         return false;
      }
    case VertexAttributeSemantic_Influence:
      switch (type)
      {
        case VertexAttributeType_Influence: return true;
        default:                            return false;
      }
    default:
      break;
  }
  
  return false;
}

const char* get_index_type_name (IndexType type)
{
  switch (type)
  {
    case IndexType_UInt32: return "uint32";
    case IndexType_UInt16: return "uint16";
    case IndexType_UInt8:  return "uint8";
    default:               throw xtl::make_argument_exception ("media::geometry::get_index_type_name", "type", type);
  }
}

unsigned char get_index_type_size (IndexType type)
{
  switch (type)
  {
    case IndexType_UInt32: return sizeof (unsigned int);
    case IndexType_UInt16: return sizeof (unsigned short);
    case IndexType_UInt8:  return sizeof (unsigned char);
    default:               throw xtl::make_argument_exception ("media::geometry::get_index_type_size", "type", type);
  }
}

}

}
