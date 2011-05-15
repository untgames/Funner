#include <cfloat>
#include <cstdio>

#include <stl/hash_map>
#include <stl/string>

#include <xtl/common_exceptions.h>
#include <xtl/iterator.h>
#include <xtl/shared_ptr.h>

#include <common/file.h>
#include <common/strlib.h>
#include <common/xml_writer.h>

#include <bv/axis_aligned_box.h>

#include <media/animation/animation_library.h>
#include <media/physics/physics_library.h>
#include <media/rfx/material_library.h>

#include <media/collada.h>
#include <media/image.h>
#include <media/mesh.h>

using namespace common;
using namespace media::collada;

/*
    Константы
*/

const char* APPLICATION_NAME = "collada_converter";

const size_t HELP_STRING_PREFIX_LENGTH  = 30;

const float EPSILON = 0.001f;

/*
    Типы
*/

typedef stl::hash_map<stl::hash_key<const char*>, stl::string>           ImagesMap;
typedef stl::hash_map<stl::hash_key<const char*>, bound_volumes::aaboxf> BoundVolumesMap;

struct Params;

typedef void (*ProcessOption)(const char* arg, Params& params);

//опция
struct Option
{
  ProcessOption process;       //функция-обработчик опции
  const char*   name;          //имя команды
  char          short_name;    //короткое имя
  const char*   argument_name; //имя аргумента
  const char*   tip;           //подсказка
};

//параметры запуска
struct Params
{
  const Option* options;                      //массив опций
  size_t        options_count;                //количество опций
  stl::string   source_name;                  //имя исходного файла
  StringArray   source_search_paths;          //пути к каталогам поиск ресурсов
  stl::string   output_textures_dir_name;     //имя каталога с сохранёнными текстурами
  stl::string   output_textures_format;       //формат текстур
  stl::string   material_textures_format;     //формат текстур сохраняемых в материале
  stl::string   output_materials_file_name;   //файл материалов
  stl::string   output_scene_file_name;       //файл сцены
  stl::string   output_animations_file_name;  //файл анимаций
  stl::string   output_meshes_file_name;      //имя файла с сохранёнными мешами
  stl::string   output_phys_meshes_file_name; //имя файла с сохранёнными мешами для физики
  stl::string   convex_phys_meshes;           //список масок имен физических мешей, являющихся конвексами
  stl::string   output_remove_file_prefix;    //отбрасываемый префикс имён файлов
  stl::string   output_resources_namespace;   //пространство имён, применяемое при сохранении ресурсов
  stl::string   exclude_nodes;                //неэкспортируемые узлы сцены
  stl::string   merge_animation;              //имя анимации в которую должны быть вклеены все остальные анимации
  size_t        max_texture_size;             //максимальный размер текстуры
  bool          fix_zero_tangents;            //нужно ли генерировать вместо нулевых касательных произвольные
  bool          pot;                          //нужно ли масштабировать текстуры до ближайшей степени двойки
  bool          silent;                       //минимальное число сообщений
  bool          remove_unused_resources;      //нужно ли выбрасывать неиспользуемые ресурсы
  bool          print_help;                   //нужно ли печатать сообщение помощи
};

typedef xtl::shared_ptr<media::Image> TexturePtr;

/*
    Функции
*/

//получение ближайшей сверху степени двойки
size_t get_next_higher_power_of_two (size_t k)
{
  if (!k)
    return 1;

  if (!(k & (k-1)))
    return k;

  k--;

  for (size_t i=1; i < sizeof (size_t) * 8; i *= 2)
          k |= k >> i;

  return k + 1;
}

//печать ошибки с выходом из программы
void error (const char* format, ...)
{
  va_list args;

  va_start (args, format);

  printf  ("error: ");
  vprintf (format, args);
  printf  ("\n");
  fflush  (stdout);

  exit (1);
}

//получение подсказки по программе
void command_line_help (const char*, Params& params)
{
  printf ("%s [<OPTIONS>] <SOURCE> ...\n", APPLICATION_NAME);
  printf ("  OPTIONS:\n");

  for (size_t i = 0; i < params.options_count; i++)
  {
    const Option& option = params.options [i];

    stl::string prefix = common::format (option.argument_name ? "    --%s=<%s>" : "    --%s", option.name, option.argument_name);

    if (option.short_name)
      prefix += common::format (option.argument_name ? ", -%c <%s>" : ", -%c", option.short_name, option.argument_name);

    printf ("%s ", prefix.c_str ());

    if (prefix.size () < HELP_STRING_PREFIX_LENGTH)
      for (size_t i=0, count=HELP_STRING_PREFIX_LENGTH-prefix.size (); i<count; i++)
        printf (" ");

    printf ("%s\n", option.tip);
  }

  params.print_help = true;
}

