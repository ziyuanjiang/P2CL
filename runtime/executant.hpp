#ifndef _P2CL_EXECUTANT_HPP_
#define _P2CL_EXECUTANT_HPP_

#include <vector>
#include <map>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#include <unistd.h>
#else
#include <CL/cl.h>
#endif

#include <functional>
#include "eventsbox.hpp"
#include "operation.hpp"

namespace p2cl{
	class Executant{
		public:
			typedef struct{
				SKType type;
				std::string kernelName;
				int numInput;
				int numOutput;

				int dim0GlobalSize;
				int dim0LocalSize;
				int dim1GlobalSize;
				
				std::vector<int> dependList;
				std::vector<size_t> localMemSize;
				
				cl_kernel ko;

				int reduceCpuLength;
				//operation info
				//that is used
				//to generate 
				//execution infomation above
				int reduceLength;
				int transposeWidth;
				int transposeHeight;
				int transposeLocalWidth;
				int transposeLocalHeight;
				int resultVarSize;
				std::vector<size_t>baseTypeSizeList;
				int tupleLength;
				int maxWorkGroupSize;
				int preferredGroupSizeMul;
				
			}ExecutionInfo;

			Executant(std::vector<size_t> bufferMaxSizeList);

			Executant(const Executant & obj);

			Executant *clone()
			{
				return new Executant(*this);
			}

			~Executant();

			void appendReduceOp(std::string kernelName, std::function<void(void *, void *, void *)>,int numInput, int length, int resultVarSize, std::vector<int> dependList = std::vector<int>(), int extNum = 1);

			void appendMapOp(std::string kernelName, int numInput, int numOutput, int length, std::vector<int> dependList = std::vector<int>(), int extNum = 1);

			void appendGatherOp(std::string kernelName, int numInput, int length, int tupleLength, std::vector<int> dependList = std::vector<int>(), int extNum = 1);

			void appendScatterOp(std::string kernelName, int numInput, int length, int tupleLength, std::vector<int> dependList = std::vector<int>(), int extNum = 1);
			void appendTransposeOp(std::string kernelName, int numInput, int width, int height, std::vector<size_t>baseTypeSizeList, int tupleLength, int extNum = 1);

		
			void clearInputOutput();

			int appendInputSize(size_t size);

			int appendOutputSize(size_t size);

			void clearPlan();

			void setContextAndProgram(cl_context, cl_device_id , cl_command_queue, cl_program);

			void setReduceFunction(std::function<void(void*, void*, void*)> func);

			bool copyOutputBuffer(int i, void * ptr);

			void preparation();

			void * getInputBuffer(int i);

			void finishWrite();

			void * getOutputBuffer(int i);

			void finishRead();

			int tempraryExtensionNum;
		private:

			bool reduceInThePlan;

			std::function<void(void *, void*, void*)>reduceFunction;

			unsigned char * pReduceResult;

			cl_ulong localMemSize;

			bool ioSizeAlreadySet()
			{
				return !outputSizeList.empty();
			}
				
			EventsBox * box;

			std::vector<size_t> bufferSizeList;

			std::vector<size_t> inputSizeList;

			std::vector<void *> inputPtrList;

			std::vector<size_t> outputSizeList;

			std::vector<void *> outputPtrList;
			///////////////////////////////

			/////////////////////
			bool contextAlreadySet;
			//////////1//////////
			cl_context context;
			
			cl_device_id device_id;

			cl_command_queue commands;

			cl_program program;

			std::vector<cl_mem> bufferList0;

			std::vector<cl_mem> bufferList1;

			/////////////////////
			bool kernelAlreadySet;//-----------------------
			//////////2//////////			      |
			std::map<std::string, cl_kernel> koTable;//<--|
								    //|
			///////////////////////////////		      |
								    //|
			std::vector<ExecutionInfo> planList;//<-------|

			int operationNum;

			std::vector<std::string> kernelNameList;

			void clearContext();
			void clearKernel();
			void appendKernelName(std::string kernelName);
	};
}

#endif
