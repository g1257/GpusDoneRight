/** \ingroup GpusDoneRight */
/*@{*/

/*! \file GpuFunction.h
 *
 * DOCUMENTATION HERE FIXME
 *
 */
#ifndef GPU_FUNCTION_H_H
#define GPU_FUNCTION_H_H

#include <typeinfo>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include "TypeToString.h"

#include "ApiWrapper.h"

namespace GpusDoneRight {
	
	template<typename ModuleType>
	class GpuFunction {
	public:
		GpuFunction(const ModuleType& module,const std::string& name,bool verbose = true) :
			verbose_(verbose)
		{
			module.getFunction(&hfunc_,name.c_str());
		}


	private:
		bool verbose_;
		CUfunction hfunc_;
	}; // class GpuFunction
} // end namespace GpusDoneRight

/*@}*/
#endif // GPU_FUNCTION_H_H
