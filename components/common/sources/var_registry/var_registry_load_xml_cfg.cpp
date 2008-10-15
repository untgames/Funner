#include "shared.h"

using namespace common;
using namespace xtl;

namespace
{

void parse_node (VarRegistry& registry, const ParseNode& node)
{
  if (!node)
    return;

  int         children_auto_index = 0, children_auto_step  = 1;
  stl::string prefix              = "AutoId", tmp_buffer;

  for (ParseNode iter = node.First (); iter; iter=iter.Next ())
  {
    const char* name = iter.Name ();

    if (!xstrcmp ("auto", name))
    {
      tmp_buffer = format ("%s%d", prefix.c_str (), children_auto_index);
      name       = tmp_buffer.c_str ();

      children_auto_index += children_auto_step;
    }
    else if (!xstrcmp ("auto_template", iter.Name ()))
    {
      const char* new_prefix = get<const char*> (iter, "prefix", 0);
      
      if (new_prefix)
        prefix = new_prefix;

      children_auto_index = get<int> (iter, "index", children_auto_index);
      children_auto_step  = get<int> (iter, "step", children_auto_step);

      continue;
    }

    if (iter.AttributesCount ())
      registry.SetValue (name, stl::string (iter.Attribute (0)));

    if (iter.First ())
    {
      VarRegistry sub_registry (format ("%s.%s", registry.BranchName (), name).c_str ());

      parse_node (sub_registry, iter);
    }
  }
}

void load_xml (VarRegistry& registry, const char* file_name, const char* root_name_mask)
{
  static const char* METHOD_NAME = "common::load_xml_configuration";

  if (!file_name)
    throw make_null_argument_exception (METHOD_NAME, "file_name");

  Parser p (file_name);
  
  ParseLog log = p.Log ();

  for (size_t i = 0; i < log.MessagesCount (); i++)
    switch (log.MessageType (i))
    {
      case ParseLogMessageType_Error:
      case ParseLogMessageType_FatalError:
        throw format_operation_exception (METHOD_NAME, log.Message (i));
      default:
        break;
    }
  

  if (!root_name_mask)
  {
    parse_node (registry, p.Root ());
    return;
  }

  if (!p.Root ().First ())
    return;

  if (wcmatch (p.Root ().First ().Name (), root_name_mask))
    parse_node (registry, p.Root ().First ());
}

}

namespace common
{

/*
   Загрузка конфигураций из XML файла
*/

void load_xml_configuration (VarRegistry& registry, const char* file_mask)
{
  static const char* METHOD_NAME = "common::load_xml_configuration";

  if (!file_mask)
    throw make_null_argument_exception (METHOD_NAME, "file_mask");

  if (!is_wildcard (file_mask))
  {
    load_xml (registry, file_mask, 0);
    return;
  }

  FileList config_files (FileSystem::Search (file_mask, FileSearch_Files));

  for (size_t i = 0; i < config_files.Size (); i++)
    load_xml (registry, config_files.Item (i).name, 0);
}

void load_xml_configuration (VarRegistry& registry, const char* file_mask, const char* root_name_mask)
{
  static const char* METHOD_NAME = "common::load_xml_configuration";

  if (!file_mask)
    throw make_null_argument_exception (METHOD_NAME, "file_mask");

  if (!root_name_mask)
    throw make_null_argument_exception (METHOD_NAME, "root_name_mask");

  if (!is_wildcard (file_mask))
  {
    load_xml (registry, file_mask, root_name_mask);
    return;
  }

  FileList config_files (FileSystem::Search (file_mask, FileSearch_Files));

  for (size_t i = 0; i < config_files.Size (); i++)
    load_xml (registry, config_files.Item (i).name, root_name_mask);
}

}