//установка имени пути исходных текстур
void command_line_source_search_path (const char* path, Params& params)
{
  params.source_search_paths.Add (path);
}

//установка имени каталога с сохраненными текстурами
void command_line_output_textures_dir_name (const char* dir, Params& params)
{
  params.output_textures_dir_name = dir;
}

//установка формата сохранения текстур
void command_line_output_textures_format (const char* format, Params& params)
{
  params.output_textures_format = format;
}

//установка формата сохранения текстур в материале
void command_line_material_textures_format (const char* format, Params& params)
{
  params.material_textures_format = format;
}

//установка максимального размера текстуры
void command_line_output_max_texture_size (const char* value, Params& params)
{
  params.max_texture_size = (size_t)atoi (value);
}

//установка файла материалов
void command_line_output_materials_file_name (const char* file_name, Params& params)
{
  params.output_materials_file_name = file_name;
}

//установка файла сцены
void command_line_output_scene_file_name (const char* file_name, Params& params)
{
  params.output_scene_file_name = file_name;
}

//установка файла анимаций
void command_line_output_animations_file_name (const char* file_name, Params& params)
{
  params.output_animations_file_name = file_name;
}

//установка папки мешей
void command_line_output_meshes_file_name (const char* file_name, Params& params)
{
  params.output_meshes_file_name = file_name;
}

//установка пути физических мешей
void command_line_output_phys_meshes_file_name (const char* file_name, Params& params)
{
  params.output_phys_meshes_file_name = file_name;
}

//установка списка конвексных физических мешей
void command_line_convex_phys_meshes (const char* wildcard_list, Params& params)
{
  params.convex_phys_meshes = wildcard_list;
}

//установка неэкспортируемых узлов сцены
void command_line_exclude_nodes (const char* string, Params& params)
{
  params.exclude_nodes = string;
}

//установка имени анимации для склеивания
void command_line_merge_animation (const char* string, Params& params)
{
  params.merge_animation = string;
}

//установка необходимости масштабировать текстуры до ближайшей степени двойки
void command_line_fix_zero_tangents (const char*, Params& params)
{
  params.fix_zero_tangents = true;
}

//установка необходимости масштабировать текстуры до ближайшей степени двойки
void command_line_pot (const char*, Params& params)
{
  params.pot = true;
}

//установка параметра вывода детальной информации
void command_line_silent (const char*, Params& params)
{
  params.silent = true;
}

//установка параметра удаления не используемых ресурсов
void command_line_remove_unused_resources (const char*, Params& params)
{
  params.remove_unused_resources = true;
}

//установка отбрасываемого префикса имён файлов
void command_line_set_remove_file_prefix (const char* prefix, Params& params)
{
  params.output_remove_file_prefix = prefix;
}

//установка пространства имён ресурсов
void command_line_set_resources_namespace (const char* prefix, Params& params)
{
  params.output_resources_namespace = prefix;
}

