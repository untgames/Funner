#include "shared.h"

using namespace stl;
using namespace media::collada;
using namespace math;

/*
    –азбор источника
*/

void DaeParser::ParseFloatArray (Parser::Iterator iter, stl::vector<mat4f>& source)
{
  Parser::Iterator accessor_iter = get_first_child (*iter, "technique_common.accessor");  

  size_t data_count = get<size_t> (*iter, "float_array.count"),
         count      = get<size_t> (*accessor_iter, "count"),
         stride     = get<size_t> (*accessor_iter, "stride");

  if (data_count < count * stride)
    raise_parser_exception (*accessor_iter, "Wrong count/stride attribute. count * stride > float_array.count. count=%u, stride=%u, "
              "float_array.count=%u", count, stride, data_count);

  source.resize (count);

  if (count)
    read (*iter, "float_array.#text", source.begin (), count);
}

void DaeParser::ParseNameArray (Parser::Iterator iter, stl::vector<string>& source)
{
  size_t data_count = get<size_t> (*iter, "Name_array.count");
  
  source.resize (data_count);

  if (data_count)
    read (*iter, "Name_array.#text", source.begin (), data_count);
}

/*
    –азбор скина
*/

void DaeParser::ParseSkin (Parser::Iterator iter, const char* id)
{
  stl::vector<string> joints;
  stl::vector<mat4f>  inv_matrixes;
  mat4f               bind_shape_matrix (1);

  common::ParseNode vertex_weights_node = get_first_child (*iter, "vertex_weights");

  const char* base_mesh = get<const char*> (*iter, "source");

  base_mesh++; //убираем префиксный '#'

  Mesh*  mesh  = model.Meshes ().Find (base_mesh);
  Morph* morph = model.Morphs ().Find (base_mesh);

  if (!mesh)
  {
    if (!morph)
      raise_parser_exception (*iter, "Incorrect url '%s'. No mesh or morph in library", base_mesh);

    mesh      = model.Meshes ().Find (morph->BaseMesh ());
    base_mesh = morph->BaseMesh ();
  }

  if (common::ParseNode param_node = iter->First ("bind_shape_matrix"))
    read (param_node, "#text", bind_shape_matrix);

  size_t influence_count = get<size_t> (vertex_weights_node, "count");

  stl::vector<size_t> per_vertex_count (influence_count),
                      vertex_first_weight (influence_count);

  read (vertex_weights_node, "vcount.#text", per_vertex_count.begin (), influence_count);

  size_t vertex_joint_weights_count = 0;

  for (size_t i=0; i<influence_count; i++)
  {
    vertex_first_weight [i]     = vertex_joint_weights_count;
    vertex_joint_weights_count += per_vertex_count [i];
  }
 
  for (size_t i=0; i<mesh->Surfaces ().Size (); i++)
  {
    Surface&          surface            = mesh->Surfaces ()[i];
    VertexIndexMapPtr vertex_indices_map = GetVertexIndicesMap (base_mesh, i);
    
      //невыполнение этого услови€ теоретически невозможно
      
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
        raise_parser_exception (vertex_weights_node.First ("vcount"), "Vertex index %u is greater than influence_count=%u",
          *index, influence_count);

        surface.InfluenceChannels ().Remove (channel);

        break;
      }

      influence->first_weight  = vertex_first_weight [*index];
      influence->weights_count = per_vertex_count [*index];
    }
  }

    //создание скина

  Skin skin;  

  skin.SetId              (id);  
  skin.WeightsResize      (vertex_joint_weights_count);
  skin.SetBaseMesh        (base_mesh);
  skin.SetBindShapeMatrix (bind_shape_matrix);  

  for (Parser::NamesakeIterator joints_iter=iter->First ("joints.input"); joints_iter; ++joints_iter)
  {
    const char *semantic    = get<const char*> (*joints_iter, "semantic"),
               *source_name = get<const char*> (*joints_iter, "source");

    source_name++; //избавл€емс€ от префиксного '#'

    if (!::strcmp (semantic, "JOINT"))
    {
      for (Parser::NamesakeIterator i = iter->First ("source"); i; ++i)
      {
        const char* source_id = get<const char*> (*i, "id", "");
        
        if (*source_id && !strcmp (source_id, source_name))
        {
          ParseNameArray (i, joints);
          break;
        }
      }

      if (joints.empty ())
        iter->Log ().Warning (*iter, "No joints");
    }
    else if (!::strcmp (semantic, "INV_BIND_MATRIX"))
    {
      for (Parser::NamesakeIterator i = iter->First("source"); i; ++i)
      {
        const char* source_id = get<const char*> (*i, "id", "");        
        
        if (*source_id && !strcmp (source_id, source_name))
        {
          ParseFloatArray (i, inv_matrixes);
          break;
        }
      }

      if (inv_matrixes.empty ())
        iter->Log ().Warning (*iter, "No inv_matrixes");
    }
  }

  if (joints.size () != inv_matrixes.size ())
    raise_parser_exception (*iter, "Different count of joints and inv_matrixes");

  for (size_t i = 0; i < joints.size (); i++)
  {
    skin.CreateJoint (joints[i].c_str ());
    skin.SetJointInvMatrix (i, inv_matrixes[i]);
  }
  
  stl::vector<float> weights;

  for (Parser::NamesakeIterator weights_iter=vertex_weights_node.First ("input"); weights_iter; ++weights_iter)
  {
    const char *semantic    = get<const char*> (*weights_iter, "semantic"),
               *source_name = get<const char*> (*weights_iter, "source");
  
    source_name++; //избавл€емс€ от префиксного '#'

    if (!::strcmp (semantic, "WEIGHT"))
    {
      for (Parser::NamesakeIterator i = iter->First ("source"); i; ++i)
      {
        const char* source_id = get<const char*> (*i, "id", "");        
        
        if (*source_id && !strcmp (source_id, source_name))        
        {
          ParseFloatArray (i, weights);
          break;
        }
      }

      if (weights.empty ())
        iter->Log ().Warning (*iter, "No weights");
    }
  }

  stl::vector<int> indices (skin.WeightsCount () * 2);

  read (vertex_weights_node, "v.#text", indices.begin (), skin.WeightsCount () * 2);

  VertexJointWeight* skin_weights = skin.Weights ();

  for (size_t i = 0; i < skin.WeightsCount (); i++)
  {
    skin_weights [i].joint  = indices [i * 2];
    skin_weights [i].weight = weights [indices [i * 2 + 1]];
  }

    //нормализаци€

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

  model.Skins ().Insert (id, skin);
}
