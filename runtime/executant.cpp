#ifdef __APPLE__
#include <OpenCL/opencl.h>
#include <unistd.h>
#else
#include <CL/cl.h>
#endif

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include "err_code.hpp"
#include "operation.hpp"
#include "executant.hpp"
#include "exception.hpp"

namespace p2cl{

	void emptyReduceFunction(void *, void* , void *)
	{
		throw Exception("ExecutionError", "no reduce function set by host program.");
	}

	Executant::Executant(std::vector<size_t> bufferSizeList)
	{
		contextAlreadySet = false;
		kernelAlreadySet == false;
		this->bufferSizeList = bufferSizeList;
		tempraryExtensionNum = 1;
		pReduceResult = NULL;
		reduceInThePlan = false;
		reduceFunction = emptyReduceFunction;
	}

	Executant::Executant(const Executant & obj)
	{
		cl_int err;
		inputSizeList = obj.inputSizeList;
		outputSizeList = obj.outputSizeList;
		bufferSizeList = obj.bufferSizeList;
		planList = obj.planList;
		kernelNameList = obj.kernelNameList;
		contextAlreadySet = false;
		kernelAlreadySet = false;
		pReduceResult = NULL;
		if(obj.contextAlreadySet)
		{
			setContextAndProgram(obj.context, obj.device_id, obj.commands, obj.program);
		}

		if(obj.kernelAlreadySet)
		{
			preparation();
		}
		reduceInThePlan = obj.reduceInThePlan;
		if(obj.reduceInThePlan)
		{ 
			reduceFunction= obj.reduceFunction;
			pReduceResult = new unsigned char[planList.back().resultVarSize];
		}
	}

	void Executant::clearInputOutput()
	{
		inputSizeList.clear();
		outputSizeList.clear();
	}

	void Executant::setReduceFunction(std::function<void(void*, void*, void*)> func)
	{
		if(reduceInThePlan)
			reduceFunction = func;
	}

	int Executant::appendInputSize(size_t size)
	{
		int listLength = inputSizeList.size();
		if(bufferSizeList.size() <= listLength)
		{
			throw Exception("ExecutionError", 
			std::string("append more input sizes than buffer sizes."));

		}

		if(bufferSizeList[listLength] < size)
		{
			throw Exception("ExecutionError", 
			std::string("Size for input list size")
			+" cannot exceeds buffer list max size.");
		}

		inputSizeList.push_back(size);
		return listLength;
	}

	int Executant::appendOutputSize(size_t size)
	{
		int listLength = outputSizeList.size();
		if(bufferSizeList.size() <= listLength)
		{
			throw Exception("ExecutionError", 
			std::string("append more output sizes than buffer sizes."));

		}

		if(bufferSizeList[listLength] < size)
		{
			throw Exception("ExecutionError", 
			std::string("Size for output list size")
			+" cannot exceeds buffer list max size.");
		}

		outputSizeList.push_back(size);
		return listLength;
	}

	void Executant::clearContext()
	{
		if(contextAlreadySet)	
		{
			clReleaseCommandQueue(commands);
			clReleaseContext(context);
			clReleaseProgram(program);
			for(int i = 0; i < bufferList0.size(); ++i)
			{
				clReleaseMemObject(bufferList0[i]);
				clReleaseMemObject(bufferList1[i]);
			}
			bufferList0.clear();
			bufferList1.clear();
		}
		contextAlreadySet = false;
	}

	void Executant::clearKernel()
	{
		if(kernelAlreadySet)
		{
			std::map<std::string, cl_kernel>::iterator it;
			for(it=koTable.begin(); it != koTable.end(); ++it)
			{
			    clReleaseKernel(it->second);
			}
			koTable.clear();
			delete box;
		}
		kernelAlreadySet = false;
	}

	Executant::~Executant()
	{
		clearContext();
		clearKernel();
		delete[] pReduceResult;
	}

	void Executant::appendKernelName(std::string kernelName)
	{
		std::vector<std::string>::iterator it;
		for(it = kernelNameList.begin(); it != kernelNameList.end(); ++it)
		{
			if((*it).compare(kernelName) == 0)
				break;
		}
		//only push_back new kernel name
		if(it == kernelNameList.end())
		{
			kernelNameList.push_back(kernelName);
		}
	}

