/*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 *
 * Copyright (c) 1996
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */

#ifndef __SGI_STL_ITERATOR_H
#define __SGI_STL_ITERATOR_H

#ifndef __SGI_STL_FUNCTION_H
#include <sgi_stl/function.h>
#endif
#include <sgi_stl/stddef.h>

#ifdef __STL_USE_NEW_IOSTREAMS 
#include <sgi_stl/iosfwd>
#else /* __STL_USE_NEW_IOSTREAMS */
#include <sgi_stl/iostream.h>
#endif /* __STL_USE_NEW_IOSTREAMS */

#ifndef __SGI_STL_INTERNAL_ITERATOR_BASE_H
#include <sgi_stl/stl_iterator_base.h>
#endif
#ifndef __SGI_STL_INTERNAL_ITERATOR_H
#include <sgi_stl/stl_iterator.h>
#endif
#ifndef __TYPE_TRAITS_H
#include <sgi_stl/type_traits.h>
#endif
#ifndef __SGI_STL_INTERNAL_CONSTRUCT_H
#include <sgi_stl/stl_construct.h>
#endif
#ifndef __SGI_STL_INTERNAL_RAW_STORAGE_ITERATOR_H
#include <sgi_stl/stl_raw_storage_iter.h>
#endif

#ifdef __STL_USE_NAMESPACES

// Names from stl_iterator.h

using __sgi_stl::input_iterator_tag;
using __sgi_stl::output_iterator_tag;
using __sgi_stl::forward_iterator_tag;
using __sgi_stl::bidirectional_iterator_tag;
using __sgi_stl::random_access_iterator_tag;

#if 0
using __sgi_stl::iterator;
#endif
using __sgi_stl::input_iterator;
using __sgi_stl::output_iterator;
using __sgi_stl::forward_iterator;
using __sgi_stl::bidirectional_iterator;
using __sgi_stl::random_access_iterator;

#ifdef __STL_CLASS_PARTIAL_SPECIALIZATION
using __sgi_stl::iterator_traits;
#endif

using __sgi_stl::iterator_category;
using __sgi_stl::distance_type;
using __sgi_stl::value_type;

using __sgi_stl::distance; 
using __sgi_stl::advance; 

using __sgi_stl::insert_iterator;
using __sgi_stl::front_insert_iterator;
using __sgi_stl::back_insert_iterator;
using __sgi_stl::inserter;
using __sgi_stl::front_inserter;
using __sgi_stl::back_inserter;

using __sgi_stl::reverse_iterator;
using __sgi_stl::reverse_bidirectional_iterator;

using __sgi_stl::istream_iterator;
using __sgi_stl::ostream_iterator;

// Names from stl_construct.h
using __sgi_stl::construct;
using __sgi_stl::destroy;

// Names from stl_raw_storage_iter.h
using __sgi_stl::raw_storage_iterator;

#endif /* __STL_USE_NAMESPACES */

#endif /* __SGI_STL_ITERATOR_H */

// Local Variables:
// mode:C++
// End:
