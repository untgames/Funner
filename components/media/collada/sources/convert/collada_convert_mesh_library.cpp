#include "shared.h"

namespace
{

const float  EPS                   = 0.001f;
const size_t SURFACES_RESERVE_SIZE = 8;     //резервируемое количество поверхностей

const char* MAIN_VERTEX_STREAM_NAME = "main";

/*
    Текстурные касательные и бинормали
*/

struct Tangent: public media::geometry::CustomAttribute<math::vec3f>
{
  static const char* name () { return "tangent"; }

  math::vec3f tangent;
};

struct Binormal: public media::geometry::CustomAttribute<math::vec3f>
{
  static const char* name () { return "binormal"; }

  math::vec3f binormal;
};

template <size_t Channel> struct TexTangent;
template <size_t Channel> struct TexBinormal;

template <> struct TexTangent<0>: public media::geometry::CustomAttribute<math::vec3f, media::geometry::VertexAttributeSemantic_Custom>
{
  static const char* name () { return "textangent0"; }

  math::vec3f textangent0;
};

template <> struct TexTangent<1>: public media::geometry::CustomAttribute<math::vec3f, media::geometry::VertexAttributeSemantic_Custom>
{
  static const char* name () { return "textangent1"; }

  math::vec3f textangent1;
};

template <> struct TexTangent<2>: public media::geometry::CustomAttribute<math::vec3f, media::geometry::VertexAttributeSemantic_Custom>
{
  static const char* name () { return "textangent2"; }

  math::vec3f textangent2;
};

template <> struct TexTangent<3>: public media::geometry::CustomAttribute<math::vec3f, media::geometry::VertexAttributeSemantic_Custom>
{
  static const char* name () { return "textangent3"; }

  math::vec3f textangent3;
};

template <> struct TexTangent<4>: public media::geometry::CustomAttribute<math::vec3f, media::geometry::VertexAttributeSemantic_Custom>
{
  static const char* name () { return "textangent4"; }

  math::vec3f textangent4;
};

template <> struct TexTangent<5>: public media::geometry::CustomAttribute<math::vec3f, media::geometry::VertexAttributeSemantic_Custom>
{
  static const char* name () { return "textangent5"; }

  math::vec3f textangent5;
};

template <> struct TexTangent<6>: public media::geometry::CustomAttribute<math::vec3f, media::geometry::VertexAttributeSemantic_Custom>
{
  static const char* name () { return "textangent6"; }

  math::vec3f textangent6;
};

template <> struct TexTangent<7>: public media::geometry::CustomAttribute<math::vec3f, media::geometry::VertexAttributeSemantic_Custom>
{
  static const char* name () { return "textangent7"; }

  math::vec3f textangent7;
};

template <> struct TexBinormal<0>: public media::geometry::CustomAttribute<math::vec3f, media::geometry::VertexAttributeSemantic_Custom>
{
  static const char* name () { return "texbinormal0"; }

  math::vec3f texbinormal0;
};

template <> struct TexBinormal<1>: public media::geometry::CustomAttribute<math::vec3f, media::geometry::VertexAttributeSemantic_Custom>
{
  static const char* name () { return "texbinormal1"; }

  math::vec3f texbinormal1;
};

template <> struct TexBinormal<2>: public media::geometry::CustomAttribute<math::vec3f, media::geometry::VertexAttributeSemantic_Custom>
{
  static const char* name () { return "texbinormal2"; }

  math::vec3f texbinormal2;
};

template <> struct TexBinormal<3>: public media::geometry::CustomAttribute<math::vec3f, media::geometry::VertexAttributeSemantic_Custom>
{
  static const char* name () { return "texbinormal3"; }

  math::vec3f texbinormal3;
};

template <> struct TexBinormal<4>: public media::geometry::CustomAttribute<math::vec3f, media::geometry::VertexAttributeSemantic_Custom>
{
  static const char* name () { return "texbinormal4"; }

  math::vec3f texbinormal4;
};

template <> struct TexBinormal<5>: public media::geometry::CustomAttribute<math::vec3f, media::geometry::VertexAttributeSemantic_Custom>
{
  static const char* name () { return "texbinormal5"; }

  math::vec3f texbinormal5;
};

template <> struct TexBinormal<6>: public media::geometry::CustomAttribute<math::vec3f, media::geometry::VertexAttributeSemantic_Custom>
{
  static const char* name () { return "texbinormal6"; }

  math::vec3f texbinormal6;
};

template <> struct TexBinormal<7>: public media::geometry::CustomAttribute<math::vec3f, media::geometry::VertexAttributeSemantic_Custom>
{
  static const char* name () { return "texbinormal7"; }

  math::vec3f texbinormal7;
};

template <size_t Channel> struct TexChannel: public media::geometry::TexChannel<Channel>
{
  typedef TexTangent<Channel>  Tangentf;
  typedef TexBinormal<Channel> Binormalf;
};

/*
    Преобразователь данных коллады
*/

class Converter
{
  private:
      //forwards
    struct Surface;
    struct Mesh;
    struct Skin;