	void Executant::appendReduceOp(std::string kernelName, 
	std::function<void(void *, void *, void *)> reduceFunc,
	int numInput, int length, int resultVarSize, std::vector<int> dependList, int extNum)
	{
		ExecutionInfo infoTmp;
		infoTmp.type = SKType::reduce;
		infoTmp.kernelName = kernelName;
		appendKernelName(kernelName);
		infoTmp.numInput = numInput;
		infoTmp.numOutput = 1;
		//dim0GlobalSize and dim0LocalSize	
		//is determined later in preparation
		//function
		infoTmp.dim1GlobalSize = extNum;

		infoTmp.dependList = dependList;
		//localMemSize is determined later

		infoTmp.reduceLength = length;
		infoTmp.resultVarSize = resultVarSize;


		planList.push_back(infoTmp);

		kernelAlreadySet = false;
		reduceFunction = reduceFunc;
		pReduceResult = new unsigned char[resultVarSize];
		reduceInThePlan = true;
	}

	void Executant::appendMapOp(std::string kernelName, int numInput, int numOutput, int length, std::vector<int> dependList, int extNum )
	{
		ExecutionInfo infoTmp;
		infoTmp.type = SKType::map;
		infoTmp.kernelName = kernelName;
		appendKernelName(kernelName);
		infoTmp.numInput = numInput;
		infoTmp.numOutput = numOutput;
		infoTmp.dim0GlobalSize = length;
		infoTmp.dim0LocalSize = 1;
		infoTmp.dim1GlobalSize = extNum;

		infoTmp.dependList = dependList;

		if(!reduceInThePlan)
			planList.push_back(infoTmp);

		kernelAlreadySet = false;

	}

	void Executant::appendGatherOp(std::string kernelName, int numInput, int length, int tupleLength, std::vector<int> dependList, int extNum)
	{
		ExecutionInfo infoTmp;
		infoTmp.type = SKType::gather;
		infoTmp.kernelName = kernelName;
		appendKernelName(kernelName);
		infoTmp.numInput = numInput;
		infoTmp.numOutput = numInput;
		infoTmp.dim0GlobalSize = length * tupleLength;
		infoTmp.dim0LocalSize = tupleLength;
		infoTmp.dim1GlobalSize = extNum;

		infoTmp.dependList = dependList;

		if(!reduceInThePlan)
			planList.push_back(infoTmp);

		kernelAlreadySet = false;

	}

	void Executant::appendScatterOp(std::string kernelName, int numInput, int length, int tupleLength, std::vector<int> dependList, int extNum)
	{
		ExecutionInfo infoTmp;
		infoTmp.type = SKType::scatter;
		infoTmp.kernelName = kernelName;
		appendKernelName(kernelName);
		infoTmp.numInput = numInput;
		infoTmp.numOutput = numInput;
		infoTmp.dim0GlobalSize = length * tupleLength;
		infoTmp.dim0LocalSize = tupleLength;
		infoTmp.dim1GlobalSize = extNum;

		infoTmp.dependList = dependList;

		infoTmp.tupleLength = tupleLength;

		if(!reduceInThePlan)
			planList.push_back(infoTmp);

		kernelAlreadySet = false;
	}


	void Executant::appendTransposeOp(std::string kernelName, int numInput, int width, int height, std::vector<size_t> baseTypeSizeList, int tupleLength, int extNum)
	{
		ExecutionInfo infoTmp;
		infoTmp.type = SKType::transpose;
		infoTmp.kernelName = kernelName;
		appendKernelName(kernelName);
		infoTmp.numInput = numInput;
		infoTmp.numOutput = numInput;

		//dim0GlobalSize and dim0LocalSize	
		//is determined later in preparation
		//function
		infoTmp.dim1GlobalSize = extNum;

		//infoTmp.dependList = dependList;
		//localMemSize is determined later

		infoTmp.tupleLength = tupleLength;
		infoTmp.baseTypeSizeList = baseTypeSizeList;
		infoTmp.transposeWidth = width;
		infoTmp.transposeHeight = height;

		if(!reduceInThePlan)
			planList.push_back(infoTmp);

		kernelAlreadySet = false;
	}

	void Executant::preparation()
	{
		cl_kernel koTmp;
		cl_int err;
		if(!contextAlreadySet)
		{
			throw Exception("ExecutionError", "cannot perform preparation before assigning context and program");
		}

		if(!ioSizeAlreadySet())
		{
			throw Exception("ExecutionError", "cannot perform preparation before append input and output size");
		}

		if(kernelAlreadySet)
		{
			clearKernel();
		}

		inputPtrList.clear();
		outputPtrList.clear();
		inputPtrList.resize(inputSizeList.size(), NULL);
		outputPtrList.resize(outputSizeList.size(), NULL);

		for(std::vector<std::string>::iterator it = kernelNameList.begin();
			it != kernelNameList.end(); ++it)
		{
			koTmp = clCreateKernel(program, (*it).c_str(), &err);
			if(err != CL_SUCCESS)
			{
				throw Exception("ExcutionError",
					std::string("cannot find kernel") + (*it)
					+" in the given program");
			}
			koTable[*it] = koTmp;

		}

		operationNum = planList.size();

		box = new EventsBox(inputSizeList.size(), operationNum);
		for(std::vector<ExecutionInfo>::iterator it = planList.begin();
			it != planList.end(); ++it)
		{
			size_t work_group_size;
			size_t preferredGroupSizeMul;
			(*it).ko = koTable[it->kernelName];

			//get work-group-size for the kernel
			err = clGetKernelWorkGroupInfo(it->ko, device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &work_group_size, NULL);

			if(err != CL_SUCCESS)
			{
				throw Exception("ExcutionError",
				std::string("fail to query work group size for ")
				+ (*it).kernelName);
			}

			(*it).maxWorkGroupSize = work_group_size;
			it->localMemSize.clear();
			

			//get work-group-size for the kernel
			err = clGetKernelWorkGroupInfo(it->ko, device_id, CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, sizeof(size_t), &preferredGroupSizeMul, NULL);

			if(err != CL_SUCCESS)
			{
				throw Exception("ExcutionError",
				std::string("fail to query preferred work group size multiple for ")
				+ (*it).kernelName);
			}
			(*it).preferredGroupSizeMul = preferredGroupSizeMul;

			if(it->type == SKType::reduce)
			{
				int numWG;
				if(it->reduceLength < it->maxWorkGroupSize)
				{
					numWG = 1;
					it->dim0LocalSize = (int)std::pow(2, std::floor(std::log(it->reduceLength)/log(2)));
					it->dim0GlobalSize = it->dim0LocalSize;
				}else{
					it->dim0LocalSize = it->maxWorkGroupSize;
					numWG = std::floor(
					(float)it->reduceLength/it->dim0LocalSize);
					if(numWG > 32)
					{
						numWG = 32;
					}
					it->dim0GlobalSize = numWG * it->dim0LocalSize;
				}
				it->reduceCpuLength = numWG;
				it->localMemSize.push_back(it->dim0LocalSize * it->resultVarSize);
			}else if(it->type == SKType::transpose)
			{
				//transposeWidth
				//transposeHeight
				//tupleLength
				//baseTypeSize
				int maxWorkGroupSize = it->maxWorkGroupSize;
				int groupDimX;
				int groupDimY;
				int nextValue;

				int xTry = std::floor(
					std::sqrt(maxWorkGroupSize / it->tupleLength)
					);

				if(xTry < 1)
				{
					xTry = 1;
				}

				groupDimX = it->transposeWidth;
				groupDimY = it->transposeHeight;

				if(groupDimX > xTry)
				{
					if(groupDimY > xTry)
					{
						groupDimX = xTry;
						groupDimY = xTry;
					}else{
						groupDimX = xTry;
						nextValue = xTry;

						while(nextValue * groupDimY < maxWorkGroupSize / it->tupleLength)
						{
							groupDimX = nextValue;
							nextValue = groupDimX * 2;
						}
					}
				}else{
					if(groupDimY > xTry)
					{
						groupDimY = xTry;
						nextValue = xTry;

						while(groupDimX * nextValue < maxWorkGroupSize / it->tupleLength)
						{
							groupDimY = nextValue;
							nextValue = groupDimY * 2;
						}

					}
				}

				it->dim0LocalSize = groupDimX * groupDimY * it->tupleLength;
				it->dim0GlobalSize = 
				std::ceil((float)(it->transposeWidth) / groupDimX) *groupDimX 
					* 
				std::ceil((float)(it->transposeHeight) / groupDimY) * groupDimY * it->tupleLength;
				for(int i = 0; i < it->numInput; ++i)
				{
					it->localMemSize.push_back(it->baseTypeSizeList[i] * groupDimY		
						* (groupDimX * it->tupleLength + 1));
				}
				it->transposeLocalWidth = groupDimX;
				it->transposeLocalHeight = groupDimY;
			}
		}
		kernelAlreadySet = true;
	}

