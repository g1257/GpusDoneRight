/** \ingroup GpusDoneRight */
/*@{*/

/*! \file GpuPointer.h
 *
 * DOCUMENTATION HERE FIXME
 *
 */
#ifndef GPU_POINTER_H_H
#define GPU_POINTER_H_H

#include <typeinfo>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include "TypeToString.h"

#include "ApiWrapper.h"

namespace GpusDoneRight {
	
	template<typename ValueType>
	class GpuPointer {
	public:
		GpuPointer(size_t n,bool verbose = true) : 
			verbose_(verbose),allocatedBytes_(n * sizeof(ValueType))
		{
			CUresult error = cuMemAlloc(&gpuPtr_,allocatedBytes_);
			ApiWrapper::check("cuMemAlloc",error,verbose_);

		}
		
		void operator=(const std::vector<ValueType>& hostVector)
		{
			CUresult error = cuMemcpyHtoD (gpuPtr_, &(hostVector[0]), allocatedBytes_);
 			ApiWrapper::check("cuMemcpyHtoD",error,verbose_);
		}
		
		void copyToHost(std::vector<ValueType>& hostVector) const
		{
			CUresult error = cuMemcpyDtoH(&(hostVector[0]),gpuPtr_,allocatedBytes_);
			ApiWrapper::check("cuMemcpyDtoH",error,verbose_);
		}
		
	private:
		bool verbose_;
		CUdeviceptr gpuPtr_;
		unsigned int allocatedBytes_;
	}; // class GpuPointer
	
} // end namespace GpusDoneRight

/*@}*/
#endif // GPU_POINTER_H_H