    typedef xtl::shared_ptr<Surface>                           SurfacePtr;
    typedef stl::vector<SurfacePtr>                            SurfaceList;
    typedef xtl::shared_ptr<Mesh>                              MeshPtr;
    typedef xtl::shared_ptr<Skin>                              SkinPtr;
    typedef stl::hash_map<stl::hash_key<const char*>, MeshPtr> MeshMap;
    typedef stl::hash_map<stl::hash_key<const char*>, SkinPtr> SkinMap;

    typedef stl::hash_map<stl::hash_key<const char*>, media::geometry::VertexStream> VertexStreamMap;
    
    //преобразованная поверхность
    struct Surface
    {
      VertexStreamMap                vertex_streams; //вершинный потоки
      media::geometry::PrimitiveType primitive_type; //тип примитивов
      size_t                         first_index;    //номер первого индекса
      size_t                         indices_count;  //количество индексов
      stl::string                    material;       //имя материала
    };    

    //преобразованный меш
    struct Mesh
    {
      SurfaceList                  surfaces;      //преобразованные поверхности
      media::geometry::IndexBuffer index_buffer;  //индексный буфер

      Mesh () { surfaces.reserve (SURFACES_RESERVE_SIZE); }
    };

    //преобразованный скин
    struct Skin
    {
      MeshPtr                             base_mesh; //базовый меш
      media::geometry::VertexWeightStream weights;   //вершинные веса
      stl::string                         id;        //идентификатор (для поиска каналов vertex-influence)
    };

  private:
    const media::collada::Model&   model;             //исходная коллада-модель
    media::geometry::MeshLibrary&  mesh_library;      //результирующая библиотека мешей
    MeshMap                        mesh_map;          //карта преобразованных мешей
    SkinMap                        skin_map;          //карта преобразованных скинов
    bool                           fix_zero_tangents; //необходимо ли исправлять нулевые касательные

  public:
    /*
        Конструктор
    */

    Converter (const media::collada::Model& in_model, media::geometry::MeshLibrary& in_library, bool in_fix_zero_tangents)
      : model (in_model)
      , mesh_library (in_library)
      , fix_zero_tangents (in_fix_zero_tangents)
    {
      ConvertLibrary (model.Meshes (), &Converter::ConvertMesh);
      ConvertLibrary (model.Skins (), &Converter::ConvertSkin);
      ConvertLibrary (model.Nodes (), &Converter::ConvertNode);
    }

    /*
        Преобразование библиотеки
    */
    
    template <class T>
    void ConvertLibrary (const media::collada::ILibrary<T>& library, void (Converter::*fn)(const char* id, const T&))
    {
      for (typename media::collada::ILibrary<T>::ConstIterator i=library.CreateIterator (); i; ++i)
        (this->*fn)(library.ItemId (i), *i);
    }

    /*
        Преобразование поверхности
    */    

