#include "shared.h"

using namespace engine;

namespace
{

struct ReplacementComponent
{
  stl::string prefix;
  stl::string replacement;

  ReplacementComponent (const char* in_prefix, const char* in_replacement) : prefix (in_prefix), replacement (in_replacement) {}
};

typedef stl::vector <ReplacementComponent> ReplacementComponents;

//заполнение массива компонентов строки
void process_replacement_component (const char* prefix, const char* replacement, ReplacementComponents& replacement_components)
{
  replacement_components.push_back (ReplacementComponent (prefix, replacement));
}

}

/*
    Реализация реестра настроек
*/

struct Config::Impl : public xtl::reference_counter
{
  public:
    //Установка/получение/удаление переменных
    void SetVar (const char* name, const char* value)
    {
      variables [name] = value;
    }

    const char* GetVar (const char* name) const
    {
      VarMap::const_iterator iter = variables.find (name);

      if (iter == variables.end ())
        return "";

      return iter->second.c_str ();
    }

    void RemoveVars (const char* var_name_mask)
    {
      for (VarMap::iterator iter = variables.begin (), end = variables.end (); iter != end;)
      {
        if (common::wcmatch (iter->first.c_str (), var_name_mask))
        {
          VarMap::iterator next = iter;

          ++next;

          variables.erase (iter);

          iter = next;
        }
        else
          ++iter;
      }
    }

    //Вычисление строки с заменами на основе установленных переменных
    void Eval (const char* value, stl::string& result)
    {
      replacement_components.clear ();
      result.clear ();

      common::parse_format_string (value, xtl::bind (&process_replacement_component, _1, _2, xtl::ref (replacement_components)));

      for (ReplacementComponents::iterator iter = replacement_components.begin (), end = replacement_components.end (); iter != end; ++iter)
      {
        result += iter->prefix;
        result += ResolveVar (iter->replacement.c_str ());
      }
    }

    //Обход всех переменных / переменных по маске
    void EnumerateVars (const EnumHandler& handler) const
    {
      for (VarMap::const_iterator iter = variables.begin (), end = variables.end (); iter != end; ++iter)
        handler (iter->first.c_str (), iter->second.c_str ());
    }

    void EnumerateVars (const char* var_name_mask, const EnumHandler& handler) const
    {
      for (VarMap::const_iterator iter = variables.begin (), end = variables.end (); iter != end; ++iter)
      {
        if (common::wcmatch (iter->first.c_str (), var_name_mask))
          handler (iter->first.c_str (), iter->second.c_str ());
      }
    }

    //Вызов обработчиков сохранения переменных
    void Save ()
    {
      save_signal ();
    }

    //Регистрация обработчиков сохранения переменных
    xtl::connection RegisterSaveHandler (const SaveHandler& handler)
    {
      return save_signal.connect (handler);
    }

  private:
    typedef xtl::signal <void ()>                    SaveSignal;
    typedef stl::hash_map <stl::string, stl::string> VarMap;
    typedef stl::vector <stl::string>                StringVector;

  private:
    //вычисление значения переменной с разрешением замен
    stl::string ResolveVar (const char* var_name, StringVector forbidden_var_names = StringVector ())
    {
      if (!xtl::xstrlen (var_name))
        return "";

      const char* var_value = GetVar (var_name);

      if (!xtl::xstrlen (var_value))
        return common::format ("{%s}", var_name);

      forbidden_var_names.push_back (var_name);

      ReplacementComponents var_components;

      common::parse_format_string (var_value, xtl::bind (&process_replacement_component, _1, _2, xtl::ref (var_components)));

      stl::string return_value;

      for (ReplacementComponents::iterator iter = var_components.begin (), end = var_components.end (); iter != end; ++iter)
      {
        for (StringVector::iterator forbidden_vars_iter = forbidden_var_names.begin (), forbidden_vars_end = forbidden_var_names.end (); forbidden_vars_iter != forbidden_vars_end; ++forbidden_vars_iter)
          if (*forbidden_vars_iter == iter->replacement)
            throw xtl::format_operation_exception ("engine::Config::Impl::ResolveVar", "Can't resolve recursive var '%s'", iter->replacement.c_str ());

        return_value += iter->prefix;
        return_value += ResolveVar (iter->replacement.c_str (), forbidden_var_names);
      }

      return return_value;
    }

  private:
    SaveSignal            save_signal;
    VarMap                variables;
    ReplacementComponents replacement_components;
};

/*
   Конструктор/деструктор/копирование
*/

Config::Config ()
  : impl (new Impl)
  {}

Config::Config (const Config& source)
  : impl (source.impl)
{
  addref (impl);
}

Config::~Config ()
{
  release (impl);
}

Config& Config::operator = (const Config& source)
{
  Config (source).Swap (*this);

  return *this;
}

/*
   Установка/получение/удаление переменных
*/

void Config::SetVar (const char* name, const char* value)
{
  static const char* METHOD_NAME = "engine::Config::SetVar";

  if (!name || !xtl::xstrlen (name))
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");

  if (!value)
    throw xtl::make_null_argument_exception (METHOD_NAME, "value");

  impl->SetVar (name, value);
}

const char* Config::GetVar (const char* name) const
{
  if (!name)
    throw xtl::make_null_argument_exception ("engine::Config::GetVar", "name");

  return impl->GetVar (name);
}

void Config::RemoveVars (const char* var_name_mask)
{
  if (!var_name_mask)
    throw xtl::make_null_argument_exception ("engine::Config::RemoveVars", "var_name_mask");

  impl->RemoveVars (var_name_mask);
}

/*
   Вычисление строки с заменами на основе установленных переменных
*/

void Config::Eval (const char* value, stl::string& result) const
{
  static const char* METHOD_NAME = "engine::Config::Eval";

  if (!value)
    throw xtl::make_null_argument_exception (METHOD_NAME, "value");

  try
  {
    impl->Eval (value, result);
  }
  catch (xtl::exception& e)
  {
    throw xtl::format_operation_exception (METHOD_NAME, "Can't eval string '%s', exception: '%s'", value, e.what ());
  }
}

stl::string Config::Eval (const char* value) const
{
  stl::string return_value;

  Eval (value, return_value);

  return return_value;
}

/*
   Обход всех переменных / переменных по маске
*/

void Config::EnumerateVars (const EnumHandler& handler) const
{
  impl->EnumerateVars (handler);
}

void Config::EnumerateVars (const char* var_name_mask, const EnumHandler& handler) const
{
  if (!var_name_mask)
    throw xtl::make_null_argument_exception ("engine::Config::EnumerateVars", "var_name_mask");

  if (!xtl::xstrcmp ("*", var_name_mask))
    EnumerateVars (handler);
  else
    impl->EnumerateVars (var_name_mask, handler);
}

/*
   Вызов обработчиков сохранения переменных
*/

void Config::Save ()
{
  impl->Save ();
}

/*
   Регистрация обработчиков сохранения переменных
*/

xtl::connection Config::RegisterSaveHandler (const SaveHandler& handler)
{
  return impl->RegisterSaveHandler (handler);
}

/*
   Обмен
*/

void Config::Swap (Config& source)
{
  stl::swap (impl, source.impl);
}

namespace engine
{

/*
   Обмен
*/

void swap (Config& source1, Config& source2)
{
  source1.Swap (source2);
}

}

