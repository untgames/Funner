#include <cstdio>

#include <stl/hash_map>
#include <stl/string>

#include <xtl/common_exceptions.h>
#include <xtl/iterator.h>
#include <xtl/shared_ptr.h>

#include <common/file.h>
#include <common/strlib.h>
#include <common/xml_writer.h>

#include <media/animation/animation_library.h>

#include <media/collada.h>
#include <media/image.h>
#include <media/mesh.h>

using namespace common;
using namespace media::collada;

/*
    Константы
*/

const char* APPLICATION_NAME = "collada_converter";

const char* SHADER_TYPE_ATTRIBUTE              = "Effect";
const char* REFLECTIVITY_ATTRIBUTE             = "Reflectivity";
const char* TRANSPARENCY_ATTRIBUTE             = "Transparency";
const char* SHININESS_ATTRIBUTE                = "Shininess";
const char* DIFFUSE_ATTRIBUTE                  = "Diffuse";
const char* DIFFUSE_TEXTURE_ATTRIBUTE          = "DiffuseTexture";
const char* DIFFUSE_TEXTURE_CHANNEL_ATTRIBUTE  = "DiffuseTextureChannel";
const char* AMBIENT_ATTRIBUTE                  = "Ambient";
const char* AMBIENT_TEXTURE_ATTRIBUTE          = "AmbientTexture";
const char* AMBIENT_TEXTURE_CHANNEL_ATTRIBUTE  = "AmbientTextureChannel";
const char* SPECULAR_ATTRIBUTE                 = "Specular";
const char* SPECULAR_TEXTURE_ATTRIBUTE         = "SpecularTexture";
const char* SPECULAR_TEXTURE_CHANNEL_ATTRIBUTE = "SpecularTextureChannel";
const char* EMISSION_ATTRIBUTE                 = "Emission";
const char* EMISSION_TEXTURE_ATTRIBUTE         = "EmissionTexture";
const char* EMISSION_TEXTURE_CHANNEL_ATTRIBUTE = "EmissionTextureChannel";
const char* BUMP_AMOUNT_ATTRIBUTE              = "BumpAmount";
const char* BUMP_TEXTURE_ATTRIBUTE             = "BumpTexture";
const char* BUMP_TEXTURE_CHANNEL_ATTRIBUTE     = "BumpTextureChannel";

const size_t HELP_STRING_PREFIX_LENGTH  = 30;

const float EPSILON = 0.001f;

/*
    Типы
*/