      //преобразование положений и нормалей
    void ConvertSurfaceVertices (const media::collada::Surface& src_surface, Surface& dst_surface)
    {
      typedef media::geometry::Vertex<media::geometry::Position3f, media::geometry::Normalf> ColladaVertex;

      size_t vertices_count = src_surface.VerticesCount ();

      media::geometry::VertexStream vs (vertices_count, media::geometry::make_vertex_declaration<ColladaVertex> ());
      
      const media::collada::Vertex* src_vertex = src_surface.Vertices ();
      ColladaVertex*                dst_vertex = vs.Data<ColladaVertex> ();

      if (!dst_vertex)
        return;

      for (size_t i=0; i<vertices_count; i++, src_vertex++, dst_vertex++)
      {
        dst_vertex->position = src_vertex->coord;
        dst_vertex->normal   = src_vertex->normal;
      }

        //добавление потока в карту вершинных потоков поверхности

      dst_surface.vertex_streams.insert_pair (MAIN_VERTEX_STREAM_NAME, vs);
    }

      //преобразование текстурных вершин
    void SetVertexTexCoord (const media::collada::TexVertex& src_vertex, media::geometry::Vertex<TexChannel<0>::Coord3f, TexChannel<0>::Tangentf, TexChannel<0>::Binormalf>& dst_vertex)
    {
      dst_vertex.texcoord0    = src_vertex.coord;
      dst_vertex.textangent0  = src_vertex.tangent;
      dst_vertex.texbinormal0 = src_vertex.binormal; 
    }

    void SetVertexTexCoord (const media::collada::TexVertex& src_vertex, media::geometry::Vertex<TexChannel<1>::Coord3f, TexChannel<1>::Tangentf, TexChannel<1>::Binormalf>& dst_vertex)
    {
      dst_vertex.texcoord1    = src_vertex.coord;
      dst_vertex.textangent1  = src_vertex.tangent;
      dst_vertex.texbinormal1 = src_vertex.binormal;       
    }

    void SetVertexTexCoord (const media::collada::TexVertex& src_vertex, media::geometry::Vertex<TexChannel<2>::Coord3f, TexChannel<2>::Tangentf, TexChannel<2>::Binormalf>& dst_vertex)
    {
      dst_vertex.texcoord2    = src_vertex.coord;
      dst_vertex.textangent2  = src_vertex.tangent;
      dst_vertex.texbinormal2 = src_vertex.binormal;       
    }

    void SetVertexTexCoord (const media::collada::TexVertex& src_vertex, media::geometry::Vertex<TexChannel<3>::Coord3f, TexChannel<3>::Tangentf, TexChannel<3>::Binormalf>& dst_vertex)
    {
      dst_vertex.texcoord3    = src_vertex.coord;
      dst_vertex.textangent3  = src_vertex.tangent;
      dst_vertex.texbinormal3 = src_vertex.binormal;       
    }

    void SetVertexTexCoord (const media::collada::TexVertex& src_vertex, media::geometry::Vertex<TexChannel<4>::Coord3f, TexChannel<4>::Tangentf, TexChannel<4>::Binormalf>& dst_vertex)
    {
      dst_vertex.texcoord4    = src_vertex.coord;
      dst_vertex.textangent4  = src_vertex.tangent;
      dst_vertex.texbinormal4 = src_vertex.binormal;       
    }

    void SetVertexTexCoord (const media::collada::TexVertex& src_vertex, media::geometry::Vertex<TexChannel<5>::Coord3f, TexChannel<5>::Tangentf, TexChannel<5>::Binormalf>& dst_vertex)
    {
      dst_vertex.texcoord5    = src_vertex.coord;
      dst_vertex.textangent5  = src_vertex.tangent;
      dst_vertex.texbinormal5 = src_vertex.binormal;       
    }

    void SetVertexTexCoord (const media::collada::TexVertex& src_vertex, media::geometry::Vertex<TexChannel<6>::Coord3f, TexChannel<6>::Tangentf, TexChannel<6>::Binormalf>& dst_vertex)
    {
      dst_vertex.texcoord6    = src_vertex.coord;
      dst_vertex.textangent6  = src_vertex.tangent;
      dst_vertex.texbinormal6 = src_vertex.binormal;       
    }