//разбор командной строки
void command_line_parse (int argc, const char* argv [], Params& params)
{
  static Option options [] = {
    {command_line_source_search_path,           "include",                'I', "dir",       "add path to resources search paths"},
    {command_line_output_textures_dir_name,     "out-textures-dir",         0, "dir",       "set output textures directory"},
    {command_line_output_textures_format,       "out-textures-format",      0, "string",    "set output textures format string"},
    {command_line_material_textures_format,     "material-textures-format", 0, "string",    "set material textures format string"},
    {command_line_output_max_texture_size,      "max-texture-size",         0, "value",     "set maximum output textures size"},
    {command_line_fix_zero_tangents,            "fix-zero-tangents",        0, 0,           "generate tangents for zero-values"},
    {command_line_pot,                          "pot",                      0, 0,           "rescale textures to power of two"},
    {command_line_output_materials_file_name,   "out-materials",            0, "file",      "set output materials file"},
    {command_line_output_meshes_file_name,      "out-meshes",               0, "file",      "set output meshes file name"},
    {command_line_output_phys_meshes_file_name, "out-phys-meshes",          0, "file",      "set output physics meshes file name"},
    {command_line_convex_phys_meshes,           "convex-meshes",            0, "wldcards",  "set selected physics meshes as convex"},
    {command_line_output_scene_file_name,       "out-scene",                0, "file",      "set output scene file"},
    {command_line_output_animations_file_name,  "out-animations",           0, "file",      "set output animations file"},
    {command_line_set_remove_file_prefix,       "remove-file-prefix",       0, "string",    "remove file prefix from all file links"},
    {command_line_set_resources_namespace,      "resources-namespace",      0, "string",    "set resources namespace"},
    {command_line_exclude_nodes,                "exclude-nodes",            0, "wildcards", "exclude selected nodes from export"},
    {command_line_merge_animation,              "merge-animation",          0, "string",    "merge all animations in one with given name"},
    {command_line_remove_unused_resources,      "remove-unused",            0, 0,           "remove unused resources from export"},
    {command_line_silent,                       "silent",                 's', 0,           "quiet mode"},
    {command_line_help,                         "help",                   '?', 0,           "print help message"},
  };

  static const size_t options_count = sizeof (options) / sizeof (*options);

  params.options       = options;
  params.options_count = options_count;

    //разбор командной строки

  for (int i=1; i<argc; i++)
  {
    const char* arg = argv [i];

    static const char*  LONG_OPTION_NAME_PREFIX        = "--";
    static const size_t LONG_OPTION_NAME_PREFIX_LENGTH = strlen (LONG_OPTION_NAME_PREFIX);

    bool long_option  = !strncmp (LONG_OPTION_NAME_PREFIX, arg, LONG_OPTION_NAME_PREFIX_LENGTH);
    bool short_option = !long_option && *arg == '-' && arg [1];

    if (!long_option && !short_option)
    {
      if (!params.source_name.empty ())
      {
        error ("source file must be one");
        return;
      }

      params.source_name = arg;

      continue;
    }

    stl::string   option_name;
    stl::string   option_argument;
    const Option* option = 0;

      //разбор длинных опций

    if (long_option)
    {
      arg += LONG_OPTION_NAME_PREFIX_LENGTH;

      const char* end_option_name = strchr (arg, '=');

      if (end_option_name)
      {
        option_name.assign (arg, end_option_name);

        arg = end_option_name + 1;
      }
      else
      {
        option_name  = arg;
        arg         += strlen (arg);
      }

      for (size_t j=0; j<options_count; j++)
      {
        const Option& test_option        = options [j];
        size_t        test_option_length = strlen (test_option.name);

        if (!strncmp (test_option.name, option_name.c_str (), test_option_length))
        {
          option = &test_option;
          break;
        }
      }

      if (!option)
      {
        error ("wrong option '--%s'", option_name.c_str ());

        return;
      }

      option_name = LONG_OPTION_NAME_PREFIX + option_name;
    }

      //разбор коротких опций

    if (short_option)
    {
      arg++;

      if (arg [1])
      {
        error ("wrong option '-%c'", *arg);

        return;
      }

      for (size_t j=0; j<options_count; j++)
      {
        if (*arg == options [j].short_name)
        {
          option = &options [j];

          break;
        }
      }

      if (!option)
      {
        error ("wrong option '-%c'", *arg);

        return;
      }

      option_name = common::format ("-%c", *arg);

      if (option->argument_name)
      {
        i++;

        if (i >= argc)
        {
          error ("option '-%c' require argument", *arg);

          return;
        }

        arg = argv [i];
      }
      else
        arg = "";
    }

      //получение аргумента

    for (;*arg; arg++)
    {
      switch (*arg)
      {
        case '\'':
        case '"':
        {
          const char* end = strchr (arg + 1, *arg);

          if (end)
          {
            option_argument.append (arg + 1, end);

            arg = end;
          }
          else
          {
            error ("unquoted string at parse option '%s'", option_name.c_str ());

            return;
          }

          break;
        }
        default:
          option_argument.push_back (*arg);
          break;
      }
    }

    if (option->argument_name && option_argument.empty ())
    {
      error ("option '%s' require argument", option_name.c_str ());

      return;
    }

    if (!option->argument_name && !option_argument.empty ())
    {
      error ("option '%s' has not arguments", option_name.c_str ());

      return;
    }

    if (option->process)
      option->process (option_argument.c_str (), params);
  }
}

