#include "Device.h"
#include "Cuda.h"
#include "Context.h"
#include "Module.h"

typedef GpusDoneRight::Device DeviceType;
typedef GpusDoneRight::Cuda<DeviceType> CudaType;
typedef GpusDoneRight::Context ContextType;
typedef GpusDoneRight::Module ModuleType;

int main(int argc,char *argv[])
{
	CudaType cuda;
	std::string s = cuda.toString();
	std::cout<<s;
	std::cout<<"--------------------\n";
	ContextType context(cuda.getHandle(0));
	ModuleType module("addTwoVectors.ptx");
	CUfunction *hfunc;
	module.getFunction(hfunc,"addTwoVectors");
	
}