    void SetVertexTexCoord (const media::collada::TexVertex& src_vertex, media::geometry::Vertex<TexChannel<7>::Coord3f, TexChannel<7>::Tangentf, TexChannel<7>::Binormalf>& dst_vertex)
    {
      dst_vertex.texcoord7    = src_vertex.coord;
      dst_vertex.textangent7  = src_vertex.tangent;
      dst_vertex.texbinormal7 = src_vertex.binormal;       
    }
    
    template<size_t channel_index>
    media::geometry::VertexStream CreateTexVertices (const media::collada::Surface& src_surface)
    {
      typedef media::geometry::Vertex<typename TexChannel<channel_index>::Coord3f,
                                      typename TexChannel<channel_index>::Tangentf,
                                      typename TexChannel<channel_index>::Binormalf> ColladaTexVertex;

      size_t vertices_count = src_surface.VerticesCount ();

        //создание вершинного потока

      media::geometry::VertexStream vs (vertices_count, media::geometry::make_vertex_declaration<ColladaTexVertex> ());

        //преобразование данных

      const media::collada::TexVertex* src_vertex = src_surface.TexVertexChannels ().Data (channel_index);
      ColladaTexVertex*                dst_vertex = vs.Data<ColladaTexVertex> ();

      if (!dst_vertex)
        return vs; //throw!!!

      for (size_t j=0; j<vertices_count; j++, src_vertex++, dst_vertex++)
        SetVertexTexCoord (*src_vertex, *dst_vertex);
      
      return vs;
    }
    
      //преобразование текстурных вершин с цветами
    media::geometry::VertexStream CreateTexColoredVertices (const media::collada::Surface& src_surface, size_t tex_channel_index, size_t color_channel_index)
    {
      typedef media::geometry::Vertex<media::geometry::Color3f, TexChannel<0>::Coord3f, Tangent, Binormal> ColladaTexVertex;

      size_t vertices_count = src_surface.VerticesCount ();

        //создание вершинного потока
      media::geometry::VertexStream vs (vertices_count, media::geometry::make_vertex_declaration<ColladaTexVertex> ());

        //преобразование данных

      const media::collada::TexVertex* src_vertex = src_surface.TexVertexChannels ().Data (tex_channel_index);
      const math::vec3f*               src_color  = src_surface.ColorChannels ().Data (color_channel_index);
      ColladaTexVertex*                dst_vertex = vs.Data<ColladaTexVertex> ();
      
      if (!dst_vertex)
        return vs; //!!!!!throw

      for (size_t j=0; j<vertices_count; j++, src_vertex++, dst_vertex++, src_color++)
      {
        dst_vertex->color      = *src_color;
        dst_vertex->texcoord0  = src_vertex->coord;
        dst_vertex->tangent    = src_vertex->tangent;
        dst_vertex->binormal   = src_vertex->binormal;
      }
      
      return vs;
    }    
    
