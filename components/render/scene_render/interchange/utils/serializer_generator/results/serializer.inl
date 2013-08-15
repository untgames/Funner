/*
    Client to server
*/

void ClientToServerSerializer::LoadResource(const char* name)
{
  BeginCommand(CommandId_LoadResource);
  write(*this, name);
  EndCommand();
}

void ClientToServerSerializer::UnloadResource(const char* name)
{
  BeginCommand(CommandId_UnloadResource);
  write(*this, name);
  EndCommand();
}

template <class Dispatcher> bool ClientToServerDeserializer::Deserialize(CommandId id, Dispatcher& dispatcher)
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


template <class Dispatcher> bool ServerToClientDeserializer::Deserialize(CommandId id, Dispatcher& dispatcher)
{
  switch (id)
  {
    default:
      return false;
  }
}

