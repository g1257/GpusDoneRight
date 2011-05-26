/** \ingroup GpusDoneRight */
/*@{*/

/*! \file Device.h
 *
 * DOCUMENTATION HERE FIXME!!
 *
 */
// adds GDR_ to avoid potential duplicates:
#ifndef GDR_DEVICE_H_H
#define GDR_DEVICE_H_H

#include <typeinfo>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include "TypeToString.h"

#include "ApiWrapper.h"

// Note: It seems that API functions starting with cu belong to the Driver API
// and functions staring with cuda belong to the Runtime API
// Note: We are NOT using the runtime API.

namespace GpusDoneRight {
	class Device {
	public:
		enum {
			MAX_THREADS_PER_BLOCK,
			PROP_SHARED_MEM_PER_BLOCK,
			PROP_TOTAL_CONSTANT_MEMORY,
			PROP_SIMD_WIDTH,
			PROP_MEM_PITCH,
			PROP_REGS_PER_BLOCK,
			PROP_CLOCK_RATE,
			PROP_TEXTURE_ALIGN,
			PROP_MAX_THREADS_DIM,
			PROP_GRID_SIZE
		};
		
		static const char *properties_[]; // see the end of this file

		static size_t const BASIC_PROPERTIES, ALL_PROPERTIES, ALL_ATTRIBUTES;

		/**	The supported attributes are: */
		static const char *attributes_[]; // see the end of this file
		
		static const CUdevice_attribute attribNumbers_[]; // see the end of this file

		Device(int dev,bool verbose) : verbose_(verbose),number_(dev)
		{
			CUresult error = cuDeviceGet(&cuDevice_, dev);
			ApiWrapper::check("cuDeviceGet",error,verbose);
			
			int len = 4096;
			name_ = new char[len];
			cuDeviceGetName (name_,len,cuDevice_);
			
			cuDeviceComputeCapability (&major_,&minor_,cuDevice_);
			
			cuDeviceTotalMem (&totalMem_,cuDevice_);
			
			error = cuDeviceGetProperties(&deviceProp_, cuDevice_);
			ApiWrapper::check("cuDeviceGetProperties",error,verbose);

			// if you need attributes call attributes function below
			// on a on-demand basis. Because there are so some many of 
			// them, we won't handle them here.
		}
		
		~Device()
		{
			delete [] name_;
		}
		
		int number() const { return number_; }
		
		const char* name() const { return name_; }
		
		int  majorVersion() const { return major_; }

		int  minorVersion() const { return minor_; }

		unsigned int globalMem() const { return totalMem_;}

		// Device properties
		
		int getProperty(size_t prop) const
		{
			switch (prop) {
			case MAX_THREADS_PER_BLOCK:
				return deviceProp_.maxThreadsPerBlock;
			case PROP_SHARED_MEM_PER_BLOCK:
				return deviceProp_.sharedMemPerBlock;
			case PROP_TOTAL_CONSTANT_MEMORY:
				return deviceProp_.totalConstantMemory;
			case PROP_SIMD_WIDTH:
				return deviceProp_.SIMDWidth;
			case PROP_MEM_PITCH:
				return deviceProp_.memPitch;
			case PROP_REGS_PER_BLOCK:
				return deviceProp_.regsPerBlock;
			case PROP_CLOCK_RATE:
				return deviceProp_.clockRate;
			case PROP_TEXTURE_ALIGN:
				return deviceProp_.textureAlign;
			}
			return -1;
		}
		
		void getProperty(std::vector<int>& x,size_t prop) const
		{
			for (size_t i=0;i<x.size();i++) x[i]=0;
			switch (prop) {
			case PROP_MAX_THREADS_DIM:
				vectorSet(x,deviceProp_.maxThreadsDim);
				return;
			case PROP_GRID_SIZE:
				vectorSet(x,deviceProp_.maxGridSize);
				return;
			}
		}
		
		bool isExtendedProperty(size_t i) const
		{
			return (i<BASIC_PROPERTIES) ? true : false;
		}
		
		int getAttribute (CUdevice_attribute attrib) const
		{
			int pi = 0;
			CUresult error = cuDeviceGetAttribute(&pi,attrib,cuDevice_);
			ApiWrapper::check("cuDeviceGetAttribute",error,verbose_);
			return pi;
		}
		
		std::string toString() const
		{
			std::ostringstream msg;

			msg << "-------------------------------------------------- ";
			msg << name() << " Cuda Device["<< number_ << "] revision number ";
			msg << major_<< "." << minor_ << "\n";
			
			msg << "  global memory:              " << totalMem_ << "\n";
			msg << "#Properties:\n";
			for (size_t i=0;i<ALL_PROPERTIES;i++) {
				if (isExtendedProperty(i)) {
					std::vector<int> x(3);
					getProperty(x,i);
					msg<<properties_[i]<<" = "<<x[0]<<" "<<x[1]<<" "<<x[2]<<"\n";
					continue;
				}
				int x = getProperty(i);
				msg<<properties_[i]<<" = "<<x<<"\n";
				
			}
			msg << "#Attributes:\n";
			for (size_t i=0;i<ALL_ATTRIBUTES;i++) {
				
				int x = getAttribute(attribNumbers_[i]);
				msg<<attributes_[i]<<" = "<<x<<"\n";
			}
			
			return msg.str();

		}

