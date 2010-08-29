#include <netdb.h>

#include <xtl/common_exceptions.h>

#include <platform/platform.h>

using namespace network;

/*
   Получение ip-адреса по имени хоста
*/

void UnistdPlatform::GetAddressByHostName (const char* host_name, size_t& address_size, unsigned char (&address) [16])
{
  static const char* METHOD_NAME = "network::UnistdPlatform::GetAddressByHostName";

  if (!host_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "host_name");

  hostent *entry = gethostbyname (host_name);

  if (!entry)
  {
    switch (h_errno)
    {
      case HOST_NOT_FOUND: throw xtl::format_operation_exception (METHOD_NAME, "Can't resolve host name '%s'. No such host is known.", host_name);
      case NO_DATA:        throw xtl::format_operation_exception (METHOD_NAME, "Can't resolve host name '%s'. The server recognized the request and the name, but no address is available.", host_name);
      case NO_RECOVERY:    throw xtl::format_operation_exception (METHOD_NAME, "Can't resolve host name '%s'. An unexpected server failure occurred which cannot be recovered.", host_name);
      case TRY_AGAIN:      throw xtl::format_operation_exception (METHOD_NAME, "Can't resolve host name '%s'. A temporary and possibly transient error occurred.", host_name);
      default:             throw xtl::format_operation_exception (METHOD_NAME, "Can't resolve host name '%s'. Reason unknown", host_name);
    }
  }

  if ((size_t)entry->h_length > sizeof (address))
    throw xtl::format_operation_exception (METHOD_NAME, "Host name '%s' resolved, but host address is too long.", host_name);

  if (!entry->h_addr_list [0])
    throw xtl::format_operation_exception (METHOD_NAME, "Error at ::gethostbyname, null address list returned");

  address_size = entry->h_length;

  for (size_t i = 0; i < address_size; i++)
    address [i] = entry->h_addr_list [0][i];
}
