#include <cstdio>

#include <errno.h>

#include <common/strlib.h>

int errors [] = {
0, EPERM, ENOENT, ESRCH, EINTR, EIO, ENXIO, E2BIG, ENOEXEC, EBADF, ECHILD, EAGAIN, ENOMEM, EACCES,
EFAULT, EBUSY, EEXIST, EXDEV, ENODEV, ENOTDIR, EISDIR, EINVAL, ENFILE, EMFILE, ENOTTY, EFBIG,
ENOSPC, ESPIPE, EROFS, EMLINK, EPIPE, EDOM, ERANGE, EDEADLK, ENAMETOOLONG, ENOLCK, ENOSYS, ENOTEMPTY, EILSEQ
};

int main ()
{
  printf ("Results of strerror_test:\n");

  const int MAX_ERROR_CODE = sizeof (errors) / sizeof (errors [0]);

  for (int i=0; i < MAX_ERROR_CODE; i++)
    printf ("%02d: '%s'\n", i, common::strerror (errors [i]));

  return 0;
}
