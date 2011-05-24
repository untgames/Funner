#include "license_manager_shared.h"

#ifdef _MSC_VER
   #include <time.h>
#endif

using namespace common;

namespace
{

/*
    Константы
*/

const size_t COMPONENTS_ARRAY_RESERVE_SIZE = 64; //резервируемое количество компонентов

const char* ALLOWED_COMPONENTS [] = { "common.*" };
const char* IGNORED_PROPERTIES [] = { "CheckFiles", "LicenseHash", "AllowedComponents" };

const size_t IGNORED_PROPERTIES_COUNT = sizeof (IGNORED_PROPERTIES) / sizeof (*IGNORED_PROPERTIES);

/*
   Описание реализации менеджера лицензий
*/

class LicenseManagerImpl
{
  public:
    ///Конструктор
    LicenseManagerImpl ()
      : license_loaded (false), till_time (0)
    {
      allowed_components.Reserve (COMPONENTS_ARRAY_RESERVE_SIZE);
    }

    ///Загрузка лицензии
    void Load (const char* license_path)
    {
      license_loaded = false;

      try
      {
        if (!license_path)
          throw xtl::make_null_argument_exception ("", "license_path");

        properties.Clear ();
        allowed_components.Clear ();

        Parser    p (license_path, "xml");
        ParseLog  log = p.Log ();
        ParseNode root = p.Root ().First ("License");

        if (!root)
          log.Error (p.Root (), "No root 'License' tag");

        for (size_t i = 0; i < log.MessagesCount (); i++)
          switch (log.MessageType (i))
          {
            case ParseLogMessageType_Error:
            case ParseLogMessageType_FatalError:
              throw xtl::format_operation_exception ("", log.Message (i));
            default:
              break;
          }

          //чтение периода действия лицензии

        const char *since_date_string = common::get<const char*> (root, "SinceDate", ""),
                   *till_date_string  = common::get<const char*> (root, "TillDate", ""),
                   *license_hash      = common::get<const char*> (root, "LicenseHash");

        time_t current_time;
        
        time (&current_time);

        if (xtl::xstrlen (since_date_string))
        {
          time_t since_time;

          ParseDate (since_date_string, since_time);

          if (difftime (current_time, since_time) < 0)
            throw xtl::format_operation_exception ("", "License '%s' is not valid yet", license_path);
        }

        if (xtl::xstrlen (till_date_string))
        {
          ParseDate (till_date_string, till_time);

          if (difftime (till_time, current_time) < 0)
            throw xtl::format_operation_exception ("", "License '%s' has expired", license_path);
        }

          //чтение произвольных свойств

        for (ParseNode node = root.First (); node; node = node.Next ())
          AddNodeProperties (node, "");

          //чтение разрешенных компонентов

        for (Parser::NamesakeIterator component_iter = root.First ("AllowedComponents.Component"); component_iter; ++component_iter)
          allowed_components.Add (common::get<const char*> (*component_iter, "Wildcard"));

        stl::string allowed_components_property_string;

        for (size_t i = 0, count = allowed_components.Size (); i < count; i++)
          allowed_components_property_string += allowed_components [i];

        properties.SetProperty ("AllowedComponents", allowed_components_property_string.c_str ());

          //проверка корректности содержимого лицензии

        stl::vector<CheckFile> check_files_list;

        for (Parser::NamesakeIterator check_file_iter = root.First ("CheckFiles.File"); check_file_iter; ++check_file_iter)
          check_files_list.push_back (CheckFile (common::get<const char*> (*check_file_iter, "Path"), common::get<size_t> (*check_file_iter, "HashSize", 0)));

        unsigned char check_string_hash [16];

        calculate_license_hash (check_files_list, properties, check_string_hash);

        stl::string hex_check_string_hash;

        for (size_t i = 0; i < 16; i++)
          hex_check_string_hash += common::format ("%02x", check_string_hash [i]);

        if (hex_check_string_hash != license_hash)
          throw xtl::format_operation_exception ("", "Invalid license file '%s', hash mismatch", license_path);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("common::LicenseManager::Load");
        throw;
      }

      license_loaded = true;
    }

    ///Проверка состояния
    int DaysToExpiration ()
    {
      static const char* METHOD_NAME = "common::LicenseManager::DaysToExpiration";

      LoadDefaultLicenses ();

      if (!license_loaded)
        throw xtl::format_operation_exception (METHOD_NAME, "License not loaded");

      if (!till_time)
        return INT_MAX;

      time_t current_time;

      time (&current_time);

      return (int)(difftime (till_time, current_time) / (60.f * 60.f * 24.f));
    }

    ///Работа с предопределенными свойствами
    bool IsComponentAllowed (const char* component_name)
    {
      static const char* METHOD_NAME = "common::LicenseManager::IsComponentAllowed";

      for (size_t i = 0, count = sizeof (ALLOWED_COMPONENTS) / sizeof (*ALLOWED_COMPONENTS); i < count; i++)
        if (common::wcmatch (component_name, ALLOWED_COMPONENTS [i]))
          return true;

      LoadDefaultLicenses ();

      if (!license_loaded)
        throw xtl::format_operation_exception (METHOD_NAME, "License not loaded");

      for (size_t i = 0, count = allowed_components.Size (); i < count; i++)
        if (common::wcmatch (component_name, allowed_components [i]))
          return true;
          
      return false;
    }

