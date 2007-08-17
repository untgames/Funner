#include <stl/vector>
#include "shared.h"

/*
    Разбор библиотеки контроллеров
*/

void DaeParser::ParseLibraryControllers (Parser::Iterator iter)
{
  if (!test (iter, "controller"))
  {
    parse_log.Warning (iter, "Incorrect 'library_controllers' node. Must be at least one 'controller' sub-tag");
    return;
  }
  
  LogScope scope (iter, *this);

  for_each_child (iter, "controller", bind (&DaeParser::ParseController, this, _1));
}

/*
    Разбор контроллера
*/

void DaeParser::ParseController (Parser::Iterator iter)
{
  const char* id = get<const char*> (iter, "id");
  
  LogScope scope (iter, *this);
  
  if (!id)
  {
    LogError (iter, "No id");
    return;
  }
  
  Parser::Iterator skin_iter  = iter->First ("skin"),
                   morph_iter = iter->First ("morph");
  
  if (skin_iter && morph_iter)
    LogError (skin_iter->NextNamesake (), "Only one 'skin' or 'morph' sub-tag allowed");          
  if (!skin_iter && !morph_iter)
    LogError (skin_iter->NextNamesake (), "Must be at least one 'skin' or 'morph' sub-tag");          

  if (skin_iter)
  { 
    if (skin_iter->NextNamesake ())
      LogError (skin_iter->NextNamesake (), "Only one 'skin' sub-tag allowed");          

    if (!test (skin_iter, "vertex_weights"))
    {
      LogError (skin_iter, "No 'vertex_weights' sub-tag");
      return;
    }

    const char* base_mesh = get<const char*> (skin_iter, "source");

    if (!base_mesh)
    {
      LogError (skin_iter, "No base mesh ('source' property)");
      return;
    }

    base_mesh++; //убираем префиксный '#'

    Mesh* mesh = model.Meshes ().Find (base_mesh);
    Morph* morph = model.Morphs ().Find (base_mesh);
    
    if (!mesh)
    {
      if (!morph)
      {
        LogError (skin_iter, "Incorrect url '%s'. No mesh or morph in library", base_mesh);
        return;
      }

      mesh = &(morph->BaseMesh ());
    }
  
    size_t influence_channel_index = mesh->Surfaces()[0].InfluenceChannels ().Create (id);
    VertexInfluence* vertex_influences = mesh->Surfaces()[0].InfluenceChannels ().Data (influence_channel_index);

    size_t vertex_joint_weights_count = 0, vertex_count;

    if (!CheckedRead (skin_iter, "vertex_weights.count", vertex_count))
    {
      LogError (skin_iter, "Error at read 'vertex_weights.count'");
      return;
    }

    stl::vector <size_t> per_vertex_count (vertex_count), vertex_start_weight (vertex_count);

    if (read_range (skin_iter, "vertex_weights.vcount.#text", per_vertex_count.begin (), vertex_count) != vertex_count)
    {
      LogError (skin_iter, "Error at read 'vertex_weights.vcount' items");
      return;
    }

    for (size_t i = 0; i < vertex_count; i++)
    {
      vertex_start_weight[i] = vertex_joint_weights_count;
      vertex_joint_weights_count += per_vertex_count[i];
    }

    size_t* vertex_indices = vertex_index_maps[mesh->EntityId ()]->Indices ();

    for (size_t i = 0; i < vertex_index_maps[mesh->EntityId ()]->Size (); i++)
    {
      vertex_influences[i].first_weight  = vertex_start_weight[vertex_indices[i]];
      vertex_influences[i].weights_count = per_vertex_count[vertex_indices[i]];
    }

    Skin& skin = model.Skins().Create(id);
    skin.WeightsResize (vertex_joint_weights_count);
    skin.SetBaseMorph  (morph);

    ParseSkin (skin_iter, skin, per_vertex_count);
  }

  if (morph_iter)
  {
    if (morph_iter->NextNamesake ())
      LogError (morph_iter->NextNamesake (), "Only one 'morph' sub-tag allowed");          

    ParseMorph (morph_iter, id);
  }
}