      //преобразование каналов текстурных вершин
    void ConvertSurfaceTexVertexChannels (const media::collada::Surface& src_surface, Surface& dst_surface)
    {
      typedef media::collada::Surface::TexVertexChannelList TexVertexChannelList;
      typedef media::collada::Surface::ColorChannelList     ColorChannelList;

      const TexVertexChannelList& tex_channels   = src_surface.TexVertexChannels ();
      const ColorChannelList&     color_channels = src_surface.ColorChannels ();

      for (size_t i=0; i<tex_channels.Size (); i++)
      {
          //определение вида потока: текстурные координаты либо текстурные координат + цвет

        int color_channel_index = color_channels.Find (tex_channels.Name (i));
        
          //преобразование вершинного потока

        media::geometry::VertexStream vs (0, media::geometry::VertexDeclaration (media::geometry::VertexFormat ()));

        if (color_channel_index != -1)
          vs = CreateTexColoredVertices (src_surface, i, color_channel_index);
        else
        {
          switch (i)
          {
            case 0:
              vs = CreateTexVertices<0> (src_surface);
              break;
            case 1:
              vs = CreateTexVertices<1> (src_surface);
              break;
            case 2:
              vs = CreateTexVertices<2> (src_surface);
              break;
            case 3:
              vs = CreateTexVertices<3> (src_surface);
              break;
            case 4:
              vs = CreateTexVertices<4> (src_surface);
              break;
            case 5:
              vs = CreateTexVertices<5> (src_surface);
              break;
            case 6:
              vs = CreateTexVertices<6> (src_surface);
              break;
            case 7:
              vs = CreateTexVertices<7> (src_surface);
              break;
            default:
              throw xtl::format_operation_exception ("media::collada::Converter::ConvertSurfaceTexVertexChannels", "Unsupported texture channel");
          }
        }

          //регистрация вершинного потока в поверхности

        dst_surface.vertex_streams.insert_pair (common::format ("texchannels.%u", tex_channels.Name (i)).c_str (), vs);
      }
    }
    
      //преобразование каналов vertex influence
    void ConvertSurfaceInfluenceChannels (const media::collada::Surface& src_surface, Surface& dst_surface)
    {
      typedef media::collada::Surface::InfluenceChannelList InfluenceChannelList;

      const InfluenceChannelList& channels       = src_surface.InfluenceChannels ();
      size_t                      vertices_count = src_surface.VerticesCount ();
      
      for (size_t i=0; i<channels.Size (); i++)
      {
          //создание вершинного потока
        
        typedef media::geometry::Vertex<media::geometry::VertexInfluence> ColladaInfluence;

        media::geometry::VertexStream vs (vertices_count, media::geometry::make_vertex_declaration<ColladaInfluence> ());

          //преобразование данных

        const media::collada::VertexInfluence* src_influence = src_surface.InfluenceChannels ().Data (i);
        ColladaInfluence*                      dst_influence = vs.Data<ColladaInfluence> ();

        if (!dst_influence)
          return;

        for (size_t j=0; j<vertices_count; j++, src_influence++, dst_influence++)
        {
          dst_influence->first_weight  = src_influence->first_weight;
          dst_influence->weights_count = src_influence->weights_count;
        }    

          //регистрация вершинного потока в поверхности
          
        dst_surface.vertex_streams.insert_pair (common::format ("influences.%s", channels.Name (i)).c_str (), vs);
      }
    }
    
      //преобразование индексов
    void ConvertSurfaceIndices (const media::collada::Surface& src_surface, media::geometry::IndexBuffer& dst_index_buffer)
    { 
      size_t indices_count             = src_surface.IndicesCount (),
             current_index_buffer_size = dst_index_buffer.Size ();
      
      dst_index_buffer.Resize (current_index_buffer_size + indices_count);
      
      if (dst_index_buffer.DataType () != media::geometry::IndexType_UInt32)
        throw xtl::format_operation_exception ("media::collada::Converter::ConvertSurfaceIndices", "Unexpected index buffer type %s", get_index_type_name (dst_index_buffer.DataType ()));

      stl::copy (src_surface.Indices (), src_surface.Indices () + indices_count, dst_index_buffer.Data<unsigned int> () + current_index_buffer_size);
    }

