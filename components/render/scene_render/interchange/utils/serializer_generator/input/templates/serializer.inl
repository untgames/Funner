inline stl::string get_command_name(CommandId command_id)
{
  switch (command_id)
  {
<<<ENUMNAMES>>>
    case CommandId_PropertyLayout: return "PropertyLayout";
    case CommandId_PropertyMap: return "PropertyMap";
    default: return common::format ("CommandId#%u", command_id);
  }
}

/*
    Client to server
*/

<<<SERIALIZATION:ClientToServer>>>
<<<DESERIALIZATION:ClientToServer>>>

/*
    Server to client
*/

<<<SERIALIZATION:ServerToClient>>>
<<<DESERIALIZATION:ServerToClient>>>
