template <class Dispatcher> bool CommandDeserializer::Deserialize(CommandId id, Dispatcher& dispatcher)
{
  switch (id)
  {
    case CommandId_LoadResource:
      dispatcher.LoadResource(Deserialize<const char*> ());
      return true;
    case CommandId_UnloadResource:
      dispatcher.UnloadResource(Deserialize<const char*> ());
      return true;
    default:
      return false;
  }
}

