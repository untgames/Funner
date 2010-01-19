#include "shared.h"

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
      square_length += local_matrix [j][i] * local_matrix [j][i];

    scale [i] = sqrt (square_length);

    //нормирование
    for (size_t j = 0; j < 3; j++)
      local_matrix [j][i] /= scale [i];
  }

  //выделение преобразования поворота
  rotation [1] = asin (-local_matrix [0][2]);

  if (cos (rotation [1]) != 0.0)
  {
    rotation [0] = atan2 (local_matrix [1][2], local_matrix [2][2]);
    rotation [2] = atan2 (local_matrix [0][1], local_matrix [0][0]);
  }
  else
  {
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

void SceneManager::ReadNodeInfo (common::Parser::Iterator node, scene_graph::Node::Pointer parent)
{
  scene_graph::Node::Pointer new_node = scene_graph::Node::Create ();

  const char* transform_string = common::get<const char*> (*node, "transform");

  math::mat4f transform;

  if (!read (transform_string, transform))
    throw xtl::format_operation_exception ("ReadNodeInfo", "Invalid transform format at line %u", node->LineNumber ());

  math::vec3f translation, rotation, scale;

  affine_decompose (transform, translation, rotation, scale);

  new_node->SetPosition    (translation);
  new_node->SetOrientation (math::radian (rotation.x), math::radian (rotation.y), math::radian (rotation.z));
  new_node->SetScale       (scale);

  new_node->BindToParent (*parent, scene_graph::NodeBindMode_AddRef);

  for_each_child (*node, "mesh", xtl::bind (&SceneManager::ReadMeshInfo, this, _1, new_node));
  for_each_child (*node, "node", xtl::bind (&SceneManager::ReadNodeInfo, this, _1, new_node));
}

void SceneManager::LoadScene (const char* file_name)
{
  common::Parser   p (file_name);
  common::ParseLog log = p.Log ();

  for_each_child (p.Root (), "scene.node", xtl::bind (&SceneManager::ReadNodeInfo, this, _1, scene_graph::Node::Pointer (&scene.Root ())));

  for (size_t i = 0; i < log.MessagesCount (); i++)
    switch (log.MessageType (i))
    {
      case common::ParseLogMessageType_Error:
      case common::ParseLogMessageType_FatalError:
        throw xtl::format_operation_exception ("LoadScene", log.Message (i));
      default:
        break;
    }
}
