/** \ingroup GpusDoneRight */
/*@{*/

/*! \file GpuStream.h
 *
 * DOCUMENTATION HERE FIXME
 *
 */
// adds GDR_ to avoid potential name clashes:
#ifndef GPU_STREAM_H_H
#define GPU_STREAM_H_H

#include <typeinfo>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include "TypeToString.h"

#include "ApiWrapper.h"

namespace GpusDoneRight {

	class GpuStream {
	public:
		GpuStream(bool verbose = true) : verbose_(verbose)
		{
			// Create context
			CUresult error = cuStreamCreate(&hStream_,0);
    		ApiWrapper::check("cuStreamCreate",error,verbose_);
		}
		
		// Using RAII here:
		~GpuStream()
		{
			CUresult error = cuStreamDestroy(hStream_);
			// Let's not throw in the destructor:
			ApiWrapper::check("cuStreamDestroy",error,verbose_,ApiWrapper::DO_NOT_THROW);
		}
		
		const CUstream& operator()() const { return hStream_; }
		
	/* CUresult 	cuStreamQuery (CUstream hStream)
 	Determine status of a compute stream.
CUresult 	cuStreamSynchronize (CUstream hStream)
 	Wait until a stream's tasks are completed. */
 
	private:
		bool verbose_;
		CUstream hStream_; // we own this
	}; // class GpuStream
} // end namespace GpusDoneRight

/*@}*/
#endif // GPU_STREAM_H_H
