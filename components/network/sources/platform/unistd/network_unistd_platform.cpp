#include "shared.h"

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
    throw xtl::format_operation_exception (METHOD_NAME, "Can't resolve host name '%s'. Error '%s' while calling ::gethostbyname", host_name, hstrerror (h_errno));

  if ((size_t)entry->h_length > sizeof (address))
    throw xtl::format_operation_exception (METHOD_NAME, "Host name '%s' resolved, but host address is too long.", host_name);

  if (!entry->h_addr_list [0])
    throw xtl::format_operation_exception (METHOD_NAME, "Error at ::gethostbyname, null address list returned");

  address_size = entry->h_length;

  for (size_t i = 0; i < address_size; i++)
    address [i] = entry->h_addr_list [0][i];
}
