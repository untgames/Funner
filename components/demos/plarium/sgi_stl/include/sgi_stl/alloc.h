/*
 * Copyright (c) 1996-1997
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

#ifndef __SGI_STL_ALLOC_H
#define __SGI_STL_ALLOC_H

#ifndef __STL_CONFIG_H
#include <sgi_stl/stl_config.h>
#endif
#ifndef __SGI_STL_INTERNAL_ALLOC_H
#include <sgi_stl/stl_alloc.h>
#endif

#ifdef __STL_USE_NAMESPACES

using __sgi_stl::__malloc_alloc_template; 
using __sgi_stl::malloc_alloc; 
using __sgi_stl::simple_alloc; 
using __sgi_stl::debug_alloc; 
using __sgi_stl::__default_alloc_template; 
using __sgi_stl::alloc; 
using __sgi_stl::single_client_alloc; 
#ifdef __STL_STATIC_TEMPLATE_MEMBER_BUG
using __sgi_stl::__malloc_alloc_oom_handler; 
#endif /* __STL_STATIC_TEMPLATE_MEMBER_BUG */
#ifdef __STL_USE_STD_ALLOCATORS 
using __sgi_stl::allocator;
#endif /* __STL_USE_STD_ALLOCATORS */

#endif /* __STL_USE_NAMESPACES */

#endif /* __SGI_STL_ALLOC_H */

// Local Variables:
// mode:C++
// End:
