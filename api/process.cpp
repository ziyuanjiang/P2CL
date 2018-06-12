#ifdef __APPLE__
#include <OpenCL/opencl.h>
#include <unistd.h>
#else
#include <CL/cl.h>
#endif

#include <pthread.h>
#include <iostream>
#include "exception.hpp"
#include "process.hpp"
#include "ctype.hpp"
#include "analyzer.hpp"
#include "xmlparser.hpp"
#include "circularindex.hpp"

namespace p2cl{
	Process::Process(int batchNum, std::string xmlFileName, 
		std::string additionalFunc,
		std::function<void(void *, void*, void*)> reduceFunction)
	{
		
		cl_int err;
		cl_program  program;
		cl_context context;
		cl_device_id device_id;
		std::string kernelSrc;
		XmlParser parser(xmlFileName);
		Analyzer *pAnalyzer;
		pAnalyzer = parser.getAnalyzer();
		pAnalyzer->setFunctionDefinition(additionalFunc);
		kernelSrc = pAnalyzer->getKernelSrc();
		std::cout<<kernelSrc<<std::endl;
		context = createContext(device_id);
		program = clCreateProgramWithSource(context, 1, (const char **) & kernelSrc, NULL, &err);
		// Build the program
		err = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
		if (err != CL_SUCCESS)
		{
			cl_int logStatus;
			char *buildLog = NULL;
			size_t buildLogSize = 0;
			logStatus = clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, buildLogSize, buildLog, &buildLogSize);

			buildLog = (char *)malloc(buildLogSize);

			memset(buildLog, 0, buildLogSize);

			logStatus = clGetProgramBuildInfo (program, device_id, CL_PROGRAM_BUILD_LOG, buildLogSize, buildLog, NULL);
			if(logStatus != CL_SUCCESS)
			{
				free(buildLog);
				throw Exception("OpenCL Error", "fail to get build log");
			}

			std::cout<<addLineNum(kernelSrc)<<std::endl;
			std::cout<<buildLog<<std::endl;
			free(buildLog);
			throw Exception("P2CL Error", "fail to compile");
		}
		Executant* pExecutant =
		pAnalyzer->getExecutant();
		updatePortList(pAnalyzer->getPortList());
		if(pExecutant == NULL)
		{
			throw Exception("P2CL Error", "fail to create execution actor");
		}

		cl_command_queue
		commands = clCreateCommandQueue(context, device_id, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &err);
		pExecutant->setContextAndProgram(context, device_id, commands,program);
		pExecutant->setReduceFunction(reduceFunction);
		pExecutant->preparation();
		readyInputNumList.resize(batchNum, 0);
		takenOutputNumList.resize(batchNum,0);
		for(int i = 0; i < batchNum; ++i)
		{
			executantList.push_back(pExecutant->clone());
		}
		
		this->batchNum = batchNum;
		for(int i = 0; i < inputPortList.size(); ++i)
		{
			inputCbIndexList.push_back(CircularIndex(batchNum, inputSize(i), 
			std::bind(&Process::oneInputWritten, this, std::placeholders::_1)));
		}

		for(int i = 0; i < outputPortList.size(); ++i)
		{
			outputCbIndexList.push_back(CircularIndex(batchNum, outputSize(i), 
			std::bind(&Process::oneOutputRead, this, std::placeholders::_1)));
		}
		executionCbStart = CircularIndex(batchNum, 1);
		executionCbEnd = CircularIndex(batchNum, 1);

		if(pthread_mutex_init(&mutex_in, NULL) != 0)
		{
			throw Exception("Process Error", "fail to create mutex");
		}

		if(pthread_mutex_init(&mutex_out, NULL) != 0)
		{
			throw Exception("Process Error", "fail to create mutex");
		}

		if(pthread_mutex_init(&mutex_ready_in, NULL) != 0)
		{
			throw Exception("Process Error", "fail to create mutex");
		}

		if(pthread_mutex_init(&mutex_ready_out, NULL) != 0)
		{
			throw Exception("Process Error", "fail to create mutex");
		}

		if(pthread_cond_init(&cond_in, NULL) != 0)
		{
			throw Exception("Process Error", "fail to create cond");
		}

		if(pthread_cond_init(&cond_out, NULL) != 0)
		{
			throw Exception("Process Error", "fail to create cond");
		}