      //коррекция нулевых касательных
    void FixZeroTangents (Surface& dst_surface)
    {
      VertexStreamMap::iterator main_vs_iter = dst_surface.vertex_streams.find (MAIN_VERTEX_STREAM_NAME);

      if (main_vs_iter == dst_surface.vertex_streams.end ())
        return;

      const media::geometry::VertexStream& main_vs          = main_vs_iter->second;
      char*                                main_vs_data     = (char*)main_vs.Data ();
      size_t                               main_vertex_size = main_vs.VertexSize ();
      size_t                               normal_offset    = 0;

      math::vec3f ort_y (0.f, 1.f, 0.f), ort_z (0.f, 0.f, 1.f);

      const media::geometry::VertexFormat& main_vs_format = main_vs.Format ();

      for (size_t i = 0, count = main_vs_format.AttributesCount (); i < count; i++)
      {
        const media::geometry::VertexAttribute& attribute = main_vs_format.Attribute (i);

        if (attribute.semantic == media::geometry::VertexAttributeSemantic_Normal)
        {
          normal_offset = attribute.offset;
          break;
        }
      }

      for (VertexStreamMap::iterator vs_iter = dst_surface.vertex_streams.begin (), end = dst_surface.vertex_streams.end (); vs_iter != end; ++vs_iter)
      {
        if (vs_iter == main_vs_iter)
          continue;

        const media::geometry::VertexStream& vs = vs_iter->second;

        bool   tangent_found  = false;
        size_t tangent_offset = 0;

        const media::geometry::VertexFormat& vertex_format = vs.Format ();

        for (size_t i = 0, count = vertex_format.AttributesCount (); i < count; i++)
        {
          const media::geometry::VertexAttribute& attribute = vertex_format.Attribute (i);

          static const char* TANGENT_PREFIX        = "textangent";
          static size_t      TANGENT_PREFIX_LENGTH = strlen (TANGENT_PREFIX);

          if (!strncmp (attribute.name, TANGENT_PREFIX, TANGENT_PREFIX_LENGTH))
            tangent_found = true;

          if (tangent_found)
          {
            tangent_offset = attribute.offset;
            break;
          }
        }

        if (!tangent_found)
          continue;

        char*  vs_data     = (char*)vs.Data ();
        size_t vertex_size = vs.VertexSize ();

        for (size_t i = 0, count = main_vs.Size (); i < count; i++)
        {
          math::vec3f& tangent = *(math::vec3f*)(vs_data + vertex_size * i + tangent_offset);

          if (tangent.x != 0.f || tangent.y != 0.f || tangent.z != 0.f)
            continue;

          const math::vec3f& normal = *(math::vec3f*)(main_vs_data + main_vertex_size * i + normal_offset);

          if (math::equal (normal, ort_y, EPS))
            tangent = math::cross (normal, ort_z);
          else
            tangent = math::cross (normal, ort_y);
        }
      }
    }

      //преобразование поверхности
    void ConvertSurface (const media::collada::Surface& src_surface, Mesh& dst_mesh)
    {
        //создание поверхности

      SurfacePtr dst_surface (new Surface);

        //задание диапазона индексов

      dst_surface->first_index   = dst_mesh.index_buffer.Size ();
      dst_surface->indices_count = src_surface.IndicesCount ();

        //преобразование массивов поверхности

      ConvertSurfaceVertices          (src_surface, *dst_surface);
      ConvertSurfaceTexVertexChannels (src_surface, *dst_surface);
      ConvertSurfaceInfluenceChannels (src_surface, *dst_surface);
      ConvertSurfaceIndices           (src_surface, dst_mesh.index_buffer);

      if (fix_zero_tangents)
        FixZeroTangents (*dst_surface);

        //преобразование типа примитива поверхности
      
      switch (src_surface.PrimitiveType ())
      {
        case media::collada::PrimitiveType_LineList:      dst_surface->primitive_type = media::geometry::PrimitiveType_LineList;      break;
        case media::collada::PrimitiveType_LineStrip:     dst_surface->primitive_type = media::geometry::PrimitiveType_LineStrip;     break;
        case media::collada::PrimitiveType_TriangleList:  dst_surface->primitive_type = media::geometry::PrimitiveType_TriangleList;  break;
        case media::collada::PrimitiveType_TriangleStrip: dst_surface->primitive_type = media::geometry::PrimitiveType_TriangleStrip; break;
        case media::collada::PrimitiveType_TriangleFan:   dst_surface->primitive_type = media::geometry::PrimitiveType_TriangleFan;   break;
        default:                                          return;
      }
      
        //задание имени материала
        
      dst_surface->material = src_surface.Material ();

        //регистрация поверхности

      dst_mesh.surfaces.push_back (dst_surface);
    }    
    
    /*
        Преобразование меша
    */

    void ConvertMesh (const char* id, const media::collada::Mesh& src_mesh)
    {
        //создание меша

      MeshPtr dst_mesh (new Mesh);
      
        //резервирование памяти для хранения индексов
        
      size_t indices_count = 0;
      
      for (size_t i=0; i<src_mesh.Surfaces ().Size (); i++)
        indices_count += src_mesh.Surfaces () [i].IndicesCount ();
        
      dst_mesh->index_buffer.Reserve (indices_count);

        //преобразование поверхностей

      for (size_t i=0; i<src_mesh.Surfaces ().Size (); i++)
        ConvertSurface (src_mesh.Surfaces () [i], *dst_mesh);

        //регистрация меша

      mesh_map.insert_pair (id, dst_mesh);
    }    
    
    /*
        Преобразование скина
    */

    void ConvertSkin (const char* id, const media::collada::Skin& src_skin)
    {
        //определение имени базового меша

      const char*                  base_mesh_name = src_skin.BaseMesh ();
      const media::collada::Morph* morph          = model.Morphs ().Find (base_mesh_name);

      if (morph)
        base_mesh_name = morph->BaseMesh ();

        //поиск преобразованного меша

      MeshMap::iterator iter = mesh_map.find (base_mesh_name);

      if (iter == mesh_map.end ())
        return; //базовый меш не найден
        
        //создание скина
        
      SkinPtr skin (new Skin);
      
      skin->base_mesh = iter->second;
      skin->id        = id;

        //преобразование весов
        
      media::geometry::VertexWeightStream& vws           = skin->weights;
      size_t                               weights_count = src_skin.WeightsCount ();

      vws.Resize (weights_count);
      
      const media::collada::VertexJointWeight* src_weight = src_skin.Weights ();
      media::geometry::VertexWeight*           dst_weight = vws.Data ();
      
      for (size_t i=0; i<weights_count; i++, src_weight++, dst_weight++)
      {
        dst_weight->joint_index  = src_weight->joint;
        dst_weight->joint_weight = src_weight->weight;
      }

        //регистрация скина

      skin_map.insert_pair (id, skin);
    }
    
    /*
        Преобразование инстанцированного меша
    */
    
    void ConvertInstanceSurface (Surface& surface, const media::collada::MaterialBinds& binds, const char* influence_channel_name,
                                 media::geometry::VertexWeightStream* vws, media::geometry::Mesh& dst_mesh)
    {
        //получение эффекта
      
      const char*                     material_name = binds.FindMaterial (surface.material.c_str ());
      const media::collada::Material* material      = material_name ? model.Materials ().Find (material_name) : 0;

      if (!material)
        return;

      const media::collada::Effect* effect = model.Effects ().Find (material->Effect ());

      if (!effect)
        return;

        //компоновка вершинного буфера
      
      media::geometry::VertexBuffer vertex_buffer;
      
        //присоединение основного вершинного потока
        
      VertexStreamMap::iterator iter = surface.vertex_streams.find ("main");
      
      if (iter == surface.vertex_streams.end ())
        return;

      vertex_buffer.Attach (iter->second);
      
        //присоединение текстурных каналов
        
      for (int i=0; i<media::collada::TextureMap_Num; i++)
      {
          //получение имени канала текстурных координат
        
        const media::collada::Texture& texture      = effect->Texture ((media::collada::TextureMap)i);
        const char*                    channel_name = texture.TexcoordChannel ();
        
          //поиск канала текстурных координат

        if (strstr (channel_name, "CHANNEL") == channel_name)
          channel_name += strlen ("CHANNEL");
        else if (strstr (channel_name, "TEX") == channel_name)
          channel_name += strlen ("TEX");

        VertexStreamMap::iterator iter = surface.vertex_streams.find (common::format ("texchannels.%s", channel_name).c_str ());

        if (iter == surface.vertex_streams.end ())
          continue; //вершинный канал не найден

          //добавление канала
          
        vertex_buffer.Attach (iter->second);
      }
      
        //присоединение канала vertex influences
      
      if (influence_channel_name && vws)
      {
        VertexStreamMap::iterator iter = surface.vertex_streams.find (common::format ("influences.%s", influence_channel_name).c_str ());

        if (iter != surface.vertex_streams.end ())
        {
          vertex_buffer.Attach (iter->second);
          vertex_buffer.AttachWeights (*vws);
        }
      }
      
        //регистрация вершинного буфера
        
      size_t vertex_buffer_index = dst_mesh.Attach (vertex_buffer);      

        //добавление примитива
        
      dst_mesh.AddPrimitive (surface.primitive_type, vertex_buffer_index, surface.first_index,
                             media::geometry::get_primitives_count (surface.primitive_type, surface.indices_count), material_name);
    }
    
    void ConvertInstanceMesh (const char* id, const media::collada::InstanceMesh& imesh)
    {
        //поиск инстанцируемого меша
      
      MeshMap::iterator iter = mesh_map.find (imesh.Mesh ());
      
      if (iter == mesh_map.end ())
        return;                

      Mesh*                                mesh  = get_pointer (iter->second);
      const media::collada::MaterialBinds& binds = imesh.MaterialBinds ();
      
        //создание меша

      media::geometry::Mesh dst_mesh;
      
      dst_mesh.Rename (id);
      
        //присоединение индексного буфера
        
      dst_mesh.Attach (mesh->index_buffer);
      
        //преобразование поверхностей

      for (SurfaceList::iterator i=mesh->surfaces.begin (), end=mesh->surfaces.end (); i!=end; ++i)
        ConvertInstanceSurface (**i, binds, "", 0, dst_mesh);
        
        //регистрация меша в библиотеке

      mesh_library.Attach (id, dst_mesh);
    }
    
    /*
        Преобразование инстанцированного контроллера (пока только скины)
    */
    
    void ConvertInstanceController (const char* id, const media::collada::InstanceController& icontroller)
    {
        //поиск инстанцируемого меша

      SkinMap::iterator iter = skin_map.find (icontroller.Controller ());

      if (iter == skin_map.end ())
        return;

      Skin*                                skin      = get_pointer (iter->second);
      Mesh*                                base_mesh = get_pointer (skin->base_mesh);
      const media::collada::MaterialBinds& binds     = icontroller.MaterialBinds ();

        //создание меша

      media::geometry::Mesh dst_mesh;

      dst_mesh.Rename (id);

        //присоединение индексного буфера

      dst_mesh.Attach (base_mesh->index_buffer);

        //преобразование поверхностей
        
      const char*                          skin_id = skin->id.c_str ();
      media::geometry::VertexWeightStream* vws     = &skin->weights;

      for (SurfaceList::iterator i=base_mesh->surfaces.begin (), end=base_mesh->surfaces.end (); i!=end; ++i)
        ConvertInstanceSurface (**i, binds, skin_id, vws, dst_mesh);

        //регистрация меша в библиотеке
        
      mesh_library.Attach (id, dst_mesh);
    }    
    
    /*
        Обход узлов коллада-сцены: инстанцирование геометрии
    */

    void ConvertNode (const char* id, const media::collada::Node& src_node)
    {
      size_t index = 0;

      for (media::collada::Node::MeshList::ConstIterator i=src_node.Meshes ().CreateIterator (); i; ++i, ++index)
        ConvertInstanceMesh (common::format ("%s.mesh#%u", id, index).c_str (), *i);
        
      index = 0;
        
      for (media::collada::Node::ControllerList::ConstIterator i=src_node.Controllers ().CreateIterator (); i; ++i, ++index)
        ConvertInstanceController (common::format ("%s.controller#%u", id, index).c_str (), *i);
    }
};

}

namespace media
{

namespace collada
{

//преобразование коллада-модели в библиотеку мешей
void convert (const media::collada::Model& src_model, geometry::MeshLibrary& dst_library, bool fix_zero_tangents)
{
  Converter (src_model, dst_library, fix_zero_tangents);
}

}

}
