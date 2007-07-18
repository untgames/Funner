#include "h/shared.h"
#include <stl/hash_map>
#include <stl/algorithm>
#include <common/hash.h>

Offsets::Offsets ()
{
  v_offset = n_offset = color_offset = -1;
  max_offset = 0;
  v_source = n_source = color_source = NULL;  
}

ColladaMesh::ColladaMesh (const ColladaMeshImpl* source)
  : ColladaEntity(source), impl (source)
  { }

size_t ColladaMesh::SurfacesCount () const
{
  return impl->surfaces.size();
}

ColladaSurface ColladaMesh::Surface (size_t index) const
{
  if ((impl->surfaces.empty()) || (index >= impl->surfaces.size()))
  {
    RaiseOutOfRange ("ColladaMesh::Surface", "index", index, (size_t)0, impl->surfaces.size());
    return ColladaWrappers::Create <ColladaSurface> (&ColladaSurfaceImpl());
  }
  return ColladaWrappers::Create <ColladaSurface> (&impl->surfaces[index]);
}

ColladaSurface::ColladaSurface (const ColladaSurfaceImpl* source)
  : ColladaEntity (source), impl (source)
  { }

const char* ColladaSurface::Material () const
{
  return impl->material.c_str();
}

ColladaPrimitiveType ColladaSurface::PrimitiveType () const
{
  return impl->type;
}     
    
size_t ColladaSurface::VerticesCount () const
{
  return impl->vertices.size();
}

size_t ColladaSurface::TextureChannels () const
{
  return impl->tex_data.size();
}

const char* ColladaSurface::TextureChannelName (size_t index) const
{
  if ((impl->tex_data.empty()) || (index >= impl->tex_data.size()))
  {
    RaiseOutOfRange ("ColladaSurface::TextureChannelName", "index", index, (size_t)0, impl->tex_data.size());
    return NULL;
  }
  return  impl->tex_data[index].channel_name.c_str();
}

const ColladaVertex* ColladaSurface::Vertices () const
{
  return &impl->vertices.front();
}

const ColladaTexVertex* ColladaSurface::TextureVertices (size_t channel) const
{
  if ((impl->tex_data.empty()) || (channel >= impl->tex_data.size()))
  {
    RaiseOutOfRange ("ColladaSurface::TextureVertices", "channel", channel, (size_t)0, impl->tex_data.size());
    return NULL;
  }
  return &impl->tex_data[channel].tex_vertices.front();
}

bool ColladaSurface::HasVertexColors () const
{
  return !impl->colors.empty();
}

const vec4f* ColladaSurface::VertexColors () const
{
  return &impl->colors.front();
}
    
size_t ColladaSurface::IndicesCount () const
{
  return impl->indexes.size();
}

const size_t* ColladaSurface::Indices () const
{
  return &impl->indexes.front();
}

void ColladaImpl::parse_library_geometries (Parser::Iterator p)
{
  if(!test (p, "geometry"))
  {
    log->Error(p, "Uncorrect 'library_geometries' tag. Must be at least one 'geometry' sub-tag");
    return;
  }
  for (Parser::NamesakeIterator i = p->First("geometry"); i; i++)
    parse_geometry (i);
}

void ColladaImpl::parse_geometry (Parser::Iterator p)
{
  Parser::NamesakeIterator i = p->First("mesh");

  if (i)
  {
    meshes.resize(meshes.size() + 1);
    read (p, "id", meshes.back().id, "No id");
    parse_mesh(i, &meshes.back());
    if (++i)
      log->Error(p, "Detected more than one child 'mesh'.");
  }
}

void ColladaImpl::parse_mesh (Parser::Iterator p, ColladaMeshImpl *destination)
{
  destination->line = p->LineNumber();
  if(!test (p, "vertices"))
  {
    log->Warning(p, "No vertices information, empty mesh created");
    return;
  }
  if(!test (p, "vertices.input"))
  {
    log->Warning(p, "Uncorrect vertices information, 'input' URL not detected, empty mesh created");
    return;
  }
  
  for (Parser::NamesakeIterator i = p->First("polygons"); i; i++) log->Error(i, "Polygons are not supported.");
  for (Parser::NamesakeIterator i = p->First("polylist"); i; i++) log->Error(i, "Polygons are not supported.");  
  
  for_each_child (p, "triangles", bind (&ColladaImpl::parse_triangles, this, _1, p, destination));
  for_each_child (p, "linestrips", bind (&ColladaImpl::parse_linestrips, this, _1, p, destination));
  for_each_child (p, "lines", bind (&ColladaImpl::parse_lines, this, _1, p, destination));
  for_each_child (p, "trifans", bind (&ColladaImpl::parse_trifans, this, _1, p, destination));
  for_each_child (p, "tristrips", bind (&ColladaImpl::parse_tristrips, this, _1, p, destination));
}

int ColladaImpl::parse_common_surface_attr (Parser::Iterator surface, Parser::Iterator mesh, ColladaSurfaceImpl *temp_surface, Offsets *offsets)
{
  read (surface, "material", temp_surface->material);

  if (!parse_inputs (surface, offsets))
    return -1;

  offsets->v_source = get<const char*> (mesh, "vertices.input.source");

  parse_source (mesh, &(temp_surface->vertices), offsets);

  if (offsets->color_offset != -1)
    parse_source (mesh, &(temp_surface->colors), &(offsets->color_source[1]), 0);

  if (temp_surface->vertices.empty())
  {
    log->Warning(mesh, "Vertices information was not loaded, information not detected or is empty, empty mesh created");
    return -1;
  }
  
  temp_surface->tex_data.resize (offsets->tex_coord_offset.size());  

  for (size_t j = 0; j < offsets->tex_coord_offset.size (); j++)
  {
    temp_surface->tex_data[j].channel_name = &((offsets->tex_coord_source[j])[1]);
    parse_source (mesh, &(temp_surface->tex_data[j].tex_vertices), offsets, j);
  }
  
  if (test (surface, "count"))
    return get<int> (surface, "count");
  else
  {
    log->Error(surface, "'Count' tag not specified.");
    return -1;
  }
}

void ColladaImpl::parse_triangles (Parser::Iterator triangles, Parser::Iterator mesh, ColladaMeshImpl *destination_mesh)
{
  int                triangles_count;
  Offsets            offsets;
  vector <PointInfo> temp_indexes;
  ColladaSurfaceImpl temp_surface, *destination;

  triangles_count = parse_common_surface_attr (triangles, mesh, &temp_surface, &offsets);
  if (triangles_count == -1) return;

  destination_mesh->surfaces.resize(destination_mesh->surfaces.size() + 1);
  destination = &destination_mesh->surfaces.back();

  destination->type = COLLADA_PRIMITIVE_TRIANGLE_LIST;
  destination->line = triangles->LineNumber();

  parse_p (triangles->First("p"), &offsets, &temp_indexes, triangles_count * 3);
  finalize_surface (&temp_surface, &temp_indexes, destination);
}

void ColladaImpl::parse_trifans (Parser::Iterator trifans, Parser::Iterator mesh, ColladaMeshImpl *destination_mesh)
{
  int                  trifans_count, i = 0;
  Parser::NamesakeIterator trifan;
  Offsets              offsets;
  vector <PointInfo>   temp_indexes;
  ColladaSurfaceImpl   temp_surface, *new_trifan;

  trifans_count = parse_common_surface_attr (trifans, mesh, &temp_surface, &offsets);
  if (trifans_count == -1) return;

  for(trifan = trifans->First("p"); trifan && (i < trifans_count); trifan++, i++)
  {
    temp_indexes.clear ();

    destination_mesh->surfaces.resize(destination_mesh->surfaces.size() + 1);
    new_trifan = &destination_mesh->surfaces.back();

    new_trifan->type = COLLADA_PRIMITIVE_TRIANGLE_FAN;
    new_trifan->line = trifan->LineNumber();

    parse_p (trifan, &offsets, &temp_indexes);
    finalize_surface (&temp_surface, &temp_indexes, new_trifan);
  }
}

void ColladaImpl::parse_tristrips (Parser::Iterator tristrips, Parser::Iterator mesh, ColladaMeshImpl *destination_mesh)
{
  int                  tristrips_count, i = 0;
  Parser::NamesakeIterator tristrip;
  Offsets              offsets;
  vector <PointInfo>   temp_indexes;
  ColladaSurfaceImpl   temp_surface, *new_tristrip;

  tristrips_count = parse_common_surface_attr (tristrips, mesh, &temp_surface, &offsets);
  if (tristrips_count == -1) return;

  for(tristrip = tristrips->First("p"); tristrip && (i < tristrips_count); tristrip++, i++)
  {
    temp_indexes.clear ();

    destination_mesh->surfaces.resize(destination_mesh->surfaces.size() + 1);
    new_tristrip = &destination_mesh->surfaces.back();

    new_tristrip->type = COLLADA_PRIMITIVE_TRIANGLE_STRIP;
    new_tristrip->line = tristrip->LineNumber();

    parse_p (tristrip, &offsets, &temp_indexes);
    finalize_surface (&temp_surface, &temp_indexes, new_tristrip);
  }
}

