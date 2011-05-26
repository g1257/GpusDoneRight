/** \ingroup GpusDoneRight */
/*@{*/

/*! \file Cuda.h
 *
 * DOCUMENTATION HERE FIXME
 *
 */
// adds GDR_ to avoid potential name clashes:
#ifndef GDR_CONTEXT_H_H
#define GDR_CONTEXT_H_H

#include <typeinfo>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include "TypeToString.h"

#include "ApiWrapper.h"

namespace GpusDoneRight {

	class Context {
	public:
		Context(const CUdevice& handle, bool verbose = true) : verbose_(verbose)
		{
			// Create context
			CUresult error = cuCtxCreate(&cuContext_, 0, handle);
    			ApiWrapper::check("cuCtxCreate",error,verbose_);
		}
		
		// Using RAII here:
		~Context()
		{
			CUresult error = cuCtxDetach(cuContext_);
			// Let's not throw in the destructor:
			ApiWrapper::check("cuCtxCreate",error,verbose_,ApiWrapper::DO_NOT_THROW);
		}
	private:
		bool verbose_;
		CUcontext cuContext_; // we own this
}; // class Context
} // end namespace GpusDoneRight

/*@}*/
#endif // GDR_CONTEXT_H_H
