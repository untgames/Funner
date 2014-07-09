#include "shared.h"

using namespace render::scene::server;
using namespace render::scene;

/*
    Описание реализации состояния соединения
*/

struct ConnectionState::Impl
{
  ServerImpl&                        server;             //ссылка на сервер
  server::Context*                   context;            //контекст
  interchange::PropertyMapAutoWriter properties_writer;  //синхронизатор свойств (запись на клиент)
  interchange::PropertyMapReader     properties_reader;  //синхронизатор свойств (чтение с клиента)
  ResourceClient                     resource_client;    //клиент менеджера ресурсов

/// Конструктор
  Impl (ServerImpl& in_server)
    : server (in_server)
    , context ()
    , resource_client (server.ResourceManager ().CreateClient ())
  {
  }  

/// Получение контекста
  server::Context& Context ()
  {
    if (!context)
      throw xtl::format_operation_exception ("render::server::ConnectionState::Impl::Context", "Context is null");

    return *context;
  }
};

/*
    Конструктор / деструктор
*/

ConnectionState::ConnectionState (ServerImpl& server)
{
  try
  {
    impl.reset (new Impl (server));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::ConnectionState::ConnectionState");
    throw;
  }
}

ConnectionState::~ConnectionState ()
{
}

/*
    Установка контекста
*/

void ConnectionState::SetContext (Context* context)
{
  impl->context = context;
}

/*
    Синхронизация свойств
*/

PropertyMapSynchronizer ConnectionState::CreateSynchronizer (const common::PropertyMap& properties)
{
  try
  {
    return impl->properties_writer.CreateSynchronizer (properties);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::ClientState::CreateSynchronizer");
    throw;
  }
}

common::PropertyMap ConnectionState::GetClientProperties (object_id_t id)
{
  try
  {
    return impl->properties_reader.GetProperties (id);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::ClientState::GetClientProperties");
    throw;
  }
}

