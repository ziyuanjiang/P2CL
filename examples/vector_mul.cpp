#include <iostream>
#include "frontendutils.hpp"
#include "process.hpp"

void vector_sum(int a, int b, int* c)
{
	*c = a + b;
}

void vector_sum_wrapper(void *a, void *b, void*c)
{
	int va = *((int *)a);
	int vb = *((int *)b);
	vector_sum(va, vb, (int *) c);
}

int main(int argc, char *argv[])
{
	int a[1] = {1};
	int b[1] = {1};
	int c[1];
	if(argc < 3)
	{
		std::cout<<"usage: "<<argv[0]<<" xmlfilename srcfilename"<<std::endl;
		return 0;
	}
	
	p2cl::Process obj(3,argv[1], p2cl::loadProgram(argv[2]), vector_sum_wrapper);
	
	
	for(int i = 0; i < 1024; ++i)
	{
		obj.pushBack(sizeof(int), 0, a);
		obj.pushBack(sizeof(int), 1, b);
	}

	obj.pop(0, c);
	std::cout<<c[0]<<std::endl;
}
