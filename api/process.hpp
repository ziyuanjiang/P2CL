	#ifndef _P2CL_PROCESS_HPP_
	#define _P2CL_PROCESS_HPP_
	#include <vector>
	#include <string>
	#include <functional>
	#include <iostream>
	#include <pthread.h>
	#include "circularindex.hpp"
	#include "analyzer.hpp"
	#include "executant.hpp"

	namespace p2cl{

		/*!
			@brief Process  interface of the library
		*/
		class Process{
			public:
				/*!
					@brief Process
					@param batchNum number of 
					@param xmlFileName file name of the xml file
				*/
				Process(int batchNum,std::string xmlFileName, 
				std::string additionalFunc = std::string(),
				std::function<void(void *, void*, void*)> reduceFunction = std::function<void(void *, void*, void*)>());
			
				/*!
					@brief push back data to the i-th in port
						it is a blocking operation
					@param size size of data that need to pushed back
					@param index index of the port
					@param ptr the pointer point to the input data
					@return number of bytes that actually writting
				*/
				int pushBack(int size, unsigned int index, void * ptr)
				{
					int remainingBytes =  size;
					int bytesWritten;
					unsigned char * p = (unsigned char *)ptr;

					if(index < numIn)
					{
						CircularIndex & cb = inputCbIndexList[index];
						while(1)
						{		
						
							pthread_mutex_lock(&mutex_out);
							if(CircularIndex::isFull(cb,executionCbStart))
							{
								pthread_cond_wait(&cond_out, &mutex_out);
							}
							pthread_mutex_unlock(&mutex_out);

							bytesWritten = 
							cb.write(executantList[cb]->getInputBuffer(index), 
							p,
							remainingBytes, (void **)&p);


							remainingBytes -= bytesWritten;
							if(remainingBytes == 0)
							{
								break;
							}

						}
						return size - remainingBytes;
					}else{
						if(numIn == 0)
						{
							pthread_mutex_lock(&mutex_out);
							if(CircularIndex::isFull(executionCbEnd, executionCbStart))
							{
								pthread_cond_wait(&cond_out, &mutex_out);
							}
							pthread_mutex_unlock(&mutex_out);

							executantList[executionCbEnd]->finishWrite();

							pthread_mutex_lock(&mutex_in);
							executionCbEnd.increment();
							pthread_cond_signal(&cond_in);
							pthread_mutex_unlock(&mutex_in);

						}
						return size;
					}
				}

				/*!
					@brief pop data from the i-th out port
						it is a blocking operation
					@param index index of the port
					@param ptr the pointer point to the input data
					@return number of bytes that is poped
				*/
				int pop(unsigned int index, void* ptr)
				{
					unsigned char * pBuffer;
					pthread_mutex_lock(&mutex_in);
					if(CircularIndex::isEmpty(executionCbEnd, executionCbStart))
					{
						pthread_cond_wait(&cond_in, &mutex_in);
					}
					pthread_mutex_unlock(&mutex_in);

					executantList[executionCbStart]->copyOutputBuffer(index, ptr);
					outputCbIndexList[index].increment();
					return outputSize(index);
				}

				/*!
					@brief get the index of an input port
					@return return the index of the port exists
						return -1 otherwise
				*/
				int getInPortIndex(std::string name) const;

				/*!
					@brief get the index of an output port
					@return return the index of the port if the port exists
					return -1 otherwise
				*/
				int getOutPortIndex(std::string name) const;

				/*!
					@brief destructor
				*/
				~Process();

				/*!
					@brief get the number of input ports
				*/
				int numInput() const;
				
				/*!
					@brief get the number of output ports
				*/
				int numOutput() const;

				/*!
					@brief get the size of the i-th input port
				*/
				size_t inputSize(int i) const;

				/*!
				@brief get the size of the i-th output port
			*/
			size_t outputSize(int i) const;

		private:
			pthread_mutex_t mutex_in;///mutex for check the input CircularIndex

			pthread_mutex_t mutex_out;///mutex for check the output CircularIndex

			pthread_mutex_t mutex_ready_in;///mutex for the readyInputNumList

			pthread_mutex_t mutex_ready_out;///mutex for the takenOutputNumList

			pthread_cond_t cond_in;///condition that signals inputs ready

			pthread_cond_t cond_out;///condition that signals outputs ready

			int batchNum;///number of Executant object, it also indicate the size of circular buffer
			int numIn;///number of in ports
			int numOut;///number of out ports

			void updatePortList(std::vector<Port> list);///update port lists

			std::string addLineNum(std::string)const;///add line numbers in front of each line of the kernel source code

			void oneOutputRead(int index);///callback function that is invocated when one output is read

			void oneInputWritten(int index);///callback function that is invocated when one input is written

			std::vector<Port> inputPortList;///list of information for the input ports

			std::vector<Port> outputPortList;///list of information for the output ports

			std::vector<CircularIndex> inputCbIndexList;///list of CircularIndexes for each input port

			std::vector<CircularIndex> outputCbIndexList;///list of CircularIndexes for each output port

			CircularIndex executionCbStart;///CircularIndex for execution start

			CircularIndex executionCbEnd;///CircularIndex for execution end

			std::vector<Executant*>	executantList;///list of executant

			std::vector<int> readyInputNumList;///list stores the number of input ports that is ready for each Executant

			std::vector<int> takenOutputNumList;///list stores the number of ouput ports whose data is already taken for each Executant 

			cl_context createContext(cl_device_id & device_id) const;
	};
}
#endif
