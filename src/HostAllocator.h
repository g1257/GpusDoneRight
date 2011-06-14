//-*-C++-*-
/** \ingroup GpusDoneRight */
/*@{*/

/*! \file HostAllocator.h
 *
 * Allocator for when host memory needs to be "in sync"
 * or mapped to GPU memory
 *
 */
#ifndef HOST_ALLOCATOR_H
#define HOST_ALLOCATOR_H

#include <typeinfo>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>

#include "ApiWrapper.h"

namespace GpusDoneRight {
	template<typename T> class HostAllocator;

	template<typename T> class HostAllocator : public std::allocator<T> {
		typedef typename std::allocator<T> BaseType;
	public:
		template <class U> struct rebind { typedef HostAllocator<U>
		other; };

		typename BaseType::pointer allocate(typename BaseType::size_type n,void* = 0)
		{
			if (n>this->max_size()) throw
					std::runtime_error("Bad allocation\n");

			typename BaseType::pointer x = 0;
			CUresult error = cuMemAllocHost((void**)&x, n*sizeof(T));
			ApiWrapper::check("cuMemAllocHost",error,true);
			return static_cast<T*>(x);
		}

		void deallocate(typename BaseType::pointer p, typename BaseType::size_type n)
		{
			CUresult error = cuMemFreeHost(p);
			ApiWrapper::check("cuMemFreeHost",error,true,ApiWrapper::DO_NOT_THROW);
		}
	}; // class HostAllocator

} // end namespace GpusDoneRight

/*@}*/
#endif // HOST_ALLOCATOR_H