	private:

		void vectorSet(std::vector<int>& x,const int* y) const
		{
			for (size_t i=0;i<x.size();i++) x[i] = y[i];
		}

		bool verbose_;
		int number_;
		CUdevice cuDevice_;
		char *name_;
		int major_,minor_;
		unsigned int totalMem_;
		CUdevprop deviceProp_;

	}; // class Device
	
	//! description of properties for the driver API
	const char *Device::properties_[] = {
		"maximum number of threads per block",
		"total amount of shared memory available per block in bytes",
		"total amount of constant memory available on the device in bytes",
		"warp size",
		"maximum pitch allowed by the memory copy functions that involve memory "
			"regions allocated through cuMemAllocPitch()",
		"regsPerBlock is the total number of registers available per block",
		"clock frequency in kilohertz",
		"alignment requirement",
		"texture base addresses that are aligned to textureAl",
		"maximum sizes of each dimension of a block",
		"maximum sizes of each dimension of a grid"
	};
	
	//! properties above that are just an integer:	
	size_t const Device::BASIC_PROPERTIES = 9;
	size_t const Device::ALL_PROPERTIES = 11;

	/*! The supported attributes of the driver API are: */
	size_t const Device::ALL_ATTRIBUTES = 22;	
	const char *Device::attributes_[] = {
		"Maximum number of threads per block",
		"Maximum x-dimension of a block",
		"Maximum y-dimension of a block",
		"Maximum z-dimension of a block",
		"Maximum x-dimension of a grid",
		"Maximum y-dimension of a grid",
		"Maximum z-dimension of a grid",
		"Maximum amount of shared memory available to a thread block in bytes",
		"This amount is shared by all thread blocks simultaneously resident on a multiprocessor",
		"Memory available on device for __constant__ variables in a CUDA C kernel in bytes",
		"Warp size in threads",
		"Maximum pitch in bytes allowed by the memory copy functions that involve memory regions "
			" allocated through cuMemAllocPitch()",
		"Maximum number of 32-bit registers available to a thread block",
		"This number is shared by all thread blocks simultaneously resident on a multiprocessor",
		"Peak clock frequency in kilohertz",
		"Alignment requirement; texture base addresses aligned to textureAlign bytes "
			" do not need an offset applied to texture fetches",
		"1 if the device can concurrently copy memory between host and device while executing a kernel,"
			" or 0 if not",
		"Number of multiprocessors on the device",
		"1 if there is a run time limit for kernels executed on the device, or 0 if not",
		"1 if the device is integrated with the memory subsystem, or 0 if not",
		"1 if the device can map host memory into the CUDA address space, or 0 if not",
		"Compute mode that device is currently in. Available modes are as follows:"
			"- CU_COMPUTEMODE_DEFAULT: Default mode - Device is not restricted and "
				"can have multiple CUDA contexts present at a single time."
			"- CU_COMPUTEMODE_EXCLUSIVE: Compute-exclusive mode - "
				"Device can have only one CUDA context present on it at a time."
			"- CU_COMPUTEMODE_PROHIBITED: Compute-prohibited mode - "
				"Device is prohibited from creating new CUDA contexts."
	};
	
	const CUdevice_attribute Device::attribNumbers_[] = {
		CU_DEVICE_ATTRIBUTE_MAX_THREADS_PER_BLOCK,
		CU_DEVICE_ATTRIBUTE_MAX_BLOCK_DIM_X,
		CU_DEVICE_ATTRIBUTE_MAX_BLOCK_DIM_Y,
		CU_DEVICE_ATTRIBUTE_MAX_BLOCK_DIM_Z,
		CU_DEVICE_ATTRIBUTE_MAX_GRID_DIM_X,
		CU_DEVICE_ATTRIBUTE_MAX_GRID_DIM_Y,
		CU_DEVICE_ATTRIBUTE_MAX_GRID_DIM_Z,
		CU_DEVICE_ATTRIBUTE_MAX_SHARED_MEMORY_PER_BLOCK,
		CU_DEVICE_ATTRIBUTE_SHARED_MEMORY_PER_BLOCK,
		CU_DEVICE_ATTRIBUTE_TOTAL_CONSTANT_MEMORY,
		CU_DEVICE_ATTRIBUTE_WARP_SIZE,
		CU_DEVICE_ATTRIBUTE_MAX_PITCH,
		CU_DEVICE_ATTRIBUTE_MAX_REGISTERS_PER_BLOCK,
		CU_DEVICE_ATTRIBUTE_REGISTERS_PER_BLOCK,
		CU_DEVICE_ATTRIBUTE_CLOCK_RATE,
		CU_DEVICE_ATTRIBUTE_TEXTURE_ALIGNMENT,
		CU_DEVICE_ATTRIBUTE_GPU_OVERLAP,
		CU_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT,
		CU_DEVICE_ATTRIBUTE_KERNEL_EXEC_TIMEOUT,
		CU_DEVICE_ATTRIBUTE_INTEGRATED,
		CU_DEVICE_ATTRIBUTE_CAN_MAP_HOST_MEMORY,
		CU_DEVICE_ATTRIBUTE_COMPUTE_MODE
	};
} // end namespace GpusDoneRight

/*@}*/
#endif // GDR_DEVICE_H_H
