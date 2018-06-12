#ifndef _P2CL_EVENTSBOX_HPP_
#define _P2CL_EVENTSBOX_HPP_

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#include <unistd.h>
#else
#include <CL/cl.h>
#endif

namespace p2cl{
	class EventsBox
	{
		public:
			EventsBox(int inputNum, int operationNum);
			void release();
			cl_event * getOneOperationEvent(int i);
			cl_event * getOneInputEvent(int i);
			cl_event * getInputEvents(cl_uint & num);
			cl_event * lastOperationEvent();
			~EventsBox();
		private:
			bool released;
			int inputNum;
			int operationNum;
			cl_event * inputEvents;
			cl_event * operationEvents;
	};
}
#endif
