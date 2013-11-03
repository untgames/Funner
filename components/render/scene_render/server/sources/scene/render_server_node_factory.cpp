#include "shared.h"

using namespace render::scene::server;
using namespace render::scene;

using interchange::NodeType;

Node* NodeFactory::CreateNode (NodeType type)
{
  try
  {
    switch (type)
    {
      case interchange::NodeType_Node: return new Node;
      default:
        throw xtl::make_argument_exception ("", "type", type);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::NodeFactory::CreateNode");
    throw;
  }
}