//сохранение мешей
void save_meshes (const Params& params, const Model& model)
{
  if (!params.silent)
    printf ("Convert meshes...");

  media::geometry::MeshLibrary mesh_library;

  convert (model, mesh_library, params.fix_zero_tangents);
  
  if (!params.output_resources_namespace.empty ())
  {
    media::geometry::MeshLibrary new_mesh_library;

    new_mesh_library.Rename (mesh_library.Name ());
    
    for (media::geometry::MeshLibrary::Iterator iter=mesh_library.CreateIterator (); iter; ++iter)
    {
      media::geometry::Mesh mesh   = *iter;
      const char*           id     = mesh_library.ItemId (iter);
      stl::string           new_id = common::format ("%s.%s", params.output_resources_namespace.c_str (), id);
      
      size_t count = mesh.PrimitivesCount ();
      
      for (size_t i=0; i<count; i++)
      {
        const media::geometry::Primitive& primitive = mesh.Primitive (i);
        
        mesh.AddPrimitive (primitive.type, primitive.vertex_buffer, primitive.first, primitive.count, 
          common::format ("%s.%s", params.output_resources_namespace.c_str (), primitive.material).c_str ());
      }

      for (size_t i=0; i<count; i++)
        mesh.RemovePrimitive (0);

      new_mesh_library.Attach (new_id.c_str (), mesh);
    }

    new_mesh_library.Swap (mesh_library);
  }

  if (!params.silent)
    printf ("Ok\n");    
 
  if (!params.silent)
    printf ("Save meshes to '%s'...", params.output_meshes_file_name.c_str ());
    
  stl::string dir_name = common::dir (params.output_meshes_file_name);
  
  if (!FileSystem::IsFileExist (dir_name.c_str ()))
    FileSystem::Mkdir (dir_name.c_str ());

  mesh_library.Save (params.output_meshes_file_name.c_str ());
  
  if (!params.silent)
    printf ("Ok\n");
}

//сохранение физических мешей
void save_physics_meshes (const Params& params, const Model& model)
{
  if (!params.silent)
    printf ("Convert physics meshes...");

  media::physics::PhysicsLibrary library;

  convert_triangle_meshes (model, library);

  if (!params.output_resources_namespace.empty ())
  {
    media::physics::PhysicsLibrary new_library;

    new_library.Rename (library.Name ());

    const media::physics::PhysicsLibrary::TriangleMeshCollection& meshes     = library.TriangleMeshes ();
    media::physics::PhysicsLibrary::TriangleMeshCollection&       new_meshes = new_library.TriangleMeshes ();

    for (media::physics::PhysicsLibrary::TriangleMeshCollection::ConstIterator iter = meshes.CreateIterator (); iter; ++iter)
    {
      media::physics::shapes::TriangleMesh mesh  = *iter;
      const char*                          id     = meshes.ItemId (iter);
      stl::string                          new_id = common::format ("%s.%s", params.output_resources_namespace.c_str (), id);

      new_meshes.Attach (new_id.c_str (), mesh);
    }

    new_library.Swap (library);
  }

  common::StringArray convex_meshes_list = common::split (params.convex_phys_meshes.c_str ());

  for (media::physics::PhysicsLibrary::TriangleMeshCollection::Iterator iter = library.TriangleMeshes ().CreateIterator (); iter; ++iter)
  {
    media::physics::shapes::TriangleMesh mesh  = *iter;
    const char*                          id     = library.TriangleMeshes ().ItemId (iter);

    mesh.SetConvex (false);

    for (size_t i = 0, count = convex_meshes_list.Size (); i < count; i++)
    {
      const char* mask = convex_meshes_list [i];

      if (common::wcmatch (id, mask))
      {
        mesh.SetConvex (true);
        break;
      }
    }
  }

  if (!params.silent)
    printf ("Ok\n");

  if (!params.silent)
    printf ("Save physics meshes to '%s'...", params.output_phys_meshes_file_name.c_str ());

  stl::string dir_name = common::dir (params.output_phys_meshes_file_name);

  if (!FileSystem::IsFileExist (dir_name.c_str ()))
    FileSystem::Mkdir (dir_name.c_str ());

  media::physics::PhysicsLibrary::SaveOptions save_options;

  save_options.separate_meshes_file = false;

  library.Save (params.output_phys_meshes_file_name.c_str (), save_options);

  if (!params.silent)
    printf ("Ok\n");
}

//сохранение анимаций
void save_animations (const Params& params, const Model& model)
{
  media::animation::AnimationLibrary animation_library;

  if (params.merge_animation.empty ())
    convert (model, animation_library);
  else
    convert (model, animation_library, params.merge_animation.c_str ());

  animation_library.Save (params.output_animations_file_name.c_str ());
}

//имя параметра источника света
const char* light_param_name (LightParam param)
{
  switch (param)
  {
    case LightParam_AttenuationConstant:  return "attenuation_constant";
    case LightParam_AttenuationLinear:    return "attenuation_linear";
    case LightParam_AttenuationQuadratic: return "attenuation_quadratic";
    case LightParam_FalloffAngle:         return "falloff_angle";
    case LightParam_FalloffExponent:      return "falloff_exponent";
    default:                              throw xtl::make_argument_exception ("::light_param_name", "param", param);
  }
}

