//-*-C++-*-
/** \ingroup GpusDoneRight */
/*@{*/

/*! \file GpuTimer.h
 *
 * DOCUMENTATION HERE FIXME
 *
 */
#ifndef GPU_TIMER_H_H
#define GPU_TIMER_H_H

#include <typeinfo>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include "TypeToString.h"

#include "ApiWrapper.h"

namespace GpusDoneRight {

	template<typename RealType = float>
	class GpuTimer {

	public:
		/* possible flags:
		* CU_EVENT_DEFAULT: Default event creation flag
		* CU_EVENT_BLOCKING_SYNC: Specifies that event should use blocking synchronization
		*/
		GpuTimer(unsigned int Flags = CU_EVENT_DEFAULT,bool verbose = true) : verbose_(verbose)
		{
			CUresult error = cuEventCreate(&eStart_,Flags);
    			ApiWrapper::check("cuEventCreate",error,verbose_);

			error = cuEventCreate(&eStop_,Flags);
    			ApiWrapper::check("cuEventCreate",error,verbose_);
		}
		
		// Using RAII here:
		~GpuTimer()
		{
			CUresult error = cuEventDestroy(eStop_);
			// Let's not throw in the destructor:
			ApiWrapper::check("cuEventDestroy",error,verbose_,ApiWrapper::DO_NOT_THROW);
			
			error = cuEventDestroy(eStart_);
			// Let's not throw in the destructor:
			ApiWrapper::check("cuEventDestroy",error,verbose_,ApiWrapper::DO_NOT_THROW);
		}
		
		RealType elapsed() const
		{
			RealType x;
			CUresult error = cuEventElapsedTime(&x,eStart_,eStop_);
			ApiWrapper::check("cuEventElapsedTime",error,verbose_);
			return x;
		}

		void start()
		{
			CUresult error = cuEventRecord(eStart_,0);
			ApiWrapper::check("cuEventRecord",error,verbose_);
		}

		void stop()
		{
			CUresult error = cuEventRecord(eStop_,0);
			ApiWrapper::check("cuEventRecord",error,verbose_);
			error = cuEventSynchronize(eStop_);
			ApiWrapper::check("cuEventSynchronize",error,verbose_);
		}
 	
	private:
		bool verbose_;
		CUevent eStart_,eStop_; // we own this
	}; // class GpuTimer
} // end namespace GpusDoneRight

/*@}*/
#endif // GPU_TIMER_H_H