	void * Executant::getInputBuffer(int i)
	{
		int inputNum;
		cl_int err;
		void * ptr;
		cl_mem memTmp;
		if(!kernelAlreadySet)
		{
			return NULL;
		}

		inputNum = inputSizeList.size();

		if(inputNum < i)
		{
			return NULL;
		}
		
		if(inputPtrList[i] == NULL)
		{
			memTmp = bufferList0[i];
			ptr= clEnqueueMapBuffer(commands, memTmp, CL_TRUE, CL_MAP_WRITE, 0, inputSizeList[i], 0, NULL, NULL, &err);
			checkError(err, "clEnqueueMapBuffer");
			inputPtrList[i] = ptr;
		}else{
			ptr = inputPtrList[i];
		}
		
		return ptr;
	}

	void Executant::finishWrite()
	{
		cl_int status;
		cl_kernel ko;
		cl_mem memTmp;

		if(kernelAlreadySet)
		{
			box->release();
		}
		for(int i = 0; i < inputSizeList.size(); ++i)
		{
			memTmp = bufferList0[i];
			if(inputPtrList[i] != NULL)
			{
#if 0
				//print out input in hex
				for(int j = 0; j < inputSizeList[i]; ++j)
				{
					std::cout << std::hex << std::uppercase << (int)(((unsigned char *)inputPtrList[i])[j])
					<< std::nouppercase << std::dec << std::endl;
				}
#endif

				status = clEnqueueUnmapMemObject(commands, memTmp, 
				inputPtrList[i], 0, NULL, box->getOneInputEvent(i));

				checkError(status, "clEnqueueUnmapMemObject");
			}
			inputPtrList[i] = NULL;
		}

		for(int i = 0; i < planList.size(); ++i)
		{
			int koIndex = 0;
			ExecutionInfo& info = planList[i];

			std::vector<cl_mem> * pInList;
			std::vector<cl_mem> * pOutList;
			if(i & 0x1)
			{
				pInList = &bufferList1;
				pOutList = &bufferList0;
			}else{
				pInList = &bufferList0;
				pOutList = &bufferList1;
			}

			for(int j = 0; j < info.numInput; ++j)
			{
				status = clSetKernelArg(info.ko, koIndex++, sizeof(cl_mem), 
				(void*)&(pInList->at(j)));
			}
			for(int j = 0; j < info.numOutput; ++j)
			{
				status = clSetKernelArg(info.ko, koIndex++, sizeof(cl_mem), 
				(void*)&(pOutList->at(j)));
			}
			for(int j = 0; j < info.dependList.size(); ++j)
			{
				int value = info.dependList[j];
				status = clSetKernelArg(info.ko, koIndex++, sizeof(int), 
				&value);
			}
			if(info.type == SKType::reduce)
			{	

				status = clSetKernelArg(info.ko, koIndex++, sizeof(int), 
				&(info.reduceLength));

				status = clSetKernelArg(info.ko, koIndex++, info.localMemSize[0]
				, 
				NULL);
			}else if(info.type == SKType::transpose)
			{
				for(int i = 0; i < info.numInput; ++i)
				{

					status = clSetKernelArg(info.ko, koIndex++, info.localMemSize[i]
					, 
					NULL);

				}

				status = clSetKernelArg(info.ko, koIndex++, sizeof(int), 
				&(info.transposeWidth));
				status = clSetKernelArg(info.ko, koIndex++, sizeof(int), 
				&(info.transposeHeight));
				status = clSetKernelArg(info.ko, koIndex++, sizeof(int), 
				&(info.transposeLocalWidth));
				status = clSetKernelArg(info.ko, koIndex++, sizeof(int), 
				&(info.transposeLocalHeight));
			}

			//Create the NDRange
			size_t global_dim[2];
			size_t local_dim[2];
			cl_uint eventNum;
			cl_event * eventPtr = NULL;

			if(i == 0)
			{
				eventPtr = box->getInputEvents(eventNum);
			}else{
				eventNum = 1;
				eventPtr = box->getOneOperationEvent(i - 1);
			}

			if(info.type == SKType::map)
			{
				global_dim[0] = info.dim0GlobalSize;
				global_dim[1] = info.dim1GlobalSize;
				//launch Kernel, letting runtime select the wg size
				status = clEnqueueNDRangeKernel(commands, info.ko, 2, NULL, global_dim, NULL, eventNum, eventPtr, box->getOneOperationEvent(i));
				checkError(status, "clEnqueueNDRangeKernel");
			}else if(info.type == SKType::gather || info.type == SKType::scatter)
			{
				global_dim[0] = info.dim0GlobalSize;
				global_dim[1] = info.dim1GlobalSize;
				local_dim[0] = info.dim0LocalSize;
				local_dim[1] = 1;
				status = clEnqueueNDRangeKernel(commands, info.ko, 2, NULL, global_dim, local_dim, eventNum, eventPtr, box->getOneOperationEvent(i));
				checkError(status, "clEnqueueNDRangeKernel");
			}else if(info.type == SKType::reduce){
				global_dim[0] = info.dim0GlobalSize;
				global_dim[1] = info.dim1GlobalSize;
				local_dim[0] = info.dim0LocalSize;
				local_dim[1] = 1;
				status = clEnqueueNDRangeKernel(commands, info.ko, 2, NULL, global_dim, local_dim, eventNum, eventPtr, box->getOneOperationEvent(i));
				checkError(status, "clEnqueueNDRangeKernel");
			}else if(info.type == SKType::transpose)
			{
				global_dim[0] = info.dim0GlobalSize;
				global_dim[1] = info.dim1GlobalSize;
				local_dim[0] = info.dim0LocalSize;
				local_dim[1] = 1;
				status = clEnqueueNDRangeKernel(commands, info.ko, 2, NULL, global_dim, local_dim, eventNum, eventPtr, box->getOneOperationEvent(i));
				checkError(status, "clEnqueueNDRangeKernel");
			}
		}
	}

