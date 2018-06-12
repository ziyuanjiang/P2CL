#include "map.ccode"

__kernel void operation0(
		__global float *	input0
		,
		__global float *	output0
)
{
	size_t g0 = get_global_id(0);
	
	size_t l0 = get_local_id(0);
	
	
	float input_variable0;
	float output_variable0;
	{
		float * p_input_var0 = (float *)(&input_variable0);
		{
			p_input_var0[0] = input0[g0];
		
		}
	
	}
	
	{
		f0(input_variable0, g0, &output_variable0);
	}
	
	{
		float * p_output_var0 = (float *)(&output_variable0);
		{
			output0[g0] = p_output_var0[0];
		
		}
	
	}
	
}
