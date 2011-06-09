//-*-C++-*-
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

#include "HostAllocator.h"

namespace GpusDoneRight {
	
	template<typename ValueType>
	class GpuPointer {
	public:

		//======================================================================

		GpuPointer(size_t n,bool verbose = true) : 
			verbose_(verbose),
			allocatedBytes_(n * sizeof(ValueType)),
			offsetDevice_(0)
		{
			CUresult error = cuMemAlloc(&gpuPtr_,allocatedBytes_);
			ApiWrapper::check("cuMemAlloc",error,verbose_);
		}
		
		//======================================================================

		size_t allocatedBytes() const {  return allocatedBytes_; }

		//======================================================================

		void setOffset(size_t x) { offsetDevice_ = x * sizeof(ValueType); }

		//======================================================================

		void copyFromHost(const ValueType& hostVal)
		{
			static const std::string errorMsg
				= "GpuPointer::copyFromHost(...) expecting size 1 pointer\n";

			if (allocatedBytes_!=sizeof(ValueType))
				throw std::runtime_error(errorMsg);
			
			CUresult error = cuMemcpyHtoD (gpuPtr_ + offsetDevice_,
										   &hostVal,
										   allocatedBytes_);
 			ApiWrapper::check("cuMemcpyHtoD",error,verbose_);
		}
		
		//======================================================================

		void copyFromHost(const std::vector<ValueType>& hostVector,
						  size_t offsetHostInUnits = 0,
						  int extent = -1)
		{
			size_t byteCount =  (extent<0) ? allocatedBytes_ : extent*sizeof(ValueType);
			CUresult error = cuMemcpyHtoD (gpuPtr_ + offsetDevice_,
										   &(hostVector[offsetHostInUnits]),
										   byteCount);
 			ApiWrapper::check("cuMemcpyHtoD",error,verbose_);
		}
		
		//======================================================================

		void copyToHost(std::vector<ValueType>& hostVector,
						size_t offsetHostInUnits = 0,
						int extent = -1) const
		{
			return copyToHost(&(hostVector[offsetHostInUnits]),0,extent);
		}

		//======================================================================

		void copyToHost(ValueType* hostVector,
						size_t offsetHostInUnits = 0,
						int extent = -1) const
		{
			size_t byteCount =  (extent<0) ? allocatedBytes_ : extent*sizeof(ValueType);
			CUresult error = cuMemcpyDtoH(hostVector + offsetHostInUnits,
										  gpuPtr_+offsetDevice_, 
										  byteCount);
			ApiWrapper::check("cuMemcpyDtoH",error,verbose_);
		}

		//======================================================================

		template<typename SomeGpuStreamType>
		void copyToHostAsync(std::vector<ValueType,HostAllocator<ValueType> >& hostVector,
							 SomeGpuStreamType& hstream,
							 size_t offsetHostInUnits = 0,
							 int extent = -1) const
		{
			return copyToHostAsync(&(hostVector[offsetHostInUnits]),hstream,0,extent);
		}

		//======================================================================

		template<typename SomeGpuStreamType>
		void copyToHostAsync(ValueType* hostVector,
							 SomeGpuStreamType& hstream,
							 size_t offsetHostInUnits = 0,
							 int extent = -1) const
		{
			static const std::string errorMsg =
				"GpuPointer::copyToHostAsync(...): out of range\n";
			size_t byteCount =  (extent<0) ? allocatedBytes_ : extent*sizeof(ValueType);

			if (offsetDevice_ + byteCount > allocatedBytes_) 
				throw std::runtime_error(errorMsg);

			CUdeviceptr newPtr = gpuPtr_ + offsetDevice_;
			ValueType* newHptr = hostVector + offsetHostInUnits;
			CUresult error = cuMemcpyDtoHAsync(newHptr,
											   newPtr, 
											   byteCount, 
											   hstream());
			ApiWrapper::check("cuMemcpyDtoHAsync",error,verbose_);
		}

		//======================================================================

		CUdeviceptr getCUdeviceptr() const { return gpuPtr_ + offsetDevice_; }

		//======================================================================

		private:
		bool verbose_;
		CUdeviceptr gpuPtr_;
		unsigned int allocatedBytes_;
		size_t offsetDevice_;

	}; // class GpuPointer

} // end namespace GpusDoneRight

/*@}*/
#endif // GPU_POINTER_H_H
