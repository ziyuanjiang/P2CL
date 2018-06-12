__kernel void mapreduce(
int niters,
__global float * in0,
__global float * in1,
__global float * out,
__local float * local_mem)
{
        size_t gp0 = get_group_id(0);

        size_t l0 = get_local_id(0);

        size_t l_size0 = get_local_size(0);

        size_t g_size0 = get_global_size(0);

        size_t index0 = get_global_id(0);

        int partial_length = l_size0;

	float partial_result = 0;
	
	int start_index = niters * index0;
	
	int end_index = start_index + niters;

	for(int i = start_index; i < end_index; ++i)
	{
		partial_result += in0[i] * in1[i];// + exp(i);
		for(int j = 0; j < 1000; ++j)
			partial_result += 1;
	}

	local_mem[l0] = partial_result;

        barrier(CLK_LOCAL_MEM_FENCE);

	for(int i = 1; i < partial_length; ++i)
	{
		partial_result += local_mem[i];
	}


	if(l0 == 0)
	{
		out[gp0] = partial_result;
	}
}
