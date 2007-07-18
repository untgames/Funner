#include <media/collada.h>
#include <common/exception.h>
#include <common/parser.h>
#include <stl/vector>
#include <xtl/bind.h>
#include "base.h"
#include "mesh.h"
#include "visual_scene.h"
#include "light.h"
#include "camera.h"
#include "controller.h"
#include "animation.h"
#include "material.h"
#include "effect.h"

using namespace common;
using namespace math;
using namespace stl;
using namespace xtl;

class Offsets
{
  public:
    Offsets ();

    short                v_offset, n_offset, color_offset,/*tangent_offset, binormal_offset,*/ max_offset;
    vector <short>       tex_coord_offset, tex_tangent_offset, tex_binormal_offset, tex_coord_set, tex_tangent_set, tex_binormal_set;
    const char           *v_source, *n_source, *color_source/*, *tangent_source, *binormal_source*/;
    vector <const char*> tex_coord_source, tex_tangent_source, tex_binormal_source;
};
  
struct PointInfo
{
  size_t          vindex, nindex, cindex/*, bindex, tindex*/;
  vector <size_t> tex_coord_index, tex_bi_index, tex_tan_index;
};

enum _ParseDirective
{
  PARSE_VERTICES,
  PARSE_NORMALS,
  PARSE_TEX_COORDS,
  PARSE_TEX_BINORMALS,
  PARSE_TEX_TANGENTS
};

void _RaiseException    (const char* source, const char* description);
void _RaiseSearchFailed (const char* source);

namespace medialib
{

class ColladaImpl
{    
  public:
    template <class T> const vector<T>& SelectLib () const;
    
    void parse_collada                 (Parser::Iterator p);
    void parse_library_geometries      (Parser::Iterator p);
    void parse_library_materials       (Parser::Iterator p);
    void parse_library_effects         (Parser::Iterator p);
    void parse_library_images          (Parser::Iterator p);
    void parse_library_lights          (Parser::Iterator p);
    void parse_library_cameras         (Parser::Iterator p);
    void parse_library_controllers     (Parser::Iterator p);
    void parse_library_nodes           (Parser::Iterator p);
    void parse_library_visual_scenes   (Parser::Iterator p);
    void parse_library_animations      (Parser::Iterator p);
    void parse_library_animation_clips (Parser::Iterator p);
    
    void check_dependencies      ();
    bool check_material_bind     (ColladaInstanceMaterialImpl* i_material, ColladaNodeImpl* node);
    bool check_anim_dependencies (ColladaAnimationImpl*        animation);
    bool check_node_dependencies (ColladaNodeImpl*             node);

    void parse_geometry            (Parser::Iterator p);
    void parse_mesh                (Parser::Iterator p, ColladaMeshImpl *destination);

    void parse_lines               (Parser::Iterator lines,      Parser::Iterator mesh, ColladaMeshImpl *destination_mesh);
    void parse_linestrips          (Parser::Iterator linestrips, Parser::Iterator mesh, ColladaMeshImpl *destination_mesh);
    void parse_triangles           (Parser::Iterator triangles,  Parser::Iterator mesh, ColladaMeshImpl *destination_mesh);
    void parse_trifans             (Parser::Iterator trifans,    Parser::Iterator mesh, ColladaMeshImpl *destination_mesh);
    void parse_tristrips           (Parser::Iterator tristrips,  Parser::Iterator mesh, ColladaMeshImpl *destination_mesh);
    int  parse_common_surface_attr (Parser::Iterator surface,    Parser::Iterator mesh, ColladaSurfaceImpl *temp_surface, Offsets *offsets);
    void parse_p                   (Parser::Iterator p,          const Offsets *offsets,     vector <PointInfo> *temp_indexes, int v_count = -1);
    bool parse_inputs              (Parser::Iterator p,          Offsets *offsets);                //заполнение Offsets
    void finalize_surface          (ColladaSurfaceImpl *temp_surface, vector <PointInfo> *temp_indexes, ColladaSurfaceImpl *destination); //окончательное создание Surface на основе временных индексов
    
    void parse_material        (Parser::Iterator p);
    void parse_instance_effect (Parser::Iterator p, ColladaMaterialImpl *destination);

