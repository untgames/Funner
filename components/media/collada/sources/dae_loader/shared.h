#ifndef MEDIALIB_DAE_SHARED_HEADER
#define MEDIALIB_DAE_SHARED_HEADER

#include <cstdarg>

#include <stl/hash_map>
#include <stl/vector>
#include <stl/string>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/ref.h>
#include <xtl/reference_counter.h>

#include <common/parser.h>
#include <common/hash.h>
#include <common/strlib.h>
#include <common/component.h>

#include <media/collada.h>

#include <math/utility.h>

using namespace media::collada;
using namespace common;
using namespace xtl;
using namespace math;

namespace media
{

namespace collada
{

//forward declarations
class  DaeParser;
class  MeshSourceMap;
class  MeshInputBuilder;
struct SurfaceInfo;

///////////////////////////////////////////////////////////////////////////////////////////////////
///ќтображение имени в значение
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct String2Value
{
  const char* string; //строковое значение
  T           value;  //значение
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///ћассив дублированни€ индексов вершин
///////////////////////////////////////////////////////////////////////////////////////////////////
class VertexIndexMap: public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///–азмер массива индексов / изменение размера массива индексов
///////////////////////////////////////////////////////////////////////////////////////////////////
   size_t Size () const { return indices.size (); }
   void   Resize (size_t new_size) { indices.resize (new_size); }
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///ћассив индексов
///////////////////////////////////////////////////////////////////////////////////////////////////
          size_t* Indices ()       { return &indices [0]; }
    const size_t* Indices () const { return &indices [0]; }

  private:
    typedef stl::vector<size_t> IndexArray;

    IndexArray indices;
};

typedef xtl::intrusive_ptr<VertexIndexMap> VertexIndexMapPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///«агрузчик коллады
///////////////////////////////////////////////////////////////////////////////////////////////////
class DaeParser
{
  friend class LogScope;
  typedef Model::LogHandler LogHandler;
  public:    
    DaeParser  (const char* file_name, Model& model, const LogHandler& log);
    ~DaeParser ();

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///ѕечать ошибок в протокол
///////////////////////////////////////////////////////////////////////////////////////////////////
    void PrintLog (const LogHandler&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///—оздание или поиск карт вершинных индексов
///////////////////////////////////////////////////////////////////////////////////////////////////
    VertexIndexMapPtr GetVertexIndicesMap  (const char* mesh_id, size_t surface_index);

///////////////////////////////////////////////////////////////////////////////////////////////////
///–азбор отдельных элементов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ParseRoot                (Parser::Iterator);
    void ParseLibraries           (Parser::Iterator);
    void ParseLibraryAnimations   (Parser::Iterator);
    void ParseLibraryImages       (Parser::Iterator);
    void ParseLibraryEffects      (Parser::Iterator);
    void ParseLibraryMaterials    (Parser::Iterator);
    void ParseLibraryGeometries   (Parser::Iterator);
    void ParseLibraryControllers  (Parser::Iterator);
    void ParseLibraryLights       (Parser::Iterator);
    void ParseLibraryCameras      (Parser::Iterator);
    void ParseLibraryVisualScenes (Parser::Iterator);
    void ParseAnimation           (Parser::Iterator, AnimationList& collection);
    void ParseAnimationChannel    (Parser::Iterator channel_iter, Parser::Iterator animation_iter, Animation& animation);
    void ParseImage               (Parser::Iterator);
    void ParseEffect              (Parser::Iterator);
    void ParseEffectProfileCommon (Parser::Iterator, Effect& effect);
    void ParseTexture             (Parser::Iterator, Parser::Iterator profile_iter, Effect& effect, TextureMap map);
    void ParseMaterial            (Parser::Iterator);
    void ParseGeometry            (Parser::Iterator);
    void ParseMesh                (Parser::Iterator, Mesh& mesh);
    void ParseMeshSource          (Parser::Iterator, MeshSourceMap& sources);
    void ParseSurfaceInput        (Parser::Iterator, Parser::Iterator mesh_iter, MeshSourceMap& sources, MeshInputBuilder& inputs);
    void ParsePolygonSurface      (Parser::Iterator);
    void ParseSurface             (Parser::Iterator, Parser::Iterator mesh_iter, Mesh& mesh, PrimitiveType type, MeshSourceMap& sources);
    void ParseSurfaceBuffers      (Parser::Iterator, Parser::Iterator surface_iter, SurfaceInfo& info);
    void ParseController          (Parser::Iterator);
    void ParseMorph               (Parser::Iterator, const char* id);
    void ParseSkin                (Parser::Iterator, const char* id);
    void ParseLight               (Parser::Iterator);
    void ParseCamera              (Parser::Iterator);
    void ParseVisualScene         (Parser::Iterator);
    void ParseNode                (Parser::Iterator, Node& parent);
    void ParseTransform           (Parser::Iterator, const char* node_id, math::mat4f& tm);
    void ParseInstanceLight       (Parser::Iterator, Node::LightList& lights);
    void ParseInstanceCamera      (Parser::Iterator, Node::CameraList& cameras);
    void ParseInstanceGeometry    (Parser::Iterator, Node::MeshList& meshes);
    void ParseInstanceController  (Parser::Iterator, Node::ControllerList& controllers);
    void ParseBindMaterial        (Parser::Iterator iter, MaterialBinds& binds);
    void ParseIdrefArray          (Parser::Iterator iter, stl::vector<stl::string>& source);
    void ParseNameArray           (Parser::Iterator iter, stl::vector<stl::string>& source);
    void ParseFloatArray          (Parser::Iterator iter, stl::vector<float>& source);
    void ParseFloatArray          (Parser::Iterator iter, stl::vector<math::mat4f>& source);

  private:
    typedef stl::hash_map<stl::hash_key<const char*>, VertexIndexMapPtr>        VertexIndexMaps;
    typedef stl::hash_map<stl::hash_key<const char*>, AnimationChannelSemantic> AnimationSemanticsMap;

  private:
    Model&                model;               //загружаема€ модель
    Parser                parser;              //парсер
    VertexIndexMaps       vertex_index_maps;   //карты вершинных индексов
    AnimationSemanticsMap animation_semantics; //найденные возможные семантики анимаций
};

}

}

#endif