	bool Executant::copyOutputBuffer(int i, void * outPtr)
	{

		int outputNum;
		cl_int err;
		void * ptr;
		cl_mem memTmp;
		if(!kernelAlreadySet)
		{
			return false;
		}

		outputNum = outputSizeList.size();

		if(outputNum < i)
		{
			return false;
		}

		if(operationNum & 0x1)
		{
			//odd operationNum
			memTmp = bufferList1[i];
		}else{
			memTmp = bufferList0[i];
		}

		if(reduceInThePlan)
		{
			ptr = clEnqueueMapBuffer(commands, memTmp, CL_TRUE, CL_MAP_READ, 0, outputSizeList[i], 1, box->lastOperationEvent(), NULL, &err);

			checkError(err,"clEnqueueMapBuffer");

			int reduceResultLength = planList.back().reduceCpuLength;
			unsigned char * pBytePtr = (unsigned char *)ptr;
			unsigned char * charOutPtr =  (unsigned char *)outPtr;
			int reduceResultVarSize = planList.back().resultVarSize;
			
			std::copy(pBytePtr, pBytePtr + reduceResultVarSize, pReduceResult);
			pBytePtr += reduceResultVarSize;

			for(int i = 1; i < reduceResultLength; ++i)
			{
				reduceFunction(pReduceResult, pBytePtr, pReduceResult);
				pBytePtr += reduceResultVarSize;
			}
			std::copy(pReduceResult, pReduceResult + reduceResultVarSize, charOutPtr);

			clEnqueueUnmapMemObject(commands, memTmp, ptr, 0, NULL, NULL);
			return true;
		}else{
			err = clEnqueueReadBuffer(commands, memTmp, CL_TRUE, 0, outputSizeList[i], outPtr, 1, box->lastOperationEvent(), NULL);  
			checkError(err,"clEnqueueReadBuffer");
			return true;
		}
	}