    void parse_effect         (Parser::Iterator p);
    void parse_profile_COMMON (Parser::Iterator p, ColladaEffectImpl  *destination);
    void parse_image          (Parser::Iterator p, ColladaImageImpl   *destination);
    void parse_texture        (Parser::Iterator p, ColladaTextureImpl *destination);

    void parse_light            (Parser::Iterator p);
    void parse_technique_common (Parser::Iterator p, ColladaLightImpl *destination);

    void parse_camera           (Parser::Iterator p);
    void parse_technique_common (Parser::Iterator p, ColladaCameraImpl *destination);

    void parse_controller (Parser::Iterator p, ColladaControllerImpl* destination);
    void parse_skin       (Parser::Iterator p, ColladaSkinImpl*       destination);
    void parse_morph      (Parser::Iterator p, ColladaMorphImpl*      destination);

    void parse_node               (Parser::Iterator p, ColladaNodeImpl* destination);
    void parse_instance_materials (Parser::Iterator p, vector<ColladaInstanceMaterialImpl> *destination, string* mesh_url);

    void parse_visual_scene (Parser::Iterator p);

    void parse_animation            (Parser::Iterator p, ColladaAnimationImpl* destination);
    void parse_interpolation_source (Parser::Iterator p, const char *source);
    void parse_interpolations       (Parser::Iterator n_array);

    void parse_animation_clip (Parser::Iterator p);

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///Поиск тега "source" внутри тега р и занесение его данных в destination
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void parse_source      (Parser::Iterator p, vector <ColladaTexVertex> *destination, const Offsets *offsets, size_t index);
    void parse_source      (Parser::Iterator p, vector <ColladaVertex>    *destination, const Offsets *offsets);
    void parse_source      (Parser::Iterator p, vector <vec3f>            *destination, const char *source);
    void parse_source      (Parser::Iterator p, vector <vec4f>            *destination, const char *source, float forth_component = 0.f);
    void parse_source      (Parser::Iterator p, vector <string>           *destination, const char *source);
    void parse_source      (Parser::Iterator p, vector <float>            *destination, const char *source);
    void parse_source      (Parser::Iterator p, vector <mat4f>            *destination, const char *source);

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///Занесение данных тега в destination
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void parse_float_array (Parser::Iterator f_array, vector <ColladaTexVertex> *destination, _ParseDirective directive);
    void parse_float_array (Parser::Iterator f_array, vector <ColladaVertex>    *destination, _ParseDirective directive);
    void parse_float_array (Parser::Iterator f_array, vector <vec3f>            *destination);
    void parse_float_array (Parser::Iterator source,  vector <vec4f>            *destination, float forth_component = 0.f);
    void parse_float_array (Parser::Iterator f_array, vector <float>            *destination);
    void parse_float_array (Parser::Iterator f_array, vector <mat4f>            *destination);
    void parse_Name_array  (Parser::Iterator n_array, vector <string>           *destination);

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///Поиск элементов
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ColladaNodeImpl* SearchNodeID (ColladaNodeImpl* start, const stl::string* id);
    ColladaNodeImpl* SearchNodeID (const stl::string* id);
    ColladaNodeImpl* SearchNodeSID (ColladaNodeImpl* start, const char* sid);
    ColladaNodeImpl* SearchNodeSID (const stl::string* sid);
    ColladaMaterialImpl* SearchMaterial   (string* id);
    bool                 SearchMesh       (string* id);
    ColladaEffectImpl*   SearchEffect     (string* id);
    bool                 SearchAnimation  (string* id);
    bool                 SearchCamera     (string* id);
    bool                 SearchLight      (string* id);
    bool                 SearchController (string* id);

    vector <ColladaMeshImpl>          meshes;
    vector <ColladaMaterialImpl>      materials;
    vector <ColladaEffectImpl>        effects;
    vector <ColladaImageImpl>         images;
    vector <ColladaLightImpl>         lights;
    vector <ColladaCameraImpl>        cameras;
    vector <ColladaControllerImpl>    controllers;
    vector <ColladaNodeImpl>          nodes;
    vector <ColladaVisualSceneImpl>   visual_scenes;
    vector <ColladaAnimationImpl>     animations;
    vector <ColladaAnimationClipImpl> animation_clips;
    string                            scene;

    Parser::Log* log;
};

}