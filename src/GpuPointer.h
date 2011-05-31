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
			verbose_(verbose),allocatedBytes_(n * sizeof(ValueType)),offsetDevice_(0)
		{
			CUresult error = cuMemAlloc(&gpuPtr_,allocatedBytes_);
			ApiWrapper::check("cuMemAlloc",error,verbose_);

		}
		
//		void setTo(const ValueType& val)
//		{
//			cudaMemset(deviceA, 0, nbytes); // set device memory to all 0s, for testing correctness
//		} 

		void setOffset(size_t x) { offsetDevice_ = x; }
		
		void copyFromHost(
				const std::vector<ValueType>& hostVector,
				size_t offsetHost = 0,
				int byteCount = -1)
		{
			if (byteCount<0) byteCount = allocatedBytes_;
			CUresult error = cuMemcpyHtoD (gpuPtr_ + offsetDevice_, &(hostVector[offsetHost]), byteCount);
 			ApiWrapper::check("cuMemcpyHtoD",error,verbose_);
		}
		
		void copyToHost(
				std::vector<ValueType>& hostVector,
				size_t offsetHost = 0,
				int byteCount = -1) const
		{
			if (byteCount<0) byteCount = allocatedBytes_;
			CUresult error = cuMemcpyDtoH(&(hostVector[offsetHost]),gpuPtr_+offsetDevice_,byteCount);
			ApiWrapper::check("cuMemcpyDtoH",error,verbose_);
		}
		
		template<typename SomeGpuStreamType>
		void copyToHostAsync(
					std::vector<ValueType>& hostVector,
					SomeGpuStreamType& hstream,
					size_t offsetHost = 0,
					int byteCount = -1) const
		{
			if (byteCount<0) byteCount = allocatedBytes_;
			CUresult error = cuMemcpyDtoHAsync(&(hostVector[offsetHost]),gpuPtr_+offsetDevice_, byteCount, hstream);
			ApiWrapper::check("cuMemcpyDtoHAsync",error,verbose_);
		}

		size_t passToGpuFunction(CUfunction& hfunc,int offset) const
		{
			void *ptr = (void*)(size_t)gpuPtr_;
			ALIGN_UP(offset, __alignof(ptr));
			CUresult error = cuParamSetv(hfunc, offset, &ptr, sizeof(ptr));
			ApiWrapper::check("cuParamSetv",error,verbose_);
			return sizeof(ptr);
		}
		
		
	private:
		bool verbose_;
		CUdeviceptr gpuPtr_;
		unsigned int allocatedBytes_;
		size_t offsetDevice_;
	}; // class GpuPointer
	
} // end namespace GpusDoneRight

/*@}*/
#endif // GPU_POINTER_H_H
