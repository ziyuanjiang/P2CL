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

	if(argc < 3)
	{
		printf("usage:\t %s length wg_size", argv[0]);
		exit(0);
	}

	int length = atoi(argv[1]);
	int work_group_size = atoi(argv[2]);
	


	float * h_in = (float *) calloc(sizeof(float) , length);


	start_time =wtime();

	cl_device_id	 device_id;	 // compute device id
	cl_context	   context;	   // compute context
	cl_command_queue commands;	  // compute command queue
	cl_program	   program;	   // compute program
	cl_kernel	ko_mapreduce;	   // compute kernel
	cl_mem		d_partial_sums;
	cl_mem		d_in0;
	cl_mem		d_in1;
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
	char * kernel_source = getKernelSource("mapreduce.cl");

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
	ko_mapreduce = clCreateKernel(program, "mapreduce", &err);
	checkError(err, "Creating kernel");
	//get work-group-size for the kernel
//	err = clGetKernelWorkGroupInfo(ko_mapreduce, device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &work_group_size, NULL);
//	checkError(err, "Getting Kernel work group info");

	size_t num_work_groups = 32;

	size_t niters = (length/ 32) / num_work_groups;

	size_t local = work_group_size;
	size_t global = num_work_groups * local;

	float * h_psum = (float *) calloc(sizeof(float), num_work_groups);
	printf("%d\t%d\n", num_work_groups, local);
	d_partial_sums = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * num_work_groups, NULL, &err);
	d_in0 = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) * length, NULL, &err);
	d_in1 = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) * length, NULL, &err);
	
	// Set the arguments to our compute kernel
	err  = clSetKernelArg(ko_mapreduce, 0, sizeof(int), &niters);
	err  |= clSetKernelArg(ko_mapreduce, 1, sizeof(cl_mem), &d_in0);
	err  |= clSetKernelArg(ko_mapreduce, 2, sizeof(cl_mem), &d_in1);
	err  |= clSetKernelArg(ko_mapreduce, 3, sizeof(cl_mem), &d_partial_sums);
	err  |= clSetKernelArg(ko_mapreduce, 4, sizeof(float) * work_group_size , NULL);
	checkError(err, "setting kernel args");
	double rtime = wtime();

	float result = 0;
	
	for(int j = 0; j < 1000; ++j)
	{

		err = clEnqueueWriteBuffer(commands, d_in0, CL_FALSE, 0, sizeof(float) * length, h_in, 0, NULL, NULL);

		err = clEnqueueWriteBuffer(commands, d_in1, CL_FALSE, 0, sizeof(float) * length, h_in, 0, NULL, NULL);

		err = clEnqueueNDRangeKernel(commands, ko_mapreduce, 1, NULL, &global, &local, 0, NULL, NULL);
		checkError(err, "Enqueueing kernel");

		err = clEnqueueReadBuffer(commands, d_partial_sums, CL_TRUE, 0, sizeof(float) * num_work_groups, h_psum, 0, NULL, NULL);
		checkError(err, "read back");
		result = 0;
		for(i = 0; i < num_work_groups; ++i)
		{
			result += h_psum[i];
		}
	}
	rtime = wtime() - rtime;
	free(h_psum);
	free(h_in);
	printf("\nThe kernel ran in\t%lf\tseconds\n",rtime /1000);
}

