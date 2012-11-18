#include "shared.h"

using namespace social;
using namespace social::facebook;

namespace
{

const char* LOG_NAME            = "social.facebook.FacebookSession";
const char* SESSION_DESCRIPTION = "Facebook";

}

/*
   Конструктор / Деструктор
*/

FacebookSessionImpl::FacebookSessionImpl ()
  : log (LOG_NAME)
{
  LogIn (LoginCallback (), common::PropertyMap ());
}

FacebookSessionImpl::~FacebookSessionImpl ()
{
}

/*
   Описание сессии
*/

const char* FacebookSessionImpl::GetDescription ()
{
  return SESSION_DESCRIPTION;
}

/*
   Показ стандартных окон
*/

void FacebookSessionImpl::ShowWindow (const char* window_name, const common::PropertyMap& properties)
{
  static const char* METHOD_NAME = "social::facebook::FacebookSessionImpl::ShowWindow";

  try
  {
    if (!window_name)
      throw xtl::make_null_argument_exception ("", "window_name");
  }
  catch (xtl::exception& e)
  {
    e.touch (METHOD_NAME);
    throw;
  }
}

/*
   Логин
*/

void FacebookSessionImpl::LogIn (const LoginCallback& callback, const common::PropertyMap& properties)
{

}

void FacebookSessionImpl::LogOut ()
{

}

bool FacebookSessionImpl::IsUserLoggedIn ()
{
  return false;  //TODO
}

/*
   Проверка наличия в PropertyMap неизвестных полей
*/

void FacebookSessionImpl::CheckUnknownProperties (const char* source, const common::PropertyMap& properties,
                                                 size_t known_properties_count, const char** known_properties_names)
{
  static const char* METHOD_NAME = "social::facebook::FacebookSessionImpl::CheckUnknownProperties";

  if (!source)
    throw xtl::make_null_argument_exception (METHOD_NAME, "source");

  if (known_properties_count && !known_properties_names)
    throw xtl::make_null_argument_exception (METHOD_NAME, "known_properties_names");

  for (size_t i = 0, count = properties.Size (); i < count; i++)
  {
    const char* property_name = properties.PropertyName (i);

    bool property_known = false;

    for (size_t j = 0; j < known_properties_count; j++)
    {
      if (!xtl::xstrcmp (property_name, known_properties_names [j]))
      {
        property_known = true;
        break;
      }
    }

    if (!property_known)
      common::Log (LOG_NAME).Printf ("Unknown property '%s' at '%s'", property_name, source);
  }
}
