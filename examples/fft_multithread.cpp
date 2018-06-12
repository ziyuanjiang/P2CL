#include <pthread.h>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include "wtime.hpp"
#include "frontendutils.hpp"
#include "process.hpp"

void * inputThread(void * processPtr)
{
	int retVal;
	p2cl::Process * ptr = (p2cl::Process *)(processPtr);
	if(ptr != NULL)
	{
		for(int j = 0; j < 1000; ++j)
		{
			for(int i = 0; i < 512; ++i)
			{
				float value = i;
				while(ptr->pushBack(sizeof(float), 0, &value) != sizeof(float));
				value = 0;
				while(ptr->pushBack(sizeof(float), 0, &value) != sizeof(float));
			}
		}
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	float fout[1024];
	int retVal;
	int num;
	pthread_t thread;
	double rtime;
	if(argc < 2)
	{
		std::cout<<"usage: "<<argv[0]<<" numbatch"<<std::endl;
		return 0;
	}
	std::istringstream iss(argv[1]);
	iss>>num;

	
	p2cl::Process obj(num,"fft.xml", p2cl::loadProgram("fft.ccode"));

	if(pthread_create(&thread, NULL, inputThread, &obj)) {
		fprintf(stderr, "Error creating thread\n");
		return 1;
	}
	
	rtime = wtime();

	for(int j = 0; j < 1000; ++j)
	{
		//retVal = obj.pop(0, fout);
		//while(retVal != 4096)std::cout<<retVal<<std::endl;
		while(obj.pop(0, fout) != 4096); 
	}

	rtime = wtime() - rtime;
	std::cout<<"Throughput:\t"<<51200000.0/rtime<<"\tflops\n";

	if(pthread_join(thread, NULL)) {

		fprintf(stderr, "Error joining thread\n");
		return 2;
	}
/*
	for(int i = 0; i < 512; ++i)
	{
		std::cout<<fout[2 * i]<<"\t+"
		<<fout[2 * i + 1]<<'i'<<std::endl;
	}
*/
}

