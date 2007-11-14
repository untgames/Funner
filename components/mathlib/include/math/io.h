#ifndef MATHLIB_IO_HEADER
#define MATHLIB_IO_HEADER

#include <stddef.h>

namespace xtl
{

namespace io
{

template <class Token, class BaseIter> class token_iterator;

}

}

namespace common
{

class OutputTextStream;

}

namespace math
{

template <class T, size_t size> class vec;
template <class T, size_t size> class matrix;
template <class T>              class quat;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Чтение математических типов
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Token, class BaseIter, class T, size_t size>
bool read (xtl::io::token_iterator<Token, BaseIter>&, vec<T, size>&);

template <class Token, class BaseIter, class T, size_t size>
bool read (xtl::io::token_iterator<Token, BaseIter>&, matrix<T, size>&);

template <class Token, class BaseIter, class T>
bool read (xtl::io::token_iterator<Token, BaseIter>&, quat<T>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сериализация математических типов
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T, size_t size>
void write (common::OutputTextStream&, const vec<T, size>& value, const char* format="");

template <class T, size_t size>
void write (common::OutputTextStream&, const matrix<T, size>& value, const char* format="");

template <class T>
void write (common::OutputTextStream&, const quat<T>& value, const char* format="");

///////////////////////////////////////////////////////////////////////////////////////////////////
///Приведение к строке
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class String, class T, size_t size>
void to_string (String& buffer, const vec<T, size>& value);

template <class String, class T, size_t size>
void to_string (String& buffer, const matrix<T, size>& value);

template <class String, class T>
void to_string (String& buffer, const quat<T>& value);

#include <math/impl/io.inl>

}

#endif
