#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>

#include <netinet/tcp.h>
#include <sys/types.h>
#include <sys/socket.h>

#ifdef TABLETOS
#include <sys/time.h>
#include <netinet/in.h>
#endif

#ifdef ANDROID
#include <linux/in.h>
#include <linux/in6.h>
#endif

#include <xtl/common_exceptions.h>
#include <xtl/reference_counter.h>

#include <common/log.h>

#include <platform/platform.h>
