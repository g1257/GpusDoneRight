extern "C" __global__ void initArray(int *g_data, int *factor, int num_iterations,int n)
{ 
	int idx = blockIdx.x * blockDim.x + threadIdx.x;
	if (idx>=n) return;

	for(int i=0;i<num_iterations;i++)
		g_data[idx] += *factor;	// non-coalesced on purpose, to burn time
}