typedef stl::hash_map<stl::hash_key<const char*>, stl::string> ImagesMap;

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
  const Option* options;                     //массив опций
  size_t        options_count;               //количество опций
  stl::string   source_name;                 //имя исходного файла
  StringArray   source_search_paths;         //пути к каталогам поиск ресурсов
  stl::string   output_textures_dir_name;    //имя каталога с сохранёнными текстурами
  stl::string   output_textures_format;      //формат текстур
  stl::string   output_materials_file_name;  //файл материалов
  stl::string   output_scene_file_name;      //файл сцены
  stl::string   output_animations_file_name; //файл анимаций
  stl::string   output_meshes_file_name;     //имя файла с сохранёнными мешами
  stl::string   output_remove_file_prefix;   //отбрасываемый префикс имён файлов
  stl::string   output_resources_namespace;  //пространство имён, применяемое при сохранении ресурсов
  stl::string   exclude_nodes;               //неэкспортируемые узлы сцены
  stl::string   merge_animation;             //имя анимации в которую должны быть вклеены все остальные анимации
  size_t        max_texture_size;            //максимальный размер текстуры
  bool          pot;                         //нужно ли масштабировать текстуры до ближайшей степени двойки
  bool          silent;                      //минимальное число сообщений
  bool          remove_unused_resources;     //нужно ли выбрасывать неиспользуемые ресурсы
  bool          print_help;                  //нужно ли печатать сообщение помощи
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
    {command_line_source_search_path,          "include",               'I', "dir",       "add path to resources search paths"},
    {command_line_output_textures_dir_name,    "out-textures-dir",        0, "dir",       "set output textures directory"},
    {command_line_output_textures_format,      "out-textures-format",     0, "string",    "set output textures format string"},
    {command_line_output_max_texture_size,     "max-texture-size",        0, "value",     "set maximum output textures size"},
    {command_line_pot,                         "pot",                     0, 0,           "rescale textures to power of two"},
    {command_line_output_materials_file_name,  "out-materials",           0, "file",      "set output materials file"},
    {command_line_output_meshes_file_name,     "out-meshes",              0, "file",      "set output meshes file name"},
    {command_line_output_scene_file_name,      "out-scene",               0, "file",      "set output scene file"},
    {command_line_output_animations_file_name, "out-animations",          0, "file",      "set output animations file"},
    {command_line_set_remove_file_prefix,      "remove-file-prefix",      0, "string",    "remove file prefix from all file links"},
    {command_line_set_resources_namespace,     "resources-namespace",     0, "string",    "set resources namespace"},
    {command_line_exclude_nodes,               "exclude-nodes",           0, "wildcards", "exclude selected nodes from export"},
    {command_line_merge_animation,             "merge-animation",         0, "string",    "merge all animations in one with given name"},
    {command_line_remove_unused_resources,     "remove-unused",           0, 0,           "remove unused resources from export"},
    {command_line_silent,                      "silent",                's', 0,           "quiet mode"},
    {command_line_help,                        "help",                  '?', 0,           "print help message"},
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

  convert (model, mesh_library);
  
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

