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
	/*
	The templateParamFlags parameter enables different options to be specified that affect the allocation, as follows.
	 * CU_MEMHOSTALLOC_PORTABLE: The memory returned by this call will be considered as pinned memory by all CUDA contexts,
		not just the one that performed the allocation.

	 * CU_MEMHOSTALLOC_DEVICEMAP: Maps the allocation into the CUDA address space. The device pointer to the memory may be obtained by calling
		cuMemHostGetDevicePointer(). This feature is available only on GPUs with compute capability greater than or equal to 1.1.

	 * CU_MEMHOSTALLOC_WRITECOMBINED: Allocates the memory as write-combined (WC). WC memory can be transferred across
		the PCI Express bus more quickly on some system configurations, but cannot be read efficiently by most CPUs.
		WC memory is a good option for buffers that will be written by the CPU and read by the GPU via mapped pinned memory or
		host->device transfers.*/
	template<typename T,int templateParamFlags> class HostAllocator : public std::allocator<T> {
		typedef typename std::allocator<T> BaseType;
	public:
		template <class U> struct rebind { typedef HostAllocator<U,templateParamFlags>
		other; };

		typename BaseType::pointer allocate(typename BaseType::size_type n,void* = 0)
		{
			if (n>this->max_size()) throw
					std::runtime_error("Bad allocation\n");

			typename BaseType::pointer x = 0;
			if (templateParamFlags<0) {
				CUresult error = cuMemAllocHost((void**)&x, n*sizeof(T));
				ApiWrapper::check("cuMemAllocHost",error,true);
				return static_cast<T*>(x);
			}
			CUresult error = cuMemHostAlloc((void**)&x, n*sizeof(T),templateParamFlags);
			ApiWrapper::check("cuMemHostAlloc",error,true);
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