void ConnectionState::SynchronizeProperties ()
{
  try
  {
    impl->properties_writer.Write (impl->Context ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::ClientState::SynchronizeProperties");
    throw;
  }  
}

/*
    Команды клиента: окна
*/

void ConnectionState::OnWindowAttached (size_t id, const char* name, const char* init_string, void* handle, size_t width, size_t height, size_t left, size_t top, size_t right, size_t bottom)
{
  try
  {
    impl->server.WindowManager ().AttachWindow (id, name, init_string, handle, width, height, manager::Rect (int (left), int (top), right - left, bottom - top));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::OnWindowAttached");
    throw;
  }
}

void ConnectionState::OnWindowDetached (size_t id)
{
  try
  {
    impl->server.WindowManager ().DetachWindow (id);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::OnWindowDetached");
    throw;
  }
}

void ConnectionState::OnWindowSizeChanged (size_t id, size_t width, size_t height)
{
  try
  {
    impl->server.WindowManager ().SetWindowSize (id, width, height);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::OnWindowSizeChanged");
    throw;
  }
}

void ConnectionState::OnWindowViewportChanged (size_t id, size_t left, size_t top, size_t right, size_t bottom)
{
  try
  {
    impl->server.WindowManager ().SetWindowViewport (id, manager::Rect (int (left), int (top), right - left, bottom - top));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::OnWindowViewportChanged");
    throw;
  }
}

void ConnectionState::OnWindowHandleChanged (size_t id, void* handle)
{
  try
  {
    impl->server.WindowManager ().SetWindowHandle (id, handle);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::OnWindowHandleChanged");
    throw;
  }
}

void ConnectionState::OnWindowPaint (size_t id)
{
  try
  {
    impl->server.WindowManager ().RepaintWindow (id);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::OnWindowPaint");
    throw;
  }
}

/*
    Команды клиента: свойства
*/

void ConnectionState::UpdatePropertyMap (interchange::InputStream& stream)
{
  try
  {
    impl->properties_reader.Read (stream);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::UpdatePropertyMap");
    throw;
  }
}

void ConnectionState::RemovePropertyMap (object_id_t id)
{
  try
  {
    impl->properties_reader.RemoveProperties (id);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::RemovePropertyMap");
    throw;
  }
}

void ConnectionState::RemovePropertyLayout (object_id_t id)
{
  try
  {
    impl->properties_reader.RemoveLayout (id);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::RemovePropertyLayout");
    throw;
  }
}

/*
    Команды клиента: работа с ресурсами
*/

void ConnectionState::LoadResource (const char* name)
{
  try
  {
    impl->resource_client.LoadResource (name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::LoadResource");
    throw;
  }
}

void ConnectionState::UnloadResource (const char* name)
{
  try
  {
    impl->resource_client.UnloadResource (name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::UnloadResource");
    throw;
  }
}

/*
    Команды клиента: максимальная глубина рендеринга
*/

void ConnectionState::SetMaxDrawDepth (uint32 depth)
{
  try
  {
    impl->server.ScreenManager ().SetMaxDrawDepth (depth);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetMaxDrawDepth");
    throw;
  }
}

/*
    Работа с ресурсами
*/

void ConnectionState::CreateTexture (const char* texture_name, const media::Image& image, interchange::TextureDimension in_dimension, bool8 create_mips)
{
  try
  {
    render::manager::TextureDimension dimension;

    switch (in_dimension)
    {
      case interchange::TextureDimension_2D:      dimension = manager::TextureDimension_2D; break;
      case interchange::TextureDimension_3D:      dimension = manager::TextureDimension_3D; break;
      case interchange::TextureDimension_Cubemap: dimension = manager::TextureDimension_Cubemap; break;
      default:                                    throw xtl::make_argument_exception ("", "dimension", in_dimension);
    }

    impl->server.MaterialManager ().CreateTexture (texture_name, image, dimension, create_mips != 0);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::CreateTexture");
    throw;
  }
}

void ConnectionState::UpdateTexture (const char* texture_name, const media::Image& image)
{
  try
  {
    impl->server.MaterialManager ().UpdateTexture (texture_name, image);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::UpdateTexture");
    throw;
  }
}

void ConnectionState::RemoveTexture (const char* texture_name)
{
  try
  {
    impl->server.MaterialManager ().RemoveTexture (texture_name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::RemoveTexture");
    throw;
  }
}

void ConnectionState::CloneMaterial (const char* material_name, const char* prototype_name)
{
  try
  {
    impl->server.MaterialManager ().CloneMaterial (material_name, prototype_name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::CloneMaterial");
    throw;
  }
}

void ConnectionState::SetMaterialTexmapImage (const char* material_name, const char* semantic, const char* image_name)
{
  try
  {
    impl->server.MaterialManager ().SetMaterialTexmapImage (material_name, semantic, image_name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetMaterialTexmapImage");
    throw;
  }
}

void ConnectionState::SetMaterialTexmapSampler (const char* material_name, const char* semantic, const char* sampler)
{
  try
  {
    impl->server.MaterialManager ().SetMaterialTexmapSampler (material_name, semantic, sampler);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetMaterialTexmapSampler");
    throw;
  }
}

void ConnectionState::RemoveMaterial (const char* material_name)
{
  try
  {
    impl->server.MaterialManager ().RemoveMaterial (material_name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::RemoveMaterial");
    throw;
  }
}

/*
    Команды клиента: области вывода и цели рендеринга
*/

void ConnectionState::SetViewportArea (object_id_t id, int32 left, int32 top, int32 width, int32 height, float32 min_depth, float32 max_depth)
{
  try
  {
    Viewport& viewport = impl->server.ScreenManager ().GetViewport (id);

    viewport.SetArea     (Rect (left, top, width, height));
    viewport.SetMinDepth (min_depth);
    viewport.SetMaxDepth (max_depth);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetViewportArea");
    throw;
  }
}

void ConnectionState::SetViewportZOrder (object_id_t id, int32 zorder)
{
  try
  {
    impl->server.ScreenManager ().GetViewport (id).SetZOrder (zorder);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetViewportZOrder");
    throw;
  }
}

void ConnectionState::SetViewportActive (object_id_t id, bool8 is_active)
{
  try
  {
    impl->server.ScreenManager ().GetViewport (id).SetActive (is_active != 0);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetViewportActive");
    throw;
  }
}

void ConnectionState::SetViewportName (object_id_t id, const char* name)
{
  try
  {
    impl->server.ScreenManager ().GetViewport (id).SetName (name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetViewportName");
    throw;
  }
}

void ConnectionState::SetViewportTechnique (object_id_t id, const char* name)
{
  try
  {
    impl->server.ScreenManager ().GetViewport (id).SetTechnique (name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetViewportTechnique");
    throw;
  }
}

void ConnectionState::SetViewportBackground (object_id_t id, bool8 state, const math::vec4f& color)
{
  try
  {
    impl->server.ScreenManager ().GetViewport (id).SetBackground (state != 0, color);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetViewportBackground");
    throw;
  }
}

void ConnectionState::SetViewportProperties (object_id_t id, object_id_t properties_id)
{
  try
  {
    impl->server.ScreenManager ().GetViewport (id).SetProperties (GetClientProperties (properties_id));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetViewportProperties");
    throw;
  }
}

void ConnectionState::SetViewportScene (object_id_t id, object_id_t scene_id)
{
  try
  {
    if (scene_id)
    {
      Scene& scene = impl->server.SceneManager ().GetScene (scene_id);

      impl->server.ScreenManager ().GetViewport (id).SetScene (&scene);
    }
    else impl->server.ScreenManager ().GetViewport (id).SetScene (0);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetViewportScene");
    throw;
  }
}

void ConnectionState::SetViewportCameraWorldMatrix (object_id_t id, const math::mat4f& tm)
{
  try
  {
    Camera* camera = impl->server.ScreenManager ().GetViewport (id).Camera ();

    if (!camera)
      throw xtl::format_operation_exception ("", "Camera has not been set");

    camera->SetWorldMatrix (tm);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetViewportCameraWorldMatrix");
    throw;
  }
}

void ConnectionState::SetViewportCameraProjectionMatrix (object_id_t id, const math::mat4f& tm)
{
  try
  {
    Camera* camera = impl->server.ScreenManager ().GetViewport (id).Camera ();

    if (!camera)
      throw xtl::format_operation_exception ("", "Camera has not been set");

    camera->SetProjectionMatrix (tm);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetViewportCameraProjectionMatrix");
    throw;
  }
}

void ConnectionState::SetViewportCameraName (object_id_t id, const char* name)
{
  try
  {
    Camera* camera = impl->server.ScreenManager ().GetViewport (id).Camera ();

    if (!camera)
      throw xtl::format_operation_exception ("", "Camera has not been set");

    camera->SetName (name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetViewportCameraName");
    throw;
  }
}

void ConnectionState::CreateRenderTarget (object_id_t id, const char* name, const char* init_string)
{
  try
  {
    impl->server.ScreenManager ().CreateScreen (id, name, init_string);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::CreateRenderTarget");
    throw;
  }
}

void ConnectionState::DestroyRenderTarget (object_id_t id)
{
  try
  {
    impl->server.ScreenManager ().RemoveScreen (id);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::DestroyRenderTarget");
    throw;
  }
}

void ConnectionState::SetRenderTargetActive (object_id_t id, bool8 active)
{
  try
  {
    impl->server.ScreenManager ().GetScreen (id).SetActive (active != 0);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetRenderTargetActive");
    throw;
  }
}

void ConnectionState::SetRenderTargetScreenArea (object_id_t id, int32 left, int32 top, int32 width, int32 height)
{
  try
  {
    impl->server.ScreenManager ().GetScreen (id).SetArea (Rect (left, top, width, height));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetRenderTargetScreenArea");
    throw;
  }
}

void ConnectionState::SetRenderTargetBackground (object_id_t id, bool8 state, const math::vec4f& color)
{
  try
  {
    impl->server.ScreenManager ().GetScreen (id).SetBackground (state != 0, color);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetRenderTargetBackground");
    throw;
  }
}

void ConnectionState::AttachViewportToRenderTarget (object_id_t render_target_id, object_id_t viewport_id)
{
  try
  {
    impl->server.ScreenManager ().GetScreen (render_target_id).AttachViewport (viewport_id);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::AttachViewportToRenderTarget");
    throw;
  }
}

void ConnectionState::DetachViewportFromRenderTarget (object_id_t render_target_id, object_id_t viewport_id)
{
  try
  {
    impl->server.ScreenManager ().GetScreen (render_target_id).DetachViewport (viewport_id);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::DetachViewportFromRenderTarget");
    throw;
  }
}

void ConnectionState::UpdateRenderTarget (object_id_t id)
{
  try
  {
    impl->server.ScreenManager ().GetScreen (id).Update ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::UpdateRenderTarget");
    throw;
  }
}

/*
    Работа со сценой
*/

void ConnectionState::CreateScene (object_id_t id)
{
  try
  {
    impl->server.SceneManager ().CreateScene (id);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::CreateScene");
    throw;
  }
}

void ConnectionState::DestroyScene (object_id_t id)
{
  try
  {
    impl->server.SceneManager ().RemoveScene (id);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::DestroyScene");
    throw;
  }
}

void ConnectionState::SetSceneName (object_id_t id, const char* name)
{
  try
  {
    impl->server.SceneManager ().GetScene (id).SetName (name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetSceneName");
    throw;
  }
}

void ConnectionState::SetSceneNodes (object_id_t id, const interchange::RawArray<object_id_t>& nodes)
{
  try
  {
    SceneManager& scene_manager = impl->server.SceneManager ();

    Scene* scene = id ? &scene_manager.GetScene (id) : (Scene*)0;

    const object_id_t* node_id = nodes.data;

    for (size_t i=0, count=nodes.size; i<count; i++, node_id++)
    {
      Node& node = scene_manager.GetNode (*node_id);

      node.Cast<Entity> ().SetSceneOwner (scene);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetSceneNodes");
    throw;
  }
}

/*
    Работа с узлами
*/

void ConnectionState::CreateNode (object_id_t id, interchange::NodeType type)
{
  try
  {
    impl->server.SceneManager ().CreateNode (id, type);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::CreateNode");
    throw;
  }
}

void ConnectionState::DestroyNode (object_id_t id)
{
  try
  {
    impl->server.SceneManager ().RemoveNode (id);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::DestroyNode");
    throw;
  }
}

void ConnectionState::SetNodeName (object_id_t id, const char* name)
{
  try
  {
    impl->server.SceneManager ().GetNode (id).SetName (name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetNodeName");
    throw;
  }
}

void ConnectionState::SetNodeWorldMatrix (object_id_t id, const math::mat4f& tm)
{
  try
  {
    impl->server.SceneManager ().GetNode (id).SetWorldMatrix (tm);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetNodeWorldMatrix");
    throw;
  }
}

void ConnectionState::SetNodeScene (object_id_t id, object_id_t scene_id)
{
  try
  {
    SetSceneNodes (scene_id, interchange::RawArray<object_id_t> (&id, 1));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetNodeScene");
    throw;
  }
}

void ConnectionState::SetEntityBounds (object_id_t id, bool is_infinite, const bound_volumes::aaboxf& box)
{
  try
  {
    impl->server.SceneManager ().GetNode (id).Cast<Entity> ().SetBounds (is_infinite, box);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetEntityBounds");
    throw;
  }
}

void ConnectionState::SetEntityVisibility (object_id_t id, bool is_visible)
{
  try
  {
    impl->server.SceneManager ().GetNode (id).Cast<Entity> ().SetVisible (is_visible);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetEntityVisibility");
    throw;
  }
}

void ConnectionState::SetVisualModelScissor (object_id_t id, object_id_t scissor_id)
{
  try
  {
    impl->server.SceneManager ().GetNode (id).Cast<VisualModel> ().SetScissor (scissor_id ? &impl->server.SceneManager ().GetNode (scissor_id).Cast<Scissor> () : static_cast<Scissor*> (0));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetVisualModelScissor");
    throw;
  }
}

void ConnectionState::SetVisualModelDynamicShaderProperties (object_id_t id, object_id_t properties_id)
{
  try
  {
    impl->server.SceneManager ().GetNode (id).Cast<VisualModel> ().SetDynamicShaderProperties (GetClientProperties (properties_id));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetVisualModelDynamicShaderProperties");
    throw;
  }
}

void ConnectionState::SetVisualModelStaticShaderProperties (object_id_t id, object_id_t properties_id)
{
  try
  {
    impl->server.SceneManager ().GetNode (id).Cast<VisualModel> ().SetStaticShaderProperties (GetClientProperties (properties_id));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetVisualModelStaticShaderProperties");
    throw;
  }
}

void ConnectionState::SetStaticMeshName (object_id_t id, const char* name)
{
  try
  {
    impl->server.SceneManager ().GetNode (id).Cast<StaticMesh> ().SetMeshName (name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetStaticMeshName");
    throw;
  }
}

void ConnectionState::SetLightParams (object_id_t id, const interchange::LightParams& params)
{
  try
  {
    impl->server.SceneManager ().GetNode (id).Cast<Light> ().SetParams (params);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetLightParams");
    throw;
  }
}

void ConnectionState::SetPageCurlParams (object_id_t id, const interchange::PageCurlParams& params, const char* front_left_material,
                                         const char* front_right_material, const char* back_left_material, const char* back_right_material)
{
  try
  {
    impl->server.SceneManager ().GetNode (id).Cast<PageCurl> ().SetParams (params, front_left_material, front_right_material,
                                                                           back_left_material, back_right_material);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetLightParams");
    throw;
  }
}

void ConnectionState::CreateSpriteList (object_id_t id, uint32 list_subid, interchange::SpriteMode mode, interchange::PrimitiveUsage usage, const math::vec3f& up, const char* batch)
{
  try
  {
    impl->server.SceneManager ().GetNode (id).Cast<SpriteList> ().CreateList (list_subid, mode, usage, up, batch);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::CreateSpriteList");
    throw;
  }
}

void ConnectionState::RemoveSpriteList (object_id_t id, uint32 list_subid)
{
  try
  {
    impl->server.SceneManager ().GetNode (id).Cast<SpriteList> ().RemoveList (list_subid);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::RemoveSpriteList");
    throw;
  }
}

void ConnectionState::ReserveSpriteLists (object_id_t id, uint32 count)
{
  try
  {
    impl->server.SceneManager ().GetNode (id).Cast<SpriteList> ().ReserveLists (count);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::ReserveSpriteList");
    throw;
  }
}

void ConnectionState::SetSpriteListMaterial (object_id_t id, uint32 list_subid, const char* material)
{
  try
  {
    impl->server.SceneManager ().GetNode (id).Cast<SpriteList> ().SetMaterial (list_subid, material);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetSpriteListMaterial");
    throw;
  }
}

void ConnectionState::SetSpriteListBuffer (object_id_t id, uint32 list_subid, uint32 count, uint32 reserve_count)
{
  try
  {
    SpriteList& list = impl->server.SceneManager ().GetNode (id).Cast<SpriteList> ();

    list.Reserve (list_subid, reserve_count);
    list.Resize (list_subid, count);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetSpriteListBuffer");
    throw;
  }
}

void ConnectionState::SetSpriteListDescs (object_id_t id, uint32 list_subid, uint32 first, interchange::RawArray<interchange::SpriteDesc> descs)
{
  try
  {
    impl->server.SceneManager ().GetNode (id).Cast<SpriteList> ().SetDescs (list_subid, first, descs.size, descs.data);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetSpriteListDescs");
    throw;
  }
}

void ConnectionState::SetLineListParams (object_id_t id, interchange::PrimitiveUsage usage, const char* batch)
{
  try
  {
    impl->server.SceneManager ().GetNode (id).Cast<LineList> ().SetParams (usage, batch);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetLineListParams");
    throw;
  }
}

void ConnectionState::SetLineListMaterial (object_id_t id, const char* material)
{
  try
  {
    impl->server.SceneManager ().GetNode (id).Cast<LineList> ().SetMaterial (material);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetLineListMaterial");
    throw;
  }
}

void ConnectionState::SetLineListBuffer (object_id_t id, uint32 count, uint32 reserve_count)
{
  try
  {
    LineList& list = impl->server.SceneManager ().GetNode (id).Cast<LineList> ();

    list.Reserve (reserve_count);
    list.Resize (count);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetLineListBuffer");
    throw;
  }
}

void ConnectionState::SetLineListDescs (object_id_t id, uint32 first, interchange::RawArray<interchange::LineDesc> descs)
{
  try
  {
    impl->server.SceneManager ().GetNode (id).Cast<LineList> ().SetDescs (first, descs.size, descs.data);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetLineListDescs");
    throw;
  }
}
