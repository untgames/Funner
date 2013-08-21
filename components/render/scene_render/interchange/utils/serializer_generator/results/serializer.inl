inline const char* get_command_name(CommandId command_id)
{
  switch (command_id)
  {
    case CommandId_LoadResource: return "LoadResource";
    case CommandId_UnloadResource: return "UnloadResource";
    case CommandId_Dummy: return "Dummy";
    default: throw xtl::make_argument_exception ("render::scene::interchange::get_command_name", "command_id", command_id);
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
    default:
      return false;
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
      return false;
  }
}

