#ifndef COMMONLIB_STREAMS_TESTS_HEADER
#define COMMONLIB_STREAMS_TESTS_HEADER

#include <stdio.h>
#include <memory.h>

#include <xtl/function.h>
#include <xtl/iterator_range.h>

#include <stl/string>
#include <stl/set>

#include <common/streams.h>
#include <common/wxf_writer.h>
#include <common/xml_writer.h>

#include <mathlib.h>
#include <math/io.h>

using namespace common;
using namespace math;
using namespace xtl;

/*
    ¬ывод текста в стандартный поток вывода
*/

size_t dump (const void* buffer, size_t size)
{
  size_t result = size;

  const char* s = (const char*)buffer;

  for (;size--; s++)
    printf ("%c", *s);

  return result;
}

#endif