void ColladaImpl::parse_linestrips (Parser::Iterator linestrips, Parser::Iterator mesh, ColladaMeshImpl *destination_mesh)
{
  int                  linestrips_count, i = 0;
  Parser::NamesakeIterator strip;
  Offsets              offsets;
  vector <PointInfo>   temp_indexes;
  ColladaSurfaceImpl   temp_surface, *new_linestrip;

  linestrips_count = parse_common_surface_attr (linestrips, mesh, &temp_surface, &offsets);
  if (linestrips_count == -1) return;

  for(strip = linestrips->First("p"); strip && (i < linestrips_count); strip++, i++)
  {
    temp_indexes.clear ();

    destination_mesh->surfaces.resize(destination_mesh->surfaces.size() + 1);
    new_linestrip = &destination_mesh->surfaces.back();

    new_linestrip->type = COLLADA_PRIMITIVE_LINE_STRIP;
    new_linestrip->line = strip->LineNumber();

    parse_p (strip, &offsets, &temp_indexes);
    finalize_surface (&temp_surface, &temp_indexes, new_linestrip);
  }
}

void ColladaImpl::parse_lines (Parser::Iterator lines, Parser::Iterator mesh, ColladaMeshImpl *destination_mesh)
{
  int                lines_count;
  Offsets            offsets;
  vector <PointInfo> temp_indexes;
  ColladaSurfaceImpl temp_surface, *destination;

  lines_count = parse_common_surface_attr (lines, mesh, &temp_surface, &offsets);  
  if (lines_count == -1) return;

  destination_mesh->surfaces.resize(destination_mesh->surfaces.size() + 1);
  destination = &destination_mesh->surfaces.back();

  destination->line = lines->LineNumber ();
  destination->type = COLLADA_PRIMITIVE_LINE_LIST;

  parse_p (lines->First("p"), &offsets, &temp_indexes, lines_count * 2);

  finalize_surface (&temp_surface, &temp_indexes, destination);
}

struct Key
{
  int vindex, nindex, cindex;
  int tex_coord_index[8], tex_bi_index[8], tex_tan_index[8];
};

struct _equal 
{
   bool operator()(const Key & a, const Key & b) const 
   {
      if (!memcmp(&a, &b, sizeof (Key)))
         return true;
      return false;
   }
};

struct HashKey
{
  size_t operator()(const Key & key) const 
  {
    return crc32(&key, sizeof (Key));
  }
};

void ColladaImpl::finalize_surface (ColladaSurfaceImpl *temp_surface, vector <PointInfo> *temp_indexes, ColladaSurfaceImpl *destination)
{  
  hash_map<Key,size_t, HashKey, _equal> map;
  size_t                                index = 0, texture_count;
  PointInfo                             *first_index = &temp_indexes->front();

  destination->material = temp_surface->material;
  destination->indexes.resize   (temp_indexes->size());

  texture_count = temp_surface->tex_data.size();
  if (texture_count > 8)
    printf("Critical error, more than 8 texture channels per surface not supported!\n");

  for (size_t i = 0; i < temp_indexes->size(); i++)
  {
    Key key = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    key.vindex = first_index[i].vindex;
    key.nindex = first_index[i].nindex;
    key.cindex = first_index[i].cindex;
    if (texture_count)
    {
      for(size_t j = 0; (j < temp_surface->tex_data.size()) && (j < 8); j++)
      {
        key.tex_coord_index[j] = first_index[i].tex_coord_index[j];
        key.tex_bi_index[j]  = first_index[i].tex_bi_index[j];
        key.tex_tan_index[j] = first_index[i].tex_tan_index[j];
      }
    }

    hash_map<Key,size_t>::iterator iter = map.find (key);
    if (iter == map.end ())
      iter = map.insert_pair(key, index++).first;
    destination->indexes[i] = iter->second;
  }

  destination->vertices.resize(index);
  if (first_index->cindex != -1)
    destination->colors.resize(index);
  if (texture_count)  
  {
    destination->tex_data.resize(texture_count);
    for(size_t i = 0; i < texture_count; i++)
    {
      destination->tex_data[i].channel_name = temp_surface->tex_data[i].channel_name;
      destination->tex_data[i].tex_vertices.resize(index);
    }
  }

  for (hash_map<Key,size_t>::iterator i = map.begin(); i != map.end(); i++)
  {
    Key    cur_key   = i->first;
    size_t cur_index = i->second;

    destination->vertices[cur_index].coord    = temp_surface->vertices[cur_key.vindex].coord;
    destination->vertices[cur_index].normal   = temp_surface->vertices[cur_key.nindex].normal;
    if (first_index->cindex != -1)
      destination->colors[cur_index] = temp_surface->colors[cur_key.cindex];
    if (texture_count)  
      for(size_t j = 0; (j < texture_count) && (j < 8); j++)
      {
        destination->tex_data[j].tex_vertices[cur_index].coord    = temp_surface->tex_data[j].tex_vertices[cur_key.tex_coord_index[j]].coord;
        destination->tex_data[j].tex_vertices[cur_index].binormal = temp_surface->tex_data[j].tex_vertices[cur_key.tex_bi_index[j]].binormal;
        destination->tex_data[j].tex_vertices[cur_index].tangent  = temp_surface->tex_data[j].tex_vertices[cur_key.tex_tan_index[j]].tangent;
      }
  }
}

