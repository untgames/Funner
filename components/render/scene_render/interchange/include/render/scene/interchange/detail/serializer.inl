inline stl::string get_command_name(CommandId command_id)
{
  switch (command_id)
  {
    case CommandId_LoadResource: return "LoadResource";
    case CommandId_UnloadResource: return "UnloadResource";
    case CommandId_CreateViewport: return "CreateViewport";
    case CommandId_SetViewportArea: return "SetViewportArea";
    case CommandId_SetViewportZOrder: return "SetViewportZOrder";
    case CommandId_SetViewportActive: return "SetViewportActive";
    case CommandId_SetViewportName: return "SetViewportName";
    case CommandId_SetViewportTechnique: return "SetViewportTechnique";
    case CommandId_SetViewportBackground: return "SetViewportBackground";
    case CommandId_DestroyViewport: return "DestroyViewport";
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
    case CommandId_Dummy: return "Dummy";
    default: return common::format ("CommandId#%u", command_id);
  }
}

/*
    Client to server
*/

inline void ClientToServerSerializer::LoadResource(const char* name)
{
  BeginCommand(CommandId_LoadResource);
  write(*this, name);
  EndCommand();
}

inline void ClientToServerSerializer::UnloadResource(const char* name)
{
  BeginCommand(CommandId_UnloadResource);
  write(*this, name);
  EndCommand();
}

inline void ClientToServerSerializer::CreateViewport(uint32 id)
{
  BeginCommand(CommandId_CreateViewport);
  write(*this, id);
  EndCommand();
}

inline void ClientToServerSerializer::SetViewportArea(uint32 id, int8 left, int8 top, int8 width, int8 height)
{
  BeginCommand(CommandId_SetViewportArea);
  write(*this, id);
  write(*this, left);
  write(*this, top);
  write(*this, width);
  write(*this, height);
  EndCommand();
}

inline void ClientToServerSerializer::SetViewportZOrder(uint32 id, int8 zorder)
{
  BeginCommand(CommandId_SetViewportZOrder);
  write(*this, id);
  write(*this, zorder);
  EndCommand();
}

inline void ClientToServerSerializer::SetViewportActive(uint32 id, bool8 is_active)
{
  BeginCommand(CommandId_SetViewportActive);
  write(*this, id);
  write(*this, is_active);
  EndCommand();
}

inline void ClientToServerSerializer::SetViewportName(uint32 id, const char* name)
{
  BeginCommand(CommandId_SetViewportName);
  write(*this, id);
  write(*this, name);
  EndCommand();
}

inline void ClientToServerSerializer::SetViewportTechnique(uint32 id, const char* name)
{
  BeginCommand(CommandId_SetViewportTechnique);
  write(*this, id);
  write(*this, name);
  EndCommand();
}

inline void ClientToServerSerializer::SetViewportBackground(uint32 id, bool8 state, const math::vec4f& color)
{
  BeginCommand(CommandId_SetViewportBackground);
  write(*this, id);
  write(*this, state);
  write(*this, color);
  EndCommand();
}

inline void ClientToServerSerializer::DestroyViewport(uint32 id)
{
  BeginCommand(CommandId_DestroyViewport);
  write(*this, id);
  EndCommand();
}

inline void ClientToServerSerializer::CreateRenderTarget(uint32 id, const char* name)
{
  BeginCommand(CommandId_CreateRenderTarget);
  write(*this, id);
  write(*this, name);
  EndCommand();
}

inline void ClientToServerSerializer::DestroyRenderTarget(uint32 id)
{
  BeginCommand(CommandId_DestroyRenderTarget);
  write(*this, id);
  EndCommand();
}

inline void ClientToServerSerializer::SetRenderTargetActive(uint32 id, bool8 active)
{
  BeginCommand(CommandId_SetRenderTargetActive);
  write(*this, id);
  write(*this, active);
  EndCommand();
}

inline void ClientToServerSerializer::SetRenderTargetScreenArea(uint32 id, int8 left, int8 top, int8 width, int8 height)
{
  BeginCommand(CommandId_SetRenderTargetScreenArea);
  write(*this, id);
  write(*this, left);
  write(*this, top);
  write(*this, width);
  write(*this, height);
  EndCommand();
}

inline void ClientToServerSerializer::SetRenderTargetBackground(uint32 id, bool8 state, const math::vec4f& color)
{
  BeginCommand(CommandId_SetRenderTargetBackground);
  write(*this, id);
  write(*this, state);
  write(*this, color);
  EndCommand();
}

inline void ClientToServerSerializer::AttachViewportToRenderTarget(uint32 render_target_id, uint32 viewport_id)
{
  BeginCommand(CommandId_AttachViewportToRenderTarget);
  write(*this, render_target_id);
  write(*this, viewport_id);
  EndCommand();
}

inline void ClientToServerSerializer::DetachViewportFromRenderTarget(uint32 render_target_id, uint32 viewport_id)
{
  BeginCommand(CommandId_DetachViewportFromRenderTarget);
  write(*this, render_target_id);
  write(*this, viewport_id);
  EndCommand();
}

inline void ClientToServerSerializer::UpdateRenderTarget(uint32 id)
{
  BeginCommand(CommandId_UpdateRenderTarget);
  write(*this, id);
  EndCommand();
}

