#include "shared.h"

using namespace stl;
using namespace medialib::collada;
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

void DaeParser::ParseSkin (Parser::Iterator iter, Skin& skin, stl::vector <size_t>& per_vertex_count)
{
  vector <string> joints;
  vector <mat4f>  inv_matrixes;
  mat4f           bind_shape_matrix (1);

  LogScope scope (iter, *this);

  if (test (iter, "bind_shape_matrix"))
    if (!CheckedRead (iter, "bind_shape_matrix.#text", bind_shape_matrix))
    {
      LogError (iter, "Uncorrect 'bind_shape_matrix' tag");
      return;
    }
  skin.SetBindShapeMatrix (bind_shape_matrix);

  for (Parser::NamesakeIterator joints_iter=iter->First ("joints")->First ("input"); joints_iter; ++joints_iter)
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
      for (Parser::NamesakeIterator i = iter->First("source"); i; ++i)
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

  for (Parser::NamesakeIterator weights_iter=iter->First ("vertex_weights")->First ("input"); weights_iter; ++weights_iter)
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
}
