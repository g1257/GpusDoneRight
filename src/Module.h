//-*-C++-*-
/** \ingroup GpusDoneRight */
/*@{*/

/*! \file Module.h
 *
 * DOCUMENTATION HERE FIXME
 *
 */
// adds GDR_ to avoid potential name clashes:
#ifndef GDR_MODULE_H_H
#define GDR_MODULE_H_H

#include <typeinfo>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include "TypeToString.h"

#include "ApiWrapper.h"

namespace GpusDoneRight {

	class Module {
	public:
		//! Loads a compute module.
		Module(const char *filename,bool verbose = true) : verbose_(verbose)
		{
			CUresult error = cuModuleLoad(&module_, filename);
			ApiWrapper::check("cuModuleLoad",error,verbose);
		}
		
		//! LLoad a module's data.
		Module(const void *image,bool isFat = false,bool verbose = true) : verbose_(verbose)
		{
			if (!isFat) {
				CUresult error = cuModuleLoadData(&module_,image);
				ApiWrapper::check("cuModuleLoadData",error,verbose);
				return;
			}
			CUresult error = cuModuleLoadFatBinary(&module_,image);
			ApiWrapper::check("cuModuleLoadFatBinary",error,verbose);
		}
 		
		//! Load a module's data with options.
		// FIXME: NEEDS WORK:
		/*Module(const void *image,bool verbose = true) : verbose_(verbose)
		{
			CUresult error = cuModuleLoadDataEx(CUmodule *module, const void *image, unsigned int numOptions, CUjit_option *options, void **optionValues);
			ApiWrapper::check("cuModuleLoadDataEx",error,verbose);
		}*/

		//! Using RAII below:
		~Module()
		{
			CUresult error = cuModuleUnload(module_);
			// let's never throw in the destructor:
			ApiWrapper::check("cuModuleUnload",error,verbose_,ApiWrapper::DO_NOT_THROW);
		}
		
		//! sets a function handle.
		void getFunction(CUfunction *hfunc,const char *name) const
		{
			CUresult error = cuModuleGetFunction(hfunc,module_,name);
			ApiWrapper::check("cuModuleGetFunction",error,verbose_);
		}
		
		//! Sets a global pointer from a module.
 		void getGlobal(CUdeviceptr *dptr, unsigned int *bytes, const char *name) const
		{
			CUresult error = cuModuleGetGlobal(dptr,bytes,module_,name);
			ApiWrapper::check("cuModuleGetGlobal",error,verbose_);
		}

		//! Returns a handle to a texture-reference.
		void getTexRef(CUtexref *pTexRef, const char *name) const
		{
			CUresult error = cuModuleGetTexRef(pTexRef,module_,name);
			ApiWrapper::check("cuModuleGetGlobal",error,verbose_);
		}
		
	private:
		bool verbose_;
		CUmodule module_; // we own this
	}; // class Module
} // end namespace GpusDoneRight

/*@}*/
#endif // GDR_MODULE_H_H
