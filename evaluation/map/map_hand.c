#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#include <unistd.h>
#else
#include <CL/cl.h>
#endif

#include "err_code.hpp"
#include "wtime.hpp"

//pick up device type from compiler command line or from
//the default type
#ifndef DEVICE
#define DEVICE CL_DEVICE_TYPE_DEFAULT
#endif

char * getKernelSource(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        fprintf(stderr, "Error: Could not open kernel source file\n");
        exit(EXIT_FAILURE);
    }
    fseek(file, 0, SEEK_END);
    int len = ftell(file) + 1;
    rewind(file);

    char *source = (char *)calloc(sizeof(char), len);
    if (!source)
    {
        fprintf(stderr, "Error: Could not allocate memory for source string\n");
        exit(EXIT_FAILURE);
    }
    fread(source, sizeof(char), len, file);
    fclose(file);
    return source;
}

int main (int argc, char ** argv)
{
	int i;
	double x, pi, sum = 0.0;
	double start_time, run_time;

	int length = 65536;


	float * h_in = (float *) calloc(sizeof(float) , length);
	float * h_out = (float *) calloc(sizeof(float) , length);


	start_time =wtime();

	cl_device_id	 device_id;	 // compute device id
	cl_context	   context;	   // compute context
	cl_command_queue commands;	  // compute command queue
	cl_program	   program;	   // compute program
	cl_kernel	ko_map;	   // compute kernel
	cl_mem		d_in0;
	cl_mem		d_out;
	cl_int err;

	// Set up platform and GPU device 
	cl_uint numPlatforms;

	// Find number of platforms
	err = clGetPlatformIDs(0, NULL, &numPlatforms);
	checkError(err, "Finding platforms");
	if (numPlatforms == 0)
	{
		printf("Found 0 platforms!\n");
		return EXIT_FAILURE;
	}

	// Get all platforms
	cl_platform_id Platform[numPlatforms];
	err = clGetPlatformIDs(numPlatforms, Platform, NULL);
	checkError(err, "Getting platforms");

	// Secure a GPU
	for (i = 0; i < numPlatforms; i++)
	{
		err = clGetDeviceIDs(Platform[i], DEVICE, 1, &device_id, NULL);
		if (err == CL_SUCCESS)
		{
			break;
		}
	}

	if (device_id == NULL)
		checkError(err, "Finding a device");


	// Create a compute context
	context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
	checkError(err, "Creating context");

	// Create a command queue
	commands = clCreateCommandQueue(context, device_id, 0, &err);
	checkError(err, "Creating command queue");

	//get the kernel source
	char * kernel_source = getKernelSource("map.cl");

	// Create the compute program from the source buffer
	program = clCreateProgramWithSource(context, 1, (const char **) & kernel_source, NULL, &err);
	checkError(err, "Creating program");

	free(kernel_source);

	// Build the program
	err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	if (err != CL_SUCCESS)
	{
		size_t len;
		char buffer[2048];

		printf("Error: Failed to build program executable!\n%s\n", err_code(err));
		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
		printf("%s\n", buffer);
		return EXIT_FAILURE;
	}
	// Create the compute kernel from the program
	ko_map = clCreateKernel(program, "operation0", &err);
	checkError(err, "Creating kernel");
	//get work-group-size for the kernel
//	err = clGetKernelWorkGroupInfo(ko_mapreduce, device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &work_group_size, NULL);
//	checkError(err, "Getting Kernel work group info");



	size_t global = length;

	d_in0 = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) * length, NULL, &err);
	d_out = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * length, NULL, &err);
	
	// Set the arguments to our compute kernel
	err  = clSetKernelArg(ko_map, 0, sizeof(cl_mem), &d_in0);
	err  |= clSetKernelArg(ko_map, 1, sizeof(cl_mem), &d_out);

	checkError(err, "setting kernel args");

	for(i = 0; i < length; ++i)
	{
		h_in[i] = 1;
	}

	double rtime = wtime();

	for(int j = 0; j < 1000; j++)
	{

		err = clEnqueueWriteBuffer(commands, d_in0, CL_FALSE, 0, sizeof(float) * length, h_in, 0, NULL, NULL);

		err = clEnqueueNDRangeKernel(commands, ko_map, 1, NULL, &global, NULL, 0, NULL, NULL);
		checkError(err, "Enqueueing kernel");

		err = clEnqueueReadBuffer(commands, d_out, CL_TRUE, 0, sizeof(float) * length, h_out, 0, NULL, NULL);
		checkError(err, "read back");
	}
	rtime = wtime() - rtime;
	free(h_in);
	printf("\nThe kernel ran in\t%lf\tseconds\n",rtime/1000);
}

