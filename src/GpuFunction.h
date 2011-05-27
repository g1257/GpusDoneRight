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

#include "Arguments.h"

namespace GpusDoneRight {
	
	template<typename ModuleType>
	class GpuFunction {
	public:
		GpuFunction(
			const ModuleType& module,
			const std::string& name,
			bool verbose = true) :
			module_(module),
			verbose_(verbose),
			argumentsHaveBeenPassed_(false)
		{
			module.getFunction(&hfunc_,name.c_str());
		}
		
		//! Write on passArguments for 1 arg, 2 arg, 3 arg, etc
		//! Here we need to check against the ptx file FIXME
		template<typename Arg1Type,typename Arg2Type,typename Arg3Type,
			typename Arg4Type>
		void passArguments(
				const Arg1Type& arg1,
				const Arg2Type& arg2,
				const Arg3Type& arg3,
				const Arg4Type& arg4)
		{
			Arguments arguments(hfunc_,verbose_);
			arguments.pass(arg1);
			arguments.pass(arg2);
			arguments.pass(arg3);
			arguments.pass(arg4);
			argumentsHaveBeenPassed_ = true;
		}

/* API below probably belongs here: please uncomment as you implement them
CUresult 	cuFuncGetAttribute (int *pi, CUfunction_attribute attrib, CUfunction hfunc)
 	Returns information about a function.
 */
		//! Sets the block-dimensions for the function.
		void setBlockShape(int x,int y,int z)
		{
			CUresult error = cuFuncSetBlockShape(hfunc_,x,y,z);
			ApiWrapper::check("cuFuncSetBlockShape",error,verbose_);
		}
 	
/*
 CUresult 	cuFuncSetSharedSize (CUfunction hfunc, unsigned int bytes)
 	Sets the dynamic shared-memory size for the function.
CUresult 	cuLaunch (CUfunction f)
 	Launches a CUDA function.
 	*/
		//! Launches a CUDA function.
		void launchGrid(int grid_width, int grid_height) const
		{
			checkForArguments();
			CUresult error = cuLaunchGrid(hfunc_,grid_width,grid_height);
			ApiWrapper::check("cuLaunchGrid",error,verbose_);
		}
 
		//! Launches a CUDA function.
		template<typename SomeGpuStreamType>
		void launchGridAsync(int grid_width, int grid_height,const SomeGpuStreamType& stream) const	
		{
			checkForArguments();
			CUresult error = cuLaunchGridAsync(hfunc_, grid_width,  grid_height, stream());
			ApiWrapper::check("cuLaunchGrid",error,verbose_);
 		}

	private:
	
		void checkForArguments() const
		{
			if (!argumentsHaveBeenPassed_) 
				throw std::runtime_error("You need to pass arguments before a launch is possible\n");
		}

		const ModuleType& module_; // need to hold a const ref. for checking arguments
		bool verbose_;
		bool argumentsHaveBeenPassed_;
		CUfunction hfunc_; // <--- this class is the owner
	}; // class GpuFunction
} // end namespace GpusDoneRight

/*@}*/
#endif // GPU_FUNCTION_H_H
