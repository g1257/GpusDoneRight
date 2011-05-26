/** \ingroup GpusDoneRight */
/*@{*/

/*! \file Memory.h
 *
 * DOCUMENTATION HERE FIXME
 *
 */
// adds GDR_ to avoid potential name clashes:
#ifndef GDR_MEMORY_H_H
#define GDR_MEMORY_H_H

#include <typeinfo>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include "TypeToString.h"

#include "ApiWrapper.h"

namespace GpusDoneRight {
	//! Wrapper over the cuda memory api, could be static, but let's not... might change in the future...
	class Memory {
	public:
		Memory(bool verbose = true) : verbose_(verbose)
		{}

//! FIXME: We'll need to wrap all these functions, as you add them please
//!  uncomment them in place as done below
/*
		CUresult 	cuArray3DCreate (CUarray *pHandle, const CUDA_ARRAY3D_DESCRIPTOR *pAllocateArray)
 	Creates a 3D CUDA array.
CUresult 	cuArray3DGetDescriptor (CUDA_ARRAY3D_DESCRIPTOR *pArrayDescriptor, CUarray hArray)
 	Get a 3D CUDA array descriptor.
CUresult 	cuArrayCreate (CUarray *pHandle, const CUDA_ARRAY_DESCRIPTOR *pAllocateArray)
 	Creates a 1D or 2D CUDA array.
CUresult 	cuArrayDestroy (CUarray hArray)
 	Destroys a CUDA array.
CUresult 	cuArrayGetDescriptor (CUDA_ARRAY_DESCRIPTOR *pArrayDescriptor, CUarray hArray)
 	Get a 1D or 2D CUDA array descriptor.
*/
		//! Allocates device memory.
		void alloc(CUdeviceptr *dptr, unsigned int bytesize) const
		{
			CUresult error = cuMemAlloc(dptr,bytesize);
			ApiWrapper::check("cuMemAlloc",error,verbose_);
		}
/* 	
CUresult 	cuMemAllocHost (void **pp, unsigned int bytesize)
 	Allocates page-locked host memory.
CUresult 	cuMemAllocPitch (CUdeviceptr *dptr, unsigned int *pPitch, unsigned int WidthInBytes, unsigned int Height, unsigned int ElementSizeBytes)
 	Allocates pitched device memory.
CUresult 	cuMemcpy2D (const CUDA_MEMCPY2D *pCopy)
 	Copies memory for 2D arrays.
CUresult 	cuMemcpy2DAsync (const CUDA_MEMCPY2D *pCopy, CUstream hStream)
 	Copies memory for 2D arrays.
CUresult 	cuMemcpy2DUnaligned (const CUDA_MEMCPY2D *pCopy)
 	Copies memory for 2D arrays.
CUresult 	cuMemcpy3D (const CUDA_MEMCPY3D *pCopy)
 	Copies memory for 3D arrays.
CUresult 	cuMemcpy3DAsync (const CUDA_MEMCPY3D *pCopy, CUstream hStream)
 	Copies memory for 3D arrays.
CUresult 	cuMemcpyAtoA (CUarray dstArray, unsigned int dstIndex, CUarray srcArray, unsigned int srcIndex, unsigned int ByteCount)
 	Copies memory from Array to Array.
CUresult 	cuMemcpyAtoD (CUdeviceptr dstDevice, CUarray hSrc, unsigned int SrcIndex, unsigned int ByteCount)
 	Copies memory from Array to Device.
CUresult 	cuMemcpyAtoH (void *dstHost, CUarray srcArray, unsigned int srcIndex, unsigned int ByteCount)
 	Copies memory from Array to Host.
CUresult 	cuMemcpyAtoHAsync (void *dstHost, CUarray srcArray, unsigned int srcIndex, unsigned int ByteCount, CUstream hStream)
 	Copies memory from Array to Host.
CUresult 	cuMemcpyDtoA (CUarray dstArray, unsigned int dstIndex, CUdeviceptr srcDevice, unsigned int ByteCount)
 	Copies memory from Device to Array.
CUresult 	cuMemcpyDtoD (CUdeviceptr dstDevice, CUdeviceptr srcDevice, unsigned int ByteCount)
 	Copies memory from Device to Device.
*/
		//! Copies memory from Device to Host.
		void cpyDtoH(void *dstHost, CUdeviceptr srcDevice, unsigned int byteCount) const
		{
			CUresult error = cuMemcpyDtoH(dstHost,srcDevice,byteCount);
			ApiWrapper::check("cuMemcpyDtoH",error,verbose_);
		}
 	
/*
CUresult 	cuMemcpyDtoHAsync (void *dstHost, CUdeviceptr srcDevice, unsigned int ByteCount, CUstream hStream)
 	Copies memory from Device to Host.
CUresult 	cuMemcpyHtoA (CUarray dstArray, unsigned int dstIndex, const void *pSrc, unsigned int ByteCount)
 	Copies memory from Host to Array.
CUresult 	cuMemcpyHtoAAsync (CUarray dstArray, unsigned int dstIndex, const void *pSrc, unsigned int ByteCount, CUstream hStream)
 	Copies memory from Host to Array.
*/
		//! Copies memory from Host to Device.
		void cpyHtoD(CUdeviceptr dstDevice, const void *srcHost, unsigned int byteCount) const
		{
			CUresult error = cuMemcpyHtoD (dstDevice, srcHost, byteCount);
 			ApiWrapper::check("cuMemcpyHtoD",error,verbose_);
		}
/*
CUresult 	cuMemcpyHtoDAsync (CUdeviceptr dstDevice, const void *srcHost, unsigned int ByteCount, CUstream hStream)
 	Copies memory from Host to Device.
CUresult 	cuMemFree (CUdeviceptr dptr)
 	Frees device memory.
CUresult 	cuMemFreeHost (void *p)
 	Frees page-locked host memory.
CUresult 	cuMemGetAddressRange (CUdeviceptr *pbase, unsigned int *psize, CUdeviceptr dptr)
 	Get information on memory allocations.
CUresult 	cuMemGetInfo (unsigned int *free, unsigned int *total)
 	Gets free and total memory.
CUresult 	cuMemHostAlloc (void **pp, size_t bytesize, unsigned int Flags)
 	Allocates page-locked host memory.
CUresult 	cuMemHostGetDevicePointer (CUdeviceptr *pdptr, void *p, unsigned int Flags)
 	Passes back device pointer of mapped pinned memory.
CUresult 	cuMemHostGetFlags (unsigned int *pFlags, void *p)
 	Passes back flags that were used for a pinned allocation.
CUresult 	cuMemsetD16 (CUdeviceptr dstDevice, unsigned short us, unsigned int N)
 	Initializes device memory.
CUresult 	cuMemsetD2D16 (CUdeviceptr dstDevice, unsigned int dstPitch, unsigned short us, unsigned int Width, unsigned int Height)
 	Initializes device memory.
CUresult 	cuMemsetD2D32 (CUdeviceptr dstDevice, unsigned int dstPitch, unsigned int ui, unsigned int Width, unsigned int Height)
 	Initializes device memory.
CUresult 	cuMemsetD2D8 (CUdeviceptr dstDevice, unsigned int dstPitch, unsigned char uc, unsigned int Width, unsigned int Height)
 	Initializes device memory.
CUresult 	cuMemsetD32 (CUdeviceptr dstDevice, unsigned int ui, unsigned int N)
 	Initializes device memory.
CUresult 	cuMemsetD8 (CUdeviceptr dstDevice, unsigned char uc, unsigned int N)
 	Initializes device memory.
*/
	private:
		bool verbose_;
	}; // class Memory
} // end namespace GpusDoneRight

/*@}*/
#endif // GDR_MEMORY_H_H
