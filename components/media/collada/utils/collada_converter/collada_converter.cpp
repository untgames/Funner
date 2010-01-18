#include <cstdio>

#include <stl/hash_map>
#include <stl/string>

#include <xtl/common_exceptions.h>
#include <xtl/iterator.h>

#include <common/file.h>
#include <common/strlib.h>
#include <common/xml_writer.h>

#include <media/collada.h>
#include <media/image.h>
#include <media/mesh.h>

using namespace common;
using namespace media::collada;

const char* APPLICATION_NAME = "collada_converter";

const char* SHADER_TYPE_ATTRIBUTE              = "ShaderType";
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
  const Option* options;                  //массив опций
  size_t        options_count;            //количество опций
  stl::string   source_name;              //имя исходного файла
  stl::string   source_textures_path;     //путь к исходным текстурам
  stl::string   output_textures_dir_name; //имя каталога с сохранёнными текстурами
  stl::string   textures_format;          //формат текстур
  stl::string   materials_file_name;      //файл материалов
  stl::string   scene_file_name;          //файл сцены
  stl::string   output_meshes_dir_name;   //имя каталога с сохраненными мешами
  stl::string   meshes_format;            //формат мешей
  stl::string   nodes_exclude;            //неэкспортируемые узлы сцены
  bool          skip_materials;           //не сохранять картинки и материалы
  bool          silent;                   //минимальное число сообщений
  bool          print_help;               //нужно ли печатать сообщение помощи
};

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
void command_line_source_textures_path (const char* file_name, Params& params)
{
  params.source_textures_path = file_name;
}

//установка имени пути сохранения текстур
void command_line_result_textures_dir (const char* file_name, Params& params)
{
  params.output_textures_dir_name = file_name;
}

//установка формата сохранения текстур
void command_line_result_textures_format (const char* value, Params& params)
{
  params.textures_format = value;
}

//установка файла материалов
void command_line_materials_file_name (const char* file_name, Params& params)
{
  params.materials_file_name = file_name;
}

//установка файла сцены
void command_line_scene_file_name (const char* file_name, Params& params)
{
  params.scene_file_name = file_name;
}

//установка папки мешей
void command_line_result_meshes_dir_name (const char* file_name, Params& params)
{
  params.output_meshes_dir_name = file_name;
}

//установка формата сохранения мешей
void command_line_result_meshes_format (const char* value, Params& params)
{
  params.meshes_format = value;
}

//установка неэкспортируемых узлов сцены
void command_line_nodes_exclude (const char* string, Params& params)
{
  params.nodes_exclude = string;
}

//установка параметра пропуска сохранения картинок и материалов
void command_line_skip_materials (const char*, Params& params)
{
  params.skip_materials = true;
}

//установка параметра вывода детальной информации
void command_line_silent (const char*, Params& params)
{
  params.silent = true;
}

//разбор командной строки
void command_line_parse (int argc, const char* argv [], Params& params)
{
  static Option options [] = {
    {command_line_source_textures_path,       "textures-path",         'o', "dir",       "set source textures path"},
    {command_line_result_textures_dir,        "textures-dir",          0, "dir",       "set output textures directory"},
    {command_line_result_textures_format,     "textures-format",       0,   "string",    "set output textures format string"},
    {command_line_materials_file_name,        "materials-file",        0,   "file",      "set output materials file"},
    {command_line_scene_file_name,            "scene-file",            0,   "file",      "set output scene file"},
    {command_line_result_meshes_dir_name,     "meshes-dir",            0,   "dir",       "set output meshes directory"},
    {command_line_result_meshes_format,       "meshes-format",         0,   "string",    "set output meshes format string"},
    {command_line_nodes_exclude,              "nodes-exclude",         0,   "wildcards", "exclude selected nodes from export"},
    {command_line_skip_materials,             "skip-materials",        0,   0,           "don't save images and materials"},
    {command_line_silent,                     "silent",                's', 0,           "quiet mode"},
    {command_line_help,                       "help",                  '?', 0,           "print help message"},
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

void save_meshes (const Params& params, const Model& model)
{
  if (!FileSystem::IsDir (params.output_meshes_dir_name.c_str ()))
    FileSystem::Mkdir (params.output_meshes_dir_name.c_str ());

  media::geometry::MeshLibrary mesh_library;

  convert (model, mesh_library);

  stl::string save_name = common::format ("%s/meshes.%s", params.output_meshes_dir_name.c_str (), params.meshes_format.c_str ());

  mesh_library.Save (save_name.c_str ());
}

void save_node (const Params& params, const Node& node, XmlWriter& writer)
{
  common::StringArray nodes_exclude_list = common::split (params.nodes_exclude.c_str ());

  for (size_t i = 0, count = nodes_exclude_list.Size (); i < count; i++)
  {
    const char* mask = nodes_exclude_list [i];

    if (common::wcmatch (node.Name (), mask))
      return;
  }

  XmlWriter::Scope scope (writer, "node");

  writer.WriteAttribute ("transform", node.Transform ());

  size_t mesh_index = 0;

  for (Node::MeshList::ConstIterator iter = node.Meshes ().CreateIterator (); iter; ++iter)
  {
    stl::string mesh_name = common::format ("%s.mesh#%u", node.Id (), mesh_index++);

    XmlWriter::Scope scope (writer, "mesh");

    writer.WriteAttribute ("name", mesh_name.c_str ());
  }

  for (Node::NodeList::ConstIterator iter = node.Nodes ().CreateIterator (); iter; ++iter)
    save_node (params, *iter, writer);
}

void save_scene (const Params& params, const Model& model)
{
  stl::string xml_name = params.scene_file_name;

  if (xml_name.empty ())
  {
    stl::string model_base_name = common::notdir (model.Name ());

    xml_name = common::format ("%s.xscene", model_base_name.c_str ());
  }

  XmlWriter writer (xml_name.c_str ());

  XmlWriter::Scope scope (writer, "scene");

  for (NodeLibrary::ConstIterator i = model.Scenes ().CreateIterator (); i; ++i)
    save_node (params, *i, writer);
}

void save_images (const Params& params, const Model& model, ImagesMap& images_map)
{
  const EffectLibrary& model_effects = model.Effects ();

  if (!FileSystem::IsDir (params.output_textures_dir_name.c_str ()))
    FileSystem::Mkdir (params.output_textures_dir_name.c_str ());

  for (MaterialLibrary::ConstIterator iter = model.Materials ().CreateIterator (); iter; ++iter)
  {
    const Effect& effect = model_effects [iter->Effect ()];

    for (int i = 0; i < TextureMap_Num; i++)
      if (effect.HasTexture ((TextureMap)i))
      {
        const char* texture_image = effect.Texture ((TextureMap)i).Image ();

        if (images_map.find (texture_image) == images_map.end ())
        {
          static const char* FILE_URL_PREFIX = "file://";

          stl::string texture_base_name = common::basename (texture_image),
                      save_name         = common::format ("%s/%s.%s", params.output_textures_dir_name.c_str (), texture_base_name.c_str (), params.textures_format.c_str ());

          stl::string texture_path = common::notdir (model.Images () [texture_image].Path ()),
                      full_texture_path;

          if (!params.source_textures_path.empty ())
          {
            full_texture_path = params.source_textures_path;
            full_texture_path += '/';
          }

          if (!xtl::xstrncmp (texture_path.c_str (), FILE_URL_PREFIX, xtl::xstrlen (FILE_URL_PREFIX)))
            full_texture_path.append (texture_path.c_str () + xtl::xstrlen (FILE_URL_PREFIX));
          else
            full_texture_path += texture_path;

          media::Image image (full_texture_path.c_str ());

          image.Save (save_name.c_str ());

          images_map.insert_pair (texture_image, save_name);
        }
      }
  }
}

int get_texture_channel_number (const char* texture_channel)
{
  if (strstr (texture_channel, "TEX") != texture_channel)
    throw xtl::format_operation_exception ("get_texture_channel_number", "Invalid texture channel name format, must begin from 'TEX'");

  return atoi (texture_channel + xtl::xstrlen ("TEX"));
}

void save_material (const Effect& effect, ImagesMap& images_map, XmlWriter& writer)
{
  XmlWriter::Scope material_scope (writer, "Material");

  writer.WriteAttribute ("Name", effect.Id ());

  XmlWriter::Scope shader_parameters_scope (writer, "ShaderParameters");

  switch (effect.ShaderType ())
  {
    case ShaderType_Constant:
      writer.WriteAttribute (SHADER_TYPE_ATTRIBUTE, "Constant");
      break;
    case ShaderType_Lambert:
      writer.WriteAttribute (SHADER_TYPE_ATTRIBUTE, "Lambert");
      break;
    case ShaderType_Phong:
      writer.WriteAttribute (SHADER_TYPE_ATTRIBUTE, "Phong");
      break;
    case ShaderType_Blinn:
      writer.WriteAttribute (SHADER_TYPE_ATTRIBUTE, "Blinn");
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

    writer.WriteAttribute (BUMP_TEXTURE_ATTRIBUTE, images_map [texture.Image ()].c_str ());
    writer.WriteAttribute (BUMP_TEXTURE_CHANNEL_ATTRIBUTE, get_texture_channel_number (texture.TexcoordChannel ()));
    writer.WriteAttribute (BUMP_AMOUNT_ATTRIBUTE, texture.Amount ());
  }

  if (effect.HasTexture (TextureMap_Diffuse))
  {
    const Texture& texture = effect.Texture (TextureMap_Diffuse);

    writer.WriteAttribute (DIFFUSE_TEXTURE_ATTRIBUTE, images_map [texture.Image ()].c_str ());
    writer.WriteAttribute (DIFFUSE_TEXTURE_CHANNEL_ATTRIBUTE, get_texture_channel_number (texture.TexcoordChannel ()));
  }
  if (effect.HasTexture (TextureMap_Ambient))
  {
    const Texture& texture = effect.Texture (TextureMap_Ambient);

    writer.WriteAttribute (AMBIENT_TEXTURE_ATTRIBUTE, images_map [texture.Image ()].c_str ());
    writer.WriteAttribute (AMBIENT_TEXTURE_CHANNEL_ATTRIBUTE, get_texture_channel_number (texture.TexcoordChannel ()));
  }
  if (effect.HasTexture (TextureMap_Specular))
  {
    const Texture& texture = effect.Texture (TextureMap_Specular);

    writer.WriteAttribute (SPECULAR_TEXTURE_ATTRIBUTE, images_map [texture.Image ()].c_str ());
    writer.WriteAttribute (SPECULAR_TEXTURE_CHANNEL_ATTRIBUTE, get_texture_channel_number (texture.TexcoordChannel ()));
  }
  if (effect.HasTexture (TextureMap_Emission))
  {
    const Texture& texture = effect.Texture (TextureMap_Emission);

    writer.WriteAttribute (EMISSION_TEXTURE_ATTRIBUTE, images_map [texture.Image ()].c_str ());
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
  stl::string xmtl_name = params.materials_file_name;

  if (xmtl_name.empty ())
  {
    stl::string model_base_name = common::notdir (model.Name ());

    xmtl_name = common::format ("%s.xmtl", model_base_name.c_str ());
  }

  XmlWriter writer (xmtl_name.c_str ());

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

    save_property_declaration (SHADER_TYPE_ATTRIBUTE, "string", writer);
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
    save_material (*i, images_map, writer);
}

//печать протокола
void print (const char* message)
{
  printf ("%s\n", message);
}

//экспорт
void export_data (Params& params)
{
  Model model (params.source_name.c_str (), &print);

  ImagesMap images_map;

  if (!params.skip_materials)
  {
    save_images (params, model, images_map);
    save_materials (params, model, images_map);
  }

  save_meshes (params, model);
  save_scene (params, model);
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

  if (params.output_textures_dir_name.empty ())
    params.output_textures_dir_name = common::notdir (common::basename (params.source_name));

  if (params.output_meshes_dir_name.empty ())
    params.output_meshes_dir_name = common::notdir (common::basename (params.source_name));
}

int main (int argc, const char *argv[])
{
  try
  {
      //инициализация
    Params params;

    params.options         = 0;
    params.options_count   = 0;
    params.textures_format = "png";
    params.meshes_format   = "xmesh";
    params.skip_materials  = false;
    params.silent          = false;
    params.print_help      = false;

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
  }

  return 0;
}
