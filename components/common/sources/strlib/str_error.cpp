#include <cstdlib>
#include <cerrno>

#include <common/strlib.h>

namespace common
{

const char* strerror (int code)
{
  switch (code)
  {
    case 0:            return  "No error";
    case EPERM:        return  "Operation not permitted";
    case ENOENT:       return  "No such file or directory";
    case ESRCH:        return  "No such process";
    case EINTR:        return  "Interrupted function call";
    case EIO:          return  "Input/output error";
    case ENXIO:        return  "No such device or address";
    case E2BIG:        return  "Arg list too long";
    case ENOEXEC:      return  "Exec format error";
    case EBADF:        return  "Bad file descriptor";
    case ECHILD:       return  "No child processes";
    case EAGAIN:       return  "Resource temporarily unavailable";
    case ENOMEM:       return  "Not enough space";
    case EACCES:       return  "Permission denied";
    case EFAULT:       return  "Bad address";
    case EBUSY:        return  "Resource device";
    case EEXIST:       return  "File exists";
    case EXDEV:        return  "Improper link";
    case ENODEV:       return  "No such device";
    case ENOTDIR:      return  "Not a directory";
    case EISDIR:       return  "Is a directory";
    case EINVAL:       return  "Invalid argument";
    case ENFILE:       return  "Too many open files in system";
    case EMFILE:       return  "Too many open files";
    case ENOTTY:       return  "Inappropriate I/O control operation";
    case EFBIG:        return  "File too large";
    case ENOSPC:       return  "No space left on device";
    case ESPIPE:       return  "Invalid seek";
    case EROFS:        return  "Read-only file system";
    case EMLINK:       return  "Too many links";
    case EPIPE:        return  "Broken pipe";
    case EDOM:         return  "Domain error";
    case ERANGE:       return  "Result too large";
    case EDEADLK:      return  "Resource deadlock avoided";
    case ENAMETOOLONG: return  "Filename too long";
    case ENOLCK:       return  "No locks available";
    case ENOSYS:       return  "Function not implemented";
    case ENOTEMPTY:    return  "Directory not empty";
    case EILSEQ:       return  "Illegal byte sequence";
    default:           return  "Unknown error";
  }
}

}
