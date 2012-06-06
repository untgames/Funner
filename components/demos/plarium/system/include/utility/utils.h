#ifndef UTILITY_UTILS_HEADER
#define UTILITY_UTILS_HEADER

#include <string>

namespace plarium
{

namespace utility
{

//Print to std::string
std::string format (const char* format,...);

//Get current time
size_t milliseconds ();

}

}

#endif
