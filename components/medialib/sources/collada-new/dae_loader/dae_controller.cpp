#include <stl/vector>
#include "shared.h"

/*
    –азбор библиотеки контроллеров
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
    –азбор контроллера
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

      mesh = &morph->BaseMesh ();
    }  
    
    size_t influence_count = 0;

    if (!CheckedRead (skin_iter, "vertex_weights.count", influence_count))
    {
      LogError (skin_iter, "Error at read 'vertex_weights.count'");
      return;
    }

    stl::vector <size_t> per_vertex_count (influence_count), vertex_first_weight (influence_count);

    if (read_range (skin_iter, "vertex_weights.vcount.#text", per_vertex_count.begin (), influence_count) != influence_count)
    {
      LogError (skin_iter, "Error at read 'vertex_weights.vcount' items");
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
      
        //не выполнение этого услови€ теоретически невозможно

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
          LogError (skin_iter->First ("vertex_weights.vcount"), "Vertex index %u is greater than influence_count=%u", *index, influence_count);
          surface.InfluenceChannels ().Remove (channel);

          break;
        }

        influence->first_weight  = vertex_first_weight [*index];
        influence->weights_count = per_vertex_count [*index];
      }
    }
    
      //создание скина

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
