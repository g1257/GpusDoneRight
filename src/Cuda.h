/** \ingroup GpusDoneRight */
/*@{*/

/*! \file Cuda.h
 *
 * DOCUMENTATION HERE FIXME
 *
 */
// adds GDR_ to avoid potential duplicates:
#ifndef GDR_CUDA_H_H
#define GDR_CUDA_H_H

#include <typeinfo>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include "TypeToString.h"

#include "ApiWrapper.h"

namespace GpusDoneRight {

	template<typename DeviceType>
	class Cuda {
	public:
		Cuda(bool verbose=true):
			devices_(getDeviceCount()),verbose_(verbose)
		{
			if (verbose_) {
				std::cout<<"Cuda::ctor(...): deviceCount=";
				std::cout<<devices_.size()<<"\n";
			}
			for (size_t i=0; i<devices_.size(); i++)
				devices_[i] = new Device(i,verbose);
		}

		~Cuda()
		{
			for (size_t i=0; i<devices_.size(); i++) {
				delete devices_[i];
				devices_[i] = 0;
			}
		}
		
		
		const CUdevice& getDevice(size_t i) const
		{
			return devices_[i]->get();
		}

		size_t size() const { return devices_.size(); }

		std::string toString() const
		{
			std::ostringstream msg;
			for (size_t i=0; i<devices_.size(); i++)
				msg << devices_[i]->toString();
			return msg.str();
		}
	private:

		int getDeviceCount()
		{
			initCudaApi();
			int result = 0;
			cuDeviceGetCount (&result);
			return result;
		}

		void initCudaApi()
		{
			static bool firstCall = true;
			if (!firstCall) return;
			CUresult error = cuInit(0);
			ApiWrapper::check("cuInit",error,verbose_);
			firstCall = false;
		}

		std::vector<DeviceType*> devices_;
		bool verbose_;
	}; // class Cuda
} // end namespace GpusDoneRight

/*@}*/
#endif // GDR_CUDA_H_H
