#include <iconv.h>

size_t iconv_wrapper (iconv_t cd, const char* * inbuf, size_t *inbytesleft, char* * outbuf, size_t *outbytesleft)
{
  return iconv (cd, (char**)inbuf, inbytesleft, outbuf, outbytesleft);
}
