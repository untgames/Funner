#ifndef NETWORK_WRAPPERS_TESTS_SHARED_HEADER
#define NETWORK_WRAPPERS_TESTS_SHARED_HEADER

#include <cstdio>
#include <exception>

#include <xtl/common_exceptions.h>
#include <xtl/function.h>

#include <common/log.h>

#include <network/inet_address.h>
#include <network/url_connection.h>

using namespace network;

void print_log (const char* stream, const char* message)
{
  printf ("%s: %s\n", stream, message);
  fflush (stdout);
}

#endif
