#include "h/shared.h"

void ColladaImpl::parse_source (Parser::Iterator p, vector <ColladaTexVertex> *destination, const Offsets *offsets, size_t index)
{
  for (Parser::NamesakeIterator i = p->First("source"); i; i++)
  {
    if(i->Test("id", offsets->tex_coord_source[index] + 1))
      for (Parser::NamesakeIterator j = i->First("float_array"); j; j++)
        parse_float_array (j, destination, PARSE_TEX_COORDS);
    if (offsets->tex_binormal_source.size() > index)
      if(i->Test("id", offsets->tex_binormal_source[index] + 1))
        for (Parser::NamesakeIterator j = i->First("float_array"); j; j++)
          parse_float_array (j, destination, PARSE_TEX_BINORMALS);
    if (offsets->tex_tangent_source.size () > index)
      if(i->Test("id", offsets->tex_tangent_source[index] + 1))
        for (Parser::NamesakeIterator j = i->First("float_array"); j; j++)
          parse_float_array (j, destination, PARSE_TEX_TANGENTS);
  }
}

void ColladaImpl::parse_source (Parser::Iterator p, vector <ColladaVertex> *destination, const Offsets *offsets)
{
  for (Parser::NamesakeIterator i = p->First("source"); i; i++)
  {
    if(i->Test("id", offsets->v_source + 1))
      parse_float_array (i->First("float_array"), destination, PARSE_VERTICES);
    if(i->Test("id", offsets->n_source + 1))
      parse_float_array (i->First("float_array"), destination, PARSE_NORMALS);
  }
}

void ColladaImpl::parse_source (Parser::Iterator p, vector <vec3f> *destination, const char *source)
{
  for (Parser::NamesakeIterator i = p->First("source"); i; i++)
    if (i->Test("id", source))
    {
      for (Parser::NamesakeIterator j = i->First("float_array"); j; j++)
        parse_float_array (j, destination);
      break;
    }
}

void ColladaImpl::parse_source (Parser::Iterator p, vector <vec4f> *destination, const char *source, float forth_component)
{
  for (Parser::NamesakeIterator i = p->First("source"); i; i++)
    if (i->Test("id", source))
    {
      parse_float_array (i, destination);
      break;
    }
}

void ColladaImpl::parse_source (Parser::Iterator p, vector <string> *destination, const char *source)
{
  for (Parser::NamesakeIterator i = p->First("source"); i; i++)
    if (i->Test("id", source))
    {
      for (Parser::NamesakeIterator j = i->First("Name_array"); j; j++)
        parse_Name_array (j, destination);
      break;
    }
}

void ColladaImpl::parse_source (Parser::Iterator p, vector <float>  *destination, const char *source)
{
  for (Parser::NamesakeIterator i = p->First("source"); i; i++)
    if (i->Test("id", source))
    {
      for (Parser::NamesakeIterator j = i->First("float_array"); j; j++)
        parse_float_array (j, destination);
      break;
    }
} 

void ColladaImpl::parse_source (Parser::Iterator p, vector <mat4f>  *destination, const char *source)
{
  for (Parser::NamesakeIterator i = p->First("source"); i; i++)
    if (i->Test("id", source))
    {
      for (Parser::NamesakeIterator j = i->First("float_array"); j; j++)
        parse_float_array (j, destination);
      break;
    }
} 

void ColladaImpl::parse_float_array (Parser::Iterator f_array, vector <ColladaTexVertex> *destination, _ParseDirective directive)
{
  size_t count = f_array->ReadUInt("count") / 3;
  vector <ColladaTexVertex>::iterator first_vertex;
  Parser::AttributeReader attr_reader = f_array->Reader("#text");

  if (destination->size() < count)
    destination->resize (count);

  first_vertex = destination->begin();

  switch (directive)
  {
    case PARSE_TEX_COORDS:
      for(size_t i = 0; i < count; i++)
        attr_reader.Read (first_vertex[i].coord);
      break;
    case PARSE_TEX_BINORMALS:
      for(size_t i = 0; i < count; i++)
        attr_reader.Read (first_vertex[i].binormal);
      break;
    case PARSE_TEX_TANGENTS:
      for(size_t i = 0; i < count; i++)
        attr_reader.Read (first_vertex[i].tangent);
      break;
    default:
      log->Error (f_array, "Uncorrect parse directive received");
  }
}

void ColladaImpl::parse_float_array (Parser::Iterator f_array, vector <ColladaVertex> *destination, _ParseDirective directive)
{
  size_t count = f_array->ReadUInt("count") / 3;
  vector <ColladaVertex>::iterator first_vertex;
  Parser::AttributeReader attr_reader = f_array->Reader("#text");

  if (destination->size() < count)
    destination->resize (count);

  first_vertex = destination->begin ();

  switch (directive)
  {
    case PARSE_VERTICES:
      for(size_t i = 0; i < count; i++)
        attr_reader.Read (first_vertex[i].coord);
      break;
    case PARSE_NORMALS:
      for(size_t i = 0; i < count; i++)
        attr_reader.Read (first_vertex[i].normal);
      break;
    default:
      log->Error (f_array, "Uncorrect parse directive received");
  }
}

void ColladaImpl::parse_float_array (Parser::Iterator f_array, vector <vec3f> *destination)
{
  size_t count = f_array->ReadUInt("count") / 3, destination_size = destination->size();

  destination->resize(destination_size + count);

  f_array->ReadArray ("#text", destination->begin() + destination_size, count);
}

void ColladaImpl::parse_float_array (Parser::Iterator source, vector <vec4f> *destination, float forth_component)
{
  size_t count, destination_size = destination->size(), stride;
  vector <vec4f>::iterator first_vec;

  if(!source->Present("float_array") || !source->Present("technique_common"))
  {
    log->Error(source, "No 'float_array' or 'technique_common' sub-tag");
    return;
  }
  if(!source->First("technique_common")->Present("accessor"))
  {
    log->Error(source->First("technique_common"), "No 'accessor' sub-tag");
    return;
  }

  source->First("technique_common")->First("accessor")->Read("stride", stride);
  count = source->First ("float_array")->ReadUInt ("count") / stride;

  switch (stride)
  {
    case 4:
      destination->resize(destination_size + count);
      source = source->First("float_array");
      source->ReadArray ("#text", destination->begin() + destination_size, count);
      break;
    case 3:
      destination->resize(destination_size + count, vec4f(0,0,0,forth_component));
      first_vec = &((destination->begin())[destination_size]);
      source = source->First("float_array");
      source->ReadArray ("#text", first_vec, count, 3);
      break;
    default: 
      log->Error(source, "Unexpected stride");
  }
}

void ColladaImpl::parse_float_array (Parser::Iterator f_array, vector <float>  *destination)
{
  size_t count = f_array->ReadUInt("count"), destination_size = destination->size();

  destination->resize (destination_size + count);
  f_array->ReadArray ("#text", destination->begin() + destination_size, count);
}

void ColladaImpl::parse_float_array (Parser::Iterator f_array, vector <mat4f>  *destination)
{
  size_t count = f_array->ReadUInt("count") / 16, destination_size = destination->size();

  destination->resize (destination_size + count);
  f_array->ReadArray ("#text", destination->begin() + destination_size, count);
}

void ColladaImpl::parse_Name_array  (Parser::Iterator n_array, vector <string> *destination)
{
  size_t count = n_array->ReadUInt("count"), destination_size = destination->size();

  destination->resize (destination_size + count);
  n_array->ReadArray ("#text", destination->begin() + destination_size, count);
}
