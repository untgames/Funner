#include "shared.h"

using namespace common;
using namespace xtl;

namespace
{

void parse_node (VarRegistry& registry, ParseNode* node, const char* prefix)
{
  if (!node)
    return;

  stl::string branch_name;

  branch_name.reserve (strlen (prefix) + strlen (node->Tag ()) + 1);

  if (*prefix)
  {
    branch_name = prefix;
    branch_name += '.';
  }

  branch_name += node->Tag ();

  for (Parser::AttributeIterator attribute_iter = make_attribute_iterator (node); attribute_iter; ++attribute_iter)
    registry.SetValue (branch_name.c_str (), stl::string (*attribute_iter));

  for (ParseNode* iter = node->First (); iter; iter = iter->Next ())
    parse_node (registry, iter, branch_name.c_str ());
}

}

namespace common
{

/*
   Загрузка конфигураций из XML файла
*/

void load_xml_configurations (VarRegistry& registry, const char* file_mask)
{
  static const char* METHOD_NAME = "common::load_xml_configurations";

  if (!file_mask)
    throw make_null_argument_exception (METHOD_NAME, "file_mask");

  FileList config_files (FileSystem::Search (file_mask, FileSearch_Files));

  for (size_t i = 0; i < config_files.Size (); i++)
    load_xml_configuration (registry, config_files.Item (i).name);
}

void load_xml_configuration (VarRegistry& registry, const char* file_name)
{
  static const char* METHOD_NAME = "common::load_xml_configuration";

  if (!file_name)
    throw make_null_argument_exception (METHOD_NAME, "file_name");

  ParseLog log;
  Parser   p (log, file_name);

  for (size_t i = 0; i < log.MessagesCount (); i++)
    if (log.MessageType (i) == PARSE_LOG_ERROR || log.MessageType (i) == PARSE_LOG_FATAL_ERROR)
      throw format_operation_exception (METHOD_NAME, log.Message(i));

  parse_node (registry, p.Root ()->First (), "");
}

}
