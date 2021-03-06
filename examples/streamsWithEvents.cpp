#include "Device.h"
#include "Cuda.h"
#include "Context.h"
#include "Module.h"
#include "GpuFunction.h"
#include "GpuPointer.h"
#include "GpuStream.h"
#include "Random48.h" // in PsimagLite
#include "Vector.h" // in PsimagLite
#include "GpuTimer.h"

typedef GpusDoneRight::Device DeviceType;
typedef GpusDoneRight::Cuda<DeviceType> CudaType;
typedef GpusDoneRight::Context ContextType;
typedef GpusDoneRight::Module ModuleType;
typedef GpusDoneRight::GpuStream GpuStreamType;
typedef GpusDoneRight::GpuFunction<ModuleType> GpuFunctionType;
typedef GpusDoneRight::GpuTimer<> GpuTimerType;

typedef int FieldType;
typedef GpusDoneRight::GpuPointer<FieldType> GpuPointerType;
typedef GpusDoneRight::HostAllocator<FieldType,-1> HostAllocatorType;
typedef std::vector<FieldType,HostAllocatorType> HostVectorType;

std::string verifyResult(const HostVectorType& a, size_t n, int c)
{
	for (size_t i = 0; i < n; i++)
        if (a[i] != c)
		return "FAILED: a["+ ttos(i) + "]=" + ttos(a[i]) + " c=" +
				ttos(c) + "\n";
    return "PASSED";
}

int main(int argc,char *argv[])
{
	// Cuda init
	CudaType cuda;
	std::string s = cuda.toString();
	std::cout<<s<<"--------------------\n";

	bool useBlockingSync = false;
	size_t contextFlag = (useBlockingSync) ? CU_CTX_BLOCKING_SYNC : 0;
	size_t deviceNumber = 0;
	ContextType context(cuda.getDevice(deviceNumber),contextFlag);

	size_t n = 16 * 1024 * 1024;       // number of ints in the data set
 
	// Anything that is less than 4 SM's will be scaled down in terms of workload
	float multiProcCount = cuda.getAttribute(deviceNumber,CU_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT);
	float scale_factor = std::max(4.0f / multiProcCount, 1.0f);
	n = size_t (n / scale_factor);
	std::cout<<"> scale_factor = "<<(1.0f/scale_factor)<<"\n";
	std::cout<<"> array_size   = "<<n<<"\n";

	// Host memory allocation
	int c = 5;                      // value to which the array will be initialized

	HostVectorType hostA(n,255);

	// Device Memory allocation
	// allocate device memory
	GpuPointerType deviceA(n),deviceC(1);
	std::vector<FieldType> zeroVector(n,static_cast<FieldType>(0));
	deviceA.copyFromHost(zeroVector);
	//cudaMemset(deviceA, 0, nbytes); // set device memory to all 0s, for testing correctness

	// Copy of memory from host to device
	deviceC.copyFromHost(c);

	// Create streams
	size_t nStreams = 4;               // number of streams for CUDA calls
	std::vector<GpuStreamType*> streams(nStreams);

	for (size_t i = 0; i < streams.size(); i++) streams[i] = new GpuStreamType();

	// Kernel function preparation
	ModuleType module("streams.ptx");
	size_t nreps = 1;                 // number of times each experiment is repeated
	size_t nIterations = (cuda.minorVersion(0) > 1) ? 5 : 1;
	size_t threadsPerBlock = 512;
	size_t blocksPerGrid = (n + threadsPerBlock*nStreams - 1) / (nStreams * threadsPerBlock);
	
	
	for (size_t k = 0; k < nreps; k++) {
		// asynchronously launch nstreams kernels, each operating on its own portion of data
		GpuTimerType gpuTimer;
		gpuTimer.start();
		for (size_t i = 0; i < nStreams; i++) {
			GpuFunctionType initArray(module,"initArray");
			deviceA.setOffset(i*n/nStreams);
			initArray.passArguments(deviceA,deviceC,nIterations,n);
			initArray.setBlockShape(threadsPerBlock, 1, 1);
			std::cerr<<"Trying to launch kernel...\n";
			initArray.launchGridAsync(blocksPerGrid, 1,*(streams[i]));
			//initArray.launchGrid(blocksPerGrid, 1);
		}
 		gpuTimer.stop();
		std::cout<<"Elapsed time = "<<gpuTimer.elapsed()<<" for kernel for iteration "<<k<<"\n";
		
		// asynchronoously launch nstreams memcopies.  Note that memcopy in stream x will only
		//   commence executing when all previous CUDA calls in stream x have completed
		gpuTimer.start();
		for (size_t i = 0; i < nStreams; i++) {
			deviceA.setOffset(i*n/nStreams);
			//deviceA.copyToHost(a,i*n/nStreams,n/nStreams);
			deviceA.copyToHostAsync(hostA,*(streams[i]),i*n/streams.size(),n/nStreams);
		}
		gpuTimer.stop();
		std::cout<<"Elapsed time = "<<gpuTimer.elapsed()<<" for memcopy for iteration "<<k<<"\n";
	}
	

	for (size_t i = 0; i < nStreams; i++) streams[i]->synchronize();

	context.synchronize();

	std::cout<<verifyResult(hostA,n,c)<<"\n";

	for (size_t i = 0; i < streams.size(); i++) delete streams[i];
}

