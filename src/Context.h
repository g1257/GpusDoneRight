//-*-C++-*-
/** \ingroup GpusDoneRight */
/*@{*/

/*! \file Context.h
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
		/** Flags can be: 
			* CU_CTX_SCHED_AUTO: The default value if the flags parameter is zero, uses a heuristic based on 
				the number of active CUDA contexts in the process C and the number of logical processors 
				in the system P. If C > P, then CUDA will yield to other OS threads when waiting for the GPU, 
				otherwise CUDA will not yield while waiting for results and actively spin on the processor.

    			* CU_CTX_SCHED_SPIN: Instruct CUDA to actively spin when waiting for results from the GPU. 
				This can decrease latency when waiting for the GPU, but may lower the performance of CPU 
				threads if they are performing work in parallel with the CUDA thread.

   	 		* CU_CTX_SCHED_YIELD: Instruct CUDA to yield its thread when waiting for results from the GPU. 
				This can increase latency when waiting for the GPU, but can increase the performance of CPU 
				threads performing work in parallel with the GPU.

    			* CU_CTX_BLOCKING_SYNC: Instruct CUDA to block the CPU thread on a synchronization primitive when 
				waiting for the GPU to finish work.

    			* CU_CTX_MAP_HOST: Instruct CUDA to support mapped pinned allocations. This flag must be set in order to 
				allocate pinned host memory that is accessible to the GPU.
		*/
		Context(const CUdevice& handle, unsigned int flags = 0,bool verbose = true) : verbose_(verbose)
		{
			// Create context
			CUresult error = cuCtxCreate(&cuContext_, flags, handle);
			ApiWrapper::check("cuCtxCreate",error,verbose_);
		}
		
		// Using RAII here:
		~Context()
		{
			CUresult error = cuCtxDetach(cuContext_);
			// Let's not throw in the destructor:
			ApiWrapper::check("cuCtxCreate",error,verbose_,ApiWrapper::DO_NOT_THROW);
		}
		
		void synchronize()
		{
			CUresult error = cuCtxSynchronize();
			ApiWrapper::check("cuCtxSynchronize",error,verbose_);
		}
	private:
		bool verbose_;
		CUcontext cuContext_; // we own this
	}; // class Context
} // end namespace GpusDoneRight

/*@}*/
#endif // GDR_CONTEXT_H_H
