/** \ingroup GpusDoneRight */
/*@{*/

/*! \file Cuda.h
 *
 * For now simple checks the api calls and interprets
 * error codes. In the future we might want to intercept (wrap) all
 * api calls here
 *
 */
// adds GDR_ to avoid potential duplicates:
#ifndef GDR_API_WRAPPER_H_H
#define GDR_API_WRAPPER_H_H

#include <typeinfo>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include "TypeToString.h"

#include <cuda.h>

namespace GpusDoneRight {
	class ApiWrapper {
	
		static const CUresult cudaErrorCode_[];
		static const char* cudaErrorDesc_[];
		static const size_t ALL_ERRORS;
	public:
		enum {THROW, DO_NOT_THROW};

		static void check(
			const std::string& apiFunc,
			const CUresult& error,
			bool verbose,
			size_t throwOrNot = THROW)
		{
			if (verbose) std::string s = "apiCallVerbose::" + apiFunc + "(...)\n";
			if (error != CUDA_SUCCESS) apiFatalError(apiFunc,error,throwOrNot);
		}

	private:
		static void apiFatalError(const std::string& apiFunc,const CUresult& error,size_t throwOrNot)
		{
			std::string s = "apiFatalError::" + apiFunc + 
				"(...) error code=" + ttos(error)  + "\n";
			s +=  std::string(getErrorString(error)) + "\n";
			if (throwOrNot == THROW) throw std::runtime_error(s.c_str());
		}

		static const char*  getErrorString(const CUresult& error) 
		{
			for (size_t i=0;i<ALL_ERRORS;i++)
				if (cudaErrorCode_[i]==error) return cudaErrorDesc_[i];
			return "ApiWrapper::getErrorString(...): Hmmm...it's probably our fault, sorry";
		}
	}; // class ApiWrapper
	
	const size_t ApiWrapper::ALL_ERRORS = 27;
	const CUresult ApiWrapper::cudaErrorCode_[] = {
		CUDA_SUCCESS,
		CUDA_ERROR_INVALID_VALUE,
		CUDA_ERROR_OUT_OF_MEMORY,
		CUDA_ERROR_NOT_INITIALIZED,
		CUDA_ERROR_DEINITIALIZED,
		CUDA_ERROR_NO_DEVICE,
		CUDA_ERROR_INVALID_DEVICE,
		CUDA_ERROR_INVALID_IMAGE,
		CUDA_ERROR_INVALID_CONTEXT,
		CUDA_ERROR_CONTEXT_ALREADY_CURRENT,
		CUDA_ERROR_MAP_FAILED,
		CUDA_ERROR_UNMAP_FAILED,
		CUDA_ERROR_ARRAY_IS_MAPPED,
		CUDA_ERROR_ALREADY_MAPPED,
		CUDA_ERROR_NO_BINARY_FOR_GPU,
		CUDA_ERROR_ALREADY_ACQUIRED,
		CUDA_ERROR_NOT_MAPPED,
		CUDA_ERROR_INVALID_SOURCE,
		CUDA_ERROR_FILE_NOT_FOUND,
		CUDA_ERROR_INVALID_HANDLE,
		CUDA_ERROR_NOT_FOUND,
		CUDA_ERROR_NOT_READY,
		CUDA_ERROR_LAUNCH_FAILED,
		CUDA_ERROR_LAUNCH_OUT_OF_RESOURCES,
		CUDA_ERROR_LAUNCH_TIMEOUT,
		CUDA_ERROR_LAUNCH_INCOMPATIBLE_TEXTURING,
		CUDA_ERROR_UNKNOWN
	};

	const char* ApiWrapper::cudaErrorDesc_[] = {
		"No errors.",
		"Invalid value.",
		"Out of memory.",
		"Driver not initialized.",
		"Driver deinitialized.",
		"No CUDA-capable device available.",
		"Invalid device.",
		"Invalid kernel image.",
		"Invalid context.",
		"Context already current.",
		"Map failed.",
		"Unmap failed.",
		"Array is mapped.",
		"Already mapped.",
		"No binary for GPU.",
		"Already acquired.",
		"Not mapped.",
		"Invalid source.",
		"File not found.",
		"Invalid handle.",
		"Not found.",
		"CUDA not ready.",
		"Launch failed.",
		"Launch exceeded resources.",
		"Launch exceeded timeout.",
		"Launch with incompatible texturing.",
		"Unknown error"
	};
} // end namespace GpusDoneRight

/*@}*/
#endif // GDR_API_WRAPPER_H_H

