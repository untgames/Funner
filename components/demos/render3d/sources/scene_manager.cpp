#include "shared.h"

namespace
{

const float EPS = 0.0001f;

}

namespace math
{

inline bool read (const char* string, math::vec4f& result)
{
  common::StringArray tokens = common::split (string);

  switch (tokens.Size ())
  {
    case 1:
    {
      float value = 0.0f;

      if (!xtl::io::read (tokens [0], value))
        return false;

      result = math::vec4f (value);

      return true;
    }
    case 2:
    case 3:
    case 4:
    {
      math::vec4f v (0.0f);

      for (size_t i=0, count=tokens.Size (); i<count; i++)
        if (!xtl::io::read (tokens [i], v [i]))
          return false;

      result = v;

      return true;
    }
    default:
      return false;
  }
}

inline bool read (const char* string, math::mat4f& result)
{
  typedef xtl::io::token_iterator<const char*> token_iterator;

  common::StringArray tokens = common::split (string);

  switch (tokens.Size ())
  {
    case 1:
    {
      float value = 0.0f;

      if (!xtl::io::read (tokens [0], value))
        return false;

      result = math::mat4f (value);

      return true;
    }
    case 16:
    {
      token_iterator iter = xtl::io::make_token_iterator (tokens.Data (), tokens.Data () + tokens.Size ());

      return read (iter, result);
    }
    default:
      return false;
  }
}

}

//декомпозиция аффинных преобразований из матрицы преобразований
void affine_decompose (const math::mat4f& matrix, math::vec3f& position, math::vec3f& rotation, math::vec3f& scale)
{
  math::mat4f local_matrix (transpose (matrix)); //копия матрицы преобразований

  //выделение преобразования переноса  
  for (size_t i = 0; i < 3; i++)
  {
    position [i] = local_matrix [3][i];
    local_matrix [3][i] = 0;
  }

  //выделение преобразования масштабирования
  for (size_t i = 0; i < 3; i++)
  {
    //определение длины вектора-столбца преобразований
    float square_length = 0;

    for (size_t j = 0; j < 3; j++)
      square_length += local_matrix [i][j] * local_matrix [i][j];

    scale [i] = sqrt (square_length);

    //нормирование
    for (size_t j = 0; j < 3; j++)
      local_matrix [i][j] /= scale [i];
  }

  math::vec3f temp_z = math::cross (math::vec3f (local_matrix [0][0], local_matrix [0][1], local_matrix [0][2]), math::vec3f (local_matrix [1][0], local_matrix [1][1], local_matrix [1][2]));

  if (math::dot (temp_z, math::vec3f (local_matrix [2][0], local_matrix [2][1], local_matrix [2][2])) < 0)
  {
    scale.x = -scale.x;
    local_matrix [0][0] = -local_matrix [0][0];
    local_matrix [0][1] = -local_matrix [0][1];
    local_matrix [0][2] = -local_matrix [0][2];
  }

  //выделение преобразования поворота
  rotation [1] = asin (-local_matrix [0][2]);

  if (fabs (cos (rotation [1])) > EPS)
  {
    rotation [0] = atan2 (local_matrix [1][2], local_matrix [2][2]);
    rotation [2] = atan2 (local_matrix [0][1], local_matrix [0][0]);
  }
  else
  {
    if (fabs (local_matrix [1][1]) < EPS)
      local_matrix [1][1] = 0;

    rotation [0] = atan2 (-local_matrix [2][0], local_matrix [1][1]);
    rotation [2] = 0;
  }
}

void SceneManager::ReadMeshInfo (common::Parser::Iterator node, scene_graph::Node::Pointer parent)
{
  scene_graph::VisualModel::Pointer model = scene_graph::VisualModel::Create ();

  model->SetMeshName (common::get<const char*> (*node, "name"));

  model->BindToParent (*parent, scene_graph::NodeBindMode_AddRef);
}

/*void print (const math::vec4f& v)
{
  printf ("  [%f %f %f %f]\n", v.x, v.y, v.z, v.w);
}

void print (const math::mat4f& m)
{
  printf ("matrix:\n");
  print (m.row (0));
  print (m.row (1));
  print (m.row (2));
  print (m.row (3));
}*/

void SceneManager::ReadNodeInfo (common::Parser::Iterator node, scene_graph::Node::Pointer parent)
{
  scene_graph::Node::Pointer new_node = scene_graph::Node::Create ();

  const char* transform_string = common::get<const char*> (*node, "transform");

  math::mat4f transform;

  if (!read (transform_string, transform))
    throw xtl::format_operation_exception ("ReadNodeInfo", "Invalid transform format at line %u", node->LineNumber ());

  math::vec3f translation, scale, rotation;

  affine_decompose (transform, translation, rotation, scale);

  new_node->SetWorldPosition    (translation);
  new_node->SetWorldOrientation (math::radian (rotation.x), math::radian (rotation.y), math::radian (rotation.z));
  new_node->SetWorldScale       (scale);

  if (!equal (transform, new_node->WorldTM (), 0.01f))
    return;

  new_node->BindToParent (*parent, scene_graph::NodeBindMode_AddRef);

  for_each_child (*node, "mesh", xtl::bind (&SceneManager::ReadMeshInfo, this, _1, new_node));
  for_each_child (*node, "node", xtl::bind (&SceneManager::ReadNodeInfo, this, _1, new_node));
}

Node::Pointer SceneManager::LoadScene (const char* file_name)
{
  common::Parser   p (file_name);
  common::ParseLog log = p.Log ();

  Node::Pointer root = Node::Create ();

  root->BindToScene (scene, scene_graph::NodeBindMode_AddRef);

  for_each_child (p.Root (), "scene.node", xtl::bind (&SceneManager::ReadNodeInfo, this, _1, root));

  for (size_t i = 0; i < log.MessagesCount (); i++)
    switch (log.MessageType (i))
    {
      case common::ParseLogMessageType_Error:
      case common::ParseLogMessageType_FatalError:
        throw xtl::format_operation_exception ("LoadScene", log.Message (i));
      default:
        break;
    }

  return root;
}

void SceneManager::AddShattle (Node::Pointer shattle)
{
  shattles.push_back (shattle);
}

void SceneManager::AddMainShip (Node::Pointer ship)
{
  main_ships.push_back (ship);
}

void SceneManager::SetDrawShattles (bool draw)
{
  for (NodesArray::iterator iter = shattles.begin (), end = shattles.end (); iter != end; ++iter)
  {
    if (draw)
      (*iter)->BindToScene (scene);
    else
      (*iter)->Unbind ();
  }
}

void SceneManager::SetDrawMainShips (bool draw)
{
  for (NodesArray::iterator iter = main_ships.begin (), end = shattles.end (); iter != end; ++iter)
  {
    if (draw)
      (*iter)->BindToScene (scene);
    else
      (*iter)->Unbind ();
  }
}
