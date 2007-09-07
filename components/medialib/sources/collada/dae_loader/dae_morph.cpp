#include "shared.h"

using namespace stl;
using namespace media::collada;

/*
    Разбор источника
*/

void DaeParser::ParseFloatArray (Parser::Iterator iter, vector <float> *source)
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

  source->resize (data_count);

  if (read_range (iter, "float_array.#text", source->begin (), data_count) != data_count)
  {
    LogError (iter, "Error at read 'float_array' items");
    return;
  }
}

void DaeParser::ParseIdrefArray (Parser::Iterator iter, vector <string> *source)
{
  LogScope scope (iter, *this);
  
  size_t data_count = 0;
  
  if (!CheckedRead (iter, "IDREF_array.count", data_count))
  {
    LogError (iter, "Error at read 'IDREF_array.count'");
    return;
  }
  
  source->resize (data_count);

  if (read_range (iter, "IDREF_array.#text", source->begin (), data_count) != data_count)
  {
    LogError (iter, "Error at read 'IDREF_array' items");
    return;
  }
}

/*
    Разбор морфа
*/

void DaeParser::ParseMorph (Parser::Iterator iter, const char* id)
{
  const char* base_mesh = get<const char*> (iter, "source");
  vector <string> targets;
  vector <float>  weights;
  int target_source_count = 0, weight_source_count = 0;

  if (!base_mesh)
  {
    LogError (iter, "No base mesh ('source' property)");
    return;
  }

  if (!test (iter, "targets"))
  {
    LogError (iter, "No 'targets' sub-tag");
    return;
  }

  base_mesh++; //убираем префиксный '#'

  Mesh* mesh = model.Meshes ().Find (base_mesh);
  
  if (!mesh)
  {
    LogError (iter, "Incorrect url '%s'. No mesh in library", base_mesh);
    return;
  }

  Morph& morph = model.Morphs().Create(*mesh, id);
  
  LogScope scope (iter, *this);

  if (test (iter, "method", "RELATIVE"))
    morph.SetMethod (MorphMethod_Relative);
  else
    morph.SetMethod (MorphMethod_Normalized);

  for (Parser::NamesakeIterator targets_iter=iter->First ("targets")->First ("input"); targets_iter; ++targets_iter)
  {
    const char *semantic    = get<const char*> (targets_iter, "semantic"),
               *source_name = get<const char*> (targets_iter, "source");

    if (!semantic)
    {
      LogError (iter, "No 'semantic' in targets input node");
      return;
    }
             
    if (!source_name)
    {
      LogError (iter, "No 'source' in targets input node");
      return;
    }
  
    source_name++;

    if (!::strcmp (semantic, "MORPH_TARGET"))
    {
      if (target_source_count++)
      {
        LogError (iter, "Only one input for 'MORPH_TARGET' allowed");
        return;
      }

      for (Parser::NamesakeIterator i = iter->First("source"); i; ++i)
        if (test (i, "id", source_name))
        {
          ParseIdrefArray (i, &targets);
          break;
        }

      if (targets.empty ())
        LogWarning (iter, "No targets");
    }
    else if (!::strcmp (semantic, "MORPH_WEIGHT"))
    {
      if (weight_source_count++)
      {
        LogError (iter, "Only one input for 'MORPH_WEIGHT' allowed");
        return;
      }

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

  if (targets.size () != weights.size ())
  {
    LogError (iter, "Different count of targets and weights");
    return;
  }

  for (size_t i = 0; i < targets.size (); i++)
  {
    Mesh* mesh = model.Meshes ().Find (targets[i].c_str ());
    
    if (!mesh)
    {
      LogError (iter, "Incorrect url '%s'. No mesh in library", targets[i].c_str ());
      return;
    }

    morph.Targets ().Create (*mesh, weights[i]);
  }
}
