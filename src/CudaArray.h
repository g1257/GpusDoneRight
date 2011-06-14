//-*-C++-*-
/** \ingroup GpusDoneRight */
/*@{*/

/*! \file CudaArray.h
 *
 * DOCUMENTATION HERE FIXME
 *
 */
#ifndef CUDA_ARRAY_H_H
#define CUDA_ARRAY_H_H

#include <typeinfo>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include "TypeToString.h"

#include "ApiWrapper.h"

namespace GpusDoneRight {

	class CudaArray {
	public:
		template<typename ImageType>
		CudaArray(const ImageType& image,bool verbose = true)
		: verbose_(verbose)
		{
			desc_.Format = CU_AD_FORMAT_FLOAT;
			desc_.NumChannels = 1;
			desc_.Width = image.width();
			desc_.Height = image.height();

			CUresult error = cuArrayCreate(&array_, &desc_);
			ApiWrapper::check("cuArrayCreate",error,verbose_);

			CUDA_MEMCPY2D copyParam;
			memset(&copyParam, 0, sizeof(copyParam));
			copyParam.dstMemoryType = CU_MEMORYTYPE_ARRAY;
			copyParam.dstArray = array_;
			copyParam.srcMemoryType = CU_MEMORYTYPE_HOST;
			copyParam.srcHost = image.data();
			copyParam.srcPitch = image.width() * sizeof(typename ImageType::ValueType);
			copyParam.WidthInBytes = copyParam.srcPitch;
			copyParam.Height = image.height();

			error = cuMemcpy2D(&copyParam);
			ApiWrapper::check("cuMemcpy2D",error,verbose_);
			//FIXME: copyParam goes out of scope here, is this ok??
		}

		// Using RAII here:
		~CudaArray()
		{
			CUresult error = cuArrayDestroy(array_);
			ApiWrapper::check("cuArrayDestroy",error,verbose_,ApiWrapper::DO_NOT_THROW);
			
		}

		CUarray operator()() { return array_; }

		// When adding function to this class, please uncomment or delete from
		// the list below:
/*CUresult 	cuArray3DCreate (CUarray *pHandle, const CUDA_ARRAY3D_DESCRIPTOR *pAllocateArray)
 	Creates a 3D CUDA array.
CUresult 	cuArray3DGetDescriptor (CUDA_ARRAY3D_DESCRIPTOR *pArrayDescriptor, CUarray hArray)
 	Get a 3D CUDA array descriptor.
CUresult 	cuArrayGetDescriptor (CUDA_ARRAY_DESCRIPTOR *pArrayDescriptor, CUarray hArray)
 	Get a 1D or 2D CUDA array descriptor.
*/

	private:
		bool verbose_;
		CUDA_ARRAY_DESCRIPTOR desc_;
		CUarray array_;
	}; // class CudaArray
} // end namespace GpusDoneRight

/*@}*/
#endif // CUDA_ARRAY_H_H
