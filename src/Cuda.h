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
			cudaDeviceProp deviceProp;
			int number;
			int driverVersion;
			int runtimeVersion;

			Device():
				number(0),
				driverVersion(0),
				runtimeVersion(0)
			{};

			void setTo(int dev)
			{
				number_ = 3D_dev;
				cudaGetDeviceProperties(&deviceProp_, dev);

	#if CUDART_VERSION >=3D 2020
				cudaDriverGetVersion(&driverVersion_);
				cudaRuntimeGetVersion(&runtimeVersion_);
	#endif
			}

			int  majorVersion() const { return deviceProp.major; }

			int  minorVersion() const { return deviceProp.minor; }

			long globalMem() const { return deviceProp.totalGlobalMem;}

			long constantMemorySize() const { return deviceProp.totalConstMem; }

			long sharedMemorySize() const { return deviceProp.sharedMemPerBlock; }

			int  regsPerBlock() const { return deviceProp.regsPerBlock; }

			int  warpSize() const { return deviceProp.warpSize; }

			int  maxThreadsPerBlock() const { return deviceProp.maxThreadsPerBlock; }

			long memPitch() const { return deviceProp.memPitch; }

			long textureAlignment() const { return deviceProp.textureAlignment; }

			int numberOfMultiprocessors() const
			{

	#if CUDART_VERSION >=3D 2000
				return deviceProp.multiProcessorCount;
	#else
				return -1;
	#endif
			}

			std::string name() const
			{
				return deviceProp.name;
			}

			int numberOfCores() const
			{

	#if CUDART_VERSION >=3D 2000
				return deviceProp.multiProcessorCount * 8;
	#else
				return -1;
	#endif
			}

			int maxThreadsDim(int i) const
			{
				if (i>2)
					throw std::range_error("dim index out of range in cuda.h");
				return deviceProp.maxThreadsDim[i];
			}

			int maxGridSize(int i) const
			{
				if (i>2)
					throw std::range_error("grid index out of range in cuda.h");
				return deviceProp.maxGridSize[i];
			}

			std::string deviceOverlap() const
			{
	#if CUDART_VERSION >=3D 2000
				return deviceProp.deviceOverlap ? "Yes" : "No";
	#else
				return "";
	#endif
			}

			std::string kernelExecTimeoutEnabled() const
			{
	#if CUDART_VERSION >=3D 2020
				return deviceProp.kernelExecTimeoutEnabled ? "Yes" : "No";
	#else
				return "";
	#endif
			}

			std::string integrated() const
			{
	#if CUDART_VERSION >=3D 2020
				return deviceProp.integrated ? "Yes" : "No";
	#else
				return "";
	#endif
			}

			std::string canMapHostMemory() const
			{
	#if CUDART_VERSION >=3D 2020
				return deviceProp.canMapHostMemory ? "Yes" : "No";
	#else
				return "";
	#endif
			}

			std::string computeMode() const
			{
	#if CUDART_VERSION >=3D 2020
				std::ostringstream msg;
				if (deviceProp.computeMode =3D=3D cudaComputeModeDefault)
					msg << "Default (multiple host threads can use this device simultaneously)\n";

				if (deviceProp.computeMode =3D=3D cudaComputeModeExclusive)
					msg << "Exclusive (only one host thread at a time can use this device)\n";

				if (deviceProp.computeMode =3D=3D cudaComputeModeProhibited)
					msg << "Prohibited (no host thread can use this device)\n";

				return msg.str();
	#else
				return "";
	#endif
			}




			//=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=

			std::string toString() const
			{
				std::ostringstream msg;

				msg << "-------------------------------------------------- ";
				msg << name() << " Cuda Device["<< number << "] revision number ";
				msg << majorVersion() << "." << minorVersion() << "\n";
				if (deviceProp.major =3D=3D 9999 && deviceProp.minor =3D=3D 9999)
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
				msg << "  Clock rate:                                    " << (deviceProp.clockRate * 1e-6f) << " GHz\n";
				msg << "  Device Overlap:                                " << deviceOverlap() << "\n";
				msg << "  Kernel Exec Timeout Enabled:                   " << kernelExecTimeoutEnabled() << "\n";
				msg << "  Integrated:                                    " << integrated() << "\n";
				msg << "  Can Map Host Memory:                           " << canMapHostMemory() << "\n";
				msg << "  Compute Mode:                                  " << computeMode() << "\n";
				return msg.str();

			}
		}; // class Device

	//=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=


		static int getDeviceCount() {
			int result;
			cudaGetDeviceCount(&result);
			return result;
		}

		int deviceCount;
		std::vector<Device> devices;

		Cuda():
			devices(getDeviceCount())
		{
			for (size_t i=3D0; i<devices.size(); i++)
				devices[i].setTo(i);
		}

		std::string toString() {
			std::ostringstream msg;
			for (size_t i=3D0; i<devices.size(); i++)
				msg << devices[i].toString();
			return msg.str();
		}

	}; // class Cuda
} // end namespace GpusDoneRight

/*@}*/
#endif // GDR_CUDA_H_H
