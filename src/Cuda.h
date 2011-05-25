/** \ingroup GpusDoneRight */
/*@{*/

/*! \file Cuda.h
 *
 *
 */
// adds GDR_ to avoid potential duplicates:
#ifndef GDR_CUDA_H_H
#define GDR_CUDA_H_H

#include <typeinfo>

#include <cuda.h>
#include <cuda_runtime_api.h>

namespace GpusDoneRight {

	class Cuda {
	public:
		class Device {
		public:
			Device(int dev) : number_(dev)
			{
				cudaGetDeviceProperties(&deviceProp_, dev);
				cudaDriverGetVersion(&driverVersion_);
				cudaRuntimeGetVersion(&runtimeVersion_);
			}

			int  majorVersion() const { return deviceProp_.major; }

			int  minorVersion() const { return deviceProp_.minor; }

			long globalMem() const { return deviceProp_.totalGlobalMem;}

			long constantMemorySize() const { return deviceProp_.totalConstMem; }

			long sharedMemorySize() const { return deviceProp_.sharedMemPerBlock; }

			int  regsPerBlock() const { return deviceProp_.regsPerBlock; }

			int  warpSize() const { return deviceProp_.warpSize; }

			int  maxThreadsPerBlock() const { return deviceProp_.maxThreadsPerBlock; }

			long memPitch() const { return deviceProp_.memPitch; }

			long textureAlignment() const { return deviceProp_.textureAlignment; }

			int numberOfMultiprocessors() const
			{
				return deviceProp_.multiProcessorCount;
			}

			std::string name() const
			{
				return deviceProp_.name;
			}

			int numberOfCores() const
			{
				return deviceProp_.multiProcessorCount * 8;
			}

			int maxThreadsDim(int i) const
			{
				if (i>2)
					throw std::range_error("dim index out of range in cuda.h");
				return deviceProp_.maxThreadsDim[i];
			}

			int maxGridSize(int i) const
			{
				if (i>2)
					throw std::range_error("grid index out of range in cuda.h");
				return deviceProp_.maxGridSize[i];
			}

			std::string deviceOverlap() const
			{
				return deviceProp_.deviceOverlap ? "Yes" : "No";
			}

			std::string kernelExecTimeoutEnabled() const
			{
				return deviceProp_.kernelExecTimeoutEnabled ? "Yes" : "No";
			}

			std::string integrated() const
			{
				return deviceProp_.integrated ? "Yes" : "No";
			}

			std::string canMapHostMemory() const
			{
				return deviceProp_.canMapHostMemory ? "Yes" : "No";
			}

			std::string computeMode() const
			{
				std::ostringstream msg;
				if (deviceProp_.computeMode =3D=3D cudaComputeModeDefault)
					msg << "Default (multiple host threads can use this device simultaneously)\n";

				if (deviceProp_.computeMode =3D=3D cudaComputeModeExclusive)
					msg << "Exclusive (only one host thread at a time can use this device)\n";

				if (deviceProp_.computeMode =3D=3D cudaComputeModeProhibited)
					msg << "Prohibited (no host thread can use this device)\n";

				return msg.str();
			}

			std::string toString() const
			{
				std::ostringstream msg;

				msg << "-------------------------------------------------- ";
				msg << name() << " Cuda Device["<< number_ << "] revision number ";
				msg << majorVersion() << "." << minorVersion() << "\n";
				if (deviceProp_.major =3D=3D 9999 && deviceProp_.minor =3D=3D 9999)
					msg << "There is no device supporting CUDA.\n";

				//msg << "  CUDA Driver Version:        " << driverVersion/1000 << "." << driverVersion%100 << "\n";
				msg << "  global memory:              " << globalMem() << "\n";
				msg << "  number of multiprocessors:  " << numberOfMultiprocessors() << "\n";
				msg << "  number of cores:            " << numberOfCores() << "\n";

				msg << "  Total amount of constant memory:               " << constantMemorySize() << " bytes\n";
				msg << "  Total amount of shared memory per block:       " << sharedMemorySize()   << " bytes\n";
				msg << "  Total number of registers available per block: " << regsPerBlock()       << "\n";
				msg << "  Warp size:                                     " << warpSize()           << "\n";;
				msg << "  Maximum number of threads per block:           " << maxThreadsPerBlock() << "\n";

				msg << "  Maximum sizes of each dimension of a block:    ";
				msg << maxThreadsDim(0) << " x " << maxThreadsDim(1) << " x " << maxThreadsDim(2) << "\n";
				msg << "  Maximum sizes of each dimension of a grid:     " << maxGridSize(0) << " x " << maxGridSize(1) << " x " << maxGridSize(2) << "\n";
				msg << "  Maximum memory pitch:                          " << memPitch()  << " bytes; \n";
				msg << "  Texture alignment:                             " << textureAlignment() << " bytes\n";
				msg << "  Clock rate:                                    " << (deviceProp_.clockRate * 1e-6f) << " GHz\n";
				msg << "  Device Overlap:                                " << deviceOverlap() << "\n";
				msg << "  Kernel Exec Timeout Enabled:                   " << kernelExecTimeoutEnabled() << "\n";
				msg << "  Integrated:                                    " << integrated() << "\n";
				msg << "  Can Map Host Memory:                           " << canMapHostMemory() << "\n";
				msg << "  Compute Mode:                                  " << computeMode() << "\n";
				return msg.str();

			}

		private:
			cudaDeviceProp deviceProp_;
			int number_;
			int driverVersion_;
			int runtimeVersion_;

		}; // inner class Device

		Cuda():
			devices_(getDeviceCount(),0)
		{
			if (verbose_) {
				std::cout<<"Cuda::ctor(...): deviceCount=";
				std::cout<<devices_.size()<<"\n";
			}
			for (size_t i=0; i<devices_.size(); i++)
				devices_[i] = new Device(i);
		}

		~Cuda()
		{
			for (size_t i=0; i<devices_.size(); i++) {
				delete devices_[i];
				devices_[i] = 0;
			}
		}

		

		size_t size() const { return devices_.size(); }

		std::string toString() {
			std::ostringstream msg;
			for (size_t i=0; i<devices_.size(); i++)
				msg << devices_[i]->toString();
			return msg.str();
		}
	private:

		int getDeviceCount() {
			initCudaApi();
			int result = 0;
			cudaGetDeviceCount(&result);
			return result;
		}

		void initCudaApi()
		{
			static bool firstCall = true;
			if (!firstCall) return;
			CUresult error = cuInit(0);
			if (error != CUDA_SUCCESS) {
				apiFatalError("cuInit",error);
			}
			if (verbose_) apiCallVerbose("cuInit");
			firstCall = false;
		}

		void apiFatalError(const std::string& apiFunc,const CUresult& error) const
		{
			std::string s = "Cuda::apiFatalError::" + apiFunc + 
				"(...) error code=" + ttos(error)  + "\n";
			throw std::runtime_error(s.c_str());
		}

		void apiCallVerbose(const std::string& apiFunc) const
		{
			std::string s = "Cuda::apiCallVerbose::" + apiFunc + "(...)\n";
		}

		std::vector<Device*> devices_;
	}; // class Cuda
} // end namespace GpusDoneRight

/*@}*/
#endif // GDR_CUDA_H_H