//сохранение узла
void save_node (const Params& params, const BoundVolumesMap& meshes_bound_volumes, const Node& node, XmlWriter& writer)
{
  static const char* METHOD_NAME = "::save_node";

  common::StringArray exclude_nodes_list = common::split (params.exclude_nodes.c_str ());

  for (size_t i = 0, count = exclude_nodes_list.Size (); i < count; i++)
  {
    const char* mask = exclude_nodes_list [i];

    if (common::wcmatch (node.Name (), mask))
      return;
  }

  XmlWriter::Scope scope (writer, "node");

  writer.WriteAttribute ("id", node.Id ());

  if (xtl::xstrlen (node.SubId ()))
    writer.WriteAttribute ("sub_id", node.SubId ());

  if (xtl::xstrlen (node.Name ()))
    writer.WriteAttribute ("name", node.Name ());

  writer.WriteAttribute ("transform", node.Transform ());
  
  if (!math::equal (node.RotationPivot (), math::vec3f (0.0f), EPSILON))
    writer.WriteAttribute ("rotation_pivot", node.RotationPivot ());
    
  if (!math::equal (node.ScalePivot (), math::vec3f (0.0f), EPSILON))
    writer.WriteAttribute ("scale_pivot", node.ScalePivot ());

  if (xtl::xstrlen (node.UserProperties ()))
  {
    XmlWriter::Scope scope (writer, "properties");

    writer.WriteData (node.UserProperties ());
  }

    //метод экспорта связан с правилами именования инстанцированных мешей в конвертере

  size_t mesh_index = 0;

  for (Node::MeshList::ConstIterator iter = node.Meshes ().CreateIterator (); iter; ++iter)
  {
    stl::string mesh_name = common::format ("%s.mesh#%u", node.Id (), mesh_index++);

    if (!params.output_resources_namespace.empty ())
      mesh_name = common::format ("%s.%s", params.output_resources_namespace.c_str (), mesh_name.c_str ());

    BoundVolumesMap::const_iterator mesh_bound_volume_iter = meshes_bound_volumes.find (mesh_name.c_str ());

    if (mesh_bound_volume_iter == meshes_bound_volumes.end ())
      throw xtl::format_operation_exception (METHOD_NAME, "Can't find bound volume for mesh '%s'", mesh_name.c_str ());

    XmlWriter::Scope scope (writer, "mesh");

    writer.WriteAttribute ("name", mesh_name.c_str ());
    writer.WriteAttribute ("bound_box_min", mesh_bound_volume_iter->second.minimum ());
    writer.WriteAttribute ("bound_box_max", mesh_bound_volume_iter->second.maximum ());
  }

  size_t light_index = 0;

  for (Node::LightList::ConstIterator iter = node.Lights ().CreateIterator (); iter; ++iter)
  {
    stl::string light_name = common::format ("%s.light#%u", node.Id (), light_index++);

    const char* light_type;

    switch (iter->Type ())
    {
      case LightType_Ambient:
        light_type = "ambient_light";
        break;
      case LightType_Point:
        light_type = "point_light";
        break;
      case LightType_Spot:
        light_type = "spot_light";
        break;
      case LightType_Direct:
        light_type = "direct_light";
        break;
      default:
        throw xtl::format_operation_exception (METHOD_NAME, "Can't save node '%s', unknown light type %d", node.Id (), iter->Type ());
    }

    XmlWriter::Scope scope (writer, light_type);

    writer.WriteAttribute ("name", light_name.c_str ());
    writer.WriteAttribute ("color", iter->Color ());

    for (size_t i = 0; i < LightParam_Num; i++)
      writer.WriteAttribute (light_param_name ((LightParam)i), iter->Param ((LightParam)i));
  }

  for (Node::NodeList::ConstIterator iter = node.Nodes ().CreateIterator (); iter; ++iter)
    save_node (params, meshes_bound_volumes, *iter, writer);
}

