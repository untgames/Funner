inline stl::string get_command_name(CommandId command_id)
{
  switch (command_id)
  {
    case CommandId_LoadResource: return "LoadResource";
    case CommandId_UnloadResource: return "UnloadResource";
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
    default: return common::format ("CommandId#%u", command_id);
  }
}

/*
    Client to server
*/

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

inline void ClientToServerSerializer::SetViewportArea(object_id_t id, int32 left, int32 top, int32 width, int32 height)
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

template <class Dispatcher> inline bool ClientToServerDeserializer::Deserialize(CommandId id, Dispatcher& dispatcher)
{
  switch (id)
  {
    case CommandId_LoadResource:
    {
      const char* arg1 = read(*this, xtl::type<const char*> ());

      dispatcher.LoadResource(arg1);

      return true;
    }
    case CommandId_UnloadResource:
    {
      const char* arg1 = read(*this, xtl::type<const char*> ());

      dispatcher.UnloadResource(arg1);

      return true;
    }
    case CommandId_SetViewportArea:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t> ());
      int32 arg2 = read(*this, xtl::type<int32> ());
      int32 arg3 = read(*this, xtl::type<int32> ());
      int32 arg4 = read(*this, xtl::type<int32> ());
      int32 arg5 = read(*this, xtl::type<int32> ());

      dispatcher.SetViewportArea(arg1, arg2, arg3, arg4, arg5);

      return true;
    }
    case CommandId_SetViewportZOrder:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t> ());
      int32 arg2 = read(*this, xtl::type<int32> ());

      dispatcher.SetViewportZOrder(arg1, arg2);

      return true;
    }
    case CommandId_SetViewportActive:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t> ());
      bool8 arg2 = read(*this, xtl::type<bool8> ());

      dispatcher.SetViewportActive(arg1, arg2);

      return true;
    }
    case CommandId_SetViewportName:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t> ());
      const char* arg2 = read(*this, xtl::type<const char*> ());

      dispatcher.SetViewportName(arg1, arg2);

      return true;
    }
    case CommandId_SetViewportTechnique:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t> ());
      const char* arg2 = read(*this, xtl::type<const char*> ());

      dispatcher.SetViewportTechnique(arg1, arg2);

      return true;
    }
    case CommandId_SetViewportBackground:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t> ());
      bool8 arg2 = read(*this, xtl::type<bool8> ());
      const math::vec4f& arg3 = read(*this, xtl::type<const math::vec4f&> ());

      dispatcher.SetViewportBackground(arg1, arg2, arg3);

      return true;
    }
    case CommandId_SetViewportProperties:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t> ());
      uint64 arg2 = read(*this, xtl::type<uint64> ());

      dispatcher.SetViewportProperties(arg1, arg2);

      return true;
    }
    case CommandId_SetViewportScene:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t> ());
      object_id_t arg2 = read(*this, xtl::type<object_id_t> ());

      dispatcher.SetViewportScene(arg1, arg2);

      return true;
    }
    case CommandId_SetViewportCameraWorldMatrix:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t> ());
      const math::mat4f& arg2 = read(*this, xtl::type<const math::mat4f&> ());

      dispatcher.SetViewportCameraWorldMatrix(arg1, arg2);

      return true;
    }
    case CommandId_SetViewportCameraProjectionMatrix:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t> ());
      const math::mat4f& arg2 = read(*this, xtl::type<const math::mat4f&> ());

      dispatcher.SetViewportCameraProjectionMatrix(arg1, arg2);

      return true;
    }
    case CommandId_SetViewportCameraName:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t> ());
      const char* arg2 = read(*this, xtl::type<const char*> ());

      dispatcher.SetViewportCameraName(arg1, arg2);

      return true;
    }
    case CommandId_CreateRenderTarget:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t> ());
      const char* arg2 = read(*this, xtl::type<const char*> ());
      const char* arg3 = read(*this, xtl::type<const char*> ());

      dispatcher.CreateRenderTarget(arg1, arg2, arg3);

      return true;
    }
    case CommandId_DestroyRenderTarget:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t> ());

      dispatcher.DestroyRenderTarget(arg1);

      return true;
    }
    case CommandId_SetRenderTargetActive:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t> ());
      bool8 arg2 = read(*this, xtl::type<bool8> ());

      dispatcher.SetRenderTargetActive(arg1, arg2);

      return true;
    }
    case CommandId_SetRenderTargetScreenArea:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t> ());
      int32 arg2 = read(*this, xtl::type<int32> ());
      int32 arg3 = read(*this, xtl::type<int32> ());
      int32 arg4 = read(*this, xtl::type<int32> ());
      int32 arg5 = read(*this, xtl::type<int32> ());

      dispatcher.SetRenderTargetScreenArea(arg1, arg2, arg3, arg4, arg5);

      return true;
    }
    case CommandId_SetRenderTargetBackground:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t> ());
      bool8 arg2 = read(*this, xtl::type<bool8> ());
      const math::vec4f& arg3 = read(*this, xtl::type<const math::vec4f&> ());

      dispatcher.SetRenderTargetBackground(arg1, arg2, arg3);

      return true;
    }
    case CommandId_AttachViewportToRenderTarget:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t> ());
      object_id_t arg2 = read(*this, xtl::type<object_id_t> ());

      dispatcher.AttachViewportToRenderTarget(arg1, arg2);

      return true;
    }
    case CommandId_DetachViewportFromRenderTarget:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t> ());
      object_id_t arg2 = read(*this, xtl::type<object_id_t> ());

      dispatcher.DetachViewportFromRenderTarget(arg1, arg2);

      return true;
    }
    case CommandId_UpdateRenderTarget:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t> ());

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
      object_id_t arg1 = read(*this, xtl::type<object_id_t> ());

      dispatcher.RemovePropertyMap(arg1);

      return true;
    }
    case CommandId_RemovePropertyLayout:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t> ());

      dispatcher.RemovePropertyLayout(arg1);

      return true;
    }
    case CommandId_CreateScene:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t> ());

      dispatcher.CreateScene(arg1);

      return true;
    }
    case CommandId_DestroyScene:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t> ());

      dispatcher.DestroyScene(arg1);

      return true;
    }
    case CommandId_SetSceneName:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t> ());
      const char* arg2 = read(*this, xtl::type<const char*> ());

      dispatcher.SetSceneName(arg1, arg2);

      return true;
    }
    default:
      return DeserializeUnknownCommand (id, *this);
  }
}


/*
    Server to client
*/


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
    case CommandId_UpdatePropertyMap:
    {
      dispatcher.UpdatePropertyMap(*this);
      return true;
    }
    case CommandId_RemovePropertyMap:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t> ());

      dispatcher.RemovePropertyMap(arg1);

      return true;
    }
    case CommandId_RemovePropertyLayout:
    {
      object_id_t arg1 = read(*this, xtl::type<object_id_t> ());

      dispatcher.RemovePropertyLayout(arg1);

      return true;
    }
    default:
      return DeserializeUnknownCommand (id, *this);
  }
}

