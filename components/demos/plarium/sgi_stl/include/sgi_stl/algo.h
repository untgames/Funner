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

#ifndef __SGI_STL_ALGO_H
#define __SGI_STL_ALGO_H

#include <sgi_stl/algobase.h>
#include <sgi_stl/tempbuf.h>
#include <sgi_stl/stl_algo.h>
#include <sgi_stl/stl_numeric.h>

#ifdef __STL_USE_NAMESPACES

// Names from <stl_algo.h>
using __sgi_stl::for_each; 
using __sgi_stl::find; 
using __sgi_stl::find_if; 
using __sgi_stl::adjacent_find; 
using __sgi_stl::count; 
using __sgi_stl::count_if; 
using __sgi_stl::search; 
using __sgi_stl::search_n; 
using __sgi_stl::swap_ranges; 
using __sgi_stl::transform; 
using __sgi_stl::replace; 
using __sgi_stl::replace_if; 
using __sgi_stl::replace_copy; 
using __sgi_stl::replace_copy_if; 
using __sgi_stl::generate; 
using __sgi_stl::generate_n; 
using __sgi_stl::remove; 
using __sgi_stl::remove_if; 
using __sgi_stl::remove_copy; 
using __sgi_stl::remove_copy_if; 
using __sgi_stl::unique; 
using __sgi_stl::unique_copy; 
using __sgi_stl::reverse; 
using __sgi_stl::reverse_copy; 
using __sgi_stl::rotate; 
using __sgi_stl::rotate_copy; 
using __sgi_stl::random_shuffle; 
using __sgi_stl::random_sample; 
using __sgi_stl::random_sample_n; 
using __sgi_stl::partition; 
using __sgi_stl::stable_partition; 
using __sgi_stl::sort; 
using __sgi_stl::stable_sort; 
using __sgi_stl::partial_sort; 
using __sgi_stl::partial_sort_copy; 
using __sgi_stl::nth_element; 
using __sgi_stl::lower_bound; 
using __sgi_stl::upper_bound; 
using __sgi_stl::equal_range; 
using __sgi_stl::binary_search; 
using __sgi_stl::merge; 
using __sgi_stl::inplace_merge; 
using __sgi_stl::includes; 
using __sgi_stl::set_union; 
using __sgi_stl::set_intersection; 
using __sgi_stl::set_difference; 
using __sgi_stl::set_symmetric_difference; 
using __sgi_stl::min_element; 
using __sgi_stl::max_element; 
using __sgi_stl::next_permutation; 
using __sgi_stl::prev_permutation; 
using __sgi_stl::find_first_of; 
using __sgi_stl::find_end; 
using __sgi_stl::is_sorted; 
using __sgi_stl::is_heap; 

// Names from stl_heap.h
using __sgi_stl::push_heap;
using __sgi_stl::pop_heap;
using __sgi_stl::make_heap;
using __sgi_stl::sort_heap;

// Names from <stl_numeric.h>
using __sgi_stl::accumulate; 
using __sgi_stl::inner_product; 
using __sgi_stl::partial_sum; 
using __sgi_stl::adjacent_difference; 
using __sgi_stl::power; 
using __sgi_stl::iota; 

#endif /* __STL_USE_NAMESPACES */

#endif /* __SGI_STL_ALGO_H */

// Local Variables:
// mode:C++
// End:
