#include <cstdio>

#include <stl/string>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/iterator.h>
#include <xtl/ref.h>

#include <common/command_line.h>
#include <common/strlib.h>

#include <media/rfx/material_library.h>

using namespace common;
using namespace media::rfx;

const char* APPLICATION_NAME = "materials-processor";

const size_t HELP_STRING_PREFIX_LENGTH  = 30;

struct Params;

//опция
struct Option
{
  CommandLine::SwitchHandler handler;       //обработчик ключа
  const char*                name;          //имя команды
  char                       short_name;    //короткое имя
  const char*                argument_name; //имя аргумента
  const char*                tip;           //подсказка
};

//параметры запуска
struct Params
{
  const Option* options;            //массив опций
  size_t        options_count;      //количество опций
  stl::string   source_material;    //файл исходного материала
  stl::string   result_material;    //файл результирующего материала
  StringArray   merge_materials;    //материалы на склейку
  bool          silent;             //отключение предупреждений
  bool          print_help;         //печать подсказки
};

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

//установка имени результирующего материала
void command_line_result_material (const char* file_name, Params& params)
{
  if (!params.result_material.empty ())
    throw xtl::format_operation_exception ("command_line_result_material", "Duplicate result material switch");

  params.result_material = file_name;
}

//имена материалов на склейку
void command_line_merge (const char* file_names, Params& params)
{
  params.merge_materials += split (file_names);
}

//установка тихого режима
void command_line_silent (const char*, Params& params)
{
  params.silent = true;
}

//проверка корректности ввода
void validate (Params& params)
{
  static const char* METHOD_NAME = "validate";

  if (params.result_material.empty ())
    throw xtl::format_operation_exception (METHOD_NAME, "Result material path not setted");

  if (params.source_material.empty ())
    throw xtl::format_operation_exception (METHOD_NAME, "Source material path not setted");
}

//генерация лицензии
void process_material (Params& params)
{
  MaterialLibrary source_library (params.source_material.c_str ());

  for (size_t i = 0, count = params.merge_materials.Size (); i < count; i++)
  {
    MaterialLibrary merge_library (params.merge_materials [i]);

    for (MaterialLibrary::ConstIterator iter = merge_library.CreateIterator (); iter; ++iter)
    {
      Material* source_material = source_library.Find (merge_library.ItemId (iter));

      if (source_material)
      {
        source_material->SetProgram (iter->Program ());

          //merge tags
        StringArray source_tags (source_material->TagsCount (), source_material->Tags ());

        for (size_t j = 0, tags_count = iter->TagsCount (); j < tags_count; j++)
        {
          const char* tag = iter->Tag (j);

          bool tag_exist = false;

          for (size_t k = 0, source_tags_count = source_tags.Size (); k < source_tags_count; k++)
          {
            if (!xtl::xstrcmp (tag, source_tags [k]))
            {
              tag_exist = true;
              break;
            }
          }

          if (!tag_exist)
            source_tags.Add (tag);
        }

        source_material->SetTags (source_tags);

          //merge properties
        PropertyMap&       source_property_map = source_material->Properties ();
        const PropertyMap& merge_property_map  = iter->Properties ();

        for (size_t j = 0, properties_count = merge_property_map.Size (); j < properties_count; j++)
        {
          const char* property_name = merge_property_map.PropertyName (j);

          if (!source_property_map.IsPresent (property_name))
            source_property_map.AddProperty (property_name, merge_property_map.PropertyType (j));

          stl::string property_string;

          merge_property_map.GetProperty (j, property_string);

          source_property_map.SetProperty (property_name, property_string.c_str ());
        }

          //merge texmaps
        for (size_t j = 0, texmaps_count = iter->TexmapCount (); j < texmaps_count; j++)
        {
          const Texmap& texmap = iter->Texmap (j);

          bool texmap_exist = false;

          for (size_t k = 0, source_texmaps_count = source_material->TexmapCount (); k < source_texmaps_count; k++)
          {
            if (!xtl::xstrcmp (source_material->Texmap (k).Semantic (), texmap.Semantic ()))
            {
              texmap_exist = true;
              source_material->SetTexmap (k, texmap);
              break;
            }
          }

          if (!texmap_exist)
            source_material->AddTexmap (texmap);
        }
      }
      else
      {
        source_library.Attach (merge_library.ItemId (iter), *iter);
      }
    }
  }

  source_library.Save (params.result_material.c_str ());
}

int main (int argc, const char *argv[])
{
  try
  {
      //инициализация
    Params params;

    static Option options [] = {
      {xtl::bind (&command_line_result_material, _1, xtl::ref (params)), "output", 'o', "output",          "result material"},
      {xtl::bind (&command_line_merge,           _1, xtl::ref (params)), "merge",  'm', "merge materials", "merge with materials"},
      {xtl::bind (&command_line_silent,          _1, xtl::ref (params)), "silent", 0,   0,                 "silent mode"},
      {xtl::bind (&command_line_help,            _1, xtl::ref (params)), "help",   '?', 0,                 "print help message"},
    };

    static const size_t options_count = sizeof (options) / sizeof (*options);

    params.options       = options;
    params.options_count = options_count;
    params.silent        = false;
    params.print_help    = false;

    CommandLine command_line;

    for (size_t i = 0; i < params.options_count; i++)
      command_line.SetSwitchHandler (options [i].name, options [i].short_name, options [i].argument_name, options [i].handler);

      //разбор командной строки
    command_line.Process (argc, argv);

      // --help только печатает сообщение помощи
    if (params.print_help)
      return 0;

    if (command_line.ParamsCount () > 1)
      throw xtl::format_operation_exception ("", "No more than one input file allowed");

    if (command_line.ParamsCount ())
      params.source_material = command_line.Param (0);

      //проверка корректности ввода
    validate (params);

      //Обработка материала
    process_material (params);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n",exception.what ()); 
  }

  return 0;
}
