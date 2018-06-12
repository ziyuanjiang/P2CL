#include <iostream>
#include "frontendutils.hpp"
#include "process.hpp"
#include "wtime.hpp"

void vector_sum(float a, float b, float* c)
{
	*c = a + b;
}

void vector_sum_wrapper(void *a, void *b, void*c)
{
	float va = *((float *)a);
	float vb = *((float *)b);
	vector_sum(va, vb, (float *) c);
}

int main(int argc, char *argv[])
{
	float a[131072];
	float b[131072];
	float c[1];
	double rtime;
	if(argc < 3)
	{
		std::cout<<"usage: "<<argv[0]<<" xmlfilename srcfilename"<<std::endl;
		return 0;
	}
	
	p2cl::Process obj(1,argv[1], p2cl::loadProgram(argv[2]), vector_sum_wrapper);
	
	rtime = wtime();

	for(int j = 0; j < 1000; ++j)
	{
		obj.pushBack(sizeof(float) * 131072, 0, a);
		obj.pushBack(sizeof(float) * 131072, 1, b);
		obj.pop(0, c);
	}

	
	rtime = wtime() - rtime;
	std::cout<<"The kernel ran in\t"<<rtime/1000 <<"\tseconds\n";

	//std::cout<<c[0]<<std::endl;
}