inline OutputStream& ClientToServerSerializer::UpdatePropertyMap()
{
  BeginCommand(CommandId_UpdatePropertyMap);
  return *this;
}

inline void ClientToServerSerializer::RemovePropertyMap(uint64 id)
{
  BeginCommand(CommandId_RemovePropertyMap);
  write(*this, id);
  EndCommand();
}

inline void ClientToServerSerializer::RemovePropertyLayout(uint64 id)
{
  BeginCommand(CommandId_RemovePropertyLayout);
  write(*this, id);
  EndCommand();
}

template <class Dispatcher> inline bool ClientToServerDeserializer::Deserialize(CommandId id, Dispatcher& dispatcher)
{
  switch (id)
  {
    case CommandId_LoadResource:
      dispatcher.LoadResource(read(*this, xtl::type<const char*> ()));
      return true;
    case CommandId_UnloadResource:
      dispatcher.UnloadResource(read(*this, xtl::type<const char*> ()));
      return true;
    case CommandId_CreateViewport:
      dispatcher.CreateViewport(read(*this, xtl::type<uint32> ()));
      return true;
    case CommandId_SetViewportArea:
      dispatcher.SetViewportArea(read(*this, xtl::type<uint32> ()), read(*this, xtl::type<int8> ()), read(*this, xtl::type<int8> ()), read(*this, xtl::type<int8> ()), read(*this, xtl::type<int8> ()));
      return true;
    case CommandId_SetViewportZOrder:
      dispatcher.SetViewportZOrder(read(*this, xtl::type<uint32> ()), read(*this, xtl::type<int8> ()));
      return true;
    case CommandId_SetViewportActive:
      dispatcher.SetViewportActive(read(*this, xtl::type<uint32> ()), read(*this, xtl::type<bool8> ()));
      return true;
    case CommandId_SetViewportName:
      dispatcher.SetViewportName(read(*this, xtl::type<uint32> ()), read(*this, xtl::type<const char*> ()));
      return true;
    case CommandId_SetViewportTechnique:
      dispatcher.SetViewportTechnique(read(*this, xtl::type<uint32> ()), read(*this, xtl::type<const char*> ()));
      return true;
    case CommandId_SetViewportBackground:
      dispatcher.SetViewportBackground(read(*this, xtl::type<uint32> ()), read(*this, xtl::type<bool8> ()), read(*this, xtl::type<const math::vec4f&> ()));
      return true;
    case CommandId_DestroyViewport:
      dispatcher.DestroyViewport(read(*this, xtl::type<uint32> ()));
      return true;
    case CommandId_CreateRenderTarget:
      dispatcher.CreateRenderTarget(read(*this, xtl::type<uint32> ()), read(*this, xtl::type<const char*> ()));
      return true;
    case CommandId_DestroyRenderTarget:
      dispatcher.DestroyRenderTarget(read(*this, xtl::type<uint32> ()));
      return true;
    case CommandId_SetRenderTargetActive:
      dispatcher.SetRenderTargetActive(read(*this, xtl::type<uint32> ()), read(*this, xtl::type<bool8> ()));
      return true;
    case CommandId_SetRenderTargetScreenArea:
      dispatcher.SetRenderTargetScreenArea(read(*this, xtl::type<uint32> ()), read(*this, xtl::type<int8> ()), read(*this, xtl::type<int8> ()), read(*this, xtl::type<int8> ()), read(*this, xtl::type<int8> ()));
      return true;
    case CommandId_SetRenderTargetBackground:
      dispatcher.SetRenderTargetBackground(read(*this, xtl::type<uint32> ()), read(*this, xtl::type<bool8> ()), read(*this, xtl::type<const math::vec4f&> ()));
      return true;
    case CommandId_AttachViewportToRenderTarget:
      dispatcher.AttachViewportToRenderTarget(read(*this, xtl::type<uint32> ()), read(*this, xtl::type<uint32> ()));
      return true;
    case CommandId_DetachViewportFromRenderTarget:
      dispatcher.DetachViewportFromRenderTarget(read(*this, xtl::type<uint32> ()), read(*this, xtl::type<uint32> ()));
      return true;
    case CommandId_UpdateRenderTarget:
      dispatcher.UpdateRenderTarget(read(*this, xtl::type<uint32> ()));
      return true;
    case CommandId_UpdatePropertyMap:
      dispatcher.UpdatePropertyMap(*this);
      return true;
    case CommandId_RemovePropertyMap:
      dispatcher.RemovePropertyMap(read(*this, xtl::type<uint64> ()));
      return true;
    case CommandId_RemovePropertyLayout:
      dispatcher.RemovePropertyLayout(read(*this, xtl::type<uint64> ()));
      return true;
    default:
      return DeserializeUnknownCommand (id, *this);
  }
}


/*
    Server to client
*/


inline void ServerToClientSerializer::Dummy()
{
  BeginCommand(CommandId_Dummy);

  EndCommand();
}

template <class Dispatcher> inline bool ServerToClientDeserializer::Deserialize(CommandId id, Dispatcher& dispatcher)
{
  switch (id)
  {
    case CommandId_Dummy:
      dispatcher.Dummy();
      return true;
    default:
      return DeserializeUnknownCommand (id, *this);
  }
}

