//-*-C++-*-
/** \ingroup GpusDoneRight */
/*@{*/

/*! \file Image.h
 *
 * DOCUMENTATION HERE FIXME
 *
 */
#ifndef IMAGE_H_H
#define IMAGE_H_H

#include <typeinfo>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include "TypeToString.h"

#include "ApiWrapper.h"

namespace GpusDoneRight {

	template<typename T>
	struct ConverterToUByte {
		unsigned char operator()( const float& val) { return 0;}
	};

	//! Data converter from unsigned char / unsigned byte to unsigned float
	template<>
	struct ConverterToUByte<float>
	{
		//! Conversion operator
		//! @return converted value
		//! @param  val  value to convert
		unsigned char operator()( const float& val)
		{
			return static_cast<unsigned char>( val * 255.0f);
		}
	}; // struct ConverterToUByte<float>
	
	template<typename T>
	struct ConverterFromUByte {};

//! Data converter from unsigned char / unsigned byte to unsigned float
	template<>
	struct ConverterFromUByte<float>
	{
		//! Conversion operator
		//! @return converted value
		//! @param  val  value to convert
		float operator()( const unsigned char& val)
		{
			return static_cast<float>( val) / 255.0f;
		}
	};

	template<typename FieldType>
	class Image {
		static const size_t  N_CHANNELS = 1;
	public:
		typedef FieldType ValueType;

		Image(const std::string& filename,size_t width,size_t height,bool verbose = true)
		: filename_(filename),width_(width),height_(height),verbose_(verbose),data_(width*height)
		{
			readImage();
		}

		~Image()
		{
		}

		size_t width() const { return width_; }

		size_t height() const { return height_; }

		const FieldType *data() const { return &(data_[0]); }

	private:

		void readImage()
		{
			std::ifstream ifp(filename_.c_str(),std::ios::in);
			if (!ifp) {
				std::string s = "Image::ctor(): problem reading file " + filename_ + "\n";
				throw std::runtime_error(s.c_str());
			}

			// read header
			char header[100];
			ifp.getline(header,100,'\n');
			if ( (header[0]!=80) ||    /* 'P' */
      				(header[1]!=53) ) {   /* '5' */
				std::string s= "Image::ctor(): " + filename_ + " is not PPM\n";
				throw std::runtime_error(s.c_str());
			}

			ifp.getline(header,100,'\n');
			while(header[0]=='#')
				ifp.getline(header,100,'\n');
			
			char *ptr;
			size_t M=strtol(header,&ptr,0);
 			size_t N=atoi(ptr);
 			if (M!=width_ || N!=height_) throw std::runtime_error(
				"Image::ctor(): Width or Height don't match\n");
 			ifp.getline(header,100,'\n');
			//int Q=strtol(header,&ptr,0); // unused???

			unsigned char *idata = (unsigned char *) new unsigned char [N_CHANNELS*M*N];

			ifp.read( reinterpret_cast<char *>(idata), (N_CHANNELS*M*N)*sizeof(unsigned char));

			if (ifp.fail()) {
				std::string s ="Image::ctor(): " + filename_ + " has wrong size\n";
				throw std::runtime_error(s.c_str());
			}
			ifp.close();
			size_t size = width_*height_*N_CHANNELS;
			std::transform( idata, idata + size, &(data_[0]), ConverterFromUByte<FieldType>());
			delete idata;
		}

		void failureToWrite(const std::string& outputFile)
		{
			std::string s = "Image::writeToFile(...): failed for file " + outputFile + "\n";
			throw std::runtime_error(s.c_str());
		}

		std::string filename_;
		size_t width_,height_;
		bool verbose_;
		std::vector<FieldType> data_;
	}; // class Image

	template<typename FieldType>
	static void writeImageToFile(
	                             const std::string& outputFile,
	                             const std::vector<FieldType>& data,
	                             size_t width,
	                             size_t height,
	                             size_t channels = 1)
	{
		size_t size = width * height * channels;
		unsigned char *idata = (unsigned char *) new unsigned char[size];
		std::transform(&(data[0]), &(data[0]) + size, idata, ConverterToUByte<FieldType>());

		std::ofstream fh(outputFile.c_str());
		fh << "P5\n";
		std::string s = "Image::writeToFile(...): failed for file " + outputFile + "\n";
		fh << width << "\n" << height << "\n" << 0xff << "\n";
		for (size_t i = 0; i < width*height*channels; ++i) {
			if (!fh.good()) throw std::runtime_error(s.c_str());
			fh<<idata[i];
		}
		fh.flush();
		if (fh.bad()) throw std::runtime_error(s.c_str());
		fh.close();
	}

} // end namespace GpusDoneRight

/*@}*/
#endif // IMAGE_H_H
