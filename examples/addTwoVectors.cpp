#include "Device.h"
#include "Cuda.h"
#include "Context.h"
#include "Module.h"
#include "Memory.h"
#include "Random48.h" // in PsimagLite
#include "Vector.h" // in PsimagLite

typedef GpusDoneRight::Device DeviceType;
typedef GpusDoneRight::Cuda<DeviceType> CudaType;
typedef GpusDoneRight::Context ContextType;
typedef GpusDoneRight::Module ModuleType;
typedef GpusDoneRight::Memory MemoryType;
typedef float RealType;

int main(int argc,char *argv[])
{
	// Cuda init and module load
	CudaType cuda;
	std::string s = cuda.toString();
	std::cout<<s;
	std::cout<<"--------------------\n";
	ContextType context(cuda.getHandle(0));
	ModuleType module("addTwoVectors.ptx");
	CUfunction *hfunc;
	module.getFunction(hfunc,"addTwoVectors");

	// Host memory allocation
	size_t n = 10000;
	std::vector<RealType> hostA(n),hostB(n);
	PsimagLite::Random48<RealType> random48;
	random48.seed(33483);
	PsimagLite::randomizeVector(hostA,0.0,1.0,random48);
	PsimagLite::randomizeVector(hostB,0.0,1.0,random48);

	// Device memory allocation
	MemoryType memory;
	unsigned int nDevice = n * sizeof(RealType);
	CUdeviceptr deviceA;
	memory.alloc(&deviceA,nDevice);
	CUdeviceptr deviceB;
	memory.alloc(&deviceB,nDevice);
	CUdeviceptr deviceC;
	memory.alloc(&deviceC,nDevice);
	
	// Copy of memory from host to device
	memory.cpyHtoD(deviceA, &(hostA[0]), nDevice);
	memory.cpyHtoD(deviceB, &(hostB[0]), nDevice);
	
	// Kernel invocation
	
	// Copy of memory from device to host
	std::vector<RealType> hostC(n);
	memory.cpyDtoH(&(hostC[0]),deviceC,nDevice);

}
