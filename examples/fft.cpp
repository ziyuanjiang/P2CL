#include <iostream>
#include "frontendutils.hpp"
#include "process.hpp"

int main(int argc, char *argv[])
{
	float fout[1024];
	if(argc < 3)
	{
		std::cout<<"usage: "<<argv[0]<<" xmlfilename srcfilename"<<std::endl;
		return 0;
	}
	
	p2cl::Process obj(3,argv[1], p2cl::loadProgram(argv[2]));

	for(int i = 0; i < 512; ++i)
	{
		float value = i;
		obj.pushBack(sizeof(float), 0, &value);
		value = 0;
		obj.pushBack(sizeof(float), 0, &value);
	}

	obj.pop(0, fout);

	for(int i = 0; i < 512; ++i)
	{
		std::cout<<fout[2 * i]<<"\t+"
		<<fout[2 * i + 1]<<'i'<<std::endl;
	}
}
