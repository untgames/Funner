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
 * Copyright (c) 1996,1997
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

#ifndef __SGI_STL_FUNCTION_H
#define __SGI_STL_FUNCTION_H

#ifndef __STL_CONFIG_H
#include <sgi_stl/stl_config.h>
#endif
#ifndef __SGI_STL_INTERNAL_RELOPS
#include <sgi_stl/stl_relops.h>
#endif
#include <sgi_stl/stddef.h>
#ifndef __SGI_STL_INTERNAL_FUNCTION_H
#include <sgi_stl/stl_function.h>
#endif

#ifdef __STL_USE_NAMESPACE_FOR_RELOPS

// Names from stl_relops.h
using __STD_RELOPS::operator!=;
using __STD_RELOPS::operator>;
using __STD_RELOPS::operator<=;
using __STD_RELOPS::operator>=;

#endif /* __STL_USE_NAMESPACE_FOR_RELOPS */

#ifdef __STL_USE_NAMESPACES

// Names from stl_function.h
using __sgi_stl::unary_function; 
using __sgi_stl::binary_function; 
using __sgi_stl::plus; 
using __sgi_stl::minus; 
using __sgi_stl::multiplies; 
using __sgi_stl::divides; 
using __sgi_stl::identity_element; 
using __sgi_stl::modulus; 
using __sgi_stl::negate; 
using __sgi_stl::equal_to; 
using __sgi_stl::not_equal_to; 
using __sgi_stl::greater; 
using __sgi_stl::less; 
using __sgi_stl::greater_equal; 
using __sgi_stl::less_equal; 
using __sgi_stl::logical_and; 
using __sgi_stl::logical_or; 
using __sgi_stl::logical_not; 
using __sgi_stl::unary_negate; 
using __sgi_stl::binary_negate; 
using __sgi_stl::not1; 
using __sgi_stl::not2; 
using __sgi_stl::binder1st; 
using __sgi_stl::binder2nd; 
using __sgi_stl::bind1st; 
using __sgi_stl::bind2nd; 
using __sgi_stl::unary_compose; 
using __sgi_stl::binary_compose; 
using __sgi_stl::compose1; 
using __sgi_stl::compose2; 
using __sgi_stl::pointer_to_unary_function; 
using __sgi_stl::pointer_to_binary_function; 
using __sgi_stl::ptr_fun; 
using __sgi_stl::identity; 
using __sgi_stl::select1st; 
using __sgi_stl::select2nd; 
using __sgi_stl::project1st; 
using __sgi_stl::project2nd; 
using __sgi_stl::constant_void_fun; 
using __sgi_stl::constant_unary_fun; 
using __sgi_stl::constant_binary_fun; 
using __sgi_stl::constant0; 
using __sgi_stl::constant1; 
using __sgi_stl::constant2; 
using __sgi_stl::subtractive_rng; 
using __sgi_stl::mem_fun_t; 
using __sgi_stl::const_mem_fun_t; 
using __sgi_stl::mem_fun_ref_t; 
using __sgi_stl::const_mem_fun_ref_t; 
using __sgi_stl::mem_fun1_t; 
using __sgi_stl::const_mem_fun1_t; 
using __sgi_stl::mem_fun1_ref_t; 
using __sgi_stl::const_mem_fun1_ref_t; 
using __sgi_stl::mem_fun; 
using __sgi_stl::mem_fun_ref; 
using __sgi_stl::mem_fun1; 
using __sgi_stl::mem_fun1_ref; 

#endif /* __STL_USE_NAMESPACES */

#endif /* __SGI_STL_FUNCTION_H */

// Local Variables:
// mode:C++
// End:
