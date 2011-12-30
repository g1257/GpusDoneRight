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
typedef GpusDoneRight::CudaArray CudaArrayType;

typedef float FieldType;
typedef GpusDoneRight::GpuPointer<FieldType> GpuPointerType;
typedef GpusDoneRight::Image<FieldType> ImageType;
typedef GpusDoneRight::Texture<FieldType> TextureType;

std::string verifyResult(const char *f1,const char* f2)
{
	std::ifstream if1(f1),if2(f2);
	char c1,c2;
	size_t counter = 0;
	while(!if1.eof() && !if2.eof()) {
		if1.get(c1);
		if2.get(c2);
		if (c1!=c2) counter++;
	}
	if1.close();
	if2.close();
	std::cout<<"Differences="<<counter<<"\n";
	return (counter<=8) ? "PASSED" : "FAILED";
}

int main(int argc, char *argv[])
{
	const char refImage[] = "ref_rotated.pgm";
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
	
	// Set array for texture
	CudaArrayType array(inputImage);

	// Allocate device memory for result:
	GpuPointerType dData(width*height);

	/* Call the GPU kernel */ 
	ModuleType module("texture.ptx");
	
	// Set texture parameters
	TextureType texture(module,"tex",array);

	GpuFunctionType transformKernel(module,"transformKernel");
	transformKernel.passArguments(dData,width,height,theta);
	// Not really passed, in the sense that offset
	// isn't affected, and it's not in the argument list
	// CUDA **is** ugly:
	transformKernel.passTexture(texture);

	size_t threadsPerBlock = 4;
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
	std::string filenameOutput = rootname + "_out.pgm";
	GpusDoneRight::writeImageToFile(filenameOutput,result,width,height);
	std::cout<<verifyResult(filenameOutput.c_str(),refImage)<<"\n";
}