struct _SortSets
{
  size_t index, set;
};

bool _SortSetsLess (const _SortSets &a, const _SortSets &b)
{
  return a.set < b.set;
};

bool ColladaImpl::parse_inputs (Parser::Iterator p, Offsets *offsets)
{
  size_t cur_set = 0;
  
  for(Parser::NamesakeIterator i = p->First("input"); i; i++)
  {
    if(test (i, "semantic", "VERTEX"))
      read (i, "offset", offsets->v_offset);
    if(test (i, "semantic", "NORMAL"))
    {
      read (i, "offset", offsets->n_offset);
      read (i, "source", offsets->n_source);
    }
    if(test (i, "semantic", "COLOR"))
    {
      read (i, "offset", offsets->color_offset);
      read (i, "source", offsets->color_source);
    }
    if(test (i, "semantic", "TEXCOORD"))
    {
      offsets->tex_coord_offset.push_back (get<int> (i, "offset"));
      offsets->tex_coord_source.push_back ((char*)get<const char*> (i, "source"));
      read (i, "set", cur_set);
      offsets->tex_coord_set.push_back (cur_set);
      offsets->max_offset = max (offsets->max_offset, (short)offsets->tex_coord_offset.back());
    }
    if(test (i, "semantic", "TEXTANGENT"))
    {
      offsets->tex_tangent_offset.push_back (get<int> (i, "offset"));
      offsets->tex_tangent_source.push_back ((char*)get<const char*> (i, "source"));
      if(test (i, "set"))
        offsets->tex_tangent_set.push_back (get<int> (i, "set"));
      else
        offsets->tex_tangent_set.push_back (cur_set);
      offsets->max_offset = max (offsets->max_offset, (short)offsets->tex_tangent_offset.back());
    }
    if(test (i, "semantic", "TEXBINORMAL"))
    {
      offsets->tex_binormal_offset.push_back (get<int> (i, "offset"));
      offsets->tex_binormal_source.push_back ((char*)get<const char*> (i, "source"));
      if(test (i, "set"))
        offsets->tex_binormal_set.push_back (get<int> (i, "set"));
      else
        offsets->tex_binormal_set.push_back (cur_set);                                
      offsets->max_offset = max (offsets->max_offset, (short)offsets->tex_binormal_offset.back());
    }
    else
    {
      if (get<int> (i, "offset") > offsets->max_offset) offsets->max_offset = get<int> (i, "offset");
    }
  }

  if(!offsets->tex_coord_set.empty())
  {
    if(offsets->tex_tangent_set.empty())
    {
      log->Error (p, "texture tangents not detected");
      return false;
    }
    if(offsets->tex_binormal_set.empty())
    {
      log->Error (p, "texture binormals not detected");
      return false;
    }
  }

  for (size_t i = 0; i < offsets->tex_coord_set.size(); i++)
  {
    if(!offsets->tex_tangent_set.empty())
    {
      bool set_exist = false;

      for(size_t j = 0; j < offsets->tex_coord_set.size(); j++)
      {
        if(offsets->tex_tangent_set[i] == offsets->tex_coord_set[j])
        {
          set_exist = true;
          break;
        }
      }
      if(set_exist) continue;
      offsets->tex_tangent_set[i] = offsets->tex_coord_set[i];
    }
    if(!offsets->tex_binormal_set.empty())
    {
      bool set_exist = false;

      for(size_t j = 0; j < offsets->tex_coord_set.size(); j++)
      {
        if(offsets->tex_binormal_set[i] == offsets->tex_coord_set[j])
        {
          set_exist = true;
          break;
        }
      }
      if(set_exist) continue;
      offsets->tex_binormal_set[i] = offsets->tex_coord_set[i];
    }
  }

  vector <_SortSets> ss(offsets->tex_coord_set.size());

  for (size_t i = 0; i < offsets->tex_coord_set.size(); i++)
  {
    ss[i].index = i;
    ss[i].set   = offsets->tex_coord_set[i];
  }
                                          
  sort (ss.begin(), ss.end(), _SortSetsLess);
  sort (offsets->tex_coord_set.begin(), offsets->tex_coord_set.end());
  sort (offsets->tex_binormal_set.begin(), offsets->tex_binormal_set.end());
  sort (offsets->tex_tangent_set.begin(), offsets->tex_tangent_set.end());

  for (size_t i = 0; i < offsets->tex_coord_set.size(); i++)
    if(ss[i].index != i)
    {
      size_t j = i;
      
      for (; j != ss[j].index; j++);
      
      swap (offsets->tex_coord_offset[i], offsets->tex_coord_offset[j]);
      swap (offsets->tex_coord_source[i], offsets->tex_coord_source[j]);
      if(!offsets->tex_tangent_set.empty())
      {
        swap (offsets->tex_binormal_offset[i], offsets->tex_binormal_offset[j]);
        swap (offsets->tex_binormal_source[i], offsets->tex_binormal_source[j]);
      }
      if(!offsets->tex_binormal_set.empty())
      {
        swap (offsets->tex_tangent_offset[i], offsets->tex_tangent_offset[j]);
        swap (offsets->tex_tangent_source[i], offsets->tex_tangent_source[j]);
      }
    }

  offsets->max_offset = max(max(offsets->max_offset, offsets->v_offset), max(offsets->n_offset, offsets->color_offset));

  return true;
}

void ColladaImpl::parse_p (Parser::Iterator p, const Offsets *offsets, vector<PointInfo> *temp_indexes, int v_count)
{
  size_t           block_size;
  Parser::Iterator text;
  PointInfo        *first_index;

  block_size = offsets->max_offset + 1;
  if (!test (p, "#text"))
  {
    log->Error (p, "'#text' sub-tag not detected");
    return;
  }
  text = p->First("#text");  

  if (v_count == -1)
    v_count = text->AttributesCount() / block_size;
  if (!v_count)
  {
    log->Error (p, "Empty tag");
    return;
  }  

  if (text->AttributesCount() - v_count * block_size)
  {
    log->Error (p, "Tag contain unexpected number of indexes");
    return;
  }  

  temp_indexes->resize (v_count);
  first_index = &temp_indexes->front();

  for (int i = 0; i < v_count; i++)
    first_index[i].vindex = atoi (text->Attribute(offsets->v_offset + block_size * i));

  for (int i = 0; i < v_count; i++)
    first_index[i].nindex = atoi (text->Attribute(offsets->n_offset + block_size * i));
  if (offsets->color_offset == -1)
    for (int i = 0; i < v_count; i++)
      first_index[i].cindex = -1;  
  else
    for (int i = 0; i < v_count; i++)
      first_index[i].cindex = atoi (text->Attribute(offsets->color_offset + block_size * i));

  size_t tex_sets_count = offsets->tex_coord_offset.size ();         //количество текстур
  
  if (!offsets->tex_coord_offset.empty())
    for (int i = 0; i < v_count; i++)
    {
      first_index[i].tex_coord_index.resize (tex_sets_count);
      for (size_t j = 0; j < offsets->tex_coord_offset.size (); j++)
        first_index[i].tex_coord_index[j] = atoi (text->Attribute(offsets->tex_coord_offset[j] + block_size * i));
    }
  if (!offsets->tex_tangent_offset.empty())
    for (int i = 0; i < v_count; i++)
    {
      first_index[i].tex_tan_index.resize (tex_sets_count);
      for (size_t j = 0; j < offsets->tex_tangent_offset.size (); j++)
        first_index[i].tex_tan_index[j] = atoi (text->Attribute(offsets->tex_tangent_offset[j] + block_size * i));
    }
  if (!offsets->tex_binormal_offset.empty())
    for (int i = 0; i < v_count; i++)
    {
      first_index[i].tex_bi_index.resize (tex_sets_count);
      for (size_t j = 0; j < offsets->tex_binormal_offset.size (); j++)
        first_index[i].tex_bi_index[j] = atoi (text->Attribute(offsets->tex_binormal_offset[j] + block_size * i));
    }
}