//сохранение сцены
void save_scene (const Params& params, const Model& model)
{
  if (!params.silent)
    printf ("Save scene to '%s'...", params.output_scene_file_name.c_str ());
    
  stl::string dir_name = common::dir (params.output_scene_file_name);
  
  if (!FileSystem::IsFileExist (dir_name.c_str ()))
    FileSystem::Mkdir (dir_name.c_str ());
    
  XmlWriter writer (params.output_scene_file_name.c_str ());

  XmlWriter::Scope scope (writer, "scene");

    //построение ограничивающих объемов для мешей
  BoundVolumesMap meshes_bound_volumes;

  const MeshLibrary& mesh_library = model.Meshes ();

  for (NodeLibrary::ConstIterator node_iter = model.Nodes ().CreateIterator (); node_iter; ++node_iter)
  {
    size_t mesh_index = 0;

    for (Node::MeshList::ConstIterator mesh_iter = node_iter->Meshes ().CreateIterator (); mesh_iter; ++mesh_iter, ++mesh_index)
    {
      MeshLibrary::ConstIterator instance_mesh_iter = mesh_library.Find (mesh_iter->Mesh ());

      if (!instance_mesh_iter)
        throw xtl::format_operation_exception ("::save_scene", "Can't find mesh '%s'", mesh_iter->Mesh ());

      math::vec3f min_extent (FLT_MAX), max_extent (-FLT_MAX);

      for (Mesh::SurfaceList::ConstIterator surface_iter = instance_mesh_iter->Surfaces ().CreateIterator (); surface_iter; ++surface_iter)
      {
        const Vertex* vertex = surface_iter->Vertices ();

        for (size_t i = 0, count = surface_iter->VerticesCount (); i < count; i++, vertex++)
        {
          min_extent.x = stl::min (min_extent.x, vertex->coord.x);
          min_extent.y = stl::min (min_extent.y, vertex->coord.y);
          min_extent.z = stl::min (min_extent.z, vertex->coord.z);
          max_extent.x = stl::max (max_extent.x, vertex->coord.x);
          max_extent.y = stl::max (max_extent.y, vertex->coord.y);
          max_extent.z = stl::max (max_extent.z, vertex->coord.z);
        }
      }

      stl::string mesh_id = common::format ("%s.%s.mesh#%u", params.output_resources_namespace.c_str (), node_iter->Id (), mesh_index);

      meshes_bound_volumes.insert_pair (mesh_id.c_str (), bound_volumes::aaboxf (min_extent, max_extent));
    }
  }

  for (NodeLibrary::ConstIterator i = model.Scenes ().CreateIterator (); i; ++i)
    save_node (params, meshes_bound_volumes, *i, writer);

  if (!params.silent)
    printf ("Ok\n");
}

//загрузка текстуры
TexturePtr load_texture (const Params& params, const char* path)
{
  const char** search_paths = params.source_search_paths.Data ();

  for (size_t i=0, count=params.source_search_paths.Size (); i<count; i++)
  {
    const char* dir = search_paths [i];
    
    stl::string texture_path = common::format ("%s/%s", dir, path);

    if (!FileSystem::IsFileExist (texture_path.c_str ()))
      continue; //файл не найден

      //загрузка картинки

    TexturePtr texture (new media::Image (texture_path.c_str ()));

    size_t new_width = texture->Width (), new_height = texture->Height (), new_depth = texture->Depth ();

    if (params.pot)
    {
      new_width  = get_next_higher_power_of_two (new_width);
      new_height = get_next_higher_power_of_two (new_height);
      new_depth  = get_next_higher_power_of_two (new_depth);
    }

    if (params.max_texture_size)
    {
      new_width  = stl::min (params.max_texture_size, new_width);
      new_height = stl::min (params.max_texture_size, new_height);
      new_depth  = stl::min (params.max_texture_size, new_depth);
    }
    
    if (new_width != texture->Width () || new_height != texture->Height () || new_depth != texture->Depth ())
    {
      if (!params.silent)
        printf (new_depth > 1 ? " (rescale to %ux%ux%u)..." : " (rescale to %ux%u)...", new_width, new_height, new_depth);

      texture->Resize (new_width, new_height, new_depth);
    }

    return texture;
  }
    
  return TexturePtr ();
}

stl::string get_texture_name (const char* texture_format, const char* dir_name, const char* image_path, size_t texture_id)
{
  if (!xtl::xstrlen (texture_format))
  {
    if (xtl::xstrlen (dir_name))
      return common::format ("%s/%s", dir_name, image_path);
    else
      return image_path;
  }
  else
  {
    stl::string format;

    if (xtl::xstrlen (dir_name))
    {
      format = common::format ("%s/%s", dir_name, texture_format);
    }
    else
    {
      format = texture_format;
    }

    return common::format (format.c_str (), texture_id);
  }
}

