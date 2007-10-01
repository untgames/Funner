#include "shared.h"

using namespace stl;
using namespace media::collada;
using namespace math;

/*
    Разбор источника
*/

void DaeParser::ParseFloatArray (Parser::Iterator iter, vector <mat4f> *source)
{
  LogScope scope (iter, *this);
  
  size_t data_count = 0, count = 0, stride = 0;
  
  if (!CheckedRead (iter, "float_array.count", data_count))
  {
    LogError (iter, "Error at read 'float_array.count'");
    return;
  }
  
  Parser::Iterator accessor_iter = iter->First ("technique_common.accessor");
  
  if (!accessor_iter || !CheckedRead (accessor_iter, "count", count) || !CheckedRead (accessor_iter, "stride", stride))
  {
    LogError (iter, "Error at read 'technique_common.accessor'");
    return;    
  }
  
  if (data_count < count * stride)
  {
    LogError (accessor_iter, "Wrong count/stride attribute. count * stride > float_array.count. count=%u, stride=%u, "
              "float_array.count=%u", count, stride, data_count);
    return;
  }

  source->resize (count);

  if (read_range (iter, "float_array.#text", source->begin (), count) != count)
  {
    LogError (iter, "Error at read 'float_array' items");
    return;
  }
}

void DaeParser::ParseNameArray (Parser::Iterator iter, vector <string> *source)
{
  LogScope scope (iter, *this);
  
  size_t data_count = 0;
  
  if (!CheckedRead (iter, "Name_array.count", data_count))
  {
    LogError (iter, "Error at read 'Name_array.count'");
    return;
  }
  
  source->resize (data_count);

  if (read_range (iter, "Name_array.#text", source->begin (), data_count) != data_count)
  {
    LogError (iter, "Error at read 'Name_array' items");
    return;
  }
}

/*
    Разбор скина
*/

