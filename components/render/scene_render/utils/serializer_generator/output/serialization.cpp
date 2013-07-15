void LoadResource(const char* name)
{
  SerializeHeader(CommandId_LoadResource);
  Serialize(name);
  SerializeTail();
}

void UnloadResource(const char* name)
{
  SerializeHeader(CommandId_UnloadResource);
  Serialize(name);
  SerializeTail();
}

