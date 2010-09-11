#include <stl/hash_map>
#include <stl/string>

#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/string.h>

#include <common/command_line.h>
#include <common/string.h>
#include <common/strlib.h>

using namespace common;

namespace
{

const char*  LONG_OPTION_NAME_PREFIX        = "--";
const size_t LONG_OPTION_NAME_PREFIX_LENGTH = xtl::xstrlen (LONG_OPTION_NAME_PREFIX);

struct SwitchHandlerDesc
{
  char                       switch_short_name; //короткое имя
  stl::string                arg_name;          //имя аргумента
  CommandLine::SwitchHandler handler;           //функция-обработчик опции

  SwitchHandlerDesc (char in_switch_short_name, const char* in_arg_name, const CommandLine::SwitchHandler& in_handler)
    : switch_short_name (in_switch_short_name), arg_name (in_arg_name ? in_arg_name : ""), handler (in_handler)
    {}
};

typedef stl::hash_map<stl::hash_key<const char*>, SwitchHandlerDesc> SwitchHandlerMap;

}

struct CommandLine::Impl
{
  SwitchHandlerMap switch_handlers;
  StringArray      params;

  ///Регистрация обработчиков параметров коммандной строки
  void SetSwitchHandler (const char* switch_long_name, char switch_short_name, const char* arg_name, const CommandLine::SwitchHandler& handler)
  {
    static const char* METHOD_NAME = "common::CommandLine::SetSwitchHandler";

    if (switch_handlers.find (switch_long_name) != switch_handlers.end ())
      throw xtl::format_operation_exception (METHOD_NAME, "Handler with switch long name '%s' already setted", switch_long_name);

    if (switch_short_name)
      for (SwitchHandlerMap::iterator iter = switch_handlers.begin (), end = switch_handlers.end (); iter != end; ++iter)
        if (iter->second.switch_short_name == switch_short_name)
          throw xtl::format_operation_exception (METHOD_NAME, "Handler with switch short name '%c' already setted", switch_short_name);

    switch_handlers.insert_pair (switch_long_name, SwitchHandlerDesc (switch_short_name, arg_name, handler));
  }

  ///Обработка коммандной строки
  void Process (int argc, const char** argv)
  {
    static const char* METHOD_NAME = "common::CommandLine::Process";

    params.Clear ();

    for (int i = 1; i < argc; i++)
    {
      const char* arg = argv [i];

      bool long_option  = !xtl::xstrncmp (LONG_OPTION_NAME_PREFIX, arg, LONG_OPTION_NAME_PREFIX_LENGTH);
      bool short_option = !long_option && *arg == '-' && arg [1];

      if (!long_option && !short_option)
      {
        params.Add (arg);
        continue;
      }

      stl::string option_name;
      stl::string option_argument;

      SwitchHandlerMap::iterator option = switch_handlers.end ();

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

        option = switch_handlers.find (option_name.c_str ());

        if (option == switch_handlers.end ())
          throw xtl::format_operation_exception (METHOD_NAME, "Wrong option '--%s'", option_name.c_str ());

        option_name = LONG_OPTION_NAME_PREFIX + option_name;
      }

        //разбор коротких опций

      if (short_option)
      {
        arg++;

        if (arg [1])
          throw xtl::format_operation_exception (METHOD_NAME, "Wrong option '-%s'", arg);

        for (SwitchHandlerMap::iterator iter = switch_handlers.begin (), end = switch_handlers.end (); iter != end; ++iter)
        {
          if (*arg == iter->second.switch_short_name)
          {
            option = iter;
            break;
          }
        }

        if (option == switch_handlers.end ())
          throw xtl::format_operation_exception (METHOD_NAME, "Wrong option '-%c'", *arg);

        option_name = common::format ("-%c", *arg);

        if (!option->second.arg_name.empty ())
        {
          i++;

          if (i >= argc)
            throw xtl::format_operation_exception (METHOD_NAME, "option '-%c' require argument", *arg);

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
              throw xtl::format_operation_exception (METHOD_NAME, "Unquoted string at parse option '%s'", option_name.c_str ());

            break;
          }
          default:
            option_argument.push_back (*arg);
            break;
        }
      }

      if (!option->second.arg_name.empty () && option_argument.empty ())
        throw xtl::format_operation_exception (METHOD_NAME, "Option '%s' require argument", option_name.c_str ());

      if (option->second.arg_name.empty () && !option_argument.empty ())
        throw xtl::format_operation_exception (METHOD_NAME, "Option '%s' has no arguments", option_name.c_str ());

      option->second.handler (option_argument.c_str ());
    }
  }

  ///Получение списка параметров (аргументы в коммандной строке без ключа)
  size_t ParamsCount ()
  {
    return params.Size ();
  }

  const char* Param (size_t index)
  {
    return params [index];
  }
};

/*
   Конструктор/деструктор
*/

CommandLine::CommandLine ()
  : impl (new Impl)
  {}

CommandLine::~CommandLine ()
{
  delete impl;
}

/*
   Регистрация обработчиков параметров коммандной строки
*/

void CommandLine::SetSwitchHandler (const char* switch_long_name, char switch_short_name, const char* arg_name, const SwitchHandler& handler)
{
  static const char* METHOD_NAME = "common::CommandLine::SetSwitchHandler";

  if (!switch_long_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "switch_long_name");

  impl->SetSwitchHandler (switch_long_name, switch_short_name, arg_name, handler);
}

/*
   Обработка коммандной строки
*/

void CommandLine::Process (int argc, const char** argv)
{
  impl->Process (argc, argv);
}

/*
   Получение списка параметров (аргументы в коммандной строке без ключа)
*/

size_t CommandLine::ParamsCount () const
{
  return impl->ParamsCount ();
}

const char* CommandLine::Param (size_t index) const
{
  if (index >= ParamsCount ())
    throw xtl::make_range_exception ("common::CommandLine::Param", "index", 0u, ParamsCount ());

  return impl->Param (index);
}
