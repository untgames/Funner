inline stl::string get_command_name(CommandId command_id)
{
  switch (command_id)
  {
    case CommandId_FenceRequest: return "FenceRequest";
    case CommandId_LoadResource: return "LoadResource";
    case CommandId_UnloadResource: return "UnloadResource";
    case CommandId_SetMaxDrawDepth: return "SetMaxDrawDepth";
    case CommandId_CreateTexture: return "CreateTexture";
    case CommandId_UpdateTexture: return "UpdateTexture";
    case CommandId_RemoveTexture: return "RemoveTexture";
    case CommandId_CloneMaterial: return "CloneMaterial";
    case CommandId_SetMaterialTexmapImage: return "SetMaterialTexmapImage";
    case CommandId_SetMaterialTexmapSampler: return "SetMaterialTexmapSampler";
    case CommandId_RemoveMaterial: return "RemoveMaterial";
    case CommandId_SetViewportArea: return "SetViewportArea";
    case CommandId_SetViewportZOrder: return "SetViewportZOrder";
    case CommandId_SetViewportActive: return "SetViewportActive";
    case CommandId_SetViewportName: return "SetViewportName";
    case CommandId_SetViewportTechnique: return "SetViewportTechnique";
    case CommandId_SetViewportBackground: return "SetViewportBackground";
    case CommandId_SetViewportProperties: return "SetViewportProperties";
    case CommandId_SetViewportScene: return "SetViewportScene";
    case CommandId_SetViewportCameraWorldMatrix: return "SetViewportCameraWorldMatrix";
    case CommandId_SetViewportCameraProjectionMatrix: return "SetViewportCameraProjectionMatrix";
    case CommandId_SetViewportCameraName: return "SetViewportCameraName";
    case CommandId_CreateRenderTarget: return "CreateRenderTarget";
    case CommandId_DestroyRenderTarget: return "DestroyRenderTarget";
    case CommandId_SetRenderTargetActive: return "SetRenderTargetActive";
    case CommandId_SetRenderTargetScreenArea: return "SetRenderTargetScreenArea";
    case CommandId_SetRenderTargetBackground: return "SetRenderTargetBackground";
    case CommandId_AttachViewportToRenderTarget: return "AttachViewportToRenderTarget";
    case CommandId_DetachViewportFromRenderTarget: return "DetachViewportFromRenderTarget";
    case CommandId_UpdateRenderTarget: return "UpdateRenderTarget";
    case CommandId_UpdatePropertyMap: return "UpdatePropertyMap";
    case CommandId_RemovePropertyMap: return "RemovePropertyMap";
    case CommandId_RemovePropertyLayout: return "RemovePropertyLayout";
    case CommandId_CreateScene: return "CreateScene";
    case CommandId_DestroyScene: return "DestroyScene";
    case CommandId_SetSceneName: return "SetSceneName";
    case CommandId_SetSceneNodes: return "SetSceneNodes";
    case CommandId_CreateNode: return "CreateNode";
    case CommandId_DestroyNode: return "DestroyNode";
    case CommandId_SetNodeName: return "SetNodeName";
    case CommandId_SetNodeWorldMatrix: return "SetNodeWorldMatrix";
    case CommandId_SetNodeScene: return "SetNodeScene";
    case CommandId_SetEntityBounds: return "SetEntityBounds";
    case CommandId_SetEntityVisibility: return "SetEntityVisibility";
    case CommandId_SetVisualModelScissor: return "SetVisualModelScissor";
    case CommandId_SetVisualModelDynamicShaderProperties: return "SetVisualModelDynamicShaderProperties";
    case CommandId_SetVisualModelStaticShaderProperties: return "SetVisualModelStaticShaderProperties";
    case CommandId_SetStaticMeshName: return "SetStaticMeshName";
    case CommandId_SetSkinMeshJointsCount: return "SetSkinMeshJointsCount";
    case CommandId_SetSkinMeshJointNodes: return "SetSkinMeshJointNodes";
    case CommandId_SetSkinMeshJointMatrices: return "SetSkinMeshJointMatrices";
    case CommandId_SetLightParams: return "SetLightParams";
    case CommandId_SetPageCurlParams: return "SetPageCurlParams";
    case CommandId_SetParticleEmitterSystemId: return "SetParticleEmitterSystemId";
    case CommandId_UpdateParticleEmitterSystem: return "UpdateParticleEmitterSystem";
    case CommandId_ReserveSpriteLists: return "ReserveSpriteLists";
    case CommandId_CreateSpriteList: return "CreateSpriteList";
    case CommandId_RemoveSpriteList: return "RemoveSpriteList";
    case CommandId_SetSpriteListMaterial: return "SetSpriteListMaterial";
    case CommandId_SetSpriteListBuffer: return "SetSpriteListBuffer";
    case CommandId_SetSpriteListDescs: return "SetSpriteListDescs";
    case CommandId_SetLineListParams: return "SetLineListParams";
    case CommandId_SetLineListMaterial: return "SetLineListMaterial";
    case CommandId_SetLineListBuffer: return "SetLineListBuffer";
    case CommandId_SetLineListDescs: return "SetLineListDescs";
    case CommandId_FenceResponse: return "FenceResponse";
    default: return common::format ("CommandId#%u", command_id);
  }
}