//сохранение изображений
void save_images (const Params& params, const Model& model, ImagesMap& images_map)
{
  if (!params.silent)
    printf ("Save textures to '%s'...\n", params.output_textures_dir_name.empty () ? "." : params.output_textures_dir_name.c_str ());    

  if (!params.output_textures_dir_name.empty () && !FileSystem::IsFileExist (params.output_textures_dir_name.c_str ()))
    FileSystem::Mkdir (params.output_textures_dir_name.c_str ());

  size_t texture_id = 0;

  for (ImageLibrary::ConstIterator iter = model.Images ().CreateIterator (); iter; ++iter)
  {
    const Image& image = *iter;
    
    if (!image.Path ())
      continue;
      
    stl::string image_path;

      //проверка есть ли картинка по указанному пути

    const char** search_paths = params.source_search_paths.Data ();

    for (size_t i=0, count=params.source_search_paths.Size (); i<count; i++)
    {
      const char* dir = search_paths [i];
      
      stl::string texture_path = common::format ("%s/%s", dir, image.Path ());

      if (FileSystem::IsFileExist (texture_path.c_str ()))
      {
        image_path = image.Path ();
        break;
      }
    }

      //если по указанному пути картинка не найдена, обрезание пути

    if (image_path.empty ())
      image_path = common::notdir (image.Path ());

    if (!params.silent)
      printf ("  process texture '%s'...", image_path.c_str ());

    TexturePtr texture = load_texture (params, image_path.c_str ());

    if (!texture)
    {
      if (!params.silent)
        printf ("Failed!\n");

      throw xtl::format_operation_exception ("::save_images", "Image '%s' not found", image_path.c_str ());
    }
      
    stl::string output_texture_name   = get_texture_name (params.output_textures_format.c_str (), params.output_textures_dir_name.c_str (), image_path.c_str (), texture_id),
                material_texture_name;

    if (params.material_textures_format.empty ())
      material_texture_name = output_texture_name;
    else
      material_texture_name = get_texture_name (params.material_textures_format.c_str (), params.output_textures_dir_name.c_str (), image_path.c_str (), texture_id);

    texture->Save (output_texture_name.c_str ());

    images_map.insert_pair (image.Id (), material_texture_name.c_str ());

    ++texture_id;

    if (!params.silent)
      printf ("Ok\n");
  }  
}

int get_texture_channel_number (const char* texture_channel)
{
  if (strstr (texture_channel, "TEX") != texture_channel)
    return atoi (texture_channel);

  return atoi (texture_channel + xtl::xstrlen ("TEX"));
}

//получение пути к текстуре
stl::string get_texture_path (const Params& params, ImagesMap& images_map, const Texture& texture)
{
  const char* path = images_map [texture.Image ()].c_str ();

  if (!params.output_remove_file_prefix.empty () && strstr (path, params.output_remove_file_prefix.c_str ()) == path)
  {
    path += params.output_remove_file_prefix.size ();
  }
  
  return path;
}

//сохранение материалов
void add_texmap (const Params& params, const Effect& effect, TextureMap map, ImagesMap& images_map, media::rfx::Material& material)
{
  if (!effect.HasTexture (map))
    return;

  const Texture&     texture = effect.Texture (map);
  media::rfx::Texmap texmap;
  const char         *semantic, *texture_channel_property;

  switch (map)
  {
    case TextureMap_Diffuse:
      semantic                 = "diffuse";
      texture_channel_property = "DiffuseTextureChannel";
      break;
    case TextureMap_Ambient:
      semantic                 = "ambient";
      texture_channel_property = "AmbientTextureChannel";
      break;
    case TextureMap_Specular:
      semantic                 = "specular";
      texture_channel_property = "SpecularTextureChannel";
      break;
    case TextureMap_Transparent:  //ignore transparency textures
      return;
    case TextureMap_Emission:
      semantic                 = "emission";
      texture_channel_property = "EmissionTextureChannel";
      break;
    case TextureMap_Reflective:
      semantic                 = "reflective";
      texture_channel_property = "ReflectiveTextureChannel";
      break;
    case TextureMap_Bump:
      semantic                 = "bump";
      texture_channel_property = "BumpTextureChannel";
      break;
    default:
      throw xtl::format_operation_exception ("add_texmap", "Unknown map %d", map);
  }

  texmap.SetSemantic (semantic);
  texmap.SetImage    (get_texture_path (params, images_map, texture).c_str ());

  material.AddTexmap (texmap);

  material.Properties ().SetProperty (texture_channel_property, get_texture_channel_number (texture.TexcoordChannel ()));
}

