//-*-C++-*-
/** \ingroup GpusDoneRight */
/*@{*/

/*! \file HostDeviceMappedAllocator.h
 *
 * Allocator for when host memory needs to be "in sync"
 * or mapped to GPU memory
 *
 */
#ifndef HOST_DM_ALLOCATOR_H
#define HOST_DM_ALLOCATOR_H

#include <typeinfo>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>

#include "ApiWrapper.h"

namespace GpusDoneRight {
	template<typename T> class HostDeviceMappedAllocator;

	template<typename T> class HostDeviceMappedAllocator : public std::allocator<T> {
		typedef typename std::allocator<T> BaseType;
	public:
		typename BaseType::pointer allocate(typename BaseType::size_type n,void* = 0)
		{
			if (n>this->max_size()) throw
					std::runtime_error("Bad allocation\n");

			typename BaseType::pointer x = 0;
			CUresult error = cuMemHostAlloc((void**)&x, n*sizeof(T),CU_MEMHOSTALLOC_DEVICEMAP);
			ApiWrapper::check("cuMemHostAlloc",error,true);
			return static_cast<T*>(x);
		}

		void deallocate(typename BaseType::pointer p, typename BaseType::size_type n)
		{
			CUresult error = cuMemFreeHost(p);
			ApiWrapper::check("cuMemFreeHost",error,true,ApiWrapper::DO_NOT_THROW);
		}

	}; // class HostDeviceMappedAllocator

} // end namespace GpusDoneRight

/*@}*/
#endif // HOST_DM_ALLOCATOR_H