/*
    Client to server
*/

inline void ClientToServerSerializer::FenceRequest(object_id_t tag)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_FenceRequest);
    write(*this, tag);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::LoadResource(const char* name)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_LoadResource);
    write(*this, name);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::UnloadResource(const char* name)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_UnloadResource);
    write(*this, name);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::SetMaxDrawDepth(uint32 depth)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_SetMaxDrawDepth);
    write(*this, depth);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::CreateTexture(const char* texture_name, media::Image image, TextureDimension dimension, bool8 create_mipmaps)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_CreateTexture);
    write(*this, texture_name);
    write(*this, image);
    write(*this, dimension);
    write(*this, create_mipmaps);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::UpdateTexture(const char* texture_name, media::Image image)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_UpdateTexture);
    write(*this, texture_name);
    write(*this, image);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::RemoveTexture(const char* texture_name)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_RemoveTexture);
    write(*this, texture_name);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::CloneMaterial(const char* material_name, const char* prototype_name)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_CloneMaterial);
    write(*this, material_name);
    write(*this, prototype_name);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::SetMaterialTexmapImage(const char* material_name, const char* semantic, const char* image_name)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_SetMaterialTexmapImage);
    write(*this, material_name);
    write(*this, semantic);
    write(*this, image_name);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::SetMaterialTexmapSampler(const char* material_name, const char* semantic, const char* sampler)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_SetMaterialTexmapSampler);
    write(*this, material_name);
    write(*this, semantic);
    write(*this, sampler);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::RemoveMaterial(const char* material_name)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_RemoveMaterial);
    write(*this, material_name);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::SetViewportArea(object_id_t id, int32 left, int32 top, int32 width, int32 height, float32 min_depth, float32 max_depth)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_SetViewportArea);
    write(*this, id);
    write(*this, left);
    write(*this, top);
    write(*this, width);
    write(*this, height);
    write(*this, min_depth);
    write(*this, max_depth);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::SetViewportZOrder(object_id_t id, int32 zorder)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_SetViewportZOrder);
    write(*this, id);
    write(*this, zorder);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::SetViewportActive(object_id_t id, bool8 is_active)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_SetViewportActive);
    write(*this, id);
    write(*this, is_active);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::SetViewportName(object_id_t id, const char* name)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_SetViewportName);
    write(*this, id);
    write(*this, name);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::SetViewportTechnique(object_id_t id, const char* name)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_SetViewportTechnique);
    write(*this, id);
    write(*this, name);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::SetViewportBackground(object_id_t id, bool8 state, const math::vec4f& color)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_SetViewportBackground);
    write(*this, id);
    write(*this, state);
    write(*this, color);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::SetViewportProperties(object_id_t id, uint64 properties_id)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_SetViewportProperties);
    write(*this, id);
    write(*this, properties_id);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::SetViewportScene(object_id_t id, object_id_t scene_id)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_SetViewportScene);
    write(*this, id);
    write(*this, scene_id);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::SetViewportCameraWorldMatrix(object_id_t id, const math::mat4f& tm)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_SetViewportCameraWorldMatrix);
    write(*this, id);
    write(*this, tm);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::SetViewportCameraProjectionMatrix(object_id_t id, const math::mat4f& tm)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_SetViewportCameraProjectionMatrix);
    write(*this, id);
    write(*this, tm);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::SetViewportCameraName(object_id_t id, const char* name)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_SetViewportCameraName);
    write(*this, id);
    write(*this, name);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::CreateRenderTarget(object_id_t id, const char* name, const char* init_string)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_CreateRenderTarget);
    write(*this, id);
    write(*this, name);
    write(*this, init_string);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::DestroyRenderTarget(object_id_t id)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_DestroyRenderTarget);
    write(*this, id);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::SetRenderTargetActive(object_id_t id, bool8 active)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_SetRenderTargetActive);
    write(*this, id);
    write(*this, active);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::SetRenderTargetScreenArea(object_id_t id, int32 left, int32 top, int32 width, int32 height)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_SetRenderTargetScreenArea);
    write(*this, id);
    write(*this, left);
    write(*this, top);
    write(*this, width);
    write(*this, height);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::SetRenderTargetBackground(object_id_t id, bool8 state, const math::vec4f& color)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_SetRenderTargetBackground);
    write(*this, id);
    write(*this, state);
    write(*this, color);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::AttachViewportToRenderTarget(object_id_t id, object_id_t viewport_id)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_AttachViewportToRenderTarget);
    write(*this, id);
    write(*this, viewport_id);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::DetachViewportFromRenderTarget(object_id_t id, object_id_t viewport_id)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_DetachViewportFromRenderTarget);
    write(*this, id);
    write(*this, viewport_id);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::UpdateRenderTarget(object_id_t id)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_UpdateRenderTarget);
    write(*this, id);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline OutputStream& ClientToServerSerializer::UpdatePropertyMap()
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_UpdatePropertyMap);
    return *this;
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::RemovePropertyMap(object_id_t id)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_RemovePropertyMap);
    write(*this, id);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::RemovePropertyLayout(object_id_t id)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_RemovePropertyLayout);
    write(*this, id);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::CreateScene(object_id_t id)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_CreateScene);
    write(*this, id);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::DestroyScene(object_id_t id)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_DestroyScene);
    write(*this, id);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::SetSceneName(object_id_t id, const char* name)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_SetSceneName);
    write(*this, id);
    write(*this, name);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::SetSceneNodes(object_id_t id, RawArray<object_id_t> nodes)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_SetSceneNodes);
    write(*this, id);
    write(*this, nodes);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::CreateNode(object_id_t id, NodeType type)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_CreateNode);
    write(*this, id);
    write(*this, type);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::DestroyNode(object_id_t id)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_DestroyNode);
    write(*this, id);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::SetNodeName(object_id_t id, const char* name)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_SetNodeName);
    write(*this, id);
    write(*this, name);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::SetNodeWorldMatrix(object_id_t id, const math::mat4f& tm)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_SetNodeWorldMatrix);
    write(*this, id);
    write(*this, tm);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::SetNodeScene(object_id_t id, object_id_t scene_id)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_SetNodeScene);
    write(*this, id);
    write(*this, scene_id);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::SetEntityBounds(object_id_t id, bool is_infinite, const bound_volumes::aaboxf& box)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_SetEntityBounds);
    write(*this, id);
    write(*this, is_infinite);
    write(*this, box);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::SetEntityVisibility(object_id_t id, bool state)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_SetEntityVisibility);
    write(*this, id);
    write(*this, state);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::SetVisualModelScissor(object_id_t id, object_id_t scissor_id)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_SetVisualModelScissor);
    write(*this, id);
    write(*this, scissor_id);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::SetVisualModelDynamicShaderProperties(object_id_t id, object_id_t properties_id)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_SetVisualModelDynamicShaderProperties);
    write(*this, id);
    write(*this, properties_id);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::SetVisualModelStaticShaderProperties(object_id_t id, object_id_t properties_id)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_SetVisualModelStaticShaderProperties);
    write(*this, id);
    write(*this, properties_id);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::SetStaticMeshName(object_id_t id, const char* mesh_name)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_SetStaticMeshName);
    write(*this, id);
    write(*this, mesh_name);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::SetSkinMeshJointsCount(object_id_t id, uint32 count)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_SetSkinMeshJointsCount);
    write(*this, id);
    write(*this, count);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::SetSkinMeshJointNodes(object_id_t id, RawArray<object_id_t> nodes)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_SetSkinMeshJointNodes);
    write(*this, id);
    write(*this, nodes);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::SetSkinMeshJointMatrices(object_id_t id, RawArray<math::mat4f> matrices)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_SetSkinMeshJointMatrices);
    write(*this, id);
    write(*this, matrices);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::SetLightParams(object_id_t id, const LightParams& params)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_SetLightParams);
    write(*this, id);
    write(*this, params);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::SetPageCurlParams(object_id_t id, const PageCurlParams& params, const char* front_left_material, const char* front_right_material, const char* back_left_material, const char* back_right_material)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_SetPageCurlParams);
    write(*this, id);
    write(*this, params);
    write(*this, front_left_material);
    write(*this, front_right_material);
    write(*this, back_left_material);
    write(*this, back_right_material);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::SetParticleEmitterSystemId(object_id_t id, const char* system_id, SpriteMode sprite_mode)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_SetParticleEmitterSystemId);
    write(*this, id);
    write(*this, system_id);
    write(*this, sprite_mode);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::UpdateParticleEmitterSystem(object_id_t id, uint32 new_time, uint64 properties_id)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_UpdateParticleEmitterSystem);
    write(*this, id);
    write(*this, new_time);
    write(*this, properties_id);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::ReserveSpriteLists(object_id_t id, uint32 list_subids_count)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_ReserveSpriteLists);
    write(*this, id);
    write(*this, list_subids_count);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::CreateSpriteList(object_id_t id, uint32 list_subid, SpriteMode mode, PrimitiveUsage usage, const math::vec3f& up, const char* batch)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_CreateSpriteList);
    write(*this, id);
    write(*this, list_subid);
    write(*this, mode);
    write(*this, usage);
    write(*this, up);
    write(*this, batch);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::RemoveSpriteList(object_id_t id, uint32 list_subid)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_RemoveSpriteList);
    write(*this, id);
    write(*this, list_subid);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::SetSpriteListMaterial(object_id_t id, uint32 list_subid, const char* material)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_SetSpriteListMaterial);
    write(*this, id);
    write(*this, list_subid);
    write(*this, material);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::SetSpriteListBuffer(object_id_t id, uint32 list_subid, uint32 count, uint32 reserve_count)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_SetSpriteListBuffer);
    write(*this, id);
    write(*this, list_subid);
    write(*this, count);
    write(*this, reserve_count);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::SetSpriteListDescs(object_id_t id, uint32 list_subid, uint32 first, RawArray<SpriteDesc> descs)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_SetSpriteListDescs);
    write(*this, id);
    write(*this, list_subid);
    write(*this, first);
    write(*this, descs);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::SetLineListParams(object_id_t id, PrimitiveUsage usage, const char* batch)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_SetLineListParams);
    write(*this, id);
    write(*this, usage);
    write(*this, batch);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::SetLineListMaterial(object_id_t id, const char* material)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_SetLineListMaterial);
    write(*this, id);
    write(*this, material);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::SetLineListBuffer(object_id_t id, uint32 count, uint32 reserve_count)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_SetLineListBuffer);
    write(*this, id);
    write(*this, count);
    write(*this, reserve_count);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ClientToServerSerializer::SetLineListDescs(object_id_t id, uint32 first, RawArray<LineDesc> descs)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_SetLineListDescs);
    write(*this, id);
    write(*this, first);
    write(*this, descs);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

template <class Dispatcher> inline bool ClientToServerDeserializer::Deserialize(CommandId id, Dispatcher& dispatcher)
{
  switch (id)
  {
    case CommandId_FenceRequest:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());

      dispatcher.FenceRequest(arg1);

      return true;
    }
    case CommandId_LoadResource:
    {
      const char* arg1 = read(*this, xtl::type<const char* > ());

      dispatcher.LoadResource(arg1);

      return true;
    }
    case CommandId_UnloadResource:
    {
      const char* arg1 = read(*this, xtl::type<const char* > ());

      dispatcher.UnloadResource(arg1);

      return true;
    }
    case CommandId_SetMaxDrawDepth:
    {
      uint32 arg1 = read(*this, xtl::type<uint32 > ());

      dispatcher.SetMaxDrawDepth(arg1);

      return true;
    }
    case CommandId_CreateTexture:
    {
      const char* arg1 = read(*this, xtl::type<const char* > ());
      media::Image arg2 = read(*this, xtl::type<media::Image > ());
      TextureDimension arg3 = read(*this, xtl::type<TextureDimension > ());
      bool8 arg4 = read(*this, xtl::type<bool8 > ());

      dispatcher.CreateTexture(arg1, arg2, arg3, arg4);

      return true;
    }
    case CommandId_UpdateTexture:
    {
      const char* arg1 = read(*this, xtl::type<const char* > ());
      media::Image arg2 = read(*this, xtl::type<media::Image > ());

      dispatcher.UpdateTexture(arg1, arg2);

      return true;
    }
    case CommandId_RemoveTexture:
    {
      const char* arg1 = read(*this, xtl::type<const char* > ());

      dispatcher.RemoveTexture(arg1);

      return true;
    }
    case CommandId_CloneMaterial:
    {
      const char* arg1 = read(*this, xtl::type<const char* > ());
      const char* arg2 = read(*this, xtl::type<const char* > ());

      dispatcher.CloneMaterial(arg1, arg2);

      return true;
    }
    case CommandId_SetMaterialTexmapImage:
    {
      const char* arg1 = read(*this, xtl::type<const char* > ());
      const char* arg2 = read(*this, xtl::type<const char* > ());
      const char* arg3 = read(*this, xtl::type<const char* > ());

      dispatcher.SetMaterialTexmapImage(arg1, arg2, arg3);

      return true;
    }
    case CommandId_SetMaterialTexmapSampler:
    {
      const char* arg1 = read(*this, xtl::type<const char* > ());
      const char* arg2 = read(*this, xtl::type<const char* > ());
      const char* arg3 = read(*this, xtl::type<const char* > ());

      dispatcher.SetMaterialTexmapSampler(arg1, arg2, arg3);

      return true;
    }
    case CommandId_RemoveMaterial:
    {
      const char* arg1 = read(*this, xtl::type<const char* > ());

      dispatcher.RemoveMaterial(arg1);

      return true;
    }
    case CommandId_SetViewportArea:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      int32 arg2 = read(*this, xtl::type<int32 > ());
      int32 arg3 = read(*this, xtl::type<int32 > ());
      int32 arg4 = read(*this, xtl::type<int32 > ());
      int32 arg5 = read(*this, xtl::type<int32 > ());
      float32 arg6 = read(*this, xtl::type<float32 > ());
      float32 arg7 = read(*this, xtl::type<float32 > ());

      dispatcher.SetViewportArea(arg1, arg2, arg3, arg4, arg5, arg6, arg7);

      return true;
    }
    case CommandId_SetViewportZOrder:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      int32 arg2 = read(*this, xtl::type<int32 > ());

      dispatcher.SetViewportZOrder(arg1, arg2);

      return true;
    }
    case CommandId_SetViewportActive:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      bool8 arg2 = read(*this, xtl::type<bool8 > ());

      dispatcher.SetViewportActive(arg1, arg2);

      return true;
    }
    case CommandId_SetViewportName:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      const char* arg2 = read(*this, xtl::type<const char* > ());

      dispatcher.SetViewportName(arg1, arg2);

      return true;
    }
    case CommandId_SetViewportTechnique:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      const char* arg2 = read(*this, xtl::type<const char* > ());

      dispatcher.SetViewportTechnique(arg1, arg2);

      return true;
    }
    case CommandId_SetViewportBackground:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      bool8 arg2 = read(*this, xtl::type<bool8 > ());
      const math::vec4f& arg3 = read(*this, xtl::type<const math::vec4f& > ());

      dispatcher.SetViewportBackground(arg1, arg2, arg3);

      return true;
    }
    case CommandId_SetViewportProperties:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      uint64 arg2 = read(*this, xtl::type<uint64 > ());

      dispatcher.SetViewportProperties(arg1, arg2);

      return true;
    }
    case CommandId_SetViewportScene:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      object_id_t arg2 = read(*this, xtl::type<object_id_t > ());

      dispatcher.SetViewportScene(arg1, arg2);

      return true;
    }
    case CommandId_SetViewportCameraWorldMatrix:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      const math::mat4f& arg2 = read(*this, xtl::type<const math::mat4f& > ());

      dispatcher.SetViewportCameraWorldMatrix(arg1, arg2);

      return true;
    }
    case CommandId_SetViewportCameraProjectionMatrix:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      const math::mat4f& arg2 = read(*this, xtl::type<const math::mat4f& > ());

      dispatcher.SetViewportCameraProjectionMatrix(arg1, arg2);

      return true;
    }
    case CommandId_SetViewportCameraName:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      const char* arg2 = read(*this, xtl::type<const char* > ());

      dispatcher.SetViewportCameraName(arg1, arg2);

      return true;
    }
    case CommandId_CreateRenderTarget:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      const char* arg2 = read(*this, xtl::type<const char* > ());
      const char* arg3 = read(*this, xtl::type<const char* > ());

      dispatcher.CreateRenderTarget(arg1, arg2, arg3);

      return true;
    }
    case CommandId_DestroyRenderTarget:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());

      dispatcher.DestroyRenderTarget(arg1);

      return true;
    }
    case CommandId_SetRenderTargetActive:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      bool8 arg2 = read(*this, xtl::type<bool8 > ());

      dispatcher.SetRenderTargetActive(arg1, arg2);

      return true;
    }
    case CommandId_SetRenderTargetScreenArea:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      int32 arg2 = read(*this, xtl::type<int32 > ());
      int32 arg3 = read(*this, xtl::type<int32 > ());
      int32 arg4 = read(*this, xtl::type<int32 > ());
      int32 arg5 = read(*this, xtl::type<int32 > ());

      dispatcher.SetRenderTargetScreenArea(arg1, arg2, arg3, arg4, arg5);

      return true;
    }
    case CommandId_SetRenderTargetBackground:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      bool8 arg2 = read(*this, xtl::type<bool8 > ());
      const math::vec4f& arg3 = read(*this, xtl::type<const math::vec4f& > ());

      dispatcher.SetRenderTargetBackground(arg1, arg2, arg3);

      return true;
    }
    case CommandId_AttachViewportToRenderTarget:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      object_id_t arg2 = read(*this, xtl::type<object_id_t > ());

      dispatcher.AttachViewportToRenderTarget(arg1, arg2);

      return true;
    }
    case CommandId_DetachViewportFromRenderTarget:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      object_id_t arg2 = read(*this, xtl::type<object_id_t > ());

      dispatcher.DetachViewportFromRenderTarget(arg1, arg2);

      return true;
    }
    case CommandId_UpdateRenderTarget:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());

      dispatcher.UpdateRenderTarget(arg1);

      return true;
    }
    case CommandId_UpdatePropertyMap:
    {
      dispatcher.UpdatePropertyMap(*this);
      return true;
    }
    case CommandId_RemovePropertyMap:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());

      dispatcher.RemovePropertyMap(arg1);

      return true;
    }
    case CommandId_RemovePropertyLayout:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());

      dispatcher.RemovePropertyLayout(arg1);

      return true;
    }
    case CommandId_CreateScene:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());

      dispatcher.CreateScene(arg1);

      return true;
    }
    case CommandId_DestroyScene:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());

      dispatcher.DestroyScene(arg1);

      return true;
    }
    case CommandId_SetSceneName:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      const char* arg2 = read(*this, xtl::type<const char* > ());

      dispatcher.SetSceneName(arg1, arg2);

      return true;
    }
    case CommandId_SetSceneNodes:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      RawArray<object_id_t> arg2 = read(*this, xtl::type<RawArray<object_id_t> > ());

      dispatcher.SetSceneNodes(arg1, arg2);

      return true;
    }
    case CommandId_CreateNode:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      NodeType arg2 = read(*this, xtl::type<NodeType > ());

      dispatcher.CreateNode(arg1, arg2);

      return true;
    }
    case CommandId_DestroyNode:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());

      dispatcher.DestroyNode(arg1);

      return true;
    }
    case CommandId_SetNodeName:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      const char* arg2 = read(*this, xtl::type<const char* > ());

      dispatcher.SetNodeName(arg1, arg2);

      return true;
    }
    case CommandId_SetNodeWorldMatrix:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      const math::mat4f& arg2 = read(*this, xtl::type<const math::mat4f& > ());

      dispatcher.SetNodeWorldMatrix(arg1, arg2);

      return true;
    }
    case CommandId_SetNodeScene:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      object_id_t arg2 = read(*this, xtl::type<object_id_t > ());

      dispatcher.SetNodeScene(arg1, arg2);

      return true;
    }
    case CommandId_SetEntityBounds:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      bool arg2 = read(*this, xtl::type<bool > ());
      const bound_volumes::aaboxf& arg3 = read(*this, xtl::type<const bound_volumes::aaboxf& > ());

      dispatcher.SetEntityBounds(arg1, arg2, arg3);

      return true;
    }
    case CommandId_SetEntityVisibility:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      bool arg2 = read(*this, xtl::type<bool > ());

      dispatcher.SetEntityVisibility(arg1, arg2);

      return true;
    }
    case CommandId_SetVisualModelScissor:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      object_id_t arg2 = read(*this, xtl::type<object_id_t > ());

      dispatcher.SetVisualModelScissor(arg1, arg2);

      return true;
    }
    case CommandId_SetVisualModelDynamicShaderProperties:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      object_id_t arg2 = read(*this, xtl::type<object_id_t > ());

      dispatcher.SetVisualModelDynamicShaderProperties(arg1, arg2);

      return true;
    }
    case CommandId_SetVisualModelStaticShaderProperties:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      object_id_t arg2 = read(*this, xtl::type<object_id_t > ());

      dispatcher.SetVisualModelStaticShaderProperties(arg1, arg2);

      return true;
    }
    case CommandId_SetStaticMeshName:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      const char* arg2 = read(*this, xtl::type<const char* > ());

      dispatcher.SetStaticMeshName(arg1, arg2);

      return true;
    }
    case CommandId_SetSkinMeshJointsCount:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      uint32 arg2 = read(*this, xtl::type<uint32 > ());

      dispatcher.SetSkinMeshJointsCount(arg1, arg2);

      return true;
    }
    case CommandId_SetSkinMeshJointNodes:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      RawArray<object_id_t> arg2 = read(*this, xtl::type<RawArray<object_id_t> > ());

      dispatcher.SetSkinMeshJointNodes(arg1, arg2);

      return true;
    }
    case CommandId_SetSkinMeshJointMatrices:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      RawArray<math::mat4f> arg2 = read(*this, xtl::type<RawArray<math::mat4f> > ());

      dispatcher.SetSkinMeshJointMatrices(arg1, arg2);

      return true;
    }
    case CommandId_SetLightParams:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      const LightParams& arg2 = read(*this, xtl::type<const LightParams& > ());

      dispatcher.SetLightParams(arg1, arg2);

      return true;
    }
    case CommandId_SetPageCurlParams:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      const PageCurlParams& arg2 = read(*this, xtl::type<const PageCurlParams& > ());
      const char* arg3 = read(*this, xtl::type<const char* > ());
      const char* arg4 = read(*this, xtl::type<const char* > ());
      const char* arg5 = read(*this, xtl::type<const char* > ());
      const char* arg6 = read(*this, xtl::type<const char* > ());

      dispatcher.SetPageCurlParams(arg1, arg2, arg3, arg4, arg5, arg6);

      return true;
    }
    case CommandId_SetParticleEmitterSystemId:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      const char* arg2 = read(*this, xtl::type<const char* > ());
      SpriteMode arg3 = read(*this, xtl::type<SpriteMode > ());

      dispatcher.SetParticleEmitterSystemId(arg1, arg2, arg3);

      return true;
    }
    case CommandId_UpdateParticleEmitterSystem:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      uint32 arg2 = read(*this, xtl::type<uint32 > ());
      uint64 arg3 = read(*this, xtl::type<uint64 > ());

      dispatcher.UpdateParticleEmitterSystem(arg1, arg2, arg3);

      return true;
    }
    case CommandId_ReserveSpriteLists:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      uint32 arg2 = read(*this, xtl::type<uint32 > ());

      dispatcher.ReserveSpriteLists(arg1, arg2);

      return true;
    }
    case CommandId_CreateSpriteList:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      uint32 arg2 = read(*this, xtl::type<uint32 > ());
      SpriteMode arg3 = read(*this, xtl::type<SpriteMode > ());
      PrimitiveUsage arg4 = read(*this, xtl::type<PrimitiveUsage > ());
      const math::vec3f& arg5 = read(*this, xtl::type<const math::vec3f& > ());
      const char* arg6 = read(*this, xtl::type<const char* > ());

      dispatcher.CreateSpriteList(arg1, arg2, arg3, arg4, arg5, arg6);

      return true;
    }
    case CommandId_RemoveSpriteList:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      uint32 arg2 = read(*this, xtl::type<uint32 > ());

      dispatcher.RemoveSpriteList(arg1, arg2);

      return true;
    }
    case CommandId_SetSpriteListMaterial:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      uint32 arg2 = read(*this, xtl::type<uint32 > ());
      const char* arg3 = read(*this, xtl::type<const char* > ());

      dispatcher.SetSpriteListMaterial(arg1, arg2, arg3);

      return true;
    }
    case CommandId_SetSpriteListBuffer:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      uint32 arg2 = read(*this, xtl::type<uint32 > ());
      uint32 arg3 = read(*this, xtl::type<uint32 > ());
      uint32 arg4 = read(*this, xtl::type<uint32 > ());

      dispatcher.SetSpriteListBuffer(arg1, arg2, arg3, arg4);

      return true;
    }
    case CommandId_SetSpriteListDescs:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      uint32 arg2 = read(*this, xtl::type<uint32 > ());
      uint32 arg3 = read(*this, xtl::type<uint32 > ());
      RawArray<SpriteDesc> arg4 = read(*this, xtl::type<RawArray<SpriteDesc> > ());

      dispatcher.SetSpriteListDescs(arg1, arg2, arg3, arg4);

      return true;
    }
    case CommandId_SetLineListParams:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      PrimitiveUsage arg2 = read(*this, xtl::type<PrimitiveUsage > ());
      const char* arg3 = read(*this, xtl::type<const char* > ());

      dispatcher.SetLineListParams(arg1, arg2, arg3);

      return true;
    }
    case CommandId_SetLineListMaterial:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      const char* arg2 = read(*this, xtl::type<const char* > ());

      dispatcher.SetLineListMaterial(arg1, arg2);

      return true;
    }
    case CommandId_SetLineListBuffer:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      uint32 arg2 = read(*this, xtl::type<uint32 > ());
      uint32 arg3 = read(*this, xtl::type<uint32 > ());

      dispatcher.SetLineListBuffer(arg1, arg2, arg3);

      return true;
    }
    case CommandId_SetLineListDescs:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());
      uint32 arg2 = read(*this, xtl::type<uint32 > ());
      RawArray<LineDesc> arg3 = read(*this, xtl::type<RawArray<LineDesc> > ());

      dispatcher.SetLineListDescs(arg1, arg2, arg3);

      return true;
    }
    default:
      return DeserializeUnknownCommand (id, *this);
  }
}


/*
    Server to client
*/


inline void ServerToClientSerializer::FenceResponse(object_id_t tag)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_FenceResponse);
    write(*this, tag);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline OutputStream& ServerToClientSerializer::UpdatePropertyMap()
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_UpdatePropertyMap);
    return *this;
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ServerToClientSerializer::RemovePropertyMap(object_id_t id)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_RemovePropertyMap);
    write(*this, id);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

inline void ServerToClientSerializer::RemovePropertyLayout(object_id_t id)
{
  size_t saved_position = Position ();

  try
  {
    BeginCommand(CommandId_RemovePropertyLayout);
    write(*this, id);
    EndCommand();
  }
  catch (...)
  {
    SetPosition (saved_position);
    throw;
  }
}

template <class Dispatcher> inline bool ServerToClientDeserializer::Deserialize(CommandId id, Dispatcher& dispatcher)
{
  switch (id)
  {
    case CommandId_FenceResponse:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());

      dispatcher.FenceResponse(arg1);

      return true;
    }
    case CommandId_UpdatePropertyMap:
    {
      dispatcher.UpdatePropertyMap(*this);
      return true;
    }
    case CommandId_RemovePropertyMap:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());

      dispatcher.RemovePropertyMap(arg1);

      return true;
    }
    case CommandId_RemovePropertyLayout:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t > ());

      dispatcher.RemovePropertyLayout(arg1);

      return true;
    }
    default:
      return DeserializeUnknownCommand (id, *this);
  }
}

