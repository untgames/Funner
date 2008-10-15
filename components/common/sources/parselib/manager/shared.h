#ifndef COMMONLIB_PARSER_SHARED_HEADER
#define COMMONLIB_PARSER_SHARED_HEADER

#include <cstdio>
#include <new>

#include <stl/hash_map>

#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/reference_counter.h>
#include <xtl/string.h>

#include <common/component.h>
#include <common/file.h>
#include <common/parser.h>
#include <common/singleton.h>
#include <common/strlib.h>

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Грамматический разбор
///////////////////////////////////////////////////////////////////////////////////////////////////
ParseNode parse (ParseLog& log, const char* type, const char* name, size_t buffer_size, char* buffer);
void      parse (ParseTreeBuilder& builder, ParseLog& log, const char* type, const char* name, size_t buffer_size, char* buffer);

}

#endif
