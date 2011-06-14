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

	template<typename FieldType>
	class Image {
		static const size_t  N_CHANNELS = 3;
	public:
		typedef FieldType ValueType;

		Image(const std::string& filename,size_t width,size_t height,bool verbose = true)
		: filename_(filename),width_(width),height_(height),verbose_(verbose),data_(0)
		{
			readImage();
		}

		Image(const std::vector<FieldType>& data,size_t width,size_t height,bool verbose = true)
		: filename_("UNDEFINED"),width_(width),height_(height),verbose_(verbose),data_(0)
		{
			size_t size = width_ * height_ * N_CHANNELS;
			data_ = (unsigned char *) new unsigned char[size];
			std::transform(&(data[0]), &(data[0]) + size, data_, ConverterToUByte<FieldType>());
		}

		~Image()
		{
			if (data_) delete [] data_;
		}

		size_t width() const { return width_; }

		size_t height() const { return height_; }

		const unsigned char *data() const { return data_; }

		void writeToFile(const std::string& outputFile)
		{
			std::ofstream fh(outputFile.c_str());
			fh << "P6\n";
			
			fh << width_ << "\n" << height_ << "\n" << 0xff << "\n";
			for (size_t i = 0; i < width_*height_*N_CHANNELS; ++i) {
				if (!fh.good()) failureToWrite(outputFile);
				fh<<data_[i];
			}
			fh.flush();
			if (fh.bad()) failureToWrite(outputFile);
			fh.close();
		}

	private:

		void readImage()
		{
			std::ifstream ifp(filename_.c_str(),std::ios::in);
			if (!ifp || ifp.bad() || !ifp.good()) {
				std::string s = "Image::ctor(): problem reading file " + filename_ + "\n";
				throw std::runtime_error(s.c_str());
			}

			// read header
			char header[100];
			ifp.getline(header,100,'\n');
			if ( (header[0]!=80) ||    /* 'P' */
      				(header[1]!=54) ) {   /* '6' */
				std::string s= "Image::ctor(): " + filename_ + " is not PPM\n";
				throw std::runtime_error(s.c_str());
			}

			ifp.getline(header,100,'\n');
			while(header[0]=='#')
				ifp.getline(header,100,'\n');
			
			char *ptr;
			int M=strtol(header,&ptr,0);
 			int N=atoi(ptr);
 
 			ifp.getline(header,100,'\n');
			//int Q=strtol(header,&ptr,0); // unused???

			data_ = (unsigned char *) new unsigned char [N_CHANNELS*M*N];

			ifp.read( reinterpret_cast<char *>(data_), (N_CHANNELS*M*N)*sizeof(unsigned char));

			if (ifp.fail()) {
				std::string s ="Image::ctor(): " + filename_ + " has wrong size\n";
				throw std::runtime_error(s.c_str());
			}

			ifp.close();
		}

		void failureToWrite(const std::string& outputFile)
		{
			std::string s = "Image::writeToFile(...): failed for file " + outputFile + "\n";
			throw std::runtime_error(s.c_str());
		}

		std::string filename_;
		size_t width_,height_;
		bool verbose_;
		unsigned char *data_;
	}; // class Image
} // end namespace GpusDoneRight

/*@}*/
#endif // IMAGE_H_H
