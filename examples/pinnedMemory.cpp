#include "Device.h"
#include "Cuda.h"
#include "Context.h"
#include "Module.h"
#include "GpuFunction.h"
#include "GpuPointer.h"
#include "GpuStream.h"
#include "Random48.h" // in PsimagLite
#include "Vector.h" // in PsimagLite
#include "Random48.h" // in PsimagLite

typedef GpusDoneRight::Device DeviceType;
typedef GpusDoneRight::Cuda<DeviceType> CudaType;
typedef GpusDoneRight::Context ContextType;
typedef GpusDoneRight::Module ModuleType;
typedef GpusDoneRight::GpuStream GpuStreamType;
typedef GpusDoneRight::GpuFunction<ModuleType> GpuFunctionType;

typedef float FieldType;
typedef PsimagLite::Random48<FieldType> Random48Type;
typedef GpusDoneRight::GpuPointer<FieldType> GpuPointerType;
// typedef GpusDoneRight::HostAllocator<FieldType> HostAllocatorType;
// typedef std::vector<FieldType,HostAllocatorType> HostVectorType;

typedef GpusDoneRight::HostDeviceMappedAllocator<FieldType> HostDeviceMappedAllocatorType;
typedef std::vector<FieldType,HostDeviceMappedAllocatorType> HostDeviceMappedVectorType;

std::string verifyResults/*(const FieldType* hostA, 
                          const FieldType* hostB,
                          const FieldType* hostC,
                          size_t nelem) */
(const HostDeviceMappedVectorType& hostA,const HostDeviceMappedVectorType& hostB,const HostDeviceMappedVectorType& hostC)
{
	printf("Checking the results...\n");
	FieldType errorNorm = 0, refNorm = 0;
	for(size_t i = 0; i < hostA.size(); i++) {
		FieldType ref = hostA[i] + hostB[i];
		FieldType diff = hostC[i] - ref;
		errorNorm += diff*diff;
		refNorm += ref*ref;
	}
	std::cerr<<"errorNorm="<<errorNorm<<" refNorm="<<refNorm<<"\n";
	errorNorm = (FieldType)sqrt((FieldType)errorNorm);
	refNorm = (FieldType)sqrt((FieldType)refNorm);
	return (errorNorm/refNorm < 1.e-6f) ? "PASSED" : "FAILED";
}

int main(int argc, char *argv[])
{
	CudaType cuda;

	size_t deviceNumber = 0;
	ContextType context(cuda.getDevice(deviceNumber),CU_CTX_SCHED_AUTO | CU_CTX_MAP_HOST); // cutilSafeCall(cudaSetDeviceFlags(cudaDeviceMapHost));

	/* Verify the selected device supports mapped memory and set the device
		flags for mapping host memory. */
	if (!cuda.getAttribute(deviceNumber,CU_DEVICE_ATTRIBUTE_CAN_MAP_HOST_MEMORY)) {
		std::cerr<<"Device "<<deviceNumber<<" cannot map host memory!\n";
		return 1;
	}

	size_t nelem = 1048576;
	HostDeviceMappedVectorType hostA(nelem);
	HostDeviceMappedVectorType hostB(nelem);
	HostDeviceMappedVectorType hostC(nelem);

	/* Initialize the vectors. */
	Random48Type::seed(349837);
	for (size_t i = 0; i < nelem; i++) {
		hostA[i] = static_cast<FieldType>(Random48Type::random());
		hostB[i] = static_cast<FieldType>(Random48Type::random()); 
	}
	
	
	/* Get the device pointers for the pinned CPU memory mapped into the GPU
		memory space. */
	GpuPointerType deviceAmem(hostA);
	GpuPointerType deviceBmem(hostB);
	GpuPointerType deviceCmem(hostC);

	/* Call the GPU kernel using the device pointers for the mapped memory. */ 
	std::cout<<"Adding vectors using mapped CPU memory...\n";
	ModuleType module("addTwoVectors.ptx");

	GpuFunctionType addTwoVectors(module,"addTwoVectors");
	addTwoVectors.passArguments(deviceAmem,deviceBmem,deviceCmem,nelem);
	size_t threadsPerBlock = 256;
	//size_t gridx = (unsigned int)ceil(nelem/(float)threadsPerBlock);
	addTwoVectors.setBlockShape(threadsPerBlock, 1,1);
	
	// Kernel invocation
	size_t blocksPerGrid = (nelem + threadsPerBlock - 1) / threadsPerBlock;
	addTwoVectors.launchGrid(blocksPerGrid, 1);

	//Synchronize is very important!!!
  	context.synchronize();

	std::cout<<verifyResults(hostA,hostB,hostC)<<"\n";
}
