#include <media/collada.h>
#include <stl/string>
#include <stl/vector>
#include <math/mathlib.h>

namespace medialib
{

class ColladaTextureImpl : public ColladaEntityImpl
{
  public:
    ColladaTextureImpl ();

    float       amount;
    math::mat4f transform;
    stl::string texture, texcoord; //texture - сменить имя
};

struct ColladaTextureMapImpl
{
  ColladaTextureImpl texture;
  bool               has_color;
  math::vec4f        color;
  
  ColladaTextureMapImpl ();
};

struct ShaderCOMMON
{
  ColladaTextureMapImpl maps [COLLADA_MAP_NUM]; //текстурные карты
  float                 reflectivity;           //степень отражения
  float                 transparency;           //степень прозрачности
  float                 index_of_refraction;    //индекс преломления
  float                 shininess;              //степень "металличности"
};

struct ColladaImageImpl
{
  stl::string id, url;
};

class ColladaEffectImpl : public ColladaEntityImpl
{
  public:
    stl::vector <ColladaImageImpl> images;
    ColladaShaderType              shader_type;
    ShaderCOMMON                   shader;
};

}

