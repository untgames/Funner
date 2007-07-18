#ifndef MATHLIB_IO_HEADER
#define MATHLIB_IO_HEADER

namespace xtl
{

namespace io
{

template <class Token, class BaseIter> class token_iterator;

}

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

#include <math/impl/io.inl>

}

#endif
