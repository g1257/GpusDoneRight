//-*-C++-*-
/** \ingroup GpusDoneRight */
/*@{*/

/*! \file Arguments.h
 *
 * DOCUMENTATION HERE FIXME
 *
 */
#ifndef GPU_ARGUMENTS_H_H
#define GPU_ARGUMENTS_H_H

#include <typeinfo>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include "TypeToString.h"

#include "GpuPointer.h"
        
namespace GpusDoneRight {
	
	class Arguments {
	public:
	
		Arguments(CUfunction& hfunc,bool verbose = false) : 
			hfunc_(hfunc),verbose_(verbose),offset_(0)
		{}
		
		~Arguments()
		{
			CUresult error = cuParamSetSize(hfunc_, offset_);
			ApiWrapper::check("cuParamSetSize",error,verbose_,ApiWrapper::DO_NOT_THROW);
		}

		//! add one pass function per type below
		void pass(size_t xs)
		{
			int x = xs;
			ALIGN_UP(offset_, __alignof(x));
 			CUresult error = cuParamSeti(hfunc_, offset_, x);
 			ApiWrapper::check("cuParamSeti",error,verbose_);
 			incrementOffset(sizeof(x));
		}

		void pass(float x)
		{
			ALIGN_UP(offset_, __alignof(x));
			CUresult error = cuParamSetf(hfunc_, offset_, x);
			ApiWrapper::check("cuParamSetf",error,verbose_);
 			incrementOffset(sizeof(x));
		}

		template<typename SomeRealType>
		void pass(const GpuPointer<SomeRealType>& gpuPtr)
		{
			CUdeviceptr newPtr = gpuPtr.getCUdeviceptr();
			void *ptr = (void*)(size_t)(newPtr);
			ALIGN_UP(offset_, __alignof(ptr));
			CUresult error = cuParamSetv(hfunc_, offset_, &ptr, sizeof(ptr));
			ApiWrapper::check("cuParamSetv",error,verbose_);
			incrementOffset(sizeof(ptr));
		}
		
	private:

		void incrementOffset(size_t size1)
		{
			offset_ += size1;
		}
		
		CUfunction& hfunc_;
		bool verbose_;
		size_t offset_;
	}; // class Arguments
	
} // end namespace GpusDoneRight

/*@}*/
#endif // GPU_ARGUMENTS_H_H