    ///Работа со свойствами
    const char* GetProperty (const char* property_name)
    {
      static const char* METHOD_NAME = "common::LicenseManager::GetProperty";

      LoadDefaultLicenses ();

      if (!license_loaded)
        throw xtl::format_operation_exception (METHOD_NAME, "License not loaded");

      return properties.GetString (property_name);
    }

    const char* GetProperty (size_t property_index)
    {
      static const char* METHOD_NAME = "common::LicenseManager::GetProperty";

      LoadDefaultLicenses ();

      if (!license_loaded)
        throw xtl::format_operation_exception (METHOD_NAME, "License not loaded");

      return properties.GetString (property_index);
    }

    const char* GetPropertyName (size_t property_index)
    {
      static const char* METHOD_NAME = "common::LicenseManager::GetPropertyName";

      LoadDefaultLicenses ();

      if (!license_loaded)
        throw xtl::format_operation_exception (METHOD_NAME, "License not loaded");

      return properties.PropertyName (property_index);
    }

    bool HasProperty (const char* property_name)
    {
      static const char* METHOD_NAME = "common::LicenseManager::HasProperty";

      LoadDefaultLicenses ();

      if (!license_loaded)
        throw xtl::format_operation_exception (METHOD_NAME, "License not loaded");

      return properties.IsPresent (property_name);
    }

    size_t PropertiesCount ()
    {
      static const char* METHOD_NAME = "common::LicenseManager::PropertiesCount";

      LoadDefaultLicenses ();

      if (!license_loaded)
        throw xtl::format_operation_exception (METHOD_NAME, "License not loaded");

      return properties.Size ();
    }

  private:
    void AddNodeProperties (ParseNode node, const char* base_name)
    {
      for (size_t i = 0; i < IGNORED_PROPERTIES_COUNT; i++)
        if (!xtl::xstrcmp (node.Name (), IGNORED_PROPERTIES [i]))
          return;

      stl::string node_full_name = base_name;

      if (!node_full_name.empty ())
        node_full_name.append (".");

      node_full_name.append (node.Name ());

      if (node.AttributesCount ())
      {
        if (properties.IsPresent (node_full_name.c_str ()))
          throw xtl::format_operation_exception ("common::LicenseManager::AddNodeProperty", "Duplicate property '%s'", node_full_name.c_str ());

        stl::string property_value;

        for (size_t i = 0, count = node.AttributesCount (); i < count; i++)
          property_value += node.Attribute (i);

        properties.SetProperty (node_full_name.c_str (), property_value.c_str ());
      }

      for (ParseNode child = node.First (); child; child = child.Next ())
        AddNodeProperties (child, node_full_name.c_str ());
    }

    void ParseDate (const char* date_string, time_t& result_date)
    {
      static const char* METHOD_NAME = "common::LicenseManager::ParseDate";

      if (!rematch (date_string, "[:digit:][:digit:][:digit:][:digit:]-[:digit:][:digit:]-[:digit:][:digit:]"))
        throw xtl::format_operation_exception (METHOD_NAME, "Invalid date string '%s' format, must be yyyy-mm-dd", date_string);

      tm tm_date;

      memset (&tm_date, 0, sizeof (tm_date));

      tm_date.tm_mday = atoi (date_string + 8);
      tm_date.tm_mon  = atoi (date_string + 5) - 1;
      tm_date.tm_year = atoi (date_string) - 1900;


#if defined (ANDROID) || defined (__MINGW32__) || defined (WINCE)
      time_t date = mktime (&tm_date);
#elif defined (_MSC_VER)
      time_t date = _mkgmtime32 (&tm_date);
#else
      time_t date = timegm (&tm_date);
#endif

      if (date == -1)
        throw xtl::format_operation_exception (METHOD_NAME, "Can't convert date string '%s'", date_string);

      result_date = date;
    }

    void LoadDefaultLicenses ()
    {
      static ComponentLoader loader ("common.licensing.loaders.*");
    }

  private:
    bool        license_loaded;
    StringArray allowed_components;
    PropertyMap properties;
    time_t      till_time;
};

typedef common::Singleton<LicenseManagerImpl> LicenseManagerSingleton;

}

/*
   Загрузка лицензии
*/

void LicenseManager::Load (const char* license_path)
{
  LicenseManagerSingleton::Instance ()->Load (license_path);
}

/*
   Проверка состояния
*/

int LicenseManager::DaysToExpiration ()
{
  return LicenseManagerSingleton::Instance ()->DaysToExpiration ();
}

/*
   Работа с предопределенными свойствами
*/
    
bool LicenseManager::IsComponentAllowed (const char* component_name)
{
  return LicenseManagerSingleton::Instance ()->IsComponentAllowed (component_name);
}

/*
   Работа со свойствами
*/
    
const char* LicenseManager::GetProperty (const char* property_name)
{
  return LicenseManagerSingleton::Instance ()->GetProperty (property_name);
}

const char* LicenseManager::GetProperty (size_t property_index)
{
  return LicenseManagerSingleton::Instance ()->GetProperty (property_index);
}

const char* LicenseManager::GetPropertyName (size_t property_index)
{
  return LicenseManagerSingleton::Instance ()->GetPropertyName (property_index);
}

bool LicenseManager::HasProperty (const char* property_name)
{
  return LicenseManagerSingleton::Instance ()->HasProperty (property_name);
}

size_t LicenseManager::PropertiesCount ()
{
  return LicenseManagerSingleton::Instance ()->PropertiesCount ();
}