		delete pExecutant;
		delete pAnalyzer;
		clReleaseProgram(program);
		clReleaseContext(context);
		clReleaseCommandQueue(commands);
	}

	int Process::getInPortIndex(std::string name) const
	{
		int i;
		for(i = 0; i < numIn; ++i)
		{
			if(inputPortList[i].name.compare(name) == 0)
			{
				break;
			}
		}

		if(i != numIn)
			return i;
		else
			return -1;

	}

	int Process::getOutPortIndex(std::string name) const
	{
		int i;
		for(i = 0; i < numIn; ++i)
		{
			if(outputPortList[i].name.compare(name) == 0)
			{
				break;
			}
		}

		if(i < numOut)
			return i;
		else
			return -1;
	}

	std::string Process::addLineNum(std::string srcBody) const
	{
		int lineNum = 0;
		std::stringstream ss;
		std::string line;
		std::istringstream isBody(srcBody);
		while (std::getline(isBody, line)) 
		{
			ss<<lineNum++<<"\t"<<line<<std::endl;
		}
		return ss.str();
	}

	void Process::oneOutputRead(int index)
	{
		pthread_mutex_lock(&mutex_ready_out);
		++takenOutputNumList[index];

		if(takenOutputNumList[executionCbStart] == numOut)
		{
			pthread_mutex_unlock(&mutex_ready_out);
			takenOutputNumList[executionCbStart] = 0;
			//executantList[executionCbStart]->finishRead();
			pthread_mutex_lock(&mutex_out);
			executionCbStart.increment();
			pthread_cond_signal(&cond_out);
			pthread_mutex_unlock(&mutex_out);
		}else{
			pthread_mutex_unlock(&mutex_ready_out);
		}
	}

	void Process::oneInputWritten(int index)
	{
		pthread_mutex_lock(&mutex_ready_in);
		++readyInputNumList[index];
		static int count = 0;

		if(readyInputNumList[executionCbEnd] == numIn)
		{
			pthread_mutex_unlock(&mutex_ready_in);
			readyInputNumList[executionCbEnd] = 0;
			executantList[executionCbEnd]->finishWrite();
			pthread_mutex_lock(&mutex_in);
			executionCbEnd.increment();
			pthread_cond_signal(&cond_in);
			pthread_mutex_unlock(&mutex_in);
		}else{
			pthread_mutex_unlock(&mutex_ready_in);
		}
	}

	void Process::updatePortList(std::vector<Port> list)
	{
		for(std::vector<Port>::iterator it = list.begin();
			it != list.end(); ++it)
		{
			if(it->dirIn)
			{
				inputPortList.push_back(*it);
			}else{
				outputPortList.push_back(*it);
			}
		}
		numIn = inputPortList.size();
		numOut = outputPortList.size();
	}

	Process::~Process()
	{
		for(int i = 0; i < executantList.size(); ++i)
		{
			delete executantList[i];
		}

		pthread_mutex_destroy(&mutex_in);
		pthread_mutex_destroy(&mutex_out);
		pthread_mutex_destroy(&mutex_ready_in);
		pthread_mutex_destroy(&mutex_ready_out);
		pthread_cond_destroy(&cond_in);
		pthread_cond_destroy(&cond_out);
	}

	int Process::numInput() const
	{
		return numIn;
	}

	int Process::numOutput() const
	{
		return numOut;
	}

	size_t Process::inputSize(int i) const
	{
		if(i < inputPortList.size())
		{
			return inputPortList[i].length * 
				clTypeSize(inputPortList[i].type);
		}else{
			return 0;
		}
	}

	size_t Process::outputSize(int i) const
	{
		if(i < outputPortList.size())
		{
			return outputPortList[i].length * 
				clTypeSize(outputPortList[i].type);
		}else{
			return 0;
		}
	}

	cl_context Process::createContext(cl_device_id & device_id) const
	{
		cl_int err;
		// Set up platform and GPU device

		cl_context       context;       // compute context
		cl_uint numPlatforms;

		// Find number of platforms
		err = clGetPlatformIDs(0, NULL, &numPlatforms);
		if (err != CL_SUCCESS || numPlatforms == 0)
		{
			throw Exception("OpenCL Error", "no OpenCL platform");
		}

		// Get all platforms
		cl_platform_id Platform[numPlatforms];
		err = clGetPlatformIDs(numPlatforms, Platform, NULL);
		if(err != CL_SUCCESS)
		{
			throw Exception("OpenCL Error", "no OpenCL platform");
		}

		// Secure a GPU
		for (int i = 0; i < numPlatforms; ++i)
		{
			err = clGetDeviceIDs(Platform[i], CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);
			if (err == CL_SUCCESS)
			{
				break;
			}
		}

		if (device_id == NULL)
		{
			throw Exception("OpenCL Error", "no GPU device");
		}

		// Create a compute context
		context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
		if(err != CL_SUCCESS)
		{
			throw Exception("OpenCL Error", "fail to create OpenCL context");
		}
		return context;
	}
}

