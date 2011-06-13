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

	// specialize for void:
	template<> class HostDeviceMappedAllocator<void> {
	public:
		typedef void*       pointer;
		typedef const void* const_pointer;
		// reference to void members are impossible.
		typedef void value_type;
		template <class U> struct rebind { typedef HostDeviceMappedAllocator<U>
		other; };
	}; // class HostDeviceMappedAllocator

	template<typename T> class HostDeviceMappedAllocator {
	public:
		typedef size_t    size_type;
		typedef ptrdiff_t difference_type;
		typedef T*        pointer;
		typedef const T*  const_pointer;
		typedef T&        reference;
		typedef const T&  const_reference;
		typedef T         value_type;
		template <class U> struct rebind { typedef HostDeviceMappedAllocator<U>
		other; };

		HostDeviceMappedAllocator() throw() { }

		HostDeviceMappedAllocator(const HostDeviceMappedAllocator&) throw() { }

		template <class U>
		HostDeviceMappedAllocator(const HostDeviceMappedAllocator<U>&) throw() { }

		~HostDeviceMappedAllocator() throw() { }

		pointer address(reference x) const { return &x; }

		const_pointer address(const_reference x) const { return &x; }

		pointer allocate(size_type n,void* = 0)
		{
			if (n>this->max_size()) throw
					std::runtime_error("Bad allocation\n");

			pointer x = 0;
			CUresult error = cuMemHostAlloc((void**)&x, n*sizeof(T),CU_MEMHOSTALLOC_DEVICEMAP);
			ApiWrapper::check("cuMemHostAlloc",error,true);
			return static_cast<T*>(x);
		}

		void deallocate(pointer p, size_type n)
		{
			CUresult error = cuMemFreeHost(p);
			ApiWrapper::check("cuMemFreeHost",error,true,ApiWrapper::DO_NOT_THROW);
		}

		size_type max_size() const throw()
		{
			return size_t(-1) / sizeof(T);
		}

		void construct(pointer p, const T& val)
		{
			//! placement new here!!  no actual allocation happens
			//! only the constructor is called
			new(static_cast<void*>(p)) T(val);
		}

		void destroy(pointer p)
		{
			p->~T();
		}
	}; // class HostDeviceMappedAllocator

} // end namespace GpusDoneRight

/*@}*/
#endif // HOST_DM_ALLOCATOR_H