void save_materials (const Params& params, const Model& model, ImagesMap& images_map)
{
  if (!params.silent)
    printf ("Save materials to '%s'...", params.output_materials_file_name.c_str ());

  stl::string dir_name = common::dir (params.output_materials_file_name);
  
  if (!FileSystem::IsFileExist (dir_name.c_str ()))
    FileSystem::Mkdir (dir_name.c_str ());

  media::rfx::MaterialLibrary material_library;

  for (EffectLibrary::ConstIterator i = model.Effects ().CreateIterator (); i; ++i)
  {
    media::rfx::Material material;

    stl::string id = params.output_resources_namespace.empty () ? i->Id () : common::format ("%s.%s", params.output_resources_namespace.c_str (), i->Id ());

    material.SetName (i->Id ());

    switch (i->ShaderType ())
    {
      case ShaderType_Constant:
        material.SetProgram ("constant");
        break;
      case ShaderType_Lambert:
        material.SetProgram ("lambert");
        break;
      case ShaderType_Phong:
        material.SetProgram ("phong");
        break;
      case ShaderType_Blinn:
        material.SetProgram ("blinn");
        break;
      default:
        throw xtl::format_operation_exception ("save_materials", "Effect '%s' has unknown shader type", i->Id ());
    }

    common::PropertyMap& properties = material.Properties ();

    properties.SetProperty ("Reflectivity", i->Param (EffectParam_Reflectivity));
    properties.SetProperty ("Transparency", i->Param (EffectParam_Transparency));
    properties.SetProperty ("Shininess",    i->Param (EffectParam_Shininess));
    properties.SetProperty ("Diffuse",      i->MapColor (TextureMap_Diffuse));
    properties.SetProperty ("Ambient",      i->MapColor (TextureMap_Ambient));
    properties.SetProperty ("Specular",     i->MapColor (TextureMap_Specular));
    properties.SetProperty ("Emission",     i->MapColor (TextureMap_Emission));

    if (i->HasTexture (TextureMap_Bump))
    {
      const Texture& texture = i->Texture (TextureMap_Bump);

      properties.SetProperty ("BumpAmount", texture.Amount ());
    }

    if (i->HasTexture (TextureMap_Transparent))
      properties.SetProperty ("BlendMode", "translucent");

    for (size_t j = 0; j < TextureMap_Num; j++)
      add_texmap (params, *i, (TextureMap)j, images_map, material);

    material_library.Attach (id.c_str (), material);
  }

  material_library.Save (params.output_materials_file_name.c_str ());
    
  if (!params.silent)
    printf ("Ok\n");
}

//печать протокола
void print (const char* message)
{
  printf ("%s\n", message);
}

//экспорт
void export_data (Params& params)
{
  if (!params.silent)
    printf ("Load model '%s'...\n", params.source_name.c_str ());

  Model model (params.source_name.c_str (), &print);

  if (!params.silent)
    printf ("Ok\n");
  
  if (params.remove_unused_resources)
  {
    if (!params.silent)
      printf ("Remove unused resources...");
      
    remove_unused_resources (model, ModelSelect_ActiveScene | ModelSelect_Animations);
    
    if (!params.silent)
      printf ("Ok\n");
  }
  
  if (!params.output_textures_dir_name.empty ())
  {
    ImagesMap images_map;    
    
    save_images (params, model, images_map);    
    
    if (!params.output_materials_file_name.empty ())
      save_materials (params, model, images_map);
  }

  if (!params.output_meshes_file_name.empty ())
    save_meshes (params, model);

  if (!params.output_phys_meshes_file_name.empty ())
    save_physics_meshes (params, model);

  if (!params.output_scene_file_name.empty ())
    save_scene (params, model);

  if (!params.output_animations_file_name.empty ())
    save_animations (params, model);
}

//проверка корректности ввода
void validate (Params& params)
{
  if (params.source_name.empty ())
  {
    printf ("%s [<OPTIONS>] <SOURCE>\n  use: %s --help\n", APPLICATION_NAME, APPLICATION_NAME);
    error ("no input file");
    return;
  }    
}

int main (int argc, const char *argv[])
{
  try
  {
      //инициализация
    Params params;

    params.options                 = 0;
    params.options_count           = 0;
    params.silent                  = false;
    params.remove_unused_resources = false;
    params.print_help              = false;
    params.fix_zero_tangents       = false;
    params.pot                     = false;
    params.max_texture_size        = 0;
    
    params.source_search_paths.Add (".");

      //разбор командной строки

    command_line_parse (argc, argv, params);

      // --help только печатает сообщение помощи

    if (params.print_help)
      return 0;

      //проверка корректности ввода

    validate (params);

      //экспорт

    export_data (params);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n",exception.what ()); 
    return 1;
  }

  return 0;
}
