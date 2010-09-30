#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>

#include <netinet/tcp.h>
#include <sys/socket.h>

#include <xtl/common_exceptions.h>
#include <xtl/reference_counter.h>

#include <common/log.h>

#include <platform/platform.h>
