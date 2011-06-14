#include "Device.h"
#include "Cuda.h"
#include "Context.h"
#include "Module.h"
#include "GpuFunction.h"
#include "GpuPointer.h"
#include "Image.h"
#include "Texture.h"

typedef GpusDoneRight::Device DeviceType;
typedef GpusDoneRight::Cuda<DeviceType> CudaType;
typedef GpusDoneRight::Context ContextType;
typedef GpusDoneRight::Module ModuleType;
typedef GpusDoneRight::GpuFunction<ModuleType> GpuFunctionType;

typedef float FieldType;
typedef GpusDoneRight::GpuPointer<FieldType> GpuPointerType;
typedef GpusDoneRight::Image<FieldType> ImageType;
typedef GpusDoneRight::Texture<FieldType> TextureType;

int main(int argc, char *argv[])
{
	CudaType cuda;

	size_t deviceNumber = 0;
	ContextType context(cuda.getDevice(deviceNumber));

	// Load image
	if (argc<4) throw std::runtime_error(
		"Need as argument, first: the image rootname (without the .pgm extension) "
		"second: the width, and third: the height\n"
		);
	std::string rootname(argv[1]);
	std::string filenameInput = rootname + ".pgm";
	size_t width = atoi(argv[2]);
	size_t height = atoi(argv[3]);
	FieldType theta = 0.5;    // angle to rotate image by (in radians)
	ImageType inputImage(filenameInput,width,height);

	// Allocate device memory for result:
	GpuPointerType dData(width*height);

	/* Call the GPU kernel */ 
	ModuleType module("transformKernel.ptx");
	
	// Set texture parameters
	TextureType texture(module,"tex",inputImage);

	GpuFunctionType transformKernel(module,"transformKernel");
	transformKernel.passArguments(dData,width,height,theta);
	// Not really passed, in the sense that offset
	// isn't affected, and it's not in the argument list
	// CUDA **is** ugly:
	transformKernel.passTexture(texture);

	size_t threadsPerBlock = 8;
	transformKernel.setBlockShape(threadsPerBlock, threadsPerBlock,1);
	
	// Kernel invocation: warmup
	transformKernel.launchGrid(width / threadsPerBlock, height / threadsPerBlock);

	//Synchronize: only needed for timer??
  	context.synchronize();

	// Kernel invocation: execution
	transformKernel.launchGrid(width / threadsPerBlock, height / threadsPerBlock);

	//Synchronize: this is important!!
  	context.synchronize();

	// allocate mem for the result on host side
	std::vector<FieldType> result(width*height);
	dData.copyToHost(result);

	// write result to file
	ImageType outputImage(result,width,height);
	std::string filenameOutput = rootname + "_out.pgm";
	outputImage.writeToFile(filenameOutput);
}
