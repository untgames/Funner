#include "shared.h"

using namespace common;

/*
    Конструктор / деструктор
*/

ParseTree::ParseTree (ParseTreeBuffer& in_buffer, const ParseLog* in_log)
{
  if (in_log)
    log = new ParseLog (*in_log);

  buffer.swap (in_buffer);  
}

ParseTree::~ParseTree ()
{
}

/*
    Протокол разбора
*/

ParseLog& ParseTree::Log ()
{
  if (!log)
    log = new ParseLog;
    
  return *log;  
}