void DaeParser::ParseSkin (Parser::Iterator iter, const char* id)
{
  vector <string> joints;
  vector <mat4f>  inv_matrixes;
  mat4f           bind_shape_matrix (1);

  LogScope scope (iter, *this);

  if (!test (iter, "vertex_weights"))
  {
    LogError (iter, "No 'vertex_weights' sub-tag");
    return;
  }

  const char* base_mesh = get<const char*> (iter, "source");

  if (!base_mesh)
  {
    LogError (iter, "No base mesh ('source' property)");
    return;
  }

  base_mesh++; //убираем префиксный '#'

  Mesh* mesh = model.Meshes ().Find (base_mesh);
  Morph* morph = model.Morphs ().Find (base_mesh);
  
  if (!mesh)
  {
    if (!morph)
    {
      LogError (iter, "Incorrect url '%s'. No mesh or morph in library", base_mesh);
      return;
    }

    mesh = model.Meshes ().Find (morph->BaseMesh ());
  }
  
  if (test (iter, "bind_shape_matrix"))
    if (!CheckedRead (iter, "bind_shape_matrix.#text", bind_shape_matrix))
    {
      LogError (iter, "Uncorrect 'bind_shape_matrix' tag");
      return;
    }

  size_t influence_count = 0;

  if (!CheckedRead (iter, "vertex_weights.count", influence_count))
  {
    LogError (iter, "Error at read 'vertex_weights.count'");
    return;
  }

  stl::vector <size_t> per_vertex_count (influence_count), vertex_first_weight (influence_count);

  if (read_range (iter, "vertex_weights.vcount.#text", per_vertex_count.begin (), influence_count) != influence_count)
  {
    LogError (iter, "Error at read 'vertex_weights.vcount' items");
    return;
  }
  
  size_t vertex_joint_weights_count = 0;

  for (size_t i=0; i<influence_count; i++)
  {
    vertex_first_weight [i]     = vertex_joint_weights_count;
    vertex_joint_weights_count += per_vertex_count [i];
  }
  
  for (size_t i=0; i<mesh->Surfaces ().Size (); i++)
  {
    Surface&        surface            = mesh->Surfaces ()[i];
    VertexIndexMap* vertex_indices_map = GetVertexIndicesMap (&surface);
    
      //невыполнение этого условия теоретически невозможно

    if (vertex_indices_map->Size () != surface.VerticesCount ())
      continue;

    size_t           channel   = surface.InfluenceChannels ().Create (id);
    VertexInfluence* influence = surface.InfluenceChannels ().Data (channel);

    size_t* index         = vertex_indices_map->Indices (),
            indices_count = vertex_indices_map->Size ();

    for (size_t i=0; i<indices_count; i++, index++, influence++)
    {
      if (*index >= influence_count)
      {
        LogError (iter->First ("vertex_weights.vcount"), "Vertex index %u is greater than influence_count=%u", *index, influence_count);
        surface.InfluenceChannels ().Remove (channel);

        break;
      }

      influence->first_weight  = vertex_first_weight [*index];
      influence->weights_count = per_vertex_count [*index];
    }
  }
  
    //создание скина

  Skin skin;  
  
  skin.SetId (id);  
  skin.WeightsResize (vertex_joint_weights_count);
  skin.SetBaseMesh   (base_mesh);
  skin.SetBindShapeMatrix (bind_shape_matrix);  

  for (Parser::NamesakeIterator joints_iter=iter->First ("joints.input"); joints_iter; ++joints_iter)
  {
    const char *semantic    = get<const char*> (joints_iter, "semantic"),
               *source_name = get<const char*> (joints_iter, "source");

    if (!semantic)
    {
      LogError (iter, "No 'semantic' in joints input node");
      return;
    }
             
    if (!source_name)
    {
      LogError (iter, "No 'source' in joints input node");
      return;
    }
  
    source_name++;

    if (!::strcmp (semantic, "JOINT"))
    {
      for (Parser::NamesakeIterator i = iter->First ("source"); i; ++i)
        if (test (i, "id", source_name))
        {
          ParseNameArray (i, &joints);
          break;
        }

      if (joints.empty ())
        LogWarning (iter, "No joints");
    }
    else if (!::strcmp (semantic, "INV_BIND_MATRIX"))
    {
      for (Parser::NamesakeIterator i = iter->First("source"); i; ++i)
        if (test (i, "id", source_name))
        {
          ParseFloatArray (i, &inv_matrixes);
          break;
        }

      if (inv_matrixes.empty ())
        LogWarning (iter, "No inv_matrixes");
    }
  }

  if (joints.size () != inv_matrixes.size ())
  {
    LogError (iter, "Different count of joints and inv_matrixes");
    return;
  }

  for (size_t i = 0; i < joints.size (); i++)
  {
    skin.CreateJoint (joints[i].c_str ());
    skin.SetJointInvMatrix (i, inv_matrixes[i]);
  }
  
  vector <float> weights;

  for (Parser::NamesakeIterator weights_iter=iter->First ("vertex_weights.input"); weights_iter; ++weights_iter)
  {
    const char *semantic    = get<const char*> (weights_iter, "semantic"),
               *source_name = get<const char*> (weights_iter, "source");

    if (!semantic)
    {
      LogError (iter, "No 'semantic' in vertex_weights input node");
      return;
    }
             
    if (!source_name)
    {
      LogError (iter, "No 'source' in vertex_weights input node");
      return;
    }
  
    source_name++;

    if (!::strcmp (semantic, "WEIGHT"))
    {
      for (Parser::NamesakeIterator i = iter->First("source"); i; ++i)
        if (test (i, "id", source_name))
        {
          ParseFloatArray (i, &weights);
          break;
        }

      if (weights.empty ())
        LogWarning (iter, "No weights");
    }
  }

  vector <int> indices (skin.WeightsCount () * 2);

  if (read_range (iter, "vertex_weights.v.#text", indices.begin (), skin.WeightsCount () * 2) != skin.WeightsCount () * 2)
  {
    LogError (iter, "Error at read 'vertex_weights.v.#text' items");
    return;
  }

  VertexJointWeight* skin_weights = skin.Weights ();

  for (size_t i = 0; i < skin.WeightsCount (); i++)
  {
    skin_weights[i].joint  = indices[i * 2];
    skin_weights[i].weight = weights[indices[i * 2 + 1]];
  }

  //нормализация
  size_t cur_index = 0;
  for (size_t i = 0; i < per_vertex_count.size (); i++)
  {
    float sum = 0;

    for (size_t j = 0; j < per_vertex_count[i]; j++)
      sum += skin_weights[cur_index + j].weight;

    sum = fabs (sum);
    
    for (size_t j = 0; j < per_vertex_count[i]; j++)
      skin_weights[cur_index + j].weight /= sum;

    cur_index += per_vertex_count[i];
  }
  
    //добавление скина в библиотеку
    
  model.Skins ().Insert (skin);
}