	void * Executant::getOutputBuffer(int i)
	{
		int outputNum;
		cl_int err;
		void * ptr;
		cl_mem memTmp;
		if(!kernelAlreadySet)
		{
			return NULL;
		}

		outputNum = outputSizeList.size();

		if(outputNum < i)
		{
			return NULL;
		}

		if(operationNum & 0x1)
		{
			//odd operationNum
			memTmp = bufferList1[i];
		}else{
			memTmp = bufferList0[i];
		}

		ptr = clEnqueueMapBuffer(commands, memTmp, CL_TRUE, CL_MAP_READ, 0, outputSizeList[i], 1, box->lastOperationEvent(), NULL, &err);

		outputPtrList[i] = ptr;
		if(reduceInThePlan)
		{
			int reduceResultLength = planList.back().reduceCpuLength;
			unsigned char * pBytePtr = (unsigned char *)ptr;
			int reduceResultVarSize = planList.back().resultVarSize;
			
			std::copy(pBytePtr, pBytePtr + reduceResultVarSize, pReduceResult);
			pBytePtr += reduceResultVarSize;

			for(int i = 1; i < reduceResultLength; ++i)
			{
				reduceFunction(pReduceResult, pBytePtr, pReduceResult);
				pBytePtr += reduceResultVarSize;
			}
			return pReduceResult;
		}else{
			return ptr;
		}
	}

	void Executant::finishRead()
	{
		cl_mem memTmp;
		for(int i = 0; i < outputSizeList.size(); ++i)
		{
			memTmp = bufferList0[i];
			if(outputPtrList[i] != NULL)
			{
				clEnqueueUnmapMemObject(commands, memTmp, 
				outputPtrList[i], 0, NULL, NULL);
				outputPtrList[i] = NULL;
			}
		}
	}

			
	void Executant::clearPlan()
	{
		clearKernel();
		planList.clear();
		reduceInThePlan = false;
		delete[] pReduceResult;
		pReduceResult = NULL;
	}

	void Executant::setContextAndProgram(cl_context contextPara, cl_device_id id, cl_command_queue queue, cl_program programPara)
	{
		cl_int err;
		cl_mem memTemp;

		clearContext();
		clearKernel();

		err = clRetainContext(contextPara);
		if(err != CL_SUCCESS)
		{
			throw Exception("ExecutionError", "fail to retain context");
		}

		context = contextPara;
		device_id = id;

		err = clRetainProgram(programPara);

		if(err != CL_SUCCESS)
		{
			throw Exception("ExecutionError", "fail to retain program");
		}

		program = programPara;

		//commands = clCreateCommandQueue(context, device_id, 0, &err);
		commands = queue;
		err = clRetainCommandQueue(queue);
		if(err != CL_SUCCESS)
		{
			throw Exception("ExecutionError", "fail to retain commandqueue");
		}

		for(int i = 0; i < bufferSizeList.size(); ++i)
		{
			memTemp = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, bufferSizeList[i], NULL, &err);
			if(err != CL_SUCCESS)
			{
				throw Exception("ExcutionError", "fail to create buffer");
			}

			bufferList0.push_back(memTemp);
			
			memTemp = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, bufferSizeList[i], NULL, &err);

			if(err != CL_SUCCESS)
			{
				throw Exception("ExcutionError", "fail to create buffer");
			}
			bufferList1.push_back(memTemp);
		}

		// Get local memory size
		err = clGetDeviceInfo(device_id, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(cl_ulong), &localMemSize, NULL);

		if(err != CL_SUCCESS)
		{
			throw Exception("ExcutionError", "fail to get maximum local memory size");
		}

		contextAlreadySet = true;
	}
}
