__kernel void add(__global float* v1, __global float* v2, __global float* result){
    size_t i = get_global_id(0);
	result[i] = v1[i];
}