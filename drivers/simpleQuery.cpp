#include "Device.h"
#include "Cuda.h"

typedef GpusDoneRight::Device DeviceType;
typedef GpusDoneRight::Cuda<DeviceType> CudaType;

int main(int argc,char *argv[])
{
	CudaType cuda;
	std::string s = cuda.toString();
	std::cout<<s;
}