//сохранение узла
void save_node (const Params& params, const Node& node, XmlWriter& writer)
{
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

    //метод экспорта связан с правилами именования инстанцированных мешей в конвертере

  size_t mesh_index = 0;

  for (Node::MeshList::ConstIterator iter = node.Meshes ().CreateIterator (); iter; ++iter)
  {
    stl::string mesh_name = common::format ("%s.mesh#%u", node.Id (), mesh_index++);

    if (!params.output_resources_namespace.empty ())
      mesh_name = common::format ("%s.%s", params.output_resources_namespace.c_str (), mesh_name.c_str ());

    XmlWriter::Scope scope (writer, "mesh");

    writer.WriteAttribute ("name", mesh_name.c_str ());
  }

  for (Node::NodeList::ConstIterator iter = node.Nodes ().CreateIterator (); iter; ++iter)
    save_node (params, *iter, writer);
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

  for (NodeLibrary::ConstIterator i = model.Scenes ().CreateIterator (); i; ++i)
    save_node (params, *i, writer);

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
    
    stl::string texture_short_path = common::notdir (path),
                texture_path       = common::format ("%s/%s", dir, texture_short_path.c_str ());                

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
      
    stl::string image_path = common::notdir (image.Path ());      
      
    if (!params.silent)
      printf ("  process texture '%s'...", image_path.c_str ());

    TexturePtr texture = load_texture (params, image_path.c_str ());

    if (!texture)
    {
      if (!params.silent)
        printf ("Failed!\n");

      throw xtl::format_operation_exception ("::save_images", "Image '%s' not found", image_path.c_str ());
    }
      
    stl::string output_texture_name;

    if (params.output_textures_format.empty ())
    {
      if (!params.output_textures_dir_name.empty ())
      {
        output_texture_name = common::format ("%s/%s", params.output_textures_dir_name.c_str (), image_path.c_str ());
      }
      else
      {
        output_texture_name = image_path;
      }
    }
    else
    {
      stl::string format;
      
      if (!params.output_textures_dir_name.empty ())
      {
        format = common::format ("%s/%s", params.output_textures_dir_name.c_str (), params.output_textures_format.c_str ());
      }
      else
      {
        format = params.output_textures_format;
      }
      
      output_texture_name = common::format (format.c_str (), texture_id);
    }

    texture->Save (output_texture_name.c_str ());

    images_map.insert_pair (image.Id (), output_texture_name.c_str ());

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
void save_material (const Params& params, const Effect& effect, ImagesMap& images_map, XmlWriter& writer)
{
  XmlWriter::Scope material_scope (writer, "Material");
  
  stl::string id = params.output_resources_namespace.empty () ? effect.Id () : common::format ("%s.%s", params.output_resources_namespace.c_str (), effect.Id ());

  writer.WriteAttribute ("Name", id.c_str ());

  XmlWriter::Scope shader_parameters_scope (writer, "ShaderParameters");

  switch (effect.ShaderType ())
  {
    case ShaderType_Constant:
      writer.WriteAttribute (SHADER_TYPE_ATTRIBUTE, "constant");
      break;
    case ShaderType_Lambert:
      writer.WriteAttribute (SHADER_TYPE_ATTRIBUTE, "lambert");
      break;
    case ShaderType_Phong:
      writer.WriteAttribute (SHADER_TYPE_ATTRIBUTE, "phong");
      break;
    case ShaderType_Blinn:
      writer.WriteAttribute (SHADER_TYPE_ATTRIBUTE, "blinn");
      break;
    default:
      throw xtl::format_operation_exception ("save_material", "Effect '%s' has unknown shader type", effect.Id ());
  }

  writer.WriteAttribute (REFLECTIVITY_ATTRIBUTE, effect.Param (EffectParam_Reflectivity));
  writer.WriteAttribute (TRANSPARENCY_ATTRIBUTE, effect.Param (EffectParam_Transparency));
  writer.WriteAttribute (SHININESS_ATTRIBUTE,    effect.Param (EffectParam_Shininess));
  writer.WriteAttribute (DIFFUSE_ATTRIBUTE,      effect.MapColor (TextureMap_Diffuse));
  writer.WriteAttribute (AMBIENT_ATTRIBUTE,      effect.MapColor (TextureMap_Ambient));
  writer.WriteAttribute (SPECULAR_ATTRIBUTE,     effect.MapColor (TextureMap_Specular));
  writer.WriteAttribute (EMISSION_ATTRIBUTE,     effect.MapColor (TextureMap_Emission));

  if (effect.HasTexture (TextureMap_Bump))
  {
    const Texture& texture = effect.Texture (TextureMap_Bump);

    writer.WriteAttribute (BUMP_TEXTURE_ATTRIBUTE, get_texture_path (params, images_map, texture).c_str ());
    writer.WriteAttribute (BUMP_TEXTURE_CHANNEL_ATTRIBUTE, get_texture_channel_number (texture.TexcoordChannel ()));
    writer.WriteAttribute (BUMP_AMOUNT_ATTRIBUTE, texture.Amount ());
  }

  if (effect.HasTexture (TextureMap_Diffuse))
  {
    const Texture& texture = effect.Texture (TextureMap_Diffuse);

    writer.WriteAttribute (DIFFUSE_TEXTURE_ATTRIBUTE, get_texture_path (params, images_map, texture).c_str ());
    writer.WriteAttribute (DIFFUSE_TEXTURE_CHANNEL_ATTRIBUTE, get_texture_channel_number (texture.TexcoordChannel ()));
  }

  if (effect.HasTexture (TextureMap_Ambient))
  {
    const Texture& texture = effect.Texture (TextureMap_Ambient);

    writer.WriteAttribute (AMBIENT_TEXTURE_ATTRIBUTE, get_texture_path (params, images_map, texture).c_str ());
    writer.WriteAttribute (AMBIENT_TEXTURE_CHANNEL_ATTRIBUTE, get_texture_channel_number (texture.TexcoordChannel ()));
  }

  if (effect.HasTexture (TextureMap_Specular))
  {
    const Texture& texture = effect.Texture (TextureMap_Specular);

    writer.WriteAttribute (SPECULAR_TEXTURE_ATTRIBUTE, get_texture_path (params, images_map, texture).c_str ());
    writer.WriteAttribute (SPECULAR_TEXTURE_CHANNEL_ATTRIBUTE, get_texture_channel_number (texture.TexcoordChannel ()));
  }

  if (effect.HasTexture (TextureMap_Emission))
  {
    const Texture& texture = effect.Texture (TextureMap_Emission);

    writer.WriteAttribute (EMISSION_TEXTURE_ATTRIBUTE, get_texture_path (params, images_map, texture).c_str ());
    writer.WriteAttribute (EMISSION_TEXTURE_CHANNEL_ATTRIBUTE, get_texture_channel_number (texture.TexcoordChannel ()));
  }
}

void save_property_declaration (const char* name, const char* type, XmlWriter& writer)
{
  XmlWriter::Scope scope (writer, "Property");

  writer.WriteAttribute ("Name", name);
  writer.WriteAttribute ("Type", type);
}

void save_materials (const Params& params, const Model& model, ImagesMap& images_map)
{
  if (!params.silent)
    printf ("Save materials to '%s'...", params.output_materials_file_name.c_str ());

  stl::string dir_name = common::dir (params.output_materials_file_name);
  
  if (!FileSystem::IsFileExist (dir_name.c_str ()))
    FileSystem::Mkdir (dir_name.c_str ());

  XmlWriter writer (params.output_materials_file_name.c_str ());

  XmlWriter::Scope library_scope (writer, "MaterialLibrary");

  {
    XmlWriter::Scope templates_scope  (writer, "Templates");
    XmlWriter::Scope properties_scope (writer, "Properties");
    writer.WriteAttribute ("Name", "ShaderParameters");

    {
      XmlWriter::Scope scope (writer, "Property");

      writer.WriteAttribute ("Name", "Effect");
      writer.WriteAttribute ("Type", "string");
      writer.WriteAttribute ("Value", "default");
    }

//    save_property_declaration (SHADER_TYPE_ATTRIBUTE, "string", writer);
    save_property_declaration (REFLECTIVITY_ATTRIBUTE, "float", writer);
    save_property_declaration (TRANSPARENCY_ATTRIBUTE, "float", writer);
    //?????Refraction??????
    save_property_declaration (SHININESS_ATTRIBUTE,                "float",  writer);
    save_property_declaration (BUMP_AMOUNT_ATTRIBUTE,              "float",  writer);
    save_property_declaration (BUMP_TEXTURE_ATTRIBUTE,             "string", writer);
    save_property_declaration (BUMP_TEXTURE_CHANNEL_ATTRIBUTE,     "int",    writer);
    save_property_declaration (DIFFUSE_ATTRIBUTE,                  "vector", writer);
    save_property_declaration (DIFFUSE_TEXTURE_ATTRIBUTE,          "string", writer);
    save_property_declaration (DIFFUSE_TEXTURE_CHANNEL_ATTRIBUTE,  "int",    writer);
    save_property_declaration (AMBIENT_ATTRIBUTE,                  "vector", writer);
    save_property_declaration (AMBIENT_TEXTURE_ATTRIBUTE,          "string", writer);
    save_property_declaration (AMBIENT_TEXTURE_CHANNEL_ATTRIBUTE,  "int",    writer);
    save_property_declaration (SPECULAR_ATTRIBUTE,                 "vector", writer);
    save_property_declaration (SPECULAR_TEXTURE_ATTRIBUTE,         "string", writer);
    save_property_declaration (SPECULAR_TEXTURE_CHANNEL_ATTRIBUTE, "int",    writer);
    save_property_declaration (EMISSION_ATTRIBUTE,                 "vector", writer);
    save_property_declaration (EMISSION_TEXTURE_ATTRIBUTE,         "string", writer);
    save_property_declaration (EMISSION_TEXTURE_CHANNEL_ATTRIBUTE, "int",    writer);
  }

  XmlWriter::Scope scope (writer, "Materials");

  for (EffectLibrary::ConstIterator i = model.Effects ().CreateIterator (); i; ++i)
    save_material (params, *i, images_map, writer);
    
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
    printf ("Load model '%s'...", params.source_name.c_str ());

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
