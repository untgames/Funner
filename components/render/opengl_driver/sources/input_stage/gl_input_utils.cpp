#include "shared.h"

using namespace common;

namespace render
{

namespace low_level
{

namespace opengl
{

namespace
{

//сопоставление семантики и её имени
struct Semantic2Name
{
  VertexAttributeSemantic semantic; //семантика
  const char*             name;     //имя
};

const Semantic2Name semantic_names [] = {
  {VertexAttributeSemantic_Position,  "gl_Position"},
  {VertexAttributeSemantic_Normal,    "gl_Normal"},
  {VertexAttributeSemantic_Color,     "gl_Color"},
  {VertexAttributeSemantic_TexCoord0, "gl_TexCoord0"},
  {VertexAttributeSemantic_TexCoord1, "gl_TexCoord1"},
  {VertexAttributeSemantic_TexCoord2, "gl_TexCoord2"},
  {VertexAttributeSemantic_TexCoord3, "gl_TexCoord3"},
  {VertexAttributeSemantic_TexCoord4, "gl_TexCoord4"},
  {VertexAttributeSemantic_TexCoord5, "gl_TexCoord5"},
  {VertexAttributeSemantic_TexCoord6, "gl_TexCoord6"},
  {VertexAttributeSemantic_TexCoord7, "gl_TexCoord7"},
};

const size_t semantic_names_count = sizeof (semantic_names) / sizeof (*semantic_names);

}

//получение семантики по имени
VertexAttributeSemantic get_semantic_by_name (const char* name)
{
  static const char* METHOD_NAME = "render::low_level::opengl::get_semantic_by_name";

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");

  const Semantic2Name* iter = semantic_names;

  for (size_t i=0; i<semantic_names_count; i++, iter++)
    if (!strcmp (iter->name, name))
      return iter->semantic;

  throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Unknown semantic");
}

//получение имени семантики
const char* get_semantic_name (VertexAttributeSemantic semantic)
{
  static const char* METHOD_NAME = "render::low_level::opengl::get_semantic_name";

  const Semantic2Name* iter = semantic_names;

  for (size_t i=0; i<semantic_names_count; i++, iter++)
    if (semantic == iter->semantic)
      return iter->name;

  throw xtl::make_argument_exception (METHOD_NAME, "semantic", semantic);
}

}

}

}
