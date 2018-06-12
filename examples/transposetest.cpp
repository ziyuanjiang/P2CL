#include <iostream>
#include "frontendutils.hpp"
#include "process.hpp"

int main(int argc, char *argv[])
{
	int a = 1;
	int c[1024];
	
	p2cl::Process obj(2,"transpose.xml");
	
	
	for(int i = 0; i < 1024; ++i)
	{
		obj.pushBack(sizeof(int), 0, &i);
	}

	obj.pop(0, &c);
	for(int j = 0; j < 2; ++j)
	{
		for(int i = 0; i < 256; ++i)
		{
			std::cout<<c[512 * j + 2 *i]<<'\t';
			std::cout<<c[512 * j + 2 *i + 1]<<'\t';
		}
		std::cout<<std::endl;
	}
}
