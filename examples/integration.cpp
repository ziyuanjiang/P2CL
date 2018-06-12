#include <iostream>
#include <cmath>
#include "process.hpp"
#include "wtime.hpp"

void add(float a, float b, float* c)
{
	*c = a + b;
}

void add_wrapper(void *a, void *b, void*c)
{
	float va = *((float*)a);
	float vb = *((float*)b);
	add(va, vb, (float*) c);
}

const char * func = 
"float f(float i)\
{\
	return sin(i) * cos(i);\
}\
"
"void f_cal(int i, float * c, int num)\n"
"{\n"
"	float x;\n"
"       x = (i+0.5f)/num;\n"
"       x = f(x);\n"
"	*c = x / num;\n"
"}\n"
"\n"
"void add(float a, float b, float* c)\
{\
	*c = a + b;\
}\
";

float f(float i)
{
	return sin(i) * cos(i);
}

void f_cal(int i, float * c, int num)
{
	float x;
       x = (i+0.5f)/num;
	x = f(x);
	*c = x / num;
}

int main(int argc, char *argv[])
{
	float c;
	float val;
	int num;
	if(argc < 3)
	{
		std::cout<<"usage: "<<argv[0]<<" xmlfilename num"<<std::endl;
		return 0;
	}
	std::istringstream iss(argv[2]);
	iss>>num;
	
	p2cl::Process obj(1,argv[1], func, add_wrapper);
	
	double rtime = wtime();
	obj.pop(0, &c);

	rtime = wtime() - rtime;
	std::cout<<"GPU ran in "<<rtime * 1000<<" miliseconds"<<std::endl;
	std::cout<<c<<std::endl;

	rtime = wtime();
	for(int j = 0; j < 10; ++j)
	{
		c= 0;
		for(int i = 0; i < num; ++i)
		{
			f_cal(i, &val, num);
			c+=val;
		}
	}

	rtime = wtime() - rtime;
	std::cout<<"CPU ran in "<<rtime * 1000/10<<" miliseconds"<<std::endl;
	std::cout<<c<<std::endl;
}
