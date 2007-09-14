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
void write (common::OutputTextStream&, const vec<T, size>& value, const char* format="", const char* separator=" ");

template <class T, size_t size>
void write (common::OutputTextStream&, const vec<T, size>& value, const char* format, const wchar_t* separator);

template <class T, size_t size>
void write (common::OutputTextStream&, const matrix<T, size>& value, const char* format="", const char* separator=" ");

template <class T, size_t size>
void write (common::OutputTextStream&, const matrix<T, size>& value, const char* format, const wchar_t* separator);

template <class T>
void write (common::OutputTextStream&, const quat<T>& value, const char* format="", const char* separator=" ");

template <class T>
void write (common::OutputTextStream&, const quat<T>& value, const char* format, const wchar_t* separator);

#include <math/impl/io.inl>

}

#endif
