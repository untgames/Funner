#include <common/parser.h>

#include "shared.h"

using namespace common;

struct Configuration::Impl
{
  public:
    Impl (const char* file_name) : parser (parse_log, file_name) {}

    int GetInteger (const char* property_name, int default_value = 0)
    {
      ParseNode *node = parser.Root ()->First ("Configuration");

      if (!node)
        return default_value;

      return get<int> (node, property_name, default_value);
    }

    const char* GetString (const char* property_name, const char* default_value = "")
    {
      ParseNode *node = parser.Root ()->First ("Configuration");

      if (!node)
        return default_value;

      return get<const char*> (node, property_name, default_value);
    }

    size_t LogMessagesCount ()
    {
      return parse_log.MessagesCount ();
    }
  
    const char* LogMessage (size_t index)
    {
      if (index >= LogMessagesCount ())
        RaiseOutOfRange ("Configuration::LogMessage", "index", index, (size_t)0, LogMessagesCount ());

      return parse_log.Message (index);
    }

  private:
    ParseLog parse_log;
    Parser   parser;
};

Configuration::Configuration (const char* file_name)
  : impl (new Impl (file_name))
  {}

Configuration::~Configuration ()
{
}

int Configuration::GetInteger (const char* property_name, int default_value)
{
  return impl->GetInteger (property_name, default_value);
}

const char* Configuration::GetString (const char* property_name, const char* default_value)
{
  return impl->GetString (property_name, default_value);
}

size_t Configuration::LogMessagesCount ()
{
  return impl->LogMessagesCount ();
}

const char* Configuration::LogMessage (size_t index)
{
  return impl->LogMessage (index);
}
