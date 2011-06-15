//-*-C++-*-
/** \ingroup GpusDoneRight */
/*@{*/

/*! \file Texture.h
 *
 * DOCUMENTATION HERE FIXME
 *
 */
#ifndef TEXTURE_H_H
#define TEXTURE_H_H

#include <typeinfo>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include "TypeToString.h"

#include "ApiWrapper.h"
#include "CudaArray.h"

namespace GpusDoneRight {

	template<typename FieldType>
	class Texture {
	public:
		template<typename ModuleType,typename CudaArrayType>
		Texture(
		        const ModuleType& module,
		        const std::string& name,
		        const CudaArrayType& array,
		        bool verbose = true) : verbose_(verbose)
		{
			// We don't really "construct" a texture, we just
			// get a reference from the kernel module to the texture
			// living there:
			module.getTexRef(&texref_,name.c_str());
			
			// Set the array:
			CUresult error = cuTexRefSetArray(texref_, array(), CU_TRSA_OVERRIDE_FORMAT);
			ApiWrapper::check("cuTexRefSetArray",error,verbose_);

			// other misc. settings:
			error = cuTexRefSetAddressMode(texref_, 0, CU_TR_ADDRESS_MODE_WRAP);
			ApiWrapper::check("cuTexRefSetAddressMode",error,verbose_);
			error = cuTexRefSetAddressMode(texref_, 1, CU_TR_ADDRESS_MODE_WRAP);
			ApiWrapper::check("cuTexRefSetAddressMode",error,verbose_);
			error = cuTexRefSetFilterMode(texref_, CU_TR_FILTER_MODE_LINEAR);
			ApiWrapper::check("cuTexRefSetFilterMode",error,verbose_);
			error = cuTexRefSetFlags(texref_, CU_TRSF_NORMALIZED_COORDINATES);
			ApiWrapper::check("cuTexRefSetFlags",error,verbose_);
			error = cuTexRefSetFormat(texref_, CU_AD_FORMAT_FLOAT, 1);
		}
		
		~Texture()
		{
			// Don't destroy the texture, we don't really own it, 
			// it lives in the kernel
		}

		CUtexref operator()() { return texref_; }

		// When adding function to this class, please uncomment or delete from
		// the list below:
/*		
		CUresult 	cuTexRefCreate (CUtexref *pTexRef)
 	Creates a texture reference.
CUresult 	cuTexRefDestroy (CUtexref hTexRef)
 	Destroys a texture reference.
CUresult 	cuTexRefGetAddress (CUdeviceptr *pdptr, CUtexref hTexRef)
 	Gets the address associated with a texture reference.
CUresult 	cuTexRefGetAddressMode (CUaddress_mode *pam, CUtexref hTexRef, int dim)
 	Gets the addressing mode used by a texture reference.
CUresult 	cuTexRefGetArray (CUarray *phArray, CUtexref hTexRef)
 	Gets the array bound to a texture reference.
CUresult 	cuTexRefGetFilterMode (CUfilter_mode *pfm, CUtexref hTexRef)
 	Gets the filter-mode used by a texture reference.
CUresult 	cuTexRefGetFlags (unsigned int *pFlags, CUtexref hTexRef)
 	Gets the flags used by a texture reference.
CUresult 	cuTexRefGetFormat (CUarray_format *pFormat, int *pNumChannels, CUtexref hTexRef)
 	Gets the format used by a texture reference.
CUresult 	cuTexRefSetAddress (unsigned int *ByteOffset, CUtexref hTexRef, CUdeviceptr dptr, unsigned int bytes)
 	Binds an address as a texture reference.
CUresult 	cuTexRefSetAddress2D (CUtexref hTexRef, const CUDA_ARRAY_DESCRIPTOR *desc, CUdeviceptr dptr, unsigned int Pitch)
 	Binds an address as a 2D texture reference.
CUresult 	cuTexRefSetAddressMode (CUtexref hTexRef, int dim, CUaddress_mode am)
 	Sets the addressing mode for a texture reference.
CUresult 	cuTexRefSetArray (CUtexref hTexRef, CUarray hArray, unsigned int Flags)
 	Binds an address as a texture reference.
CUresult 	cuTexRefSetFilterMode (CUtexref hTexRef, CUfilter_mode fm)
 	Sets the filtering mode for a texture reference.
CUresult 	cuTexRefSetFlags (CUtexref hTexRef, unsigned int Flags)
 	Sets the flags for a texture reference.
CUresult 	cuTexRefSetFormat (CUtexref hTexRef, CUarray_format fmt, int NumPackedComponents)
 	Sets the format for a texture reference. 
*/
	private:
		bool verbose_;
		CUtexref texref_; // we own this
	}; // class Texture
} // end namespace GpusDoneRight

/*@}*/
#endif // TEXTURE_H_H
