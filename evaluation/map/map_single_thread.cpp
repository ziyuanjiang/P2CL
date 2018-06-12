#include <pthread.h>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include "wtime.hpp"
#include "frontendutils.hpp"
#include "process.hpp"

int main(int argc, char *argv[])
{
	float fout[65536];
	int retVal;
	double rtime;
	float value;

	p2cl::Process obj(1,"map.xml", p2cl::loadProgram("map.ccode"));

	rtime = wtime();

	for(int j = 0; j < 1000; ++j)
	{

		obj.pushBack(sizeof(float) *65536, 0, fout);

		obj.pop(0, fout); 
	}

	rtime = wtime() - rtime;
	std::cout<<"ran in\t"<< rtime/1000 <<"\tseconds"<<std::endl;

}

