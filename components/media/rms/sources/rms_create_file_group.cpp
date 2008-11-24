#include "shared.h"

using namespace media::rms;
using namespace common;

namespace
{

/*
    Константы
*/

static const char* RESOURCE_LIST_FILE_MASK = "*.xreslist"; //шаблон имени файла со списокм ресурсов

/*
    Утилиты
*/

//обработка списка ресурсов
void process_resource_list (size_t count, const char** list, Group& result);

//обработка файла со списком ресурсов
void process_resource_list_file (const char* file_name, Group& result)
{
  try
  {
    Parser   parser (file_name, "xml");
    ParseLog log = parser.Log ();
    
    for (size_t i=0; i<log.MessagesCount (); i++)
      switch (log.MessageType (i))
      {
        case ParseLogMessageType_FatalError:
        case ParseLogMessageType_Error:
          throw xtl::format_operation_exception ("", "%s", log.Message (i));
        default:
          break;
      }

    ParseNode root = parser.Root ().First ("ResourceList.#text");
    
    if (!root)
      throw xtl::format_operation_exception ("", "'ResourceList' root node not found");

    process_resource_list (root.AttributesCount (), root.Attributes (), result);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::rms::process_resource_list_file(%s)", file_name);
    throw;
  }
}

//обработка отдельного файла
void process_file (const char* file_name, Group& result)
{
  if (!*file_name)
    return;

  if (wcimatch (file_name, RESOURCE_LIST_FILE_MASK))
  {
    process_resource_list_file (file_name, result);
  }
  else
  {
    result.Add (file_name);
  }
}

//обработка wildcard
void process_wildcard (const char* file_mask, Group& result)
{
  for (FileListIterator i=FileSystem::Search (file_mask, FileSearch_Files | FileSearch_Sort); i; ++i)
    process_file (i->name, result);
}

//обработка списка ресурсов
void process_resource_list (size_t count, const char** list, Group& result)
{
  for (size_t i=0; i<count; i++)
  {
    static const char* WILDCARD_CHARS     = "?*";
    static const char* WILDCARD_CHARS_END = WILDCARD_CHARS + strlen (WILDCARD_CHARS);
    
    const char *item = list [i], *item_end = item + strlen (item);
    
    if (stl::find_first_of (item, item_end, WILDCARD_CHARS, WILDCARD_CHARS_END) != item_end)
    {
      process_wildcard (item, result); 
    }
    else
    {
      process_file (item, result);
    }
  }
}

}

/*
    Создание группы из списка файлов / файла ресурсов
*/

namespace media
{

namespace rms
{

Group create_file_group (const char* files)
{
  try
  {
    Group result;

    add_file_group (files, result);

    return result;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::rms::create_file_group");
    throw;
  }
}

void add_file_group (const char* files, Group& group)
{
  try
  {
    if (!files)
      throw xtl::make_null_argument_exception ("", "files");

    StringArray list = split (files);

    process_resource_list (list.Size (), list.Data (), group);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::rms::add_file_group");
    throw;
  }
}

}

}
