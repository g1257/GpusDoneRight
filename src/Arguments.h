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
		void pass(int x)
		{
			ALIGN_UP(offset_, __alignof(x));
 			CUresult error = cuParamSeti(hfunc_, offset_, x);
 			ApiWrapper::check("cuParamSeti",error,verbose_);
 			incrementOffset(sizeof(x));
		}
		
		template<typename SomeRealType>
		void pass(const GpuPointer<SomeRealType>& gpuPtr)
		{
			size_t size1 = gpuPtr.passToGpuFunction(hfunc_,offset_);
			incrementOffset(size1);
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
