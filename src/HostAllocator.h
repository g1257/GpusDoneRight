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

	// specialize for void:
	template<> class HostAllocator<void> {
	public:
		typedef void*       pointer;
		typedef const void* const_pointer;
		// reference to void members are impossible.
		typedef void value_type;
		template <class U> struct rebind { typedef allocator<U>
		other; };
	}; // class HostAllocator

	template<typename T> class HostAllocator {
	public:
		typedef size_t    size_type;
		typedef ptrdiff_t difference_type;
		typedef T*        pointer;
		typedef const T*  const_pointer;
		typedef T&        reference;
		typedef const T&  const_reference;
		typedef T         value_type;
		template <class U> struct rebind { typedef allocator<U>
		other; };

		HostAllocator() throw() { }

		HostAllocator(const HostAllocator&) throw() { }

		template <class U>
		HostAllocator(const allocator<U>&) throw() { }

		~HostAllocator() throw() { }

		pointer address(reference x) const { return &x; }

		const_pointer address(const_reference x) const { return &x; }

		pointer allocate(size_type n,void* = 0)
		{
			if (n>this->maxSize()) throw
					runtime_error("Bad allocation\n");

			pointer x = 0;
			cuMemAllocHost((void**)&x, n*sizeof(T));
			return static_cast<T*>(x);
		}

		void deallocate(pointer p, size_type n)
		{
			cuMemFreeHost(p);
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
	}; // class HostAllocator

} // end namespace GpusDoneRight

/*@}*/
#endif // HOST_ALLOCATOR_H
