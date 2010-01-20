#include "shared.h"

namespace
{

const size_t SURFACES_RESERVE_SIZE = 8; //резервируемое количество поверхностей

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
    const media::collada::Model&   model;        //исходная коллада-модель
    media::geometry::MeshLibrary&  mesh_library; //результирующая библиотека мешей
    MeshMap                        mesh_map;     //карта преобразованных мешей
    SkinMap                        skin_map;     //карта преобразованных скинов

  public:
    /*
        Конструктор
    */

    Converter (const media::collada::Model& in_model, media::geometry::MeshLibrary& in_library) : model (in_model), mesh_library (in_library)
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

      dst_surface.vertex_streams.insert_pair ("main", vs);
    }

      //преобразование текстурных вершин
    media::geometry::VertexStream CreateTexVertices (const media::collada::Surface& src_surface, size_t channel_index)
    {
      typedef media::geometry::Vertex<media::geometry::TexChannel<0>::Coord3f, media::geometry::Tangentf,
                                      media::geometry::Binormalf> ColladaTexVertex;

      size_t vertices_count = src_surface.VerticesCount ();

        //создание вершинного потока

      media::geometry::VertexStream vs (vertices_count, media::geometry::make_vertex_declaration<ColladaTexVertex> ());

        //преобразование данных

      const media::collada::TexVertex* src_vertex = src_surface.TexVertexChannels ().Data (channel_index);
      ColladaTexVertex*                dst_vertex = vs.Data<ColladaTexVertex> ();

      if (!dst_vertex)
        return vs; //throw!!!

      for (size_t j=0; j<vertices_count; j++, src_vertex++, dst_vertex++)
      {
        dst_vertex->texcoord0 = src_vertex->coord;
        dst_vertex->tangent   = src_vertex->tangent;
        dst_vertex->binormal  = src_vertex->binormal;
      }
      
      return vs;
    }
    
      //преобразование текстурных вершин с цветами
    media::geometry::VertexStream CreateTexColoredVertices (const media::collada::Surface& src_surface, size_t tex_channel_index, size_t color_channel_index)
    {
      typedef media::geometry::Vertex<media::geometry::Color3f, media::geometry::TexChannel<0>::Coord3f,
                                      media::geometry::Tangentf, media::geometry::Binormalf> ColladaTexVertex;

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
        
        media::geometry::VertexStream vs = color_channel_index != -1 ? CreateTexColoredVertices (src_surface, i, color_channel_index) :
                                           CreateTexVertices (src_surface, i);

          //регистрация вершинного потока в поверхности

        dst_surface.vertex_streams.insert_pair (common::format ("texchannels.%s", tex_channels.Name (i)).c_str (), vs);
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

      stl::copy (src_surface.Indices (), src_surface.Indices () + indices_count, dst_index_buffer.Data () + current_index_buffer_size);
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
void convert (const media::collada::Model& src_model, geometry::MeshLibrary& dst_library)
{
  Converter (src_model, dst_library);
}

}

}
