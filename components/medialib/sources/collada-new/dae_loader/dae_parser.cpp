#include <stdarg.h>

#include "shared.h"

/*
    �����������
*/

DaeParser::DaeParser (const char* file_name, Model& in_model, const LogFunction& log)
  : model (in_model), parser (parse_log, file_name), current_scope (0)
{
  try
  {
    ParseRoot (parser.Root ()->First ("COLLADA"));
  }
  catch (...)
  {
    PrintLog (log);
    throw;
  }
  
  PrintLog (log);  
}

/*
    ������ ������
*/

void DaeParser::PrintLog (const LogFunction& log)
{
  for (size_t i=0; i<parse_log.MessagesCount (); ++i)
    log (parse_log.Message (i));    
}

/*
    ����������������
*/

void DaeParser::LogError (Parser::Node* node, const char* format, ...)
{
  va_list list;
  
  va_start (list, format);
  parse_log.VError (node, format, list);  
  
  LogScope* scope = current_scope;
  
  if (!scope)
    return;
  
  if (scope->Node () == node)
    scope = scope->Prev ();
  
  for (; scope; scope=scope->Prev ())
  {
    Parser::Node* node = scope->Node ();
    const char* id = get<const char*> (node, "id");

    if (id) parse_log.Error (node, "at parse %s '%s'", node->Tag (), id);
    else    parse_log.Error (node, "at parse %s", node->Tag ());
  }
}

void DaeParser::LogWarning (Parser::Node* node, const char* format, ...)
{
  va_list list;
  
  va_start (list, format);
  parse_log.VWarning (node, format, list);
}

/*
    ������ ��������� ����
*/

void DaeParser::ParseRoot (Parser::Iterator iter)
{
  if (!iter)
  {
    LogError (iter, "Wrong file format. No 'COLLADA' tag\n");
    return;
  }
  
  if (!test (iter, "version", "1.4.1"))  
    LogWarning (iter, "Currently supported Collada version - 1.4.1, document uses other version (%s), "
                      "some features may be not fully supported or unsupported", get<const char*> (iter, "version", "UNDEFINED"));
    
  ParseLibraries (iter);
}

/*
    ������ ���������
*/

void DaeParser::ParseLibraries (Parser::Iterator iter)
{
  for_each_child (iter, "library_effects", bind (&DaeParser::ParseLibraryEffects, this, _1));
  for_each_child (iter, "library_materials", bind (&DaeParser::ParseLibraryMaterials, this, _1));
}

/*
    ������� �������� ������
*/

void Model::DefaultDaeLoader (const char* file_name, Model& model, const LogFunction& log)
{
  DaeParser (file_name, model, log);
}

/*
    LogScope
*/

LogScope::LogScope (Parser::Node* in_node, DaeParser& in_parser)
  : parser (in_parser), node (in_node), prev (parser.current_scope)
{
  parser.current_scope = this;
}

LogScope::~LogScope ()
{
  parser.current_scope = prev;
}
