#include <stl/string>
#include <math/mathlib.h>

enum NodeType
{
  NODE,
  JOINT
};

struct Skew
{
  float       angle;
  math::vec3f rotate_axis, translate_axis;
};

struct Transform
{
  Skew        skew;
  math::mat4f matrix;
  math::vec3f lookat[3], scale, translate;
  math::vec4f rotate;
};

struct MaterialBind
{
  stl::string semantic, target;
};

struct ColladaBindData
{
  stl::vector <ColladaInstanceMaterialImpl> materials;
  stl::vector <stl::string>                 skeletons;
};

namespace medialib
{

class ColladaInstanceMaterialImpl : public ColladaEntityImpl
{
  public:
    stl::string                target, symbol;
    stl::vector <MaterialBind> binds;
    ColladaMeshImpl*           mesh;
};

struct ColladaInstanceGeometryImpl
{
  stl::string     url;
  ColladaBindData bind;
};

class ColladaInstanceControllerImpl : public ColladaEntityImpl
{
  public:
    stl::string     url;
    ColladaBindData bind;
};

class ColladaNodeImpl : public ColladaEntityImpl
{
  public:
    stl::string                                 sid, name;    
    stl::vector <stl::string>                   layers, instance_camera, instance_light, instance_node;
    NodeType                                    type;
    Transform                                   transform;
    stl::vector <ColladaNodeImpl>               child_nodes;
    stl::vector <ColladaInstanceGeometryImpl>   instance_geometry;
    stl::vector <ColladaInstanceControllerImpl> instance_controller;
};

}
