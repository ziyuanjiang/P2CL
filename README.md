# P2CL
Patterns-to-OpenCL (P2CL) is a library developed with the goal to 
help the creation of OpenCL stream programs using data parallel patterns.

# Dependencies 
The project is developed using C++. It uses several features of C++11, so it has to be compiled using a compiler with support for C++11.
CMake is used for the management of the project. The scripts used in this project requires a cmake with its version higher than 3.1.
The minimum requirements for OpenCL versions is OpenCL 1.2.
Both the version of OpenCL library provided for compilation and the OpenCL version the target device supported should both be higher or equal to 1.2.

# Building
The Project uses cmake to manage building.
Before generating the native building environment using cmake, the user have to modify the settings for the OpenCL header and library locations in the main CMakeList.txt
The settings are illustrated as follows.
```cmake
include_directories($ENV{INTELOCLSDKROOT}/include) 
link_directories($ENV{INTELOCLSDKROOT}/lib/x64) 
```
After correctly setting the OpenCL locations, on Linux systems, the build environment can be created using the following commands.
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
```
These commands create a build folder under the root of the project and generate the build environment inside it.

Then a simple make command can be used to compile the library as well as the examples and evaluations.

The compiled examples should be found in "./build/bin/"
and the compiled library should be found in "./build/libs/" .

# Usage
The details of the library and a tutorial on using the library can be found in Ziyuan Jiang's Master Thesis "Synthesis of GPU Programs from Higher-Level
Models".
Here only the vector production application in the example folder of is explained.
Using the library to create programs first requires a xml file that describes the algorithm using parallel patterns.
Here the function_descriptions container is the place to specify functions that is used by patterns below.
The process container is the place where the algorithm is specified.
The port elements are the input and output interface of the application.
The operation elements specifies parallel operations using parallel patterns and function.

```xml
<?xml version="1.0" encoding="UTF-8"?>
<document>
<function_descriptions>
	<function>
		<decl>void vector_mul(int a, int b, int* c)</decl>
		<num_input>2</num_input>
		<num_output>1</num_output>
		<num_para>0</num_para>
	</function>
	<function>
		<decl>void vector_sum(int a, int b, int* c)</decl>
		<num_input>2</num_input>
		<num_output>1</num_output>
		<num_para>0</num_para>
	</function>
</function_descriptions>
<process>
	<port name="iport0" direction="in" type="int" length="1024"/>
	<port name="iport1" direction="in" type="int" length="1024"/>
	<port name="oport" direction="out" type="int" length="1"/>

	<operation>
			<skeleton_type>map</skeleton_type>
			<function>vector_mul</function>
			<length value_type="value">1024</length>
	</operation>
	<operation>
			<skeleton_type>reduce</skeleton_type>
			<function>vector_sum</function>
			<length value_type="value">1024</length>
	</operation>
</process>
</document>

```
After creation the algorithm using the xml file.
In the source code of the program, the user simply create a process object which has the API of the library.
The decription of the process constructor is shown below.
```
Process Constructor
p2cl::Process::Process (
int batchNum,
std::string xmlFileName,
std::string additionalFunc = std::string(),
std::function<void(void *, void *, void *)> reduceFunction
= std::function<void(void *, void*, void*)>()
)
Parameters:
batchNum the max number of computation that the process object
can handle at the same time
xmlFileName le name of the xml le that describes the algorithm
```

In this example, the process object is created given the provided xml file, the wrapper function for the final reduce pattern and also the function definitions that declared in the xml file as strings.
Then the program can simply enqueue the input data to the process object and pop back the result from it by specifying the correct input and output port.
Note the APIs of the process object is thread-safe. 
The user may use the APIs in different thread and enable the process object to support concurrent execution to achieve better performance.


```Cpp
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
```
