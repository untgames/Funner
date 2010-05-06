#include "shared.h"

using namespace stl;
using namespace media::collada;

/*
    Разбор источника
*/

void DaeParser::ParseFloatArray (Parser::Iterator iter, stl::vector<float>& source)
{
  Parser::Iterator accessor_iter = get_first_child (*iter, "technique_common.accessor");    

  size_t data_count = get<size_t> (*iter, "float_array.count"),
         count      = get<size_t> (*accessor_iter, "count"),
         stride     = get<size_t> (*accessor_iter, "stride", 1);

  if (data_count < count * stride)
    raise_parser_exception (*accessor_iter, "Wrong count/stride attribute. count * stride > float_array.count. count=%u, stride=%u, "
    "float_array.count=%u", count, stride, data_count);

  source.resize (data_count);

  if (data_count)
    read (*iter, "float_array.#text", source.begin (), data_count);
}

void DaeParser::ParseIdrefArray (Parser::Iterator iter, stl::vector<string>& source)
{
  size_t data_count = get<size_t> (*iter, "IDREF_array.count");  

  source.resize (data_count);

  if (data_count)
    read (*iter, "IDREF_array.#text", source.begin (), data_count);
}

/*
    Разбор морфа
*/

void DaeParser::ParseMorph (Parser::Iterator iter, const char* id)
{
  const char* base_mesh = get<const char*> (*iter, "source");

  stl::vector<string> targets;
  stl::vector<float>  weights;

  size_t target_source_count = 0, weight_source_count = 0;

  if (!iter->First ("targets"))
    raise_parser_exception (*iter, "No 'targets' sub-tag");

  base_mesh++; //убираем префиксный '#'

  Mesh* mesh = model.Meshes ().Find (base_mesh);

  if (!mesh)
    raise_parser_exception (*iter, "Incorrect url '%s'. No mesh in library", base_mesh);

    //создание морфера

  Morph morph;
  
  morph.SetId (id);
  morph.SetBaseMesh (base_mesh);
  
    //разбор параметров морфера
   
  const char* method = get<const char*> (*iter, "method");

  if (!strcmp (method, "RELATIVE"))
  {
    morph.SetMethod (MorphMethod_Relative);
  }
  else
  {
    morph.SetMethod (MorphMethod_Normalized);
  }

  for (Parser::NamesakeIterator targets_iter=iter->First ("targets.input"); targets_iter; ++targets_iter)
  {
    const char *semantic    = get<const char*> (*targets_iter, "semantic"),
               *source_name = get<const char*> (*targets_iter, "source");

    source_name++; //избавляемся от префиксого '#'

    if (!::strcmp (semantic, "MORPH_TARGET"))
    {
      if (target_source_count++)
        raise_parser_exception (*iter, "Only one input for 'MORPH_TARGET' allowed");

      for (Parser::NamesakeIterator i = iter->First ("source"); i; ++i)
      {
        const char* source_id = get<const char*> (*i, "id", "");
        
        if (*source_id && !strcmp (source_id, source_name))
        {
          ParseIdrefArray (i, targets);
          break;
        }
      }

      if (targets.empty ())
        iter->Log ().Warning (*iter, "No targets");
    }
    else if (!::strcmp (semantic, "MORPH_WEIGHT"))
    {
      if (weight_source_count++)
        raise_parser_exception (*iter, "Only one input for 'MORPH_WEIGHT' allowed");

      for (Parser::NamesakeIterator i = iter->First ("source"); i; ++i)
      {
        const char* source_id = get<const char*> (*i, "id", "");

        if (*id && !strcmp (source_id, source_name))
        {
          ParseFloatArray (i, weights);
          break;
        }
      }

      if (weights.empty ())
        iter->Log ().Warning (*iter, "No weights");
    }
  }

  if (targets.size () != weights.size ())
    raise_parser_exception (*iter, "Different count of targets and weights");

  for (size_t i = 0; i < targets.size (); i++)
  {
    Mesh* mesh = model.Meshes ().Find (targets [i].c_str ());

    if (!mesh)
      raise_parser_exception (*iter, "Incorrect url '%s'. No mesh in library", targets [i].c_str ());

      //создание цели

    MorphTarget target;

    target.SetMesh   (targets [i].c_str ());
    target.SetWeight (weights [i]);

      //добавление цели в морфер
      
    morph.Targets ().Add (target);
  }
  
    //добавление морфера в библиотеку
    
  model.Morphs ().Insert (id, morph);
}
