#include "eventsbox.hpp"

namespace p2cl{
	EventsBox::EventsBox(int inputNum, int operationNum)
	{
		this->inputNum = inputNum;
		this->operationNum = operationNum;
		inputEvents = new cl_event[inputNum];
		operationEvents = new cl_event[operationNum];
		released = true;
	}

	void EventsBox::release()
	{
		if(!released)
		{
			for(int i = 0; i < inputNum; ++i)
			{
				clReleaseEvent(inputEvents[i]);
			}
			for(int i = 0; i < operationNum; ++i)
			{
				clReleaseEvent(operationEvents[i]);
			}
			released= true;
		}
	}

	EventsBox::~EventsBox()
	{
		release();
		delete[] inputEvents;
		delete[] operationEvents;
	}


	cl_event * EventsBox::getOneOperationEvent(int i)
	{
		released = false;
		return operationEvents + i;
	}

	cl_event * EventsBox::getOneInputEvent(int i)
	{
		released = false;
		return inputEvents + i;
	}

	cl_event * EventsBox::getInputEvents(cl_uint & num)
	{
		released = false;
		num = inputNum;
		return inputEvents;
	}

	cl_event * EventsBox::lastOperationEvent()
	{
		released = false;
		return operationEvents + operationNum - 1;
	}

}
