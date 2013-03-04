#ifdef _MSC_VER
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netdb.h>
#include <unistd.h>

#include <sys/socket.h>
#endif

#include <stdio.h>

#ifdef ANDROID
#include <linux/in.h>
#include <linux/in6.h>
#include <sys/endian.h>
#endif

#ifdef TABLETOS
#include <sys/time.h>
#include <netinet/in.h>
#endif

#include <sgi_stl/stdexcept>

#include <errno.h>
#include <fcntl.h>

#include <system/tcp_client.h>
#include <utility/utils.h>
