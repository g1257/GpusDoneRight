#include "Device.h"
#include "Cuda.h"
#include "Context.h"
#include "Module.h"
#include "GpuFunction.h"
#include "GpuPointer.h"
#include "Random48.h" // in PsimagLite
#include "Vector.h" // in PsimagLite

typedef GpusDoneRight::Device DeviceType;
typedef GpusDoneRight::Cuda<DeviceType> CudaType;
typedef GpusDoneRight::Context ContextType;
typedef GpusDoneRight::Module ModuleType;
typedef GpusDoneRight::GpuFunction<ModuleType> GpuFunctionType;

typedef float RealType;
typedef GpusDoneRight::GpuPointer<RealType> GpuPointerType;

std::string verifyResult(
			const std::vector<RealType>& hostA,
			const std::vector<RealType>& hostB,
			const std::vector<RealType>& hostC)
{
	size_t i = 0;
	for (i = 0; i <hostA.size(); ++i) {
        RealType sum = hostA[i] + hostB[i];
        if (fabs(hostC[i] - sum) > 1e-7f)
            break;
    }
     return (i == hostA.size()) ? "PASSED" : "FAILED";
}
    
int main(int argc,char *argv[])
{
	// Cuda init and module load
	CudaType cuda;
	std::string s = cuda.toString();
	std::cout<<s<<"--------------------\n";
	
	ContextType context(cuda.getDevice(0));
	
	// Host memory allocation
	size_t n = 10000;
	std::vector<RealType> hostA(n),hostB(n);
	PsimagLite::Random48<RealType> random48;
	random48.seed(33483);
	PsimagLite::randomizeVector(hostA,0.0,1.0,random48);
	PsimagLite::randomizeVector(hostB,0.0,1.0,random48);

	// Device memory allocation
	GpuPointerType deviceA(n),deviceB(n),deviceC(n);
	
	// Copy of memory from host to device
	deviceA = hostA;
	deviceB = hostB;
	
	// Kernel function preparation
	ModuleType module("addTwoVectors.ptx");

	GpuFunctionType addTwoVectors(module,"addTwoVectors");
	addTwoVectors.passArguments(deviceA,deviceB,deviceC,n);
	size_t threadsPerBlock = 256;
	addTwoVectors.setBlockShape(threadsPerBlock, 1, 1);
	
	// Kernel invocation
	size_t blocksPerGrid = (n + threadsPerBlock - 1) / threadsPerBlock;
	addTwoVectors.launchGrid(blocksPerGrid, 1);
	
	// Copy of memory from device to host
	std::vector<RealType> hostC(n);
	deviceC.copyToHost(hostC);
	
	std::cout<<verifyResult(hostA,hostB,hostC)<<"\n";
}
